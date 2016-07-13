/*
 *usart.c
 *Project:			miniSam USART bootloader.
 *Author:			Weistek Engineering (jeremy G.)
 *website:			www.weistekengineering.com
 *date:				07-11-2016
 *Summery:			setup usart functions, read and write functions.
 *
 *updated:			07/12/2016 Added send_string function.
 */ 

#include "includes.h"

/*send confirmation of command received*/
void sendConfirm()
{
	uart_write_byte('s');
}

/*send a char array and format as a string*/
void send_string(char s[],uint32_t i)
{
	while(s[i] != 0x00)
	{
		uart_write_byte(s[i]);
		i++;
	}
}

//write to the usart register.
void uart_write_byte(uint8_t data)
{
	while(!BOOT_SERCOM->USART.INTFLAG.bit.DRE);
	
	#ifdef NEW_FRONT_END
	if(!specialTalk)
	{
		BOOT_SERCOM->USART.DATA.reg = (uint16_t)data;
	}
	else
	{
		if(data == '\n')
		{
			data = ('*');
		}
		
		BOOT_SERCOM->USART.DATA.reg = (uint16_t)data;
	}
	#endif
	BOOT_SERCOM->USART.DATA.reg = (uint16_t)data;
	
}

//read from the usart register.
uint8_t uart_read_byte(void)
{
	while(!BOOT_SERCOM->USART.INTFLAG.bit.RXC);
	return((uint8_t)(BOOT_SERCOM->USART.DATA.reg & 0x00FF));
}

/*pin pad setup for SERCOM1 and USART*/
void pin_set_peripheral_function(uint32_t pinmux)
{
    /* the variable pinmux consist of two components:
        31:16 is a pad, witch includes:
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

/*init USART module on SERCOM1*/
void UART_sercom_init()
{
	//Pmux eve = n/1, odd = (n-1)/2
	pin_set_peripheral_function(PINMUX_PA25C_SERCOM1_PAD3); // SAMD09 TX
	pin_set_peripheral_function(PINMUX_PA24C_SERCOM1_PAD2); // SAMD09 RX
	
	//apbcmak
	PM->APBCMASK.reg |= PM_APBCMASK_SERCOM1;
	
	//gclk config
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(SERCOM1_GCLK_ID_CORE) | GCLK_CLKCTRL_GEN(0) | GCLK_CLKCTRL_CLKEN;
	
	//Config SERCOM1 module for UART
	SERCOM1->USART.CTRLA.reg = SERCOM_USART_CTRLA_MODE_USART_INT_CLK | SERCOM_USART_CTRLA_DORD | SERCOM_USART_CTRLA_RXPO(0x3) | SERCOM_USART_CTRLA_TXPO(0x1);
	
	SERCOM1->USART.CTRLB.reg = SERCOM_USART_CTRLB_RXEN | SERCOM_USART_CTRLB_TXEN | SERCOM_USART_CTRLB_CHSIZE(0);
	
	/*configure baud rate at 115200*/
	SERCOM1->USART.BAUD.reg = BAUD_VAL;
	
	SERCOM1->USART.CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;
	
}

