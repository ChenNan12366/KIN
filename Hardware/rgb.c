/**
 * @file    rgb.c
 * @brief   PA1=红, PA2=绿, PA3=蓝  共阳极低电平有效
 */

#include "rgb.h"

/* 引脚定义 */
#define RGB_R_PORT  GPIOA
#define RGB_R_PIN   GPIO_Pin_1
#define RGB_G_PORT  GPIOA
#define RGB_G_PIN   GPIO_Pin_2
#define RGB_B_PORT  GPIOA
#define RGB_B_PIN   GPIO_Pin_3

#define RGB_ALL_PIN (RGB_R_PIN | RGB_G_PIN | RGB_B_PIN)

/**
 * @brief  初始化 RGB 引脚，全部拉高（灭）
 */
void RGB_Init(void)
{
    GPIO_InitTypeDef gpio;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    gpio.GPIO_Pin   = RGB_ALL_PIN;
    gpio.GPIO_Mode  = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

    /* 共阳极：全部拉高 = 全灭 */
    GPIO_SetBits(GPIOA, RGB_ALL_PIN);
}

/**
 * @brief  设置 RGB 颜色（使用 BSRR 寄存器原子操作，消除闪烁）
 * @param  color  颜色枚举值
 * @note   共阳极：BSRR 低 16 位置位（SetBits 灭），高 16 位复位（ResetBits 亮）
 */
void RGB_SetColor(RGB_Color color)
{
    uint16_t set_mask   = 0;  /* BSRR 低 16 位：置高 → 灭 */
    uint16_t reset_mask = 0;  /* BSRR 高 16 位：拉低 → 亮 */

    switch (color)
    {
        case RGB_RED:
            reset_mask = RGB_R_PIN;                         /* 红亮 */
            set_mask   = RGB_G_PIN | RGB_B_PIN;             /* 绿、蓝灭 */
            break;

        case RGB_GREEN:
            reset_mask = RGB_G_PIN;                         /* 绿亮 */
            set_mask   = RGB_R_PIN | RGB_B_PIN;             /* 红、蓝灭 */
            break;

        case RGB_BLUE:
            reset_mask = RGB_B_PIN;                         /* 蓝亮 */
            set_mask   = RGB_R_PIN | RGB_G_PIN;             /* 红、绿灭 */
            break;

        case RGB_YELLOW:
            reset_mask = RGB_R_PIN | RGB_G_PIN;             /* 红+绿=黄亮 */
            set_mask   = RGB_B_PIN;                         /* 蓝灭 */
            break;

        case RGB_WHITE:
            reset_mask = RGB_ALL_PIN;                       /* 全亮 */
            set_mask   = 0;                                 /* 无 */
            break;

        case RGB_OFF:
        default:
            reset_mask = 0;                                 /* 无 */
            set_mask   = RGB_ALL_PIN;                       /* 全灭 */
            break;
    }

    /* 单次 BSRR 原子写入，消除"先全灭再亮"的中间闪烁态 */
    GPIOA->BSRR = set_mask | ((uint32_t)reset_mask << 16);
}
