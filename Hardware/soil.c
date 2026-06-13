#include "soil.h"
#include "bsp_adc.h"

static uint16_t soil_filtered = 0;

void Soil_Init(void)
{
    soil_filtered = adc_values[0];
}

/**
 * @brief  获取土壤湿度百分比（含一阶低通滤波）
 * @note   α = 0.95（强滤波，适合缓慢变化的土壤湿度）
 */
uint16_t Soil_ReadPercent(void)
{
    uint16_t raw = adc_values[0];
    soil_filtered = (19 * soil_filtered + raw) / 20;   // 19:1 滤波
    return (soil_filtered * 100) / 4095;
}