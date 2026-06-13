#include "pot.h"
#include "bsp_adc.h"

static uint16_t pot_filtered = 0;

void Pot_Init(void)
{
    pot_filtered = adc_values[1];
}

/**
 * @brief  获取电位器百分比（含一阶低通滤波）
 * @note   α = 0.85（响应比土壤快，适合手动调节的电位器）
 */
uint16_t Pot_ReadPercent(void)
{
    uint16_t raw = adc_values[1];
    pot_filtered = (17 * pot_filtered + 3 * raw) / 20;
    return (pot_filtered * 100) / 4095;
}
