/*
 * MiniSamBootLoader.cpp
 *
 * Created: 6/29/2016 7:33:38 PM
 * Author : Owner
 */ 


#include "sam.h"

void init_clk_output()
{
	uint32_t temp_genctrl_config = 0;
	// Write to the PINCFG register to enable the peripheral multiplexer
	PORT->Group[0].PINCFG[27].reg = 1;
	// Enable peripheral function H for PA27, refer chapter I/O Multiplexing in the device datasheet
	PORT->Group[0].PMUX[13].bit.PMUXO = 7;
	
	SYSCTRL->OSC8M.bit.RUNSTDBY = 1; /*set system clock to run in standby, operate while in sleep mode.*/
	//SYSCTRL->XOSC.bit.RUNSTDBY = 1;
	//SYSCTRL->OSC32K.bit.RUNSTDBY = 1;
	//SYSCTRL->XOSC32K.bit.RUNSTDBY = 1;
	
	SYSCTRL->OSC8M.bit.ENABLE = 0x1;
	SYSCTRL->OSC8M.bit.PRESC = 0x0;
	
	//GCLK->GENDIV.reg = GCLK_GENDIV_ID(0) | GCLK_GENDIV_DIV(0);
	while(GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY){/*wait for sync.*/}
	
	GCLK->GENCTRL.reg =  (GCLK_GENCTRL_GENEN | /*Enable clock*/
	GCLK_GENCTRL_SRC_OSC8M | /*Set clock source*/
	GCLK_GENCTRL_ID(0) | /*Select clock generator 0-4*/
	GCLK_GENCTRL_RUNSTDBY | /*Run in standby mode.*/ 
	/*GCLK_GENCTRL_DIVSEL |*/ /*Specify prescaler mode.*/
	GCLK_GENCTRL_OE); /*output enable.*/
}


int main(void)
{
    /* Initialize the SAM system */
    SystemInit();
	init_clk_output();

    /* Replace with your application code */
    while (1) 
    {
    }
}
