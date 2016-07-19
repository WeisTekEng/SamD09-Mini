/*
 *usart.h
 *Project:			miniSam USART bootloader.
 *Author:			Weistek Engineering (jeremy G.)
 *website:			www.weistekengineering.com
 *date:				07-11-2016
 *Summery:			setup usart functions, read and write functions.
 *
 *updated:			07/12/2016 Added send_string function.
 */ 

#ifndef USART_H_
#define USART_H_

void sendConfirm();


void send_string(volatile char s[]);								/*sends a char array formated string over uart*/


void uart_write_byte(uint8_t data);						/*writes one byte to uart*/


uint8_t uart_read_byte(void);							/*reads one byte from uart*/


void pin_set_peripheral_function(uint32_t pinmux);		/*pin pad setup for SERCOM1 and USART*/


void UART_sercom_init();								/*init USART module on SERCOM1*/

#endif /* USART_H_ */