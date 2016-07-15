/*
 *EEpromTest24LC256.c
 *Project:			EEprom tests..
 *Author:			Weistek Engineering (jeremy G.)
 *website:			www.weistekengineering.com
 *date:				06-29-2016
 *Summery:			
 *
 *License:	        GNU GENERAL PUBLIC LICENSE Version 3 or newer. The header of this file may not change.
 *
 *					unless new features are added then the update section may be updated. The License file in
 *					the root of this repo Trunk/LICENSE should have been provided to you. If it was not you may
 *					find a copy of the GNU Open source license at https://www.gnu.org/licenses/gpl.html
 */ 


#include "includes.h"

uint32_t i;

char aHello[] = {"Hello World\n"};

int main(void)
{
    /* Initialize the SAM system */
    SystemInit();
	
	/* Make CPU to run at 8MHz by clearing prescalar bits */
	SYSCTRL->OSC8M.bit.PRESC = 0;
	NVMCTRL->CTRLB.bit.CACHEDIS = 1;
	
	//PORT->Group[BOOT_PORT].OUTSET.reg = (1>>14);
	//PORT->Group[BOOT_PORT].OUTSET.reg = (1>>15);  
	
	/*setup pinmux for i2c*/
	pin_set_peripheral_function(SDA);
	pin_set_peripheral_function(SCL);
	
	/*init USART*/
	UART_sercom_init();
	/*init I2C*/
	I2C_sercom_init();
	enableWire();
	
	send_string(aHello,i);
	startTransmissionWire(0x50,WIRE_WRITE_FLAG);
    /* Replace with your application code */
    while (1) 
    {
    }
}
