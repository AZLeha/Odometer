#ifndef DYNAMOMETERS_H_
#define DYNAMOMETERS_H_

#include <stdbool.h>
#include <stdint.h>



void Dynamometers_init(void);
bool Dynamometers_isDataReady(void);
void Dynamometers_StartMeasurement(void);
uint32_t Dynamometers_Data1(void);
uint32_t Dynamometers_Data2(void);


#endif /* DYNAMOMETERS_H_ */
