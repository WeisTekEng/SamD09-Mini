/*
 *clocks.c
 *Project:			SAMD09/D10 Peripheral Libraries
 *Author:			Weistek Engineering (jeremy G.)
 *website:			www.weistekengineering.com
 *date:				06-29-2016
 *Created:			7/18/2016 10:49:47 PM
 *Summary:			Initializes system clocks and generic clock generators (GCLK) for SAMD09/D10.
 *					Configures OSC8M, GCLK0/1, and assigns GCLKs to SERCOM0 and SERCOM1.
 *
 *License:	        GNU GENERAL PUBLIC LICENSE Version 3 or newer. The header of this file may not change.
 *
 *					unless new features are added then the update section may be updated. The License file in
 *					the root of this repo Trunk/LICENSE should have been provided to you. If it was not you may
 *					find a copy of the GNU Open source license at https://www.gnu.org/licenses/gpl.html
 */ 
 


#include "includes.h"

/************************************************************************/
/* \brief init_clocks() configures system clocks
/*- OSC8M clock source is enabled with a divider by  8 (1Mhz).
/*- Generic Clock Generator 0 (GCCLKMAIN) is using OSC8M as source
/*Things that need to happen
/*1) remove prescalers from OSC8M so we run at 8Mhz default
/*2.1) Gen(0) clock source OSC8M
/*2.2) GEN(1) clock source OSC8M
/*3) set Gen 0 for SERCOM0 at 8Mhz
/*4) set Gen 1 for SERCOM1 at 8Mhz   
/*5) set APBCMASK for SERCOM0 and SERCOM1                                       
/************************************************************************/

#define GEN_CLOCK_DIV_ZERO				(0u) // Retained for clarity if used elsewhere, though GCLK_GENCTRL_ID_0 is similar for ID
#define GEN_CLOCK_GENERATOR_ZERO		(0u) // Retained for clarity if used elsewhere, maps to GCLK_GENCTRL_ID_0
#define GEN_CLOCK_GENERATOR_ONE			(1u) // Retained for clarity if used elsewhere, maps to GCLK_GENCTRL_ID_1
#define GEN_CLOCK_GENERATOR_OSC8M		(3u)

void init_clocks(void)
{
	/************************************************************************/
	/* 1) remove prescalers from OSC8M so we run at 8Mhz					*/
	/************************************************************************/
	SYSCTRL->OSC8M.bit.PRESC = OSC8M_PRESCALER_DIV_1;
	NVMCTRL->CTRLB.bit.CACHEDIS = NVM_CACHE_DISABLED;
	
	/************************************************************************/
	/*2.1)Select Gen(0) set source as OSC8M no Div and enable clock         */
	/* Main clock															*/
	/************************************************************************/
	GCLK->GENDIV.reg = GCLK_GENDIV_ID(GCLK_GENCTRL_ID_0); // GENDIV applies to the generator selected by ID
	/*wait for sync*/
	while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);
	
	/*write the Gen clock 0 configureation*/
	GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(GCLK_GENCTRL_ID_0) |
						GCLK_GENCTRL_SRC_OSC8M | 
						GCLK_GENCTRL_GENEN;
						
	/*wait for sync*/
	while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);
	
	/************************************************************************/
	/*2.2) Select Gen(1) set source as OSC8M no Div and enable clock        */
	/************************************************************************/
	GCLK->GENDIV.reg = GCLK_GENDIV_ID(GCLK_GENCTRL_ID_1); // GENDIV applies to the generator selected by ID
	/*wait for sync*/
	while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);
	
	/*write the Gen clock 0 configureation*/
	GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(GCLK_GENCTRL_ID_1) |
	GCLK_GENCTRL_SRC_OSC8M |
	GCLK_GENCTRL_GENEN;
	
	/*wait for sync*/
	while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);
	
	
	/************************************************************************/
	/*3) I2C clocks SERCOM0 GEN0                                            */
	/************************************************************************/				
	GCLK->CLKCTRL.reg |= GCLK_CLKCTRL_ID(SERCOM0_GCLK_ID_CORE) |
	GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(GCLK_CLKCTRL_GEN_ID_0);
	
	/*wait for sync*/
	while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);
	
	/************************************************************************/
	/*4) USART clocks SERCOM1 GEN(1)                                        */
	/************************************************************************/	
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(SERCOM1_GCLK_ID_CORE) | 
						GCLK_CLKCTRL_GEN(GCLK_CLKCTRL_GEN_ID_1) |
						GCLK_CLKCTRL_CLKEN;
						
	/*wait for sync*/
	while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);
	
	/************************************************************************/
	/*5) APBx masks APBCMASK is setup for SERCOM0 and SERCOM1                                                           
	/************************************************************************/
	PM->APBCMASK.reg = PM_APBCMASK_SERCOM0 | PM_APBCMASK_SERCOM1;
				
							
}
// Note: GEN_CLOCK_DIV_ZERO was used as the argument to GCLK_GENDIV_ID for both GCLK_GENCTRL_ID_0 and GCLK_GENCTRL_ID_1.
// This implies that the DIV factor set by GCLK_GENDIV_DIV applies to the generator selected by GCLK_GENDIV_ID.
// So, for GCLK_GENCTRL_ID_0, its GENDIV.ID should be GCLK_GENCTRL_ID_0.
// And for GCLK_GENCTRL_ID_1, its GENDIV.ID should be GCLK_GENCTRL_ID_1.
// The original code set GCLK_GENDIV_ID(GEN_CLOCK_DIV_ZERO) which is GCLK_GENDIV_ID(0u) for both generators.
// This has been changed to GCLK_GENDIV_ID(GCLK_GENCTRL_ID_0) and GCLK_GENDIV_ID(GCLK_GENCTRL_ID_1) respectively.
// Assuming the divider value itself (usually 0 or 1 for no/minimal division) is intended to be the same for both.
// GCLK->GENDIV.bit.DIV is set by the lower bits of GCLK_GENDIV_DIV(val) if used, or defaults.
// The existing code did not explicitly set GCLK_GENDIV_DIV, so it implies a default division factor (likely 1).