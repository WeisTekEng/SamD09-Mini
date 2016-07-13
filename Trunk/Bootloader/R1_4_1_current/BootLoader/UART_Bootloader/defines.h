/*
 *defines.h
 *Project:			miniSam USART bootloader.
 *Author:			Weistek Engineering (jeremy G.)
 *website:			www.weistekengineering.com
 *date:				07-11-2016
 *Summery:			includes all the defines needed for the bootloader.
 */ 


#ifndef DEFINES_H_
#define DEFINES_H_																										/*used for the specialTalk variable, remove this if you want the bootloader smaller.*/

#define SAMD_DID_DEVSEL_MASK				0x0F
#define SAMD_DID_DEVSEL_POS					0
#define SAMD_DID_REVISION_MASK				0x0F
#define SAMD_DID_REVISION_POS				8
#define SAMD_DID_SERIES_MASK				0x03
#define SAMD_DID_SERIES_POS					16

/*board spacific identifirs*/
#define Zero								5a65726f
#define Zero_DID							01041000
#define One									4f6e6500
#define One_DID								00000000
#define Two									54776f00
#define Two_DID								00000000
#define Al1									414c3100
#define AL1_DID								00000000

/*function defines*/
#define bool								_Bool
#define div_ceil(a,b)(((a)+(b)-1)/(b))
bool manual_page_write;																										/* If \c false, a page write command will be issued automatically when the page buffer is full. */

/*Micro related defines*/
#define _DID								DSU->DID.reg																	/*so we can access the DID reg*/
//#define VERSION																												/*tells the compiler to compile versioning info, comment out if you want more space.*/
//#define NEW_FRONT_END

#ifdef NEW_FRONT_END																										/*extracted function from samd_math.h <- something like that.*/
bool specialTalk;																											/*used for talking to an experimential front end.*/
#endif

/* Change the following if different SERCOM and boot pins are used */
#define BOOT_SERCOM							SERCOM1																			/*miniSam uses Sercom1 for USART*/
#define BOOT_SERCOM_BAUD					115200
#define BOOT_PORT							0
#define BOOT_PIN							15																				/*PA15 for bootloader en, toggled by the python script. or DTR from serial coms.*/
/* Application starts from 1kB memory - Bootloader size is 1kB */
/* Change the address if higher boot size is needed */
/*good site for quick conversions.*/
/*http://www.binaryhexconverter.com/hex-to-decimal-converter*/
#define APP_START							0x800																			/*This gives 1536 bytes of bootloader space.*/

/* Target application size can be 15kB */
/* APP_SIZE is the application section size in kB */
#if defined(NEW_FRONT_END)
	#define APP_SIZE						13
#elif defined(VERSION)
	#define APP_SIZE						13
#else
	#define APP_SIZE						14
#endif

/*Stuff that should not change*/
#define SERCOM_GCLK							8000000UL																		/*base clock speed, the bootloader will run at this speed.*/
#define BAUD_VAL							(65536.0*(1.0-((float)(16.0*(float)BOOT_SERCOM_BAUD)/(float)SERCOM_GCLK)))		/* calculate baud rate from SERCOM_GCLK SERCOM USART GCLK Frequency */
												
#define NVM_MEMORY							((volatile uint16_t *)FLASH_ADDR)												/* Memory pointer for flash memory */

/*Board defines*/
#define miniSam_Zero						0x10040100																		/*this should be different between the SOIC, and QFN part.*/
//#define Al1									0x00000000																		/*need a dev board.*/

#endif /* DEFINES_H_ */