#ifndef __BSP_TIM_H
#define __BSP_TIM_H
#include "stm32f10x.h"

extern volatile uint32_t timer_tick_ms;

void BSP_TIM_Init(void);
#endif
