/*
 *usart.h
 *Project:			SAMD09/D10 Peripheral Libraries
 *Author:			Weistek Engineering (jeremy G.)
 *website:			www.weistekengineering.com
 *date:				07-11-2016
 *Summary:			Header file for usart.c. Declares USART (SERCOM1) communication functions
 *					for SAMD09/D10, including initialization, byte read/write, and string transmission.
 *
 *updated:			07/12/2016 Added send_string function.
 */ 

#ifndef USART_H_
#define USART_H_

void sendConfirm();


void send_string(volatile char s[]);								/*sends a char array formated string over uart*/


void uart_write_byte(uint8_t data);						/*writes one byte to uart*/


int uart_read_byte(void);							/*reads one byte from uart, returns -1 on error, 0-255 on success*/


void UART_sercom_init();								/*init USART module on SERCOM1*/

#endif /* USART_H_ */