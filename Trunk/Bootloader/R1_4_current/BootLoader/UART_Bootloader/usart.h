/*
 *usart.h
 *Project:			miniSam USART bootloader.
 *Author:			Weistek Engineering (jeremy G.)
 *website:			www.weistekengineering.com
 *date:				07-11-2016
 *Summery:			setup usart functions, read and write functions.
 */ 

#ifndef USART_H_
#define USART_H_

void sendConfirm();

/*pin pad setup for SERCOM1 and USART*/
void pin_set_peripheral_function(uint32_t pinmux);

/*init USART module on SERCOM1*/
void UART_sercom_init();

//this will be replaced with UART_sercom_simpleWrite function.
void uart_write_byte(uint8_t data);

uint8_t uart_read_byte(void);



#endif /* USART_H_ */