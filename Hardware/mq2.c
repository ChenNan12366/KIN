#include "mq2.h"
#include "bsp_adc.h"

static uint16_t mq2_filtered = 0;

void MQ2_Init(void)
{
    mq2_filtered = adc_values[3];
}

uint16_t MQ2_ReadPercent(void)
{
    uint16_t raw = adc_values[3];
    mq2_filtered = (19 * mq2_filtered + raw) / 20;
    return (mq2_filtered * 100) / 4095;
}