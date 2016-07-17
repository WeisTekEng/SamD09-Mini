/*
 * eeprom.h
 *
 * Created: 7/14/2016 6:38:39 PM
 *  Author: Jeremy
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_

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


//void i2c_setup();

I2C_sercom_init();

bool startTransmissionWire(uint8_t address, SercomWireReadWriteFlag flag);

uint8_t isBusIdleWIRE( void );

uint8_t isBusOwnerWIRE( void );

void resetWIRE();

void i2c_send_string(char s[],uint32_t i);

void i2c_write_byte(uint8_t data);

uint8_t i2c_read_byte(void);

// TWI clock frequency
static const uint32_t TWI_CLOCK = 100000;

#endif /* EEPROM_H_ */