#ifndef __BSP_USART_H
#define __BSP_USART_H
#include "stm32f10x.h"

void USART1_Init(uint32_t baud);
void USART1_SendBytes(const uint8_t *data, uint16_t len);
void USART1_SendString(const char *str);
#endif
