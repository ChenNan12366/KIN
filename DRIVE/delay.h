/**
 * @file    delay.h
 * @brief   延时函数 - DWT 微秒延时 + 毫秒延时
 */

#ifndef __DELAY_H
#define __DELAY_H
#include "stm32f10x.h"

void delay_init(void);
void delay_us(uint32_t us);
void delay_ms(uint32_t ms);

#endif
