/*
 * main.c
 *Project:	miniSam USART bootloader.
 *Author:	Weistek Engineering (jeremy G.)
 *website:	www.weistekengineering.com
 *date:		06-29-2016f
 *Summery:	If PA15 bootpin is held low, micro will enter USART bootloader mode.
 *		if PA15 is high, micro runs user program if there is one at new start
 *		memor. Look at APP_START for start location of user flash.
 *
 *Update:	fixed write_nvm function, would fall to dummy handler.
 *
 *Todo:		need to fix Verify flash function, flash contents don't match.
 */ 


#include "sam.h"

#define PORTA 0

/* Application starts from 1kB memory - Bootloader size is 1kB */
/* Change the address if higher boot size is needed */
#define APP_START	0x00000500 //This gives 1280 bytes of bootloader space.

/* Target application size can be 15kB */
/* APP_SIZE is the application section size in kB */
/* Change as per APP_START */
#define APP_SIZE	13	//This is how much flash memory is left for the application.

/* Flash page size is 64 bytes */
#define PAGE_SIZE	64	//used to read and write to flash.
/* Memory pointer for flash memory */
#define NVM_MEMORY        ((volatile uint16_t *)FLASH_ADDR)

/* Change the following if different SERCOM and boot pins are used */
#define BOOT_SERCOM			SERCOM1		//miniSam uses Sercom1 for USART
#define BOOT_SERCOM_BAUD	115200
#define BOOT_PORT			PORTA
#define BOOT_PIN			15 //14		//PA15 for bootloader en, toggled by the python script. or DTR from serial coms.

uint32_t USART_BAUD_MODIFIER_SLOW = 1048553;
#define MY_BAUDREG_VALUE 64278    // 64278 (F_CPU=F_SERCOM=8MHz) -> 9600 BAUD
//#define MY_BAUDREG_VALUE 55470      // 55470 (F_CPU=F_SERCOM=1MHz) -> 9600 BAUD
#define div_ceil(a,b)(((a)+(b)-1)/(b))			//extracted function from samd_math.h <- something like that.

/* SERCOM USART GCLK Frequency */
#define SERCOM_GCLK		8000000UL		//processor speed.
#define BAUD_VAL	(65536.0*(1.0-((float)(16.0*(float)BOOT_SERCOM_BAUD)/(float)SERCOM_GCLK))) //calculate baud rate from SERCOM_GCLK

uint8_t data_8 = 1;
uint32_t file_size, i, dest_addr, app_start_address;
uint8_t page_buffer[PAGE_SIZE];
uint32_t *flash_ptr;

//Version information.
uint8_t aVER[31] = {'m','i','n','i','S','a','m','d',' ','R','1','.','2',
					' ','s','e','r','i','a','l',' ','b','o','o','t',
					'l','o','a','d','e','r'};

enum uart_pad_settings {
	UART_RX_PAD0_TX_PAD2 = SERCOM_USART_CTRLA_RXPO(0) | SERCOM_USART_CTRLA_TXPO(1),
	UART_RX_PAD1_TX_PAD2 = SERCOM_USART_CTRLA_RXPO(1) | SERCOM_USART_CTRLA_TXPO(1),
	UART_RX_PAD2_TX_PAD0 = SERCOM_USART_CTRLA_RXPO(2),
	UART_RX_PAD3_TX_PAD0 = SERCOM_USART_CTRLA_RXPO(3),
	UART_RX_PAD1_TX_PAD0 = SERCOM_USART_CTRLA_RXPO(1),
	UART_RX_PAD3_TX_PAD2 = SERCOM_USART_CTRLA_RXPO(3) | SERCOM_USART_CTRLA_TXPO(1),
};

/*
void uart_init(uint16_t baud_val, enum uart_pad_settings pad_conf)
{
	/* Enable & configure alternate function D for pins PA10 & PA11 */
	/* Change following 3 lines if different SERCOM/SERCOM pins are used *//*
	PORT->Group[0].WRCONFIG.reg = 0x53010C00;
	PM->APBCMASK.reg |= (1u << 4);
	GCLK->CLKCTRL.reg = 0x4010;
	
	BOOT_SERCOM->USART.CTRLA.reg = pad_conf | SERCOM_USART_CTRLA_MODE(1) | SERCOM_USART_CTRLA_DORD;
	BOOT_SERCOM->USART.CTRLB.reg = SERCOM_USART_CTRLB_RXEN | SERCOM_USART_CTRLB_TXEN | SERCOM_USART_CTRLB_CHSIZE(0);
	while(BOOT_SERCOM->USART.SYNCBUSY.bit.CTRLB);
	BOOT_SERCOM->USART.BAUD.reg = baud_val;
	BOOT_SERCOM->USART.CTRLA.bit.ENABLE = 1;
	while(BOOT_SERCOM->USART.SYNCBUSY.bit.ENABLE);
}
*/
	
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
	PORT->Group[1].PINCFG[PINMUX_PA24C_SERCOM1_PAD2].bit.PMUXEN = 1;
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
	
	SERCOM1->USART.BAUD.reg = BAUD_VAL;//65535.0f * (1.0f - (float)(16*(float)(9600)/(USART_BAUD_MODIFIER_SLOW))); //This gets the miniSam exactly at 9800 baud.
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

void uart_write_byte(uint8_t data)
{
	while(!BOOT_SERCOM->USART.INTFLAG.bit.DRE);
	BOOT_SERCOM->USART.DATA.reg = (uint16_t)data;
	
}

void UART_sercom_simpleWrite(Sercom *const sercom_module, uint8_t data)
{
	while(!(sercom_module->USART.INTFLAG.reg & 1)); //wait UART module ready to receive data
	sercom_module->USART.DATA.reg = data;
	while(!(sercom_module->USART.INTFLAG.reg & 2)); //wait until TX complete;
}

