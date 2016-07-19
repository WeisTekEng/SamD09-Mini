/*
 *i2c.c
 *Project:			Generic clock library for miniSam-Zero.
 *Author:			Weistek Engineering (jeremy G.)
 *website:			www.weistekengineering.com
 *date:				06-29-2016
 *Created:			7/16/2016 2:14:26 PM
 *Summery:			header file for i2c for samd09 without asf drivers, files are still a work in progress.
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
uint8_t i2c_read_byte(Sercom *sercom, uint8_t *data,uint8_t address,uint8_t locationH,uint8_t locationL);
void i2c_write_byte(Sercom *sercom, uint8_t data, int size,uint8_t address, uint8_t locationH,uint8_t locationL);
uint8_t i2c_startTransmissionWire(uint8_t address, SercomWireReadWriteFlag flag);
void i2c_endTransmition(Sercom *sercom);
uint8_t isBusIdleWIRE( void );
uint8_t isBusOwnerWIRE( void );


#endif /* I2C_H_ */