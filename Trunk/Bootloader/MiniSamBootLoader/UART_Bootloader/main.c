/*
 * main.c
 *Project:		miniSam USART bootloader.
 *Author:		Weistek Engineering (jeremy G.)
 *website:		www.weistekengineering.com
 *date:			06-29-2016f
 *Summery:		Modified version of the Samd10 bootloader.
 *				If PA15 bootpin is held low, micro will enter USART bootloader mode.
 *				if PA15 is high, micro runs user program if there is one at new start
 *				memory. Look at APP_START for start location of user flash.
 *
 *Important pins : 	UART pins [PA25 PAD3 -> TXd, PA24 PAD2 -> RXd]
 *					Boot En Pin PA15: enabled boot on reset when DTR pin LOW. Change to PA27?
 *					USART reset pin -> RTS -> RST PIN#. Used to reset the micro when
 *					Serial is plugged in, pulse RTS LOW. Almost arduino esqe.
 *
 *Update:			fixed write_nvm function, would fall to dummy handler.
 *
 *Todo:				need to fix Verify flash function, flash contents don't match.
 *					or they seem not to.
 */ 


#include "sam.h"
//#include "nvmctrl.h"
//#include <system.h>

#define bool	_Bool

#define PORTA 0 //Samd09 only has one port Port0

/**
 * \internal Internal device instance struct
 *
 * This struct contains information about the NVM module which is
 * often used by the different functions. The information is loaded
 * into the struct in the nvm_init() function.
 */
struct _nvm_module {
	/** Number of bytes contained per page. */
	uint16_t page_size;
	/** Total number of pages in the NVM memory. */
	uint16_t number_of_pages;
	/** If \c false, a page write command will be issued automatically when the
	 *  page buffer is full. */
	bool manual_page_write;
};

/**
 * \internal Instance of the internal device struct
 */
static struct _nvm_module _nvm_dev;
/**
 * \brief NVM controller configuration structure.
 *
 * Configuration structure for the NVM controller within the device.
 */
struct nvm_config {

	bool manual_page_write;

};

volatile enum status_code{
	STATUS_OK                       = 0x00,
	STATUS_FAIL						= 0x01,
};

volatile enum status_code nvm_get_config()
{
	
	/*get a pointer to the module hardware instance.*/
	Nvmctrl *const nvm_module = NVMCTRL;
	
	/* Initialize the internal device struct */
	_nvm_dev.page_size         = (8 << nvm_module->PARAM.bit.PSZ);
	_nvm_dev.number_of_pages   = nvm_module->PARAM.bit.NVMP;

	/* If the security bit is set, the auxiliary space cannot be written */
	if (nvm_module->STATUS.reg & NVMCTRL_STATUS_SB) {
		return STATUS_FAIL;
	}
};

/* Change the following if different SERCOM and boot pins are used */
#define BOOT_SERCOM			SERCOM1		//miniSam uses Sercom1 for USART
#define BOOT_SERCOM_BAUD	115200
#define BOOT_PORT			PORTA
#define BOOT_PIN			15 //14		//PA15 for bootloader en, toggled by the python script. or DTR from serial coms.

#define div_ceil(a,b)(((a)+(b)-1)/(b))			//extracted function from samd_math.h <- something like that.

/* SERCOM USART GCLK Frequency */
#define SERCOM_GCLK		8000000UL		//processor speed.
#define BAUD_VAL	(65536.0*(1.0-((float)(16.0*(float)BOOT_SERCOM_BAUD)/(float)SERCOM_GCLK))) //calculate baud rate from SERCOM_GCLK

/* Application starts from 1kB memory - Bootloader size is 1kB */
/* Change the address if higher boot size is needed */
/*good site for quick conversions.*/
/*http://www.binaryhexconverter.com/hex-to-decimal-converter*/
#define APP_START	0x00000800 //This gives 1536 bytes of bootloader space.

/* Target application size can be 15kB */
/* APP_SIZE is the application section size in kB */
/* Change as per APP_START */
#define APP_SIZE	13	//This is how much flash memory is left for the application.

