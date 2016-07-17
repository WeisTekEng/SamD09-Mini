/*
 * SPI_TEST.c
 *
 * Created: 7/16/2016 9:58:48 PM
 * Author : ocybr
 */ 


#include "sam.h"
#include "spi.h"
#include "globalfunctions.h"

uint8_t data;
uint8_t temp;


int main(void)
{
    /* Initialize the SAM system */
    SystemInit();
	
	REG_PORT_DIR0 |= (1 << 5); //set /CS as output
	REG_PORT_OUT0 |= (1 << 5); //pull /CS HIGH.
	
	pin_set_peripheral_function(PINMUX_PA06C_SERCOM0_PAD0); //MISO
	pin_set_peripheral_function(PINMUX_PA04C_SERCOM0_PAD2); //MOSI
	pin_set_peripheral_function(PINMUX_PA07C_SERCOM0_PAD1); //SCLK
	
	SPI_setup(0);
	
	SPI_write_wait(0x90);
	//SPI_read_RXBuffer(data);
	data = SPI_read_ID(0x90,0x0);
	
	
    /* Replace with your application code */
    while (1) 
    {
    }
}
