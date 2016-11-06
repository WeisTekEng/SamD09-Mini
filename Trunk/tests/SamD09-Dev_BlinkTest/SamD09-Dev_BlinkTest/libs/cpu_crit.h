/*
 * cpu_crit.h
 *
 * Created: 6/26/2016 10:29:18 PM
 *  Author: ocybr
 */ 


#ifndef CPU_CRIT_H_
#define CPU_CRIT_H_


#define CpuCriticalVar()  uint8_t cpuSR

#define CpuEnterCritical()              \
do {                                  \
	asm (                               \
	"MRS   R0, PRIMASK\n\t"             \
	"CPSID I\n\t"                       \
	"STRB R0, %[output]"                \
	: [output] "=m" (cpuSR) :: "r0");   \
} while(0)

#define CpuExitCritical()               \
do{                                   \
	asm (                               \
	"ldrb r0, %[input]\n\t"             \
	"msr PRIMASK,r0;\n\t"               \
	::[input] "m" (cpuSR) : "r0");      \
} while(0)


#endif /* CPU_CRIT_H_ */