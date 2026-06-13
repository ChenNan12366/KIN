#include "mq2.h"
#include "bsp_adc.h"

static uint16_t mq2_filtered = 0;

void MQ2_Init(void)
{
    mq2_filtered = adc_values[3];
}

/**
 * @brief  获取烟雾浓度百分比（含一阶低通滤波）
 * @note   α = 0.5（快速响应，适用于安全报警场景）
 */
uint16_t MQ2_ReadPercent(void)
{
    uint16_t raw = adc_values[3];
    mq2_filtered = (mq2_filtered + raw) / 2;   // 1:1 快速跟踪
    return (mq2_filtered * 100) / 4095;
}
