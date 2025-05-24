/*
 *usart.c
 *Project:			SAMD09/D10 Peripheral Libraries
 *Author:			Weistek Engineering (jeremy G.)
 *website:			www.weistekengineering.com
 *date:				07-11-2016
 *Summary:			Provides USART communication functions for SERCOM1 on SAMD09/D10,
 *					including byte read/write and string transmission.
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
void send_string(volatile char s[])
{
	uint32_t i;
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
    if(specialTalk) // Only apply special logic if specialTalk is true
    {
        if(data == '\n')
        {
            data = ('*');
        }
    }
#endif
    BOOT_SERCOM->USART.DATA.reg = (uint16_t)data; // Single write point
	
}

// Define USART error flags mask for clearing if not available from headers
#ifndef SERCOM_USART_STATUS_ERROR_MASK
#define SERCOM_USART_STATUS_ERROR_MASK (SERCOM_USART_STATUS_PERR | SERCOM_USART_STATUS_FERR | SERCOM_USART_STATUS_BUFOVF)
#endif

//read from the usart register.
int uart_read_byte(void)
{
	uint8_t status_reg;
	uint16_t data_reg;

	// Wait for data to be received
	while(!BOOT_SERCOM->USART.INTFLAG.bit.RXC) {
		// Check for errors while waiting for RXC. 
		// Some errors (like BUFOVF) might prevent RXC from ever setting if not handled.
		if(BOOT_SERCOM->USART.INTFLAG.bit.ERROR) {
			status_reg = BOOT_SERCOM->USART.STATUS.reg;
			// Clear error flags by writing them to 1
			BOOT_SERCOM->USART.STATUS.reg = status_reg & SERCOM_USART_STATUS_ERROR_MASK; 
			// Optionally, clear the ERROR flag in INTFLAG by writing 1 to it (if applicable for hardware)
			// BOOT_SERCOM->USART.INTFLAG.reg = SERCOM_USART_INTFLAG_ERROR; 
			return -1; // Error occurred
		}
	}

	// Check for errors after RXC is set
	if(BOOT_SERCOM->USART.INTFLAG.bit.ERROR) {
		status_reg = BOOT_SERCOM->USART.STATUS.reg;
		// Clear error flags by writing them to 1
		BOOT_SERCOM->USART.STATUS.reg = status_reg & SERCOM_USART_STATUS_ERROR_MASK;
		// BOOT_SERCOM->USART.INTFLAG.reg = SERCOM_USART_INTFLAG_ERROR; // Clear error interrupt flag
		
		// Even if there was an error, try to read data to clear RXC, but still return error
		// (though data might be corrupt)
		data_reg = BOOT_SERCOM->USART.DATA.reg; 
		return -1; // Error occurred
	}

	// No error, read data
	data_reg = BOOT_SERCOM->USART.DATA.reg;
	return (int)(data_reg & 0x00FF); // Return data byte (0-255)
}

/*init USART module on SERCOM1*/
void UART_sercom_init()
{
	//Pmux eve = n/1, odd = (n-1)/2
	#if defined(SAMD10)
		pin_set_peripheral_function(PINMUX_PA30C_SERCOM1_PAD0);
		pin_set_peripheral_function(PINMUX_PA31C_SERCOM1_PAD1);
	#else
		pin_set_peripheral_function(PINMUX_PA25C_SERCOM1_PAD3); // SAMD09 TX
		pin_set_peripheral_function(PINMUX_PA24C_SERCOM1_PAD2); // SAMD09 RX
	#endif
	
	//apbcmak
	
	
	//gclk config
	
	//Config SERCOM1 module for UART
	SERCOM1->USART.CTRLA.reg = SERCOM_USART_CTRLA_MODE_USART_INT_CLK | SERCOM_USART_CTRLA_DORD | SERCOM_USART_CTRLA_RXPO(0x3) | SERCOM_USART_CTRLA_TXPO(0x1);
	
	SERCOM1->USART.CTRLB.reg = SERCOM_USART_CTRLB_RXEN | SERCOM_USART_CTRLB_TXEN | SERCOM_USART_CTRLB_CHSIZE(0);
	
	/*configure baud rate at 115200*/
	SERCOM1->USART.BAUD.reg = BAUD_VAL;
	
	SERCOM1->USART.CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;
	
}

