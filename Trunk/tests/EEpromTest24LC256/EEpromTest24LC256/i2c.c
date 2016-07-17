/*
 * i2c.c
 *
 * Created: 7/16/2016 2:14:26 PM
 *  Author: ocybr
 */ 

#include "includes.h"

void i2c_setup(Sercom *sercom)
{
	/*setup power management controler*/
	PM->APBCMASK.reg = PM_APBCMASK_SERCOM0;
	
	/*setup clocks*/
	
	GCLK->CLKCTRL.reg |= GCLK_CLKCTRL_ID(SERCOM0_GCLK_ID_CORE) |
							GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(0);
							
	/*reset i2c module*/
	sercom->I2CM.CTRLA.reg |= SERCOM_I2CM_CTRLA_SWRST;
	
	sercom->I2CM.CTRLB.reg = SERCOM_I2CM_CTRLB_SMEN;
	while(sercom->I2CM.SYNCBUSY.reg);
	
	/*wait for soft reset to finish*/
	//while(sercom->I2CM.SYNCBUSY.bit.SWRST);
	
	/*setup baud rate*/
	/* Set baudrate */
	//uint32_t fgclk        = 8000000;    /* 8MHz */
	//uint32_t fscl        = 1000; /* 1kHz SCL */
	//uint32_t trise        = 215; /* 215 ns rising time */
	//int32_t numerator    = fgclk - fscl*(10 + fgclk*trise/1000000000);
	//int32_t denominator    = 2*fscl;
	//int32_t tmp_baud = (int32_t)(div_ceil(numerator, denominator));
	//sercom->I2CM.BAUD.bit.BAUD = SERCOM_I2CM_BAUD_BAUD(tmp_baud);
	
    // Synchronous arithmetic baudrate
	//SERCOM0->I2CM.BAUD.bit.BAUD = SystemCoreClock / ( 2 * TWI_CLOCK) - 1 ;
	sercom->I2CM.BAUD.reg = SERCOM_I2CM_BAUD_BAUD(160);
	while (sercom->I2CM.SYNCBUSY.reg);
	
	/*setup module*/
	SERCOM0->I2CM.CTRLA.reg =    SERCOM_I2CM_CTRLA_ENABLE | /* enable module */
	SERCOM_I2CM_CTRLA_MODE_I2C_MASTER |    /* i2c master mode */
	SERCOM_I2CM_CTRLA_SDAHOLD(3);        /* SDA hold time to 600ns */
	while (sercom->I2CM.SYNCBUSY.bit.ENABLE);
	
	sercom->I2CM.STATUS.reg |= SERCOM_I2CM_STATUS_BUSSTATE(1);
	while(sercom->I2CM.SYNCBUSY.reg);
	//uint16_t timeout_counter = 0;
	//
	///*if buss state is uknown set to idle after timeout*/
	//while(!(sercom->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_BUSSTATE(1)))
	//{
		//timeout_counter++;
		//if(timeout_counter >= 65535)
		//{
			//sercom->I2CM.STATUS.reg |= SERCOM_I2CM_STATUS_BUSSTATE(1); /* set to idle state */
		//}
	//}
	
}

uint8_t i2c_read(Sercom *sercom, uint8_t *data, int size, uint8_t address)
{
	sercom->I2CM.ADDR.reg = (address << 0x1ul) | 1;//| 0x0ul; //i2c transfer read = 1
	
	//while (0==(sercom->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_SB));
	
	if(sercom->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_RXNACK)
	{
		sercom->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);
		return False;
	}
	
	sercom->I2CM.CTRLB.reg &= -SERCOM_I2CM_CTRLB_ACKACT;
	
	for(int i = 0;i < size-1;i++)
	{
		data[i] = sercom->I2CM.DATA.reg;
		while (0==(sercom->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_SB));
	}
	
	if(size)
	{
		sercom->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_ACKACT;
		sercom->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);
		data[size-1] = sercom->I2CM.DATA.reg;
	}
	return *data;
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
	SERCOM0->I2CM.STATUS.bit.BUSSTATE = 1 ;

	while ( SERCOM0->I2CM.SYNCBUSY.bit.SYSOP != 0 )
	{
		// Wait the SYSOP bit from SYNCBUSY coming back to 0
	}
}

uint8_t startTransmissionWire(uint8_t address, SercomWireReadWriteFlag flag)
{
	// 7-bits address + 1-bits R/W
	address = (0x50 << 0x1ul) | flag;

	// Wait idle or owner bus mode
	while ( !isBusIdleWIRE() && !isBusOwnerWIRE() );

	// Send start and address
	//SERCOM0->I2CM.ADDR.bit.ADDR = address;
	
	SERCOM0->I2CM.ADDR.reg = address;//| 0x0ul; //i2c transfer read = 1


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

		// Clean the 'Slave on Bus' flag, for further usage.
		//sercom->I2CM.INTFLAG.bit.SB = 0x1ul;
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