/* Memory pointer for flash memory */
#define NVM_MEMORY			((volatile uint16_t *)FLASH_ADDR)
#define NVM_USER_MEMORY		((volatile uint16_t *)NVMCTRL_USER)

uint8_t data_8 = 1;
uint32_t file_size, i, dest_addr;
uint32_t volatile app_start_address;
uint32_t *flash_ptr;

//Version information.
uint8_t aVER[78] = {'m','i','n','i','S','a','m','d',' ','R','1','.','2',
					'\n','b','o','o','t','l','o','a','d','e','r',' ','V','0','.','1','\n',
					'D','e','v',' ','B','o','a','r','d',' ','r','e','g','i','s','t','e','r',
					'e','d',' ','t','o',' ','J','e','r','e','m','y',' ','G','\n',
					'B','o','a','r','d',' ','I','D',' ','0','x','0','0','1','\n'};

/*pin pad setup for SERCOM1 and USART*/
static inline void pin_set_peripheral_function(uint32_t pinmux)
{
    /* the variable pinmux consist of two components:
        31:16 is a pad, wich includes:
            31:21 : port information 0->PORTA, 1->PORTB
            20:16 : pin 0-31
        15:00 pin multiplex information
        there are defines for pinmux like: PINMUX_PA09D_SERCOM2_PAD1 
    */
    uint16_t pad = pinmux >> 16;    // get pad (port+pin)
    uint8_t port = pad >> 5;        // get port
    uint8_t pin  = pad & 0x1F;      // get number of pin - no port information anymore
    
    PORT->Group[port].PINCFG[pin].bit.PMUXEN =1;
    
    /* each pinmux register is for two pins! with pin/2 you can get the index of the needed pinmux register
       the p mux resiter is 8Bit   (7:4 odd pin; 3:0 evan bit)  */
    // reset pinmux values.                             VV shift if pin is odd (if evan:  (4*(pin & 1))==0  )
    PORT->Group[port].PMUX[pin/2].reg &= ~( 0xF << (4*(pin & 1)) );
                    //          
    // set new values
    PORT->Group[port].PMUX[pin/2].reg |=  ( (uint8_t)( (pinmux&0xFFFF) <<(4*(pin&1)) ) ); 
}

/*init USART module on SERCOM1*/
void UART_sercom_init()
{
	
	//Pmux eve = n/1, odd = (n-1)/2
	pin_set_peripheral_function(PINMUX_PA25C_SERCOM1_PAD3); // SAMD09 TX
	pin_set_peripheral_function(PINMUX_PA24C_SERCOM1_PAD2); // SAMD09 RX
	
	//apbcmak
	PM->APBCMASK.reg |= PM_APBCMASK_SERCOM1;
	
	//gclk config
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(SERCOM1_GCLK_ID_CORE) | GCLK_CLKCTRL_GEN(0) | GCLK_CLKCTRL_CLKEN;
	
	//Config SERCOM1 module for UART
	SERCOM1->USART.CTRLA.reg = SERCOM_USART_CTRLA_MODE_USART_INT_CLK | SERCOM_USART_CTRLA_DORD | SERCOM_USART_CTRLA_RXPO(0x3) | SERCOM_USART_CTRLA_TXPO(0x1);
	
	SERCOM1->USART.CTRLB.reg = SERCOM_USART_CTRLB_RXEN | SERCOM_USART_CTRLB_TXEN | SERCOM_USART_CTRLB_CHSIZE(0);
	
	SERCOM1->USART.BAUD.reg = BAUD_VAL;//65535.0f * (1.0f - (float)(16*(float)(9600)/(USART_BAUD_MODIFIER_SLOW))); //This gets the miniSam exactly at 9800 baud.
	/* for 115200 baud compiler does not like this.*/
	//SERCOM1->USART.BAUD.reg = 65535.0f * (1.0f - (float)(16*(float)(USART_BAUD_MODIFIER_FAST)/(8000000)));
	
	SERCOM1->USART.CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;
	
}

/* interrupt handler for Sercom1 USART */
void SERCOM1_Handler()  // SERCOM1 ISR
{
	uint8_t buffer;
	buffer  = SERCOM1->USART.DATA.reg;
	while(!(SERCOM1->USART.INTFLAG.reg & 1)); // wait UART module ready to receive data
	SERCOM1->USART.DATA.reg = buffer;               // just sent that byte aback
	while(!(SERCOM1->USART.INTFLAG.reg & 2)); // wait until TX complete;
}	

//this will be replaced with UART_sercom_simpleWrite function.
void uart_write_byte(uint8_t data)
{
	while(!BOOT_SERCOM->USART.INTFLAG.bit.DRE);
	BOOT_SERCOM->USART.DATA.reg = (uint16_t)data;
	
}

void UART_sercom_simpleWrite(Sercom *const sercom_module, uint8_t data)
{
	while(!(sercom_module->USART.INTFLAG.reg & 1)); //wait UART module ready to receive data
	sercom_module->USART.DATA.reg = data;
	while(!(sercom_module->USART.INTFLAG.reg & 2)); //wait until TX complete;
}

//this will be replaced with UART_sercom_simpleRead function.
uint8_t uart_read_byte(void)
{
	while(!BOOT_SERCOM->USART.INTFLAG.bit.RXC);
	return((uint8_t)(BOOT_SERCOM->USART.DATA.reg & 0x00FF));
}


void nvm_erase_row(const uint32_t row_address, uint32_t PAGE_SIZE)
{
	/* Check if the address to erase is not aligned to the start of a row */
	if(row_address > ((uint32_t)_nvm_dev.page_size * _nvm_dev.number_of_pages))
	{
		return 0;
	}

	/* Get a pointer to the module hardware instance */
	if(row_address & ((_nvm_dev.page_size * NVMCTRL_ROW_PAGES)-1))
	{
		return 0;
	}
	
	
	/* Check if the module is busy */
	while(!NVMCTRL->INTFLAG.bit.READY);
	
	/* Clear error flags */
	NVMCTRL->STATUS.reg &= ~NVMCTRL_STATUS_MASK;
	

	/* Set address and command *//*
	NVMCTRL->ADDR.reg  = (uintptr_t)&NVM_MEMORY[row_address /2 ];
	
	NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMD_ER | NVMCTRL_CTRLA_CMDEX_KEY;
	
	while(!NVMCTRL->INTFLAG.bit.READY);
	*/
	
	while(!(NVMCTRL->INTFLAG.bit.READY));
		
	NVMCTRL->ADDR.reg = (row_address / 2);
	NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_ER;
	while(!(NVMCTRL->INTFLAG.bit.READY));
	//row_address += _nvm_dev.page_size * 4; //skipping a row.
	
	return 1;
}


void nvm_write_buffer(uint32_t destination_address, const uint8_t *buffer, uint16_t length)
{
	
	/* Check if the destination address is valid */
	if (destination_address >
	((uint32_t)_nvm_dev.page_size * _nvm_dev.number_of_pages)) {
		return 0;
	}
	
	/* Check if the write address not aligned to the start of a page */
	if (destination_address & (_nvm_dev.page_size - 1)) {
		return 0;
	}
	
	/* Check if the write length is longer than a NVM page */
	if (length > _nvm_dev.page_size) {
		return 0;
	}

	/* Check if the module is busy */
	while(!NVMCTRL->INTFLAG.bit.READY);
	
	//set auto page writes
	NVMCTRL->CTRLB.bit.MANW = 0;

	/* Erase the page buffer before buffering new data */
	NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMD_PBC | NVMCTRL_CTRLA_CMDEX_KEY;

	/* Check if the module is busy */
	while(!NVMCTRL->INTFLAG.bit.READY);

	/* Clear error flags */
	NVMCTRL->STATUS.reg &= ~NVMCTRL_STATUS_MASK;

	uint32_t nvm_address = destination_address / 2;

	/* NVM _must_ be accessed as a series of 16-bit words, perform manual copy
	 * to ensure alignment */
	for (uint16_t k = 0; k < length; k += 2) 
	{
		uint16_t data;
		
		/* Copy first byte of the 16-bit chunk to the temporary buffer */
		data = buffer[k];
		
		/* If we are not at the end of a write request with an odd byte count,
		 * store the next byte of data as well */
		if (k < (length - 1)) {
			data |= (buffer[k + 1] << 8);
		}
		/* Store next 16-bit chunk to the NVM memory space */
		NVM_MEMORY[nvm_address++] = data;
		NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_WP;

	}
	
	/* If automatic page write mode is enable, then perform a manual NVM
	 * write when the length of data to be programmed is less than page size
	 */
	if ((_nvm_dev.manual_page_write == 0) && (length < NVMCTRL_PAGE_SIZE)) {
		NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_WP;
	}
	
	while(!NVMCTRL->INTFLAG.bit.READY);
}

int main(void)
{ 
	/* Check if boot pin is held low - Jump to application if boot pin is high */
	//PORT->Group[BOOT_PORT].OUTSET.reg = (1u << BOOT_PIN); //<- works without this definition.
	volatile uint32_t sp;
	PORT->Group[BOOT_PORT].PINCFG[BOOT_PIN].reg = PORT_PINCFG_INEN | PORT_PINCFG_PULLEN;
	if ((PORT->Group[BOOT_PORT].IN.reg & (1u << BOOT_PIN)))
	{
		//__disable_irg();
		
		app_start_address = *(uint32_t *)(APP_START + 4);
		
		//get the current stack pointer location.
		sp = __get_MSP();
		/* Rebase the Stack Pointer */
		__set_MSP(*(uint32_t *) APP_START + 4);

		/* Rebase the vector table base address */
		SCB->VTOR = ((uint32_t) APP_START & SCB_VTOR_TBLOFF_Msk);

		/* Make CPU to run at 8MHz by clearing prescalar bits */ 
		SYSCTRL->OSC8M.bit.PRESC = 0;
		NVMCTRL->CTRLB.bit.CACHEDIS = 0;

		/* Jump to application Reset Handler in the application */
		asm("bx %0"::"r"(app_start_address));
	}
	REG_PORT_DIR0 |= (1 << 14);
	REG_PORT_OUT0 |= (1<<14);
	
	/* Make CPU to run at 8MHz by clearing prescalar bits */ 
    SYSCTRL->OSC8M.bit.PRESC = 0;
	NVMCTRL->CTRLB.bit.CACHEDIS = 1;
	
	/* Config Usart */
	nvm_get_config();
	
	/* Flash page size is 64 bytes */
	#define PAGE_SIZE	_nvm_dev.page_size	//used to read and write to flash.
	uint8_t page_buffer[PAGE_SIZE];
	
	UART_sercom_init();
	//info();
    while (1) 
    {
        data_8 = uart_read_byte();
		if (data_8 == '#')
		{
			uart_write_byte('s');
			uart_write_byte((uint8_t)APP_SIZE);
			//uart_write_byte('\n');
		}
		else if (data_8 == 'e')
		{
			/*this has been fixed, it no longer fails to 
			a dummy handler*/
			//erase from 0x800 to the top of nvm.
			for(i = APP_START; i < FLASH_SIZE; i = i + 256)
			{
				nvm_erase_row(i,PAGE_SIZE);
			}
			dest_addr = APP_START;
			flash_ptr = APP_START;
			uart_write_byte('s');
			//uart_write_byte('\n');
		}
		else if (data_8 == 'p')
		{
			
			uart_write_byte('s');
			//uart_write_byte('\n');
			for (i = 0; i < _nvm_dev.page_size; i++)
			{
				page_buffer[i] = uart_read_byte();
			}
			nvm_write_buffer(dest_addr, page_buffer, _nvm_dev.page_size);
			dest_addr += _nvm_dev.page_size;
			//uart_write_byte('\n');
			uart_write_byte('s');
			REG_PORT_OUT0 &= ~(1<<14); //blinks light
			//uart_write_byte('\n');

		}
		else if (data_8 == 'v')
		{
			/* now we get stuck here... varifing pages fails on the first page.
			don't know why.*/
			//uart_write_byte('\n');
			uart_write_byte('s');
			//uart_write_byte('\n');
			for (i = 0; i < (_nvm_dev.page_size); i++)
			{
				flash_ptr = APP_START;
				//app_start_address = *flash_ptr;
				//uart_write_byte((uint8_t)app_start_address);
				//UART_sercom_simpleWrite(SERCOM1,(uint8_t)(flash_ptr >> 8));
				uart_write_byte((uint8_t)(*flash_ptr >> 8));
				//UART_sercom_simpleWrite(SERCOM1,(uint8_t)(flash_ptr >> 16));
				uart_write_byte((uint8_t)(*flash_ptr >> 16));
				//UART_sercom_simpleWrite(SERCOM1,(uint8_t)(flash_ptr >> 24));
				uart_write_byte((uint8_t)(*flash_ptr >> 24));
				flash_ptr++;
			}
		}
		//set values, for flash pointer.
		else if (data_8 == 'i')
		{
			info();
		}
    }
}

