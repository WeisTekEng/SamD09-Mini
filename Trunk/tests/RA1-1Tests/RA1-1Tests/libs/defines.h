/*
 * defines.h
 *
 * Created: 6/28/2016 8:23:46 PM
 *  Author: ocybr
 */ 


#ifndef DEFINES_H_
#define DEFINES_H_

//#define MY_BAUDREG_VALUE 64278    // 64278 (F_CPU=F_SERCOM=8MHz) -> 9600 BAUD
#define MY_BAUDREG_VALUE 55470      // 55470 (F_CPU=F_SERCOM=1MHz) -> 9600 BAUD
#define USART_BAUD_MODIFIER_SLOW = 1048553;
#define USART_BAUD_MODIFIER_FAST = 115199;



#endif /* DEFINES_H_ */