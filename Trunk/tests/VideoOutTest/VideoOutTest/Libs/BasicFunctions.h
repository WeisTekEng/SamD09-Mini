/*
 * BasicFunctions.h
 *
 * Created: 6/25/2016 10:51:17 AM
 *  Author: Weistek Engineering (Jeremy G)
 */ 

#ifndef BASICFUNCTIONS_H
#define BASICFUNCTIONS_H

//Twiddle some bits.
#define setPin(pin)(REG_PORT_OUT0) |= (1 << (pin))
#define clearPin(pin)(REG_PORT_OUT0) &= ~(1 << (pin))
#define setDir(pin)(REG_PORT_DIR0) &= ~(1 << (pin))


//init
void init_TC1(void);
void TC1_Handler();

//timers and wait
void wait(uint32_t	count);
#endif