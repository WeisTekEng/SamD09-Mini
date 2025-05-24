/*
 *i2c.c
 *Project:			SAMD09/D10 Peripheral Libraries
 *Author:			Weistek Engineering (jeremy G.)
 *website:			www.weistekengineering.com
 *date:				06-29-2016
 *Created:			7/16/2016 2:14:26 PM
 *Summary:			Header file for `i2c.c`. Declares I2C master mode functions for SAMD09,
 *					including direct register access and Wire-like transmission helpers.
 *					Some advanced features or error recovery scenarios might require further testing.
 *
 *License:	        GNU GENERAL PUBLIC LICENSE Version 3 or newer. The header of this file may not change.
 *
 *					unless new features are added then the update section may be updated. The License file in
 *					the root of this repo Trunk/LICENSE should have been provided to you. If it was not you may
 *					find a copy of the GNU Open source license at https://www.gnu.org/licenses/gpl.html
 *
 */ 


#ifndef I2C_H_
#define I2C_H_

typedef enum
{
	I2C_MASTER_OPERATION = 0x5u
}SercomI2CMode;

typedef enum
{
	WIRE_WRITE_FLAG = 0x0ul,
	WIRE_READ_FLAG
} SercomWireReadWriteFlag;

typedef enum
{
	WIRE_UNKNOWN_STATE = 0x0ul,
	WIRE_IDLE_STATE,
	WIRE_OWNER_STATE,
	WIRE_BUSY_STATE
} SercomWireBusState;

// TWI clock frequency
static const uint32_t TWI_CLOCK = 100000;

/*used to setup i2c*/
void i2c_setup(Sercom *sercom);
bool i2c_read_byte(Sercom *sercom, uint8_t device_address, uint8_t locationH, uint8_t locationL, uint8_t *data_byte);
bool i2c_write_byte(Sercom *sercom, uint8_t device_address, uint8_t locationH, uint8_t locationL, uint8_t data_byte);
uint8_t i2c_startTransmissionWire(uint8_t device_address, SercomWireReadWriteFlag flag);
void i2c_endTransmition(Sercom *sercom);
uint8_t isBusIdleWIRE( void );
uint8_t isBusOwnerWIRE( void );


#endif /* I2C_H_ */