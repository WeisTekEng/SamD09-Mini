/*
 *nvmctrl.h
 *Project:			miniSam USART bootloader.
 *Author:			Weistek Engineering (jeremy G.)
 *website:			www.weistekengineering.com
 *date:				07-11-2016
 *Summery:			Contains all of our functions for manipulating nvm.
 */ 


#ifndef NVMCTRL_H_
#define NVMCTRL_H_

/*function allows us to erase nvm by rows and pages.*/
void nvm_erase_row(const uint32_t row_address, uint32_t PAGE_SIZE);

/*allows us to write to nvm using page write.*/
void nvm_write_buffer(uint32_t destination_address, const uint8_t *buffer, uint16_t length);

#endif /* NVMCTRL_H_ */