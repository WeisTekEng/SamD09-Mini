/*
 * i2c.h
 *
 * Created: 7/16/2016 2:14:38 PM
 *  Author: ocybr
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
uint8_t i2c_read(Sercom *sercom, uint8_t *data, int size, uint8_t address);
uint8_t startTransmissionWire(uint8_t address, SercomWireReadWriteFlag flag);
uint8_t isBusIdleWIRE( void );
uint8_t isBusOwnerWIRE( void );


#endif /* I2C_H_ */