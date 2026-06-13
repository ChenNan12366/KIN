/**
 * @file    delay.c
 * @brief   DWT 微秒/毫秒延时，不依赖任何中断
 */
#include "delay.h"

#define DWT_CTRL    (*(volatile uint32_t*)0xE0001000)
#define DWT_CYCCNT  (*(volatile uint32_t*)0xE0001004)
#define DEM_CR      (*(volatile uint32_t*)0xE000EDFC)

void delay_init(void)
{
    DEM_CR |= (1 << 24);        /* 使能 DWT */
    DWT_CYCCNT = 0;
    DWT_CTRL |= 1;              /* 使能 CYCCNT */
}

/**
 * @brief  微秒级精确延时
 */
void delay_us(uint32_t us)
{
    uint32_t start = DWT_CYCCNT;
    uint32_t ticks = us * (SystemCoreClock / 1000000);
    while ((DWT_CYCCNT - start) < ticks);
}

/**
 * @brief  毫秒级延时（基于 DWT，不阻塞中断响应）
 */
void delay_ms(uint32_t ms)
{
    delay_us(ms * 1000);
}
