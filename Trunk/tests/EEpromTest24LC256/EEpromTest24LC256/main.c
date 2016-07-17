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
#include <inttypes.h>
#include <string.h>

uint32_t i;

char aHello[] = {"Hello World\n"};
uint8_t data[] = {};
uint8_t Size = 8;
uint8_t found = 0;
uint32_t addr;

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
	//UART_sercom_init();
	
	/*init I2C*/
	i2c_setup(SERCOM0); /*this works*/
	
	/*search for device. eeprom should be at 0x50*/
	for(uint32_t y = 0;y<= 0x65;y++)
	{
		uint8_t stuff = startTransmissionWire(y,0x0ul);
		if(stuff !=0)
		{
			found = 1;
			addr = y;
		} 
	}
	
	startTransmissionWire(0x50,0x1ul);
	
	volatile uint8_t stuff;
	stuff = i2c_read(SERCOM0,data,8,&addr); //this at least gives some data. 0xff
	stuff = i2c_read(SERCOM0,data,8,&addr);
	//send_string(aHello,i);
    while (1) 
    {
    }
}
