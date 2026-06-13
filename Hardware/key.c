/**
 * @file    key.c
 * @brief   外部中断 EXTI + 软件消抖
 */

#include "key.h"
#include "delay.h"

static volatile uint8_t short_flag = 0;    // PC13 短按标志
static volatile uint8_t clr_flag   = 0;    // PA0 短按标志（清日志）

/**
 * @brief  初始化按键（PC13 + PA0），配置外部中断
 */
void Key_Init(void)
{
    GPIO_InitTypeDef gpio;
    EXTI_InitTypeDef  exti;
    NVIC_InitTypeDef  nvic;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC |
                           RCC_APB2Periph_GPIOA |
                           RCC_APB2Periph_AFIO, ENABLE);

    /* PC13 上拉输入 */
    gpio.GPIO_Pin  = GPIO_Pin_13;
    gpio.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &gpio);

    /* PA0 上拉输入 */
    gpio.GPIO_Pin  = GPIO_Pin_0;
    gpio.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &gpio);

    /* ---------- PC13 → EXTI13，下降沿触发 ---------- */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource13);
    exti.EXTI_Line    = EXTI_Line13;
    exti.EXTI_Mode    = EXTI_Mode_Interrupt;
    exti.EXTI_Trigger = EXTI_Trigger_Falling;
    exti.EXTI_LineCmd = ENABLE;
    EXTI_Init(&exti);

    /* ---------- PA0 → EXTI0，下降沿触发 ---------- */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
    exti.EXTI_Line    = EXTI_Line0;
    exti.EXTI_Mode    = EXTI_Mode_Interrupt;
    exti.EXTI_Trigger = EXTI_Trigger_Falling;
    exti.EXTI_LineCmd = ENABLE;
    EXTI_Init(&exti);

    /* NVIC：EXTI15_10（PC13 在此），最高优先级 */
    nvic.NVIC_IRQChannel                   = EXTI15_10_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority  = 0;
    nvic.NVIC_IRQChannelSubPriority         = 0;
    nvic.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&nvic);

    /* NVIC：EXTI0（PA0） */
    nvic.NVIC_IRQChannel                   = EXTI0_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority  = 0;
    nvic.NVIC_IRQChannelSubPriority         = 1;
    nvic.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&nvic);
}

/**
 * @brief  EXTI15_10 中断服务函数（PC13 短按）
 */
void EXTI15_10_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line13) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line13);

        delay_us(20000);   /* 20ms 消抖 */

        if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == 0)
            short_flag = 1;
    }
}

/**
 * @brief  EXTI0 中断服务函数（PA0 清日志键）
 */
void EXTI0_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line0);

        delay_us(20000);   /* 20ms 消抖 */

        if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0)
            clr_flag = 1;
    }
}

/**
 * @brief  获取 PC13 短按标志并清除
 * @return 1 = 有按键按下
 */
uint8_t Key_GetFlag(void)
{
    if (short_flag)
    {
        short_flag = 0;
        return 1;
    }
    return 0;
}

/**
 * @brief  获取 PA0 清日志键标志并清除
 * @return 1 = 有按键按下
 */
uint8_t Key_ClrFlag(void)
{
    if (clr_flag)
    {
        clr_flag = 0;
        return 1;
    }
    return 0;
}
