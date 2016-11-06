/*
 * Init_clocks.c
 *
 * Created: 6/28/2016 8:19:39 PM
 *  Author: ocybr
 */ 

extern INCLUDES_H_
//init clock for use with blinking led.
void init_TC1(void)
{
	//setup clock for timer/counters
	REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_TC1_TC2;
	REG_PM_APBCMASK |= PM_APBCMASK_TC1;
	REG_TC1_CTRLA |=TC_CTRLA_PRESCALER_DIV8;	// prescaler: 8
	REG_TC1_INTENSET = TC_INTENSET_OVF;		// enable overflow interrupt
	REG_TC1_CTRLA |= TC_CTRLA_ENABLE;		// enable TC1
	while(TC1->COUNT16.STATUS.bit.SYNCBUSY==1);	// wait on sync
	NVIC_EnableIRQ(TC1_IRQn);			// enable TC1 interrupt in the nested interrupt controller
}

void TC1_Handler()
{
	REG_PORT_OUTTGL0 = PORT_PA14;		// troggle PA02
	REG_TC1_INTFLAG = TC_INTFLAG_OVF;	// reset interrupt flag - NEEDED HERE!
}