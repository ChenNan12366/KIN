#include "soil.h"
#include "adc_filter.h"

void Soil_Init(void)
{
}

uint16_t Soil_ReadPercent(void)
{
    return ADC_ReadPercent_Filtered(ADC_CH_SOIL, 19, 20);  /* α=0.95 */
}

