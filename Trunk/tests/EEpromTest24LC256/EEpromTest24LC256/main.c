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

char aHello[] = {"Hello World\n"};
uint8_t data[] = {};

/*device address*/
#define DEVICE_ADDRESS		0x50

/*test data 2 bytes each*/
#define TEST_DATA_DE		0xDE
#define TEST_DATA_AD		0xAD
#define TEST_DATA_BE		0xBE
#define TEST_DATA_EF		0xEF

/*device flags, will move later. also included in define.h*/
#define DEVICE_READ			0x00
#define DEVICE_WRITE		0x01

int main(void)
{
    /* Initialize the SAM system */
    SystemInit();
	/*system clocks setup.*/
	init_clocks();
	
	/*init USART*/
	UART_sercom_init();
	
	/*init I2C*/
	i2c_setup(SERCOM0); /*this works*/

	volatile uint8_t stuff;
	/*read and write some stuff to eeprom. look at data with logic analyzer to see if its
	*sending properly*/
	/*initiate transmition*/
	i2c_startTransmissionWire(DEVICE_ADDRESS,DEVICE_WRITE);
	
	/*write data, this kinda works. needs to be completely rewritin*/
	i2c_write_byte(SERCOM0,TEST_DATA_DE,sizeof(TEST_DATA_DE),DEVICE_ADDRESS,0x0000,0x0000);
	/*read data, same as above. these both at least put data on the line and receive acks*/
	i2c_read_byte(SERCOM0,data,DEVICE_ADDRESS,0x0000,0x0000);
	/*done transmitting*/
	i2c_endTransmition(SERCOM0);
	
	/*lets us know USART is still working*/
	send_string(aHello);
	
	/*infini loop*/
    while (1) 
    {
    }
}
