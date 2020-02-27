
#ifndef PERIPHERAL_H_
#define PERIPHERAL_H_
#include "stm32f10x.h"

//DWT macro
#define    DWT_CYCCNT    *(volatile unsigned long *)0xE0001004
#define    DWT_CONTROL   *(volatile unsigned long *)0xE0001000
#define    SCB_DEMCR     *(volatile unsigned long *)0xE000EDFC

void InitExternalInterrupt(void);
void InitTimer(void);
void InitDWT(void);



#endif /* PERIPHERAL_H_ */
