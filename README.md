# miniSam-Zero R1.3 USART bootloader and test software.

Test software and Serial Uart Bootloader for Atmels Samd09 Arm Cortex M0+ controllers. The documentation for this project can be found on hackaday.io -> https://hackaday.io/project/12438-minisam-zero. More information available soon at my blog, www.weistekengineering.com

#Folder structure.
Current stable build of the bootloader is located in
Trunk/Bootloader/R1_0_current

This directory has everything needed to load a ATSAMD09 with the bootloader. 

#Bootloader readme.
ill update with a more detailed how to shortly. 07/04/2016

The modified samd09d14_flash.ld file is used when compiling new apps (code,software) for the micro. This ld file gives the program an offset of 0x800 in NVM. This is needed as the current bootloader resids at location 0x00. This may change at a later date.

#Test programs.
todo.

#Major updates.

#readme revisions list
