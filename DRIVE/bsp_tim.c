/**
 * @file    bsp_tim.c
 * @brief   TIM2 提供 1ms 节拍，通道2 内部触发 ADC（PA1 被手动恢复为 GPIO）
 */
#include "bsp_tim.h"

volatile uint32_t timer_tick_ms = 0;

void BSP_TIM_Init(void)
{
    TIM_TimeBaseInitTypeDef tim;
    TIM_OCInitTypeDef oc;
    NVIC_InitTypeDef nvic;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    tim.TIM_Period        = 999;
    tim.TIM_Prescaler     = 71;
    tim.TIM_ClockDivision = 0;
    tim.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &tim);

    /* 通道2 使能输出（产生内部触发信号给 ADC），但 PA1 会被短暂占用 */
    oc.TIM_OCMode       = TIM_OCMode_PWM1;
    oc.TIM_OutputState  = TIM_OutputState_Enable;   // 关键：必须使能才能触发 ADC
    oc.TIM_Pulse        = 500;
    oc.TIM_OCPolarity   = TIM_OCPolarity_High;
    TIM_OC2Init(TIM2, &oc);

    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM2, ENABLE);

    nvic.NVIC_IRQChannel                   = TIM2_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority  = 1;
    nvic.NVIC_IRQChannelSubPriority         = 0;
    nvic.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&nvic);
}

void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        timer_tick_ms++;
    }
}