void info()
{
	uint8_t i;
	
	for(i = 0;i<=78-1;i++)
	{
		UART_sercom_simpleWrite(SERCOM1,aVER[i]);	
	}
}

////////////////////////////////////////////
/* NOTES old functions kept for reference.*/
////////////////////////////////////////////


//Address-based interworking uses the lowest bit of the address to determine the instruction 
//set at the target. If the lowest bit is 1, the branch will switch to Thumb state. If the lowest 
//bit is 0, the branch will switch to ARM state. Note that the lowest bit is never actually used as 
//part of the address as all instructions are either 4-byte aligned (as in ARM) or 2-byte aligned (as in Thumb).

//kept as a good reminder for what pads are attached to what pins.
/*
enum uart_pad_settings {
	UART_RX_PAD0_TX_PAD2 = SERCOM_USART_CTRLA_RXPO(0) | SERCOM_USART_CTRLA_TXPO(1),
	UART_RX_PAD1_TX_PAD2 = SERCOM_USART_CTRLA_RXPO(1) | SERCOM_USART_CTRLA_TXPO(1),
	UART_RX_PAD2_TX_PAD0 = SERCOM_USART_CTRLA_RXPO(2),
	UART_RX_PAD3_TX_PAD0 = SERCOM_USART_CTRLA_RXPO(3),
	UART_RX_PAD1_TX_PAD0 = SERCOM_USART_CTRLA_RXPO(1),
	UART_RX_PAD3_TX_PAD2 = SERCOM_USART_CTRLA_RXPO(3) | SERCOM_USART_CTRLA_TXPO(1),
};
*/

/*
void uart_init(uint16_t baud_val, enum uart_pad_settings pad_conf)
{
	/* Enable & configure alternate function D for pins PA10 & PA11 */
	/* Change following 3 lines if different SERCOM/SERCOM pins are used *//*
	PORT->Group[0].WRCONFIG.reg = 0x53010C00;
	PM->APBCMASK.reg |= (1u << 4);
	GCLK->CLKCTRL.reg = 0x4010;
	
	BOOT_SERCOM->USART.CTRLA.reg = pad_conf | SERCOM_USART_CTRLA_MODE(1) | SERCOM_USART_CTRLA_DORD;
	BOOT_SERCOM->USART.CTRLB.reg = SERCOM_USART_CTRLB_RXEN | SERCOM_USART_CTRLB_TXEN | SERCOM_USART_CTRLB_CHSIZE(0);
	while(BOOT_SERCOM->USART.SYNCBUSY.bit.CTRLB);
	BOOT_SERCOM->USART.BAUD.reg = baud_val;
	BOOT_SERCOM->USART.CTRLA.bit.ENABLE = 1;
	while(BOOT_SERCOM->USART.SYNCBUSY.bit.ENABLE);
}
*/

