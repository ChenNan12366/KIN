/**
 * @file    adc_filter.h
 * @brief   ADC 通用滤波模块 - 消除各传感器重复的滤波代码
 */

#ifndef __ADC_FILTER_H
#define __ADC_FILTER_H
#include <stdint.h>

/* ADC 通道索引常量 */
#define ADC_CH_SOIL     0   /* PA4  - 土壤湿度传感器 */
#define ADC_CH_POT      1   /* PA7  - 电位器         */
#define ADC_CH_RAIN     2   /* PB0  - 雨量传感器     */
#define ADC_CH_MQ2      3   /* PB1  - 烟雾传感器     */

/**
 * @brief  ADC 滤波读取百分比
 * @param  channel: ADC 通道 (0-3)，建议使用 ADC_CH_SOIL 等宏
 * @param  alpha_num: 滤波系数分子（建议值 1-19）
 * @param  alpha_denom: 滤波系数分母（通常为 2 或 20）
 * @return 百分比值 (0-100)
 * @note   滤波公式: out = (alpha_num * out + (denom - alpha_num) * raw) / denom
 *         alpha_num=19, denom=20 => α=0.95（强滤波）
 *         alpha_num=17, denom=20 => α=0.85（中滤波）
 *         alpha_num=1,  denom=2  => α=0.5（弱滤波/快速响应）
 */
uint16_t ADC_ReadPercent_Filtered(uint8_t channel, uint8_t alpha_num, uint8_t alpha_denom);

/**
 * @brief  初始化各 ADC 通道的滤波状态
 * @note   在所有传感器模块之前调用一次
 */
void ADC_Filter_Init(void);

#endif
