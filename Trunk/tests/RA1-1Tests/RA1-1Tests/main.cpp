/*
 * blink.cpp
 *
 * Created: 6/27/2016 8:52:16 PM
 * Author : WeistekEng (Jeremy G).
 * USART  : Tx pin is now PA24 PAD2, Rx pin is now PA25 PAD3
 */ 


#include "sam.h"

//#define MY_BAUDREG_VALUE 64278    // 64278 (F_CPU=F_SERCOM=8MHz) -> 9600 BAUD
#define MY_BAUDREG_VALUE 55470      // 55470 (F_CPU=F_SERCOM=1MHz) -> 9600 BAUD
uint32_t USART_BAUD_MODIFIER_SLOW = 1048553;
long USART_BAUD_MODIFIER_FAST = 115199;

// prototypes
void init_TC1(void);
void UART_sercom_simpleWrite(Sercom *const sercom_module, uint8_t data);
//static inline void pin_set_peripheral_function(uint32_t pinmux);
uint8_t UART_sercom_init(Sercom *const sercom_module);

void init_TC1(void)
{
	//setup clock for timer/counters
	REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_TC1_TC2;
	REG_PM_APBCMASK |= PM_APBCMASK_TC1;
	REG_TC1_CTRLA |=TC_CTRLA_PRESCALER_DIV8;	// prescaler: 8
	REG_TC1_INTENSET = TC_INTENSET_OVF;		// enable overflow interrupt
	REG_TC1_CTRLA |= TC_CTRLA_ENABLE;		// enable TC1
	while(TC1->COUNT16.STATUS.bit.SYNCBUSY==1);	// wait on sync
	NVIC_EnableIRQ(TC1_IRQn);			// enable TC1 interrupt in the nested interrupt controller
}

void TC1_Handler()
{
	REG_PORT_OUTTGL0 = PORT_PA14;		// troggle PA02
	REG_TC1_INTFLAG = TC_INTFLAG_OVF;	// reset interrupt flag - NEEDED HERE!
}

static inline void pin_set_peripheral_function(uint32_t pinmux)
{
    /* the variable pinmux consist of two components:
        31:16 is a pad, wich includes:
            31:21 : port information 0->PORTA, 1->PORTB
            20:16 : pin 0-31
        15:00 pin multiplex information
        there are defines for pinmux like: PINMUX_PA09D_SERCOM2_PAD1 
    */
    uint16_t pad = pinmux >> 16;    // get pad (port+pin)
    uint8_t port = pad >> 5;        // get port
    uint8_t pin  = pad & 0x1F;      // get number of pin - no port information anymore
    
    PORT->Group[port].PINCFG[pin].bit.PMUXEN =1;
    
    /* each pinmux register is for two pins! with pin/2 you can get the index of the needed pinmux register
       the p mux resiter is 8Bit   (7:4 odd pin; 3:0 evan bit)  */
    // reset pinmux values.                             VV shift if pin is odd (if evan:  (4*(pin & 1))==0  )
    PORT->Group[port].PMUX[pin/2].reg &= ~( 0xF << (4*(pin & 1)) );
                    //          
    // set new values
    PORT->Group[port].PMUX[pin/2].reg |=  ( (uint8_t)( (pinmux&0xFFFF) <<(4*(pin&1)) ) ); 
}

void UART_sercom_init()
{
	//port muxer config
	PORT->Group[1].PINCFG[PINMUX_PA22C_SERCOM1_PAD0].bit.PMUXEN = 1;
	PORT->Group[1].PINCFG[PINMUX_PA25C_SERCOM1_PAD3].bit.PMUXEN = 1;
	
	//Pmux eve = n/1, odd = (n-1)/2
	//PORT->Group[1].PMUX[PINMUX_PA22C_SERCOM1_PAD0 >> 1].reg = 0x23;
	pin_set_peripheral_function(PINMUX_PA25C_SERCOM1_PAD3); // SAMD09 TX
	pin_set_peripheral_function(PINMUX_PA24C_SERCOM1_PAD2); // SAMD09 RX
	
	//apbcmak
	PM->APBCMASK.reg |= PM_APBCMASK_SERCOM1;
	
	
	//gclk config
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(SERCOM1_GCLK_ID_CORE) | GCLK_CLKCTRL_GEN(0) | GCLK_CLKCTRL_CLKEN;
	
	//Config SERCOM1 module for UART
	SERCOM1->USART.CTRLA.reg = SERCOM_USART_CTRLA_MODE_USART_INT_CLK | SERCOM_USART_CTRLA_DORD | SERCOM_USART_CTRLA_RXPO(0x3) | SERCOM_USART_CTRLA_TXPO(0x1);
	
	SERCOM1->USART.CTRLB.reg = SERCOM_USART_CTRLB_RXEN | SERCOM_USART_CTRLB_TXEN | SERCOM_USART_CTRLB_CHSIZE(0);
	
	SERCOM1->USART.BAUD.reg = 65535.0f * (1.0f - (float)(16*(float)(9600)/float(USART_BAUD_MODIFIER_SLOW))); //This gets the miniSam exactly at 9800 baud.
	/* for 115200 baud compiler does not like this.*/
	//SERCOM1->USART.BAUD.reg = 65535.0f * (1.0f - (float)(16*(float)(USART_BAUD_MODIFIER_FAST)/(8000000)));
	
	SERCOM1->USART.CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;
	
}

void SERCOM1_Handler()  // SERCOM1 ISR
{
    uint8_t buffer;
    buffer  = SERCOM1->USART.DATA.reg;
    while(!(SERCOM1->USART.INTFLAG.reg & 1)); // wait UART module ready to receive data
    SERCOM1->USART.DATA.reg = buffer;               // just sent that byte aback
    while(!(SERCOM1->USART.INTFLAG.reg & 2)); // wait until TX complete;
}


void UART_sercom_simpleWrite(Sercom *const sercom_module, uint8_t data)
{
    while(!(sercom_module->USART.INTFLAG.reg & 1)); //wait UART module ready to receive data
    sercom_module->USART.DATA.reg = data;
    while(!(sercom_module->USART.INTFLAG.reg & 2)); //wait until TX complete;
}

int main(void)
{
    /* Initialize the SAM system */
    SystemInit();
	init_TC1();
	REG_PORT_DIR0 |= (1 << 14);
	
	UART_sercom_init();
	
	UART_sercom_simpleWrite(SERCOM1, 'H');
	UART_sercom_simpleWrite(SERCOM1, 'e');
	UART_sercom_simpleWrite(SERCOM1, 'l');
	UART_sercom_simpleWrite(SERCOM1, 'l');
	UART_sercom_simpleWrite(SERCOM1, 'o');
	UART_sercom_simpleWrite(SERCOM1, ' ');
	UART_sercom_simpleWrite(SERCOM1, 'W');
	UART_sercom_simpleWrite(SERCOM1, 'o');
	UART_sercom_simpleWrite(SERCOM1, 'r');
	UART_sercom_simpleWrite(SERCOM1, 'l');
	UART_sercom_simpleWrite(SERCOM1, 'd');
	UART_sercom_simpleWrite(SERCOM1, '!');
	
    /* Replace with your application code */
    while (1) 
    {
		/*
		REG_PORT_OUT0 |= (1 << 14);
		_delay_ms()
		REG_PORT_OUT0 &= ~(1 << 14);
		_delay_ms(500);
		*/
    }
}
