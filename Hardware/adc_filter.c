/**
 * @file    adc_filter.c
 * @brief   ADC 通用滤波模块实现
 */

#include "adc_filter.h"
#include "bsp_adc.h"

static uint16_t filter_state[4] = {0};

void ADC_Filter_Init(void)
{
    for (uint8_t i = 0; i < 4; i++)
        filter_state[i] = adc_values[i];
}

uint16_t ADC_ReadPercent_Filtered(uint8_t channel, uint8_t alpha_num, uint8_t alpha_denom)
{
    if (channel >= 4)
        return 0;

    uint16_t raw = adc_values[channel];
    uint8_t alpha_inv = alpha_denom - alpha_num;

    /* 一阶低通滤波：out = (alpha * out + (1-alpha) * raw) / 1
       其中 alpha = alpha_num / alpha_denom */
    filter_state[channel] = (alpha_num * filter_state[channel] + alpha_inv * raw) / alpha_denom;

    return (filter_state[channel] * 100) / 4095;
}
