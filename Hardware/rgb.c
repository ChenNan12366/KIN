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

/**
 * @brief  初始化 RGB 引脚，全部拉高（灭）
 */
void RGB_Init(void)
{
    GPIO_InitTypeDef gpio;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    gpio.GPIO_Pin   = RGB_R_PIN | RGB_G_PIN | RGB_B_PIN;
    gpio.GPIO_Mode  = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

    /* 共阳极：全部拉高 = 全灭 */
    GPIO_SetBits(GPIOA, RGB_R_PIN | RGB_G_PIN | RGB_B_PIN);
}

/**
 * @brief  设置 RGB 颜色
 * @param  color  颜色枚举值
 * @note   共阳极：ResetBits 点亮，SetBits 熄灭
 */
void RGB_SetColor(RGB_Color color)
{
    /* 先全部拉高（灭） */
    GPIO_SetBits(GPIOA, RGB_R_PIN | RGB_G_PIN | RGB_B_PIN);

    switch (color)
    {
        case RGB_RED:
            GPIO_ResetBits(RGB_R_PORT, RGB_R_PIN);   /* 红亮 */
            break;

        case RGB_GREEN:
            GPIO_ResetBits(RGB_G_PORT, RGB_G_PIN);   /* 绿亮 */
            break;

        case RGB_BLUE:
            GPIO_ResetBits(RGB_B_PORT, RGB_B_PIN);   /* 蓝亮 */
            break;

        case RGB_YELLOW:
            GPIO_ResetBits(RGB_R_PORT, RGB_R_PIN);   /* 红+绿=黄 */
            GPIO_ResetBits(RGB_G_PORT, RGB_G_PIN);
            break;

        case RGB_WHITE:
            GPIO_ResetBits(GPIOA, RGB_R_PIN | RGB_G_PIN | RGB_B_PIN); /* 全亮 */
            break;

        case RGB_OFF:
        default:
            /* 已全灭，无需操作 */
            break;
    }
}
