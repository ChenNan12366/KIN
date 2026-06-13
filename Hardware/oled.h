#ifndef __OLED_H
#define __OLED_H
#include "stm32f10x.h"

void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowString(uint8_t x, uint8_t y, const char *str);

#endif