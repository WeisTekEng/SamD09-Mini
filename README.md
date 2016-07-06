# miniSam-Zero R1.3 USART bootloader and test software.

Test software and Serial Uart Bootloader for Atmels Samd09 Arm Cortex M0+ controllers. The documentation for this project can be found on hackaday.io -> https://hackaday.io/project/12438-minisam-zero. More information available soon at my blog, www.weistekengineering.com

#Folder structure.
Current stable build of the bootloader is located in
Trunk/Bootloader/R1_0_current

This directory has everything needed to load a ATSAMD09 with the bootloader. 

#Bootloader readme: how to use it.
Setting up atmel studio 7

Start by creating a new project.

Name your project and select GCC C Executable Project.

Select the proper chip family, in our case SamD09.

Select the chip present on the board, if you use @al1's board you would select ATSAMD09C13A. (hope its ok i used your board as an example.)

Once atmel studio has completed setting up your project. Right click on your project in the right panel and select properties. You should see this window. If you do not you right clicked on the wrong item. usually the 2rd entry down.

Now we need to tell the linker that we want the program to start at address 0x800. This will make the program compatible with the miniSam bootloader. Find ARM/GNU linker and select Miscellaneous. Copy the code given below into the field Linker Flags.
'''
 -Wl,--section-start=.text=0x800
'''
Then click ok. Now you can select your main.c in the right panel and start coding.

a simple blink app.

'''
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
'''

This code should blink the on board led at 1hz. Here's how to upload it to the miniSam bootloader.
Uploading.

Currently the miniSam bootloader uses a python front end, I did not code this entire front end. This front end came with the suggested non working bootloader on a forum post and I updated it and modified it. So i can not take credit for this one. I will take credit for the modifications though :)

open a command line and navigate to the bootloader directory.

The python script you will be using is called upload.py instead of what is currently in the picture. The options are -c com# -b baudrate -i yourfilename.bin

If you have entered all of that properly you will presented with a quick question.

This question will ensure the miniSam bootloader is loaded and active. type y to proceed. Its fast but you will see a lot of programming page # and verifying page #

That's it your done!. Hopefully this is not to painful :), now you have no reason not to pick up a Arm micro (SamD variant at this point) and have some fun.

I will be making some mini libraries I think, maybe. I'll see whats already out there.

#Test programs.
todo.

#Major updates.

#readme revisions list
07/06/2016: updated github readme with instructions for setting up atmel studio 7 and using the bootloader to run user code.

