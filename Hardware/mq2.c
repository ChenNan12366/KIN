#include "mq2.h"
#include "adc_filter.h"

void MQ2_Init(void)
{
}

uint16_t MQ2_ReadPercent(void)
{
    return ADC_ReadPercent_Filtered(ADC_CH_MQ2, 1, 2);  /* α=0.5（快速响应） */
}

