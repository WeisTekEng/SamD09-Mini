/*
 * BasicFunctions.cpp
 *
 * Created: 6/25/2016 10:50:57 AM
 *  Author: Weistek Engineering (Jeremy G)
 */ 

#include "Includes.h"

//Init timers and clocks.
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

//Handlers
void TC1_Handler()
{
	//REG_PORT_OUTTGL0 = PORT_PA11;			// internal command to toggle pin.
	REG_TC1_INTFLAG = TC_INTFLAG_OVF;		// reset interrupt flag
}

//crude wait function.
void wait(uint32_t count)
{
	//////////////////////////////////////////////////////////////////////////
	//Poor mans wait counter, very inefficient
	//////////////////////////////////////////////////////////////////////////
	
	for(uint32_t x = 0;x>=count;x++);;
}