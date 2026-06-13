/**
 * @file    key.c
 * @brief   外部中断 EXTI + 软件消抖
 */

#include "key.h"
#include "delay.h"

static volatile uint8_t short_flag = 0;    // PC13 短按标志
static volatile uint8_t clr_flag   = 0;    // PA0 短按标志（清日志）

/* 快速软件消抖：连续读取两次确认电平稳定（利用 GPIO 施密特触发器 + 指令间隙） */
static uint8_t Debounce_ReadPin(GPIO_TypeDef *port, uint16_t pin)
{
    uint8_t r1 = (GPIO_ReadInputDataBit(port, pin) == 0);
    uint8_t r2 = (GPIO_ReadInputDataBit(port, pin) == 0);
    return r1 && r2;  /* 两次读取均为低，确认为有效按下 */
}

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
        /* 快速双重采样 + 硬件施密特触发器消抖，避免 20ms 阻塞 */
        if (Debounce_ReadPin(GPIOC, GPIO_Pin_13))
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
        /* 快速双重采样 + 硬件施密特触发器消抖，避免 20ms 阻塞 */
        if (Debounce_ReadPin(GPIOA, GPIO_Pin_0))
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
