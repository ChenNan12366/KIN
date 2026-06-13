/**
 * @file    soil.h
 * @brief   土壤湿度传感器（PA4，ADC1_CH4）
 */

#ifndef __SOIL_H
#define __SOIL_H
#include "stm32f10x.h"

void     Soil_Init(void);
uint16_t Soil_ReadPercent(void);

#endif
