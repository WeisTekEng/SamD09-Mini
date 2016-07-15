/*
 * eeprom.c
 *
 * Created: 7/14/2016 6:38:29 PM
 *  Author: Jeremy
 */ 

#include "includes.h"



/*send a char array and format as a string*/
void i2c_send_string(char s[],uint32_t i)
{
	while(s[i] != 0x00)
	{
		i2c_write_byte(s[i]);
		i++;
	}
}

//write to the i2c register.
void i2c_write_byte(uint8_t data)
{
	while(!I2C_SERCOM->I2CM.INTFLAG.bit.SB);
	{
		I2C_SERCOM->I2CM.DATA.reg = (uint16_t)data;
	}
}

//read from the i2c register.
uint8_t i2c_read_byte(void)
{
	while(!I2C_SERCOM->I2CM.INTFLAG.bit.MB);
	return((uint8_t)(I2C_SERCOM->I2CM.DATA.reg & 0x00FF));
}


I2C_sercom_init()
{
  /* APBCMASK */
  PM->APBCMASK.reg |= PM_APBCMASK_SERCOM0;
  	
  // Initialize the peripheral clock and interruption
  //initClockNVIC() ;
  /*gclk configuration for sercom1 module*/
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID (SERCOM1_GCLK_ID_CORE) |
  GCLK_CLKCTRL_GEN(0) |
  GCLK_CLKCTRL_CLKEN;

  resetWIRE() ;

  // Set master mode and enable SCL Clock Stretch mode (stretch after ACK bit)
  SERCOM0->I2CM.CTRLA.reg =  SERCOM_I2CM_CTRLA_MODE( I2C_MASTER_OPERATION )/* |
                            SERCOM_I2CM_CTRLA_SCLSM*/ ;

  // Enable Smart mode and Quick Command
  //sercom->I2CM.CTRLB.reg =  SERCOM_I2CM_CTRLB_SMEN /*| SERCOM_I2CM_CTRLB_QCEN*/ ;


  // Enable all interrupts
//  sercom->I2CM.INTENSET.reg = SERCOM_I2CM_INTENSET_MB | SERCOM_I2CM_INTENSET_SB | SERCOM_I2CM_INTENSET_ERROR ;

  // Synchronous arithmetic baudrate
  SERCOM0->I2CM.BAUD.bit.BAUD = SystemCoreClock / ( 2 * TWI_CLOCK) - 1 ;
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

bool startTransmissionWire(uint8_t address, SercomWireReadWriteFlag flag)
{
	// 7-bits address + 1-bits R/W
	address = (address << 0x1ul) | flag;

	// Wait idle or owner bus mode
	//while ( !isBusIdleWIRE() && !isBusOwnerWIRE() );

	// Send start and address
	SERCOM0->I2CM.ADDR.bit.ADDR = address;

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

void initClockNVIC( void )
{
  uint8_t clockId = 0;
  IRQn_Type IdNvic=PendSV_IRQn ; // Dummy init to intercept potential error later

   clockId = GCM_SERCOM0_CORE;
   IdNvic = SERCOM0_IRQn;
	
	// Setting NVIC
  NVIC_EnableIRQ(IdNvic);
  NVIC_SetPriority (IdNvic, (1<<__NVIC_PRIO_BITS) - 1);  /* set Priority */

  //Setting clock
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID( clockId ) | // Generic Clock 0 (SERCOMx)
                      GCLK_CLKCTRL_GEN_GCLK0 | // Generic Clock Generator 0 is source
                      GCLK_CLKCTRL_CLKEN ;

  while ( GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY )
  {
    /* Wait for synchronization */
  }
}