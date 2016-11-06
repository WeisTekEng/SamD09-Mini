/*
 * init_clks.cpp
 *
 * Created: 6/26/2016 9:52:11 PM
 *  Author: ocybr
 */ 

#include "sam.h"
#include "init_clks.h"

#define F_INTERM 3200 /*intermediate freq 32khz for FLL*/
#define CONF_CLOCK_DPLL_REFERENCE_CLOCK SYSCTRL_XOSC32K_MASK


void clk_setup(uint8_t rtc, long EXTCLK_FREQ)
{
	if(rtc)
	{
		clk_config(EXTCLK_FREQ);
		RTC_init();
		
	}
}

void clk_config(long EXTCLK_FREQ)
{
	#define EXTCLK_DIV ((EXTCLK_FREQ/(2*F_INTERM))-1)
	
	//enable 8Mhz osc input
	SYSCTRL->XOSC.reg = (SYSCTRL_XOSC_ENABLE);
	
	//wait for crystal osc to start up.
	while((SYSCTRL->PCLKSR.reg & (SYSCTRL_PCLKSR_XOSCRDY)) == 0);
	
	REG_NVMCTRL_CTRLB |= (NVMCTRL_CTRLB_RWS(1));
	
	//config the DPLL
	SYSCTRL->DPLLCTRLA.reg = 0; // set to defaults
	
	//SYSCTRL->DPLLRATIO.reg = SYSCTRL_DPLLRATIO_LDR(2*F_CPU/F_INTERM);
	SYSCTRL->DPLLRATIO.reg = SYSCTRL_DPLLRATIO_LDR(2*EXTCLK_FREQ/F_INTERM);
	SYSCTRL->DPLLCTRLB.reg = SYSCTRL_DPLLCTRLB_DIV(EXTCLK_DIV);
	SYSCTRL->DPLLCTRLA.reg = SYSCTRL_DPLLCTRLA_ENABLE;
	
	while(!(SYSCTRL->DPLLSTATUS.reg & (SYSCTRL_DPLLSTATUS_CLKRDY | SYSCTRL_DPLLSTATUS_LOCK)) ==
		(SYSCTRL_DPLLSTATUS_CLKRDY | SYSCTRL_DPLLSTATUS_LOCK));
		
	//select gen clock gen 0, set DPLL as input devide by 2.
	GCLK->GENDIV.reg = ((2 << GCLK_GENDIV_DIV_Pos) | (0 << GCLK_GENDIV_ID_Pos));
	GCLK->GENDIV.reg = ((0 << GCLK_GENCTRL_ID_Pos) | (GCLK_SOURCE_FDPLL << GCLK_GENCTRL_SRC_Pos) | GCLK_GENCTRL_GENEN);
	GCLK->CLKCTRL.reg = ((GCLK_CLKCTRL_GEN_GCLK0) | (GCLK_CLKCTRL_CLKEN));
	
	//set GLCK gen 2 to use devided FDPLL 96Mhz as input, feed this clock to RTC, this gives us a 16Mhz RTC clock
	GCLK->GENDIV.reg = ((2 << GCLK_GENDIV_ID_Pos) | (6 << GCLK_GENDIV_DIV_Pos));
	GCLK->GENCTRL.reg = ((2 << GCLK_GENCTRL_ID_Pos) | (GCLK_GENCTRL_GENEN) | (GCLK_SOURCE_FDPLL << GCLK_GENCTRL_SRC_Pos));
	
}

void RTC_init()
{
	GCLK->CLKCTRL.reg = ((GCLK_CLKCTRL_GEN_GCLK2) | (GCLK_CLKCTRL_CLKEN) |
	(GCLK_CLKCTRL_ID(RTC_GCLK_ID)));
	// Now configure the RTC preacaler for divide-by-16, for 1MHz "tick", and have it
	//   count to 1000.  Microsecond ticks, millisecond interrupts.
	// Note that using a higher input clock and larger prescaler divider cuts down on
	//  clock synchronization issues that are otherwise really annoying.
	RTC->MODE0.CTRL.reg = RTC_MODE0_CTRL_SWRST; //reset
	while (RTC->MODE0.CTRL.bit.SWRST)
	;  // wait for reset complete
	RTC->MODE0.CTRL.reg = RTC_MODE0_CTRL_MODE_COUNT32;
	RTC->MODE0.CTRL.reg |= RTC_MODE0_CTRL_MATCHCLR | RTC_MODE0_CTRL_PRESCALER_DIV16;
	RTC->MODE0.COMP[0].reg = 1000 - 2;
	RTC->MODE0.READREQ.reg |= RTC_READREQ_RCONT;
	RTC->MODE0.INTENSET.reg = RTC_MODE0_INTENSET_CMP0;
	RTC->MODE0.CTRL.reg |= RTC_MODE0_CTRL_ENABLE;
	RTC->MODE0.INTFLAG.reg = 0xFF;
	NVIC_EnableIRQ(RTC_IRQn);
}


