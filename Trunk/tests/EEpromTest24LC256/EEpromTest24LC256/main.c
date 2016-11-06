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
#include "eeprom.h"

char aHello[] = {"Hello World\n"};
uint8_t page_test[] = {0xEF,0xEB,0xDA,0xAD,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,
						0x0A,0x0B,0x0C,0x0D,0x0E,0x0F};
uint8_t data[] = {};

/*device address*/
#define DEVICE_ADDRESS		0x52

/*test data 2 bytes each*/
#define TEST_DATA_DE		0xDE
#define TEST_DATA_AD		0xAD
#define TEST_DATA_BE		0xBE
#define TEST_DATA_EF		0xEF

int main(void)
{
    /* Initialize the SAM system */
    SystemInit();
	/*system clocks setup.*/
	init_clocks();
	
	/*init USART*/
	UART_sercom_init();
	
	/*initiate transmition*/
	i2c_init(DEVICE_ADDRESS);

	/*this section works, word write, writes 2x8byte words or 1 16byte word*/
	eeprom_write_word(0xBC,0x0000);
	eeprom_read_word(data,0x0000);
	
	uart_write_byte(data[0]);
	send_string("\r\n");
	/*end*/
	
	/*page write block*/
	send_string("EEProm page write data: \r\n");
	for(int x = 0;x<=sizeof(page_test);x++)
	{
		uart_write_byte(page_test[x]);
	}
	eeprom_page_write(page_test,0x0000);
	
	/*end*/
	
	/*read page and send to uart*/
	uint8_t temp[] = {};
	send_string("EEPProm Page data\n");
	
	uint8_t count = 0;
	for(int x = 0;x<=sizeof(page_test);x++)
	{
		//eeprom_page_read(data,0x0000);
		uart_write_byte(i2c_read_byte()>>count);
		//send_string(data[x]);
		count = count + 1;
	}
	
    while (1) 
    {
    }
}
