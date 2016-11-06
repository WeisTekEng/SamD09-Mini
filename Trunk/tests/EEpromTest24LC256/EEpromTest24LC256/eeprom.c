/*
 * eeprom.c
 *
 * Created: 8/9/2016 5:59:15 PM
 *  Author: ocybr
 */ 

/*
24lc256 et el use 8 byte words to store data,
the address range is 0x0000 to 0x7FFFF
the address of the onboard eeprom is 
0x52

1010(A2,A1,A0) = 1010 010 = 52 = 0xA2
address to write 0xA2
address to read 0xA3
*/

#include "includes.h"
#include "eeprom.h"

uint16_t ua, ub;
int16_t address_high, address_low = 0x00;

void eeprom_init(int16_t address)
{
	i2c_init(address);
}

/*write data*/
void eeprom_write_word(uint16_t data,uint32_t address)
{
	split_address(address);
	
	/*this is how you write 16 bytes [a word] to the eeprom.*/
	/*this has to be repeated for each 16bit word, or you need*/
	/*to do a page write.. working on it.*/
	i2c_write_start();
	i2c_write_byte(address_high); //data address high byte.
	i2c_write_byte(address_low); //data address low byte.
	i2c_write_byte(data); //test data.
	i2c_write_stop();
	
	/*bash the eeprom until it no longer sends a nack*/
	while(!0==(i2c_busy()));
}

void eeprom_write_byte(uint8_t data)
{
	
}

void eeprom_page_write(uint8_t *data,uint32_t address)
{
	split_address(address);
	
	i2c_write_start();
	i2c_write_byte(address_high);
	i2c_write_byte(address_low);
	for(int x = 0;x<=sizeof(data);x++)
	{
		i2c_write_byte(data[x]);
	}
	i2c_write_stop();
	while(!0==(i2c_busy()));
	
}

/*read data*/
void eeprom_read_word(uint16_t *data,uint32_t address)
{
	split_address(address);
		
		/*read the contents of address 0x0000 [1x16byte word]*/
		//send a stop bit to clear the line..
		i2c_write_stop();
		i2c_write_start(); //send start bit + address + w bit
		i2c_write_byte(address_high); //data address high byte
		i2c_write_byte(address_low); //data address low byte
		//need to make a dedicated i2c_read_start() function.
		i2c_read_start();
		i2c_read(data,1);
}

void eeprom_read_byte(uint8_t *data)
{
	
}

uint8_t eeprom_page_read(/*uint8_t *data,uint32_t address*/)
{
	//split_address(address);
	
	i2c_write_stop();
	i2c_write_start();
	i2c_write_byte(address_high);
	i2c_write_byte(address_low);
	i2c_read_start();
	return i2c_read_byte();
	//i2c_read(data,32);
	
}

void split_address(uint32_t address)
{
	/*clear the address range*/
	address_high = 0x00;
	address_low = 0x00;
	
	//Split
	ua = (uint16_t) (address >> 16);
	ub = (uint16_t) (address & 0x0000FFFFuL);
	address_high = (int16_t)ua;
	address_low = (int16_t)ub;
}