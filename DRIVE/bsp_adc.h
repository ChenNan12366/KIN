#ifndef __BSP_ADC_H
#define __BSP_ADC_H
#include "stm32f10x.h"

extern uint16_t adc_values[4];  // 0:土壤,1:电位器,2:雨量,3:MQ-2

void ADC_DMA_Init(void);
#endif