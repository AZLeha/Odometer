
#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include <stdbool.h>


void UART_Init(void);
bool TransmitDataViaDMA(void * aData, uint16_t dataSize);


#endif /* UART_H_ */
