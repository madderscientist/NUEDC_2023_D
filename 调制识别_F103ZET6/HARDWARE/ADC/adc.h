#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"
void Adc_Init(uint32_t speed);
void RMS_AD_Init(void);
uint16_t RMS_AD_GetValue(void);
float RMS(void);
#endif
