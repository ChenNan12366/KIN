/**
 * @file    bsp_wdg.h
 * @brief   IWDG 独立看门狗驱动（~4 秒超时）
 */

#ifndef __BSP_WDG_H
#define __BSP_WDG_H
#include "stm32f10x.h"

void IWDG_Init(void);
void IWDG_Feed(void);

#endif