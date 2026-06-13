#ifndef __DATA_LOG_H
#define __DATA_LOG_H
#include "stm32f10x.h"

#define LOG_SIZE  14

void     Log_Init(void);
void     Log_Save(uint8_t temp, uint8_t humi, uint8_t soil, uint8_t rain, uint8_t smoke, uint8_t pot);
void     Log_Flush(void);
uint16_t Log_Count(void);
void     Log_Read(uint16_t index, uint8_t *buf);

#endif