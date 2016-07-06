/*
 * Blink.c
 *
 * Created: 7/6/2016 12:41:00 AM
 * Author : Owner
 */ 


#include "sam.h"

void init_TC1(void)
{
	//Thank you Al1 for sharing this timer setup.
	//setup clock for timer/counters
	REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_TC1_TC2;
	REG_PM_APBCMASK |= PM_APBCMASK_TC1;
	REG_TC1_CTRLA |=TC_CTRLA_PRESCALER_DIV8;			// prescaler: 8
	REG_TC1_INTENSET = TC_INTENSET_OVF;					// enable overflow interrupt
	REG_TC1_CTRLA |= TC_CTRLA_ENABLE;					// enable TC1
	while(TC1->COUNT16.STATUS.bit.SYNCBUSY==1);			// wait on sync
	NVIC_EnableIRQ(TC1_IRQn);							// enable TC1 interrupt in the nested interrupt controller
}

void TC1_Handler()
{
	REG_PORT_OUTTGL0 = (1 << 14);						// toggle led PA14
	REG_TC1_INTFLAG = TC_INTFLAG_OVF;					// reset interrupt flag - NEEDED HERE!
}

int main(void)
{
    /* Initialize the SAM system */
    SystemInit();
	init_TC1();											//init the clock.
	REG_PORT_DIR0 = (1 << 14);							//set the direction to output of PA14
	
    while (1) 
    {
    }
}