uint8_t uart_read_byte(void)
{
	while(!BOOT_SERCOM->USART.INTFLAG.bit.RXC);
	return((uint8_t)(BOOT_SERCOM->USART.DATA.reg & 0x00FF));
}

void nvm_erase_row(const uint32_t row_address)
{
	/* Check if the module is busy */
	while(!NVMCTRL->INTFLAG.bit.READY);
	/* Clear error flags */
	NVMCTRL->STATUS.reg &= ~NVMCTRL_STATUS_MASK;
	/* Set address and command */
	NVMCTRL->ADDR.reg  = (uintptr_t)&NVM_MEMORY[row_address / 2];
	NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMD_ER | NVMCTRL_CTRLA_CMDEX_KEY;
	while(!NVMCTRL->INTFLAG.bit.READY);
}

void nvm_write_buffer(const uint32_t destination_address, const uint8_t *buffer, uint16_t length)
{

	/* Check if the module is busy */
	while(!NVMCTRL->INTFLAG.bit.READY);

	/* Erase the page buffer before buffering new data */
	NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMD_PBC | NVMCTRL_CTRLA_CMDEX_KEY;

	/* Check if the module is busy */
	while(!NVMCTRL->INTFLAG.bit.READY);

	/* Clear error flags */
	NVMCTRL->STATUS.reg &= ~NVMCTRL_STATUS_MASK;

	uint32_t nvm_address = destination_address / 2;

	/* NVM _must_ be accessed as a series of 16-bit words, perform manual copy
	 * to ensure alignment */
	for (uint16_t k = 0; k < length; k += 2) 
	{
		uint16_t data;
		/* Copy first byte of the 16-bit chunk to the temporary buffer */
		data = buffer[k];
		/* If we are not at the end of a write request with an odd byte count,
		 * store the next byte of data as well */
		if (k < (length - 1)) {
			data |= (buffer[k + 1] << 8);
		}
		/* Store next 16-bit chunk to the NVM memory space */
		NVM_MEMORY[nvm_address++] = data;
	}
	while(!NVMCTRL->INTFLAG.bit.READY);
}

int main(void)
{ 
	/* Check if boot pin is held low - Jump to application if boot pin is high */
	//PORT->Group[BOOT_PORT].OUTSET.reg = (1u << BOOT_PIN);

	PORT->Group[BOOT_PORT].PINCFG[BOOT_PIN].reg = PORT_PINCFG_INEN | PORT_PINCFG_PULLEN;
	if ((PORT->Group[BOOT_PORT].IN.reg & (1u << BOOT_PIN)))
	{
		app_start_address = *(uint32_t *)(APP_START + 4);
		/* Rebase the Stack Pointer */
		__set_MSP(*(uint32_t *) APP_START);

		/* Rebase the vector table base address */
		SCB->VTOR = ((uint32_t) APP_START & SCB_VTOR_TBLOFF_Msk);

		/* Jump to application Reset Handler in the application */
		asm("bx %0"::"r"(app_start_address));
	}
	
	/* Make CPU to run at 8MHz by clearing prescalar bits */ 
    SYSCTRL->OSC8M.bit.PRESC = 0;
	NVMCTRL->CTRLB.bit.CACHEDIS = 1;
	/* Change pad_conf argument if different pad settings is used */
	//uart_init(BAUD_VAL, UART_RX_PAD3_TX_PAD2);
	UART_sercom_init();
	info();
	//uart_init(64278, UART_RX_PAD3_TX_PAD2);
	//UART_sercom_simpleWrite(SERCOM1,'*');
    while (1) 
    {
        data_8 = uart_read_byte();
		if (data_8 == '#')
		{
			uart_write_byte('s');
			uart_write_byte((uint8_t)APP_SIZE);
		}
		else if (data_8 == 'e')
		{
			/*this has been fixed, it no longer fails to 
			a dummy handler*/
			for(i = APP_START; i < FLASH_SIZE; i = i + 256)
			{
				nvm_erase_row(i);
			}
			dest_addr = APP_START;
			flash_ptr = APP_START;
			uart_write_byte('s');
		}
		else if (data_8 == 'p')
		{
			uart_write_byte('s');
			for (i = 0; i < PAGE_SIZE; i++)
			{
				page_buffer[i] = uart_read_byte();
			}
			nvm_write_buffer(dest_addr, page_buffer, PAGE_SIZE);
			dest_addr += PAGE_SIZE;
			uart_write_byte('s');
		}
		else if (data_8 == 'v')
		{
			/* now we get stuck here... varifing pages fails on the first page.
			don't know why.*/
			uart_write_byte('s');
			for (i = 0; i < (PAGE_SIZE); i++)
			{
				app_start_address = *flash_ptr;
				//uart_write_byte((uint8_t)app_start_address);
				UART_sercom_simpleWrite(SERCOM1,(uint8_t)(app_start_address >> 8));
				//uart_write_byte((uint8_t)(app_start_address >> 8));
				UART_sercom_simpleWrite(SERCOM1,(uint8_t)(app_start_address >> 16));
				//uart_write_byte((uint8_t)(app_start_address >> 16));
				UART_sercom_simpleWrite(SERCOM1,(uint8_t)(app_start_address >> 24));
				//uart_write_byte((uint8_t)(app_start_address >> 24));
				flash_ptr++;
			}
		}
    }
}

void info()
{
	uint8_t i;
	
	for(i = 0;i<=31;i++)
	{
		UART_sercom_simpleWrite(SERCOM1,aVER[i]);	
	}
}
