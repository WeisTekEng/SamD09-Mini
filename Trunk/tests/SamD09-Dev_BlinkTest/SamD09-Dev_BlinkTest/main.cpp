/*
 * SamD09-Dev_BlinkTest.cpp
 *
 * Created: 6/20/2016 8:40:54 PM
 * Author : ocybr
 */ 


#include "sam.h"
//#include "samd09d14a.h"

//void setPin(uint16_t pin);
//void pinState(uint16_t pin);

void init_TC1(void);

void SetPinState(uint16_t op, uint16_t togg, uint16_t pin);

void wait(uint32_t	count);

void init_TC1(void)
{
	//////////////////////////////////////////////////////////////////////////
	//This sets our clocks for timers etc.
	//////////////////////////////////////////////////////////////////////////
	REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_TC1_TC2;
	REG_PM_APBAMASK |= PM_APBCMASK_TC1;
	REG_TC1_CTRLA |= TC_CTRLA_PRESCALER_DIV8;		// Set our prescaler to 8
	REG_TC1_INTENSET = TC_INTENSET_OVF;				// Enable overflow interrupt
	REG_TC1_CTRLA |= TC_CTRLA_ENABLE;				// Enable TC1 clock
	while(TC1->COUNT16.STATUS.bit.SYNCBUSY==1);		// Wait for clock to sync
	NVIC_EnableIRQ(TC1_IRQn);						// Enable TCI interrupt in the nested interrupt controller.
}

void wait(uint32_t count)
{
	//////////////////////////////////////////////////////////////////////////
	//Poor mans wait counter, very inefficient
	//////////////////////////////////////////////////////////////////////////
	
	for(uint32_t x = 0;x>=count;x++);;
}

void SetPinState(uint16_t dir, uint16_t togg, uint16_t pin)
{
	//////////////////////////////////////////////////////////////////////////
	//This function takes three arguments
	//variable dir = pin direction 0 = input, 1 = output
	//variable togg = boolean 0 = no toggle, 1 = toggle pin.
	//variable pin = pin # eg PA11 
	//////////////////////////////////////////////////////////////////////////
	
	uint16_t dirState = REG_PORT_DIR0;
	//uint16_t pinState = REG_PORT_OUT0;
	
	if(dirState != dir)
	{
		REG_PORT_DIR0 &= ~(1 << pin);
	}
	
	if(togg == 1)
	{
		REG_PORT_OUT0 &= ~(1 << pin);
	}
}

void TC1_Handler()
{
	REG_PORT_OUTTGL0 = PORT_PA11;			// internal command to toggle pin.
	REG_TC1_INTFLAG = TC_INTFLAG_OVF;		// reset interrupt flag
}


int main(void)
{
    /* Initialize the SAM system */
    SystemInit();
	init_TC1(); // <-- toggle the LED using TC1 interrupt handler.
	
	//REG_PORT_DIR0 |= (1 << 11); // Set the output of PB11 as output.
	SetPinState(1,0,11); //set the output of PB11 as output using our own function.

    /* Replace with your application code */
    while (1) 
    {
		
		//REG_PORT_OUT0 &= ~(1 << 11); // Cycle pin high or low.
		SetPinState(0,1,11); // Cycle pin high or low.
		
		 wait(100000);//Wait some time.
		
		//We can do this easier with this code segment.
		//REG_PORT_DIR0 |= PORT_PA11; // Set the output of PB11 as output using its port definition.
		SetPinState(0,1,11); // Cycle pin high or low.
		
		wait(100000); //Wait some time.
		
    }
}
