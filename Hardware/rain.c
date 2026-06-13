#include "rain.h"
#include "adc_filter.h"

void Rain_Init(void)
{
}

uint16_t Rain_ReadPercent(void)
{
    return ADC_ReadPercent_Filtered(ADC_CH_RAIN, 19, 20);  /* α=0.95 */
}

