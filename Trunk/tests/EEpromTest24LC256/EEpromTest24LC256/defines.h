/*
 *defines.h
 *Project:			miniSam USART bootloader.
 *Author:			Weistek Engineering (jeremy G.)
 *website:			www.weistekengineering.com
 *date:				07-11-2016
 *Summery:			includes all the defines needed for the bootloader.
 */ 


#ifndef DEFINES_H_
#define DEFINES_H_																										

#include "includes.h"

#define SAMD09

#define False								0
#define True								1

#define GCM_SERCOM0_CORE					(0x14U)
#define BL_VER								"R1.4.1"																	/*bootloader version*/
#define BOARD_VER							"R1.3"																		/*board version*/
#define BOARD_ID							"0x0001"																	/*used for dev versions of the board.*/
#define REG_TO								"Jeremy G"																	/*used for dev versions, who the board was assigned to.*/

/*function defines*/
#define bool								_Bool
#define div_ceil(a,b)(((a)+(b)-1)/(b))																									/* If \c false, a page write command will be issued automatically when the page buffer is full. */

/* Change the following if different SERCOM and boot pins are used */
#define BOOT_SERCOM							SERCOM1																			/*miniSam uses Sercom1 for USART*/
#define BOOT_SERCOM_BAUD					115200
#define BOOT_PORT							0

#define I2C_SERCOM							SERCOM0
#define SCL									PINMUX_PA15C_SERCOM0_PAD1
#define SDA									PINMUX_PA14C_SERCOM0_PAD0

/*Stuff that should not change*/
#if defined(SAMD09)
	#define SERCOM_GCLK						8000000UL																		/*base clock speed, the bootloader will run at this speed.*/
#elif defined(SAMD10)
	#define SERCOM_GCLK						1000000UL
#endif

#define BAUD_VAL							(65536.0*(1.0-((float)(16.0*(float)BOOT_SERCOM_BAUD)/(float)SERCOM_GCLK)))		/* calculate baud rate from SERCOM_GCLK SERCOM USART GCLK Frequency */
									
#endif /* DEFINES_H_ */