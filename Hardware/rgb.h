/**
 * @file    rgb.h
 * @brief   RGB LED 驱动 - 共阳极，低电平点亮
 */

#ifndef __RGB_H
#define __RGB_H
#include "stm32f10x.h"

/** RGB 颜色枚举 */
typedef enum {
    RGB_OFF   = 0,
    RGB_RED,
    RGB_GREEN,
    RGB_BLUE,
    RGB_YELLOW,
    RGB_WHITE
} RGB_Color;

void RGB_Init(void);
void RGB_SetColor(RGB_Color color);

#endif
