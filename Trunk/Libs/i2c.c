/*
 *i2c.c
 *Project:			SAMD09/D10 Peripheral Libraries
 *Author:			Weistek Engineering (jeremy G.)
 *website:			www.weistekengineering.com
 *date:				06-29-2016
 *Created:			7/16/2016 2:14:26 PM
 *Summary:			Provides I2C master mode communication functions for SERCOM0 on SAMD09 microcontrollers,
 *					without using ASF drivers. Includes core functions for direct register access
 *					(`i2c_read_byte`, `i2c_write_byte`) and a set of Arduino Wire-library inspired helper functions.
 *
 *License:	        GNU GENERAL PUBLIC LICENSE Version 3 or newer. The header of this file may not change.
 *
 *					unless new features are added then the update section may be updated. The License file in
 *					the root of this repo Trunk/LICENSE should have been provided to you. If it was not you may
 *					find a copy of the GNU Open source license at https://www.gnu.org/licenses/gpl.html
 *
 *Basic Mechs:		page = 32 bytes. m = memory(i2c eeprom), MSTR(samd09), S=(start cmd), P=(stop cmd)
 *					[S addr w] ack(m) [8bitsHigh] ack(m) [8bitsLow] ack(m) {8bits,ack(m)}*n P(MSTR)
 *					[S addr r] ack(m) [8bitsHigh] ack(m) [8bitsLow] ack(m) {8bits,ack(MSTR)}*n P(MSTR) 	
 */

#include "includes.h"

void i2c_setup(Sercom *sercom)
{	
	/*setup pinmux for i2c*/
	pin_set_peripheral_function(SDA);
	pin_set_peripheral_function(SCL);
	
	/*reset i2c module*/
	sercom->I2CM.CTRLA.reg |= SERCOM_I2CM_CTRLA_SWRST;
	
	/*wait for soft reset to finish*/
	while(sercom->I2CM.SYNCBUSY.bit.SWRST);
	
    // Synchronous arithmetic baudrate
	//SERCOM0->I2CM.BAUD.bit.BAUD = SystemCoreClock / ( 2 * TWI_CLOCK) - 1 ;
	// Comment regarding I2C_BAUD_VALUE_CUSTOM_160 is in defines.h
	sercom->I2CM.BAUD.reg = SERCOM_I2CM_BAUD_BAUD(I2C_BAUD_VALUE_CUSTOM_160);
	while (sercom->I2CM.SYNCBUSY.reg);
	
	/*setup module*/
	SERCOM0->I2CM.CTRLA.reg = SERCOM_I2CM_CTRLA_ENABLE | /* enable module */
	SERCOM_I2CM_CTRLA_MODE_I2C_MASTER |    /* i2c master mode */
	SERCOM_I2CM_CTRLA_SDAHOLD(SERCOM_I2CM_SDAHOLD_VALUE_3);        /* SDA hold time to ~500-600ns */
	while (sercom->I2CM.SYNCBUSY.bit.ENABLE);
	
	sercom->I2CM.STATUS.reg |= SERCOM_I2CM_STATUS_BUSSTATE(WIRE_IDLE_STATE);
	while(sercom->I2CM.SYNCBUSY.reg);
	
}

void i2c_endTransmition(Sercom *sercom)
{
	sercom->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(2); // Send STOP condition
	while(sercom->I2CM.SYNCBUSY.reg); // Wait for synchronization
}

bool i2c_read_byte(Sercom *sercom, uint8_t device_address, uint8_t locationH, uint8_t locationL, uint8_t *data_byte)
{
	/* Send START, Device Address with WRITE bit */
	sercom->I2CM.ADDR.reg = (device_address << 1) | WIRE_WRITE_FLAG;
	while (!(sercom->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_MB) && !(sercom->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_SB)); // Wait for master on bus or slave on bus flag

	if (sercom->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_RXNACK) {
		sercom->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3); // Send STOP
		return false; // NACK received from slave
	}

	/* Send Register Address High Byte */
	sercom->I2CM.DATA.reg = locationH;
	while (!(sercom->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_MB)); // Wait for master on bus flag (transmit complete)
	if (sercom->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_RXNACK) {
		sercom->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3); // Send STOP
		return false; // NACK received from slave
	}

	/* Send Register Address Low Byte */
	sercom->I2CM.DATA.reg = locationL;
	while (!(sercom->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_MB)); // Wait for master on bus flag (transmit complete)
	if (sercom->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_RXNACK) {
		sercom->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3); // Send STOP
		return false; // NACK received from slave
	}

	/* Send REPEATED START, Device Address with READ bit */
	sercom->I2CM.ADDR.reg = (device_address << 1) | WIRE_READ_FLAG;
	while (!(sercom->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_SB)); // Wait for slave on bus flag (indicates slave ACK'd address)

	if (sercom->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_RXNACK) { // Check for NACK after sending address with read bit
		sercom->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3); // Send STOP
		return false;
	}
	
	/* Master sends NACK after reading the byte, then STOP */
	sercom->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_ACKACT; // Set ACK action to NACK
	// Reading DATA register clears SB flag
	*data_byte = sercom->I2CM.DATA.reg;

	sercom->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3); // Send STOP condition
	
	return true;
}

bool i2c_write_byte(Sercom *sercom, uint8_t device_address, uint8_t locationH, uint8_t locationL, uint8_t data_byte)
{
	/* Send START, Device Address with WRITE bit */
	// Master issues start condition by writing device address with R/W bit = 0 to ADDR register
	sercom->I2CM.ADDR.reg = (device_address << 1) | WIRE_WRITE_FLAG; 
	while (!(sercom->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_MB) && !(sercom->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_SB)); // Wait for MB (transmission complete) or SB (slave ack)

	if (sercom->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_RXNACK) { // Check if slave NACK'd the address
		sercom->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3); // Issue STOP condition
		return false; 
	}

	/* Send Register Address High Byte */
	sercom->I2CM.DATA.reg = locationH;
	while (!(sercom->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_MB)); // Wait for MB (transmission complete)
	if (sercom->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_RXNACK) { // Check if slave NACK'd the data
		sercom->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3); // Issue STOP condition
		return false;
	}

	/* Send Register Address Low Byte */
	sercom->I2CM.DATA.reg = locationL;
	while (!(sercom->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_MB)); // Wait for MB (transmission complete)
	if (sercom->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_RXNACK) { // Check if slave NACK'd the data
		sercom->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3); // Issue STOP condition
		return false;
	}

	/* Send Data Byte */
	sercom->I2CM.DATA.reg = data_byte;
	while (!(sercom->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_MB)); // Wait for MB (transmission complete)
	if (sercom->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_RXNACK) { // Check if slave NACK'd the data
		sercom->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3); // Issue STOP condition
		return false;
	}

	/* Send STOP Condition */
	sercom->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3); // CMD = 3 issues STOP condition
	// No need to wait for SYNCBUSY after STOP for this simple case, bus will be released.
	
	return true;
}

void enableWire()
{
	// I2C Master and Slave modes share the ENABLE bit function.

	// Enable the I²C master mode
	SERCOM0->I2CM.CTRLA.bit.ENABLE = 1 ;

	while ( SERCOM0->I2CM.SYNCBUSY.bit.ENABLE != 0 )
	{
		// Waiting the enable bit from SYNCBUSY is equal to 0;
	}

	// Setting bus idle mode
	SERCOM0->I2CM.STATUS.bit.BUSSTATE = WIRE_IDLE_STATE ;

	while ( SERCOM0->I2CM.SYNCBUSY.bit.SYSOP != 0 )
	{
		// Wait the SYSOP bit from SYNCBUSY coming back to 0
	}
}									

uint8_t i2c_startTransmissionWire(uint8_t device_address, SercomWireReadWriteFlag flag)
{
	// 7-bits device_address + 1-bit R/W
	uint16_t transmit_address = (device_address << 1) | flag;

	// Wait idle or owner bus mode
	while ( !isBusIdleWIRE() && !isBusOwnerWIRE() );

	// Send start and address
	//SERCOM0->I2CM.ADDR.bit.ADDR = transmit_address; // This would be for bit field access
	
	SERCOM0->I2CM.ADDR.reg = transmit_address;


	// Address Transmitted
	if ( flag == WIRE_WRITE_FLAG ) // Write mode
	{
		while( !SERCOM0->I2CM.INTFLAG.bit.MB )
		{
			// Wait transmission complete
		}
	}
	else  // Read mode
	{
		while( !SERCOM0->I2CM.INTFLAG.bit.SB )
		{
			// If the slave NACKS the address, the MB bit will be set.
			// In that case, send a stop condition and return False.
			if (SERCOM0->I2CM.INTFLAG.bit.MB) {
				SERCOM0->I2CM.CTRLB.bit.CMD = 3; // Stop condition
				return False;
			}
			// Wait transmission complete
		}

		// Clean the 'Slave on Bus' flag, for further usage. (This is normally cleared by reading DATA or writing ADDR/DATA)
	}

	//ACK received (0: ACK, 1: NACK)
	if(SERCOM0->I2CM.STATUS.bit.RXNACK)
	{
		return False;
	}
	else
	{
		return True;
	}
}

uint8_t isBusIdleWIRE( void )
{
	return SERCOM0->I2CM.STATUS.bit.BUSSTATE == WIRE_IDLE_STATE;
}

uint8_t isBusOwnerWIRE( void )
{
	return SERCOM0->I2CM.STATUS.bit.BUSSTATE == WIRE_OWNER_STATE;
}

void resetWIRE()
{
  //I2CM OR I2CS, no matter SWRST is the same bit.

  //Setting the Software bit to 1
  SERCOM0->I2CM.CTRLA.bit.SWRST = 1;

  //Wait both bits Software Reset from CTRLA and SYNCBUSY are equal to 0
  while(SERCOM0->I2CM.CTRLA.bit.SWRST || SERCOM0->I2CM.SYNCBUSY.bit.SWRST);
}