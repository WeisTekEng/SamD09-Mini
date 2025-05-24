/*
 *nvmctrl.c
 *Project:			SAMD09/D10 Peripheral Libraries
 *Author:			Weistek Engineering (jeremy G.)
 *website:			www.weistekengineering.com
 *date:				07-11-2016
 *Summary:			Implements Non-Volatile Memory (NVM) controller functions for SAMD09/D10,
 *					such as row erase and page buffer writing.
 */ 

#include "includes.h"

// Define NVM error flags mask if not available from headers (assuming typical values)
#ifndef NVMCTRL_STATUS_ERROR_MASK
#define NVMCTRL_STATUS_ERROR_MASK (NVMCTRL_STATUS_PROGE | NVMCTRL_STATUS_LOCKE | NVMCTRL_STATUS_NVME)
#endif

bool nvm_erase_row(const uint32_t row_address, uint32_t PAGE_SIZE)
{
	// PAGE_SIZE parameter is currently unused.
	// The primary error checking relies on the NVM controller's status bits.
	/* Check if the module is busy */
	while(!NVMCTRL->INTFLAG.bit.READY);
	
	/* Clear error flags by writing '1' to them */
	NVMCTRL->STATUS.reg = NVMCTRL_STATUS_ERROR_MASK; 
	
	/* Set address and command */	
	NVMCTRL->ADDR.reg = (row_address / 2); // Address is word address
	NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_ER;
	
	/* Wait for the command to complete */
	while(!NVMCTRL->INTFLAG.bit.READY);
	
	/* Check for errors */
	if (NVMCTRL->STATUS.reg & NVMCTRL_STATUS_ERROR_MASK) {
		return false; // Error occurred
	}
	
	return true; // Success
}

bool nvm_write_buffer(uint32_t destination_address, const uint8_t *buffer, uint16_t length)
{
	// The function currently relies on the NVM controller's status for error detection during operation.
	/* Check if the module is busy */
	while(!NVMCTRL->INTFLAG.bit.READY);
	
	/* Set automatic page write mode (MANW = 0) */
	NVMCTRL->CTRLB.bit.MANW = NVM_MANUAL_WRITE_DISABLE;

	/* Clear error flags by writing '1' to them */
	NVMCTRL->STATUS.reg = NVMCTRL_STATUS_ERROR_MASK;

	/* Erase the page buffer before buffering new data */
	NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMD_PBC | NVMCTRL_CTRLA_CMDEX_KEY;
	while(!NVMCTRL->INTFLAG.bit.READY);

	/* Check for errors after Page Buffer Clear */
	if (NVMCTRL->STATUS.reg & NVMCTRL_STATUS_ERROR_MASK) {
		return false; 
	}

	uint32_t nvm_address = destination_address / 2; // Address is word address

	/* NVM _must_ be accessed as a series of 16-bit words, perform manual copy
	 * to ensure alignment. Data is written to page buffer first.
	 */
	for (uint16_t k = 0; k < length; k += 2) 
	{
		uint16_t data;
		
		/* Copy first byte of the 16-bit chunk to the temporary buffer */
		data = buffer[k];
		
		/* If we are not at the end of a write request with an odd byte count,
		 * store the next byte of data as well */
		if (k < (length - 1)) {
			data |= (buffer[k + 1] << 8);
		} else if (length % 2 != 0) { // Handle odd length buffer by padding with 0xFF for the last byte
            data |= (0xFF << 8); // Or choose another padding value if appropriate
        }

		/* Store next 16-bit chunk to the NVM memory space (page buffer) */
		NVM_MEMORY[nvm_address++] = data;
	}
    
    /* Issue Write Page command after all data is in page buffer */
    /* The original code issued CMD_WP after each word, which is generally incorrect.
     * CMD_WP should be issued once the page buffer is filled with data to be written.
     * If NVM_MEMORY directly writes to flash without an explicit CMD_WP for each word
     * (which is unlikely for true NVM/Flash), then the structure is different.
     * Assuming NVM_MEMORY writes to a page buffer that is then committed to Flash.
     */

    // Before issuing the final write command, ensure address is set if not implicit
    // NVMCTRL->ADDR.reg = (destination_address / 2); // Usually ADDR is for row/page start, not each word.
                                                 // For page writes, ADDR is set before CMD_WP if not using page buffer directly.
                                                 // The loop writing to NVM_MEMORY populates the page buffer.
                                                 // ADDR might need to be set to the start of the page being written.
                                                 // For simplicity here, assuming ADDR was set correctly by the caller or is implicit.

    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_WP;
    while(!NVMCTRL->INTFLAG.bit.READY);

    /* Check for errors after Write Page */
    if (NVMCTRL->STATUS.reg & NVMCTRL_STATUS_ERROR_MASK) {
        return false;
    }
	
	/* The original code had an additional CMD_WP under certain conditions.
	 * if ((manual_page_write == 0) && (length < NVMCTRL_PAGE_SIZE)) {
	 *    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_WP;
	 * }
	 * This is redundant if CMD_WP is already issued above and MANW=0.
	 * If MANW=0, page write happens automatically when buffer is full or explicitly with CMD_WP.
     * The single CMD_WP after filling the buffer (or partial buffer) should be sufficient.
	 */
	
	return true; // Success
}
