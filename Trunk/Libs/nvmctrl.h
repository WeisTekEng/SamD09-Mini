/*
 *nvmctrl.h
 *Project:			SAMD09/D10 Peripheral Libraries
 *Author:			Weistek Engineering (jeremy G.)
 *website:			www.weistekengineering.com
 *date:				07-11-2016
 *Summary:			Header file for nvmctrl.c. Declares functions for Non-Volatile Memory (NVM)
 *					operations on SAMD09/D10, like `nvm_erase_row` and `nvm_write_buffer`.
 */ 


#ifndef NVMCTRL_H_
#define NVMCTRL_H_

/* NVMCTRL_CTRLB.MANW (Manual Write) bit values */
#define NVM_MANUAL_WRITE_DISABLE            (0U)  /* Automatic page write is enabled */
#define NVM_MANUAL_WRITE_ENABLE             (1U)  /* Manual page write is enabled (explicit CMD_WP needed) */

/*function allows us to erase nvm by rows and pages. Returns true on success, false on failure.*/
bool nvm_erase_row(const uint32_t row_address, uint32_t PAGE_SIZE);

/*allows us to write to nvm using page write. Returns true on success, false on failure.*/
bool nvm_write_buffer(uint32_t destination_address, const uint8_t *buffer, uint16_t length);

#endif /* NVMCTRL_H_ */