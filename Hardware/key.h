/**
 * @file    key.h
 * @brief   按键驱动 - PC13 短按切换页面，PA0 短按清零日志
 */

#ifndef __KEY_H
#define __KEY_H
#include "stm32f10x.h"

void Key_Init(void);
uint8_t Key_GetFlag(void);          // PC13 短按（切换页面）
uint8_t Key_ClrFlag(void);          // PA0 短按（清零日志）

#endif
