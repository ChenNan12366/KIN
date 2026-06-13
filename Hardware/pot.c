#include "pot.h"
#include "adc_filter.h"

void Pot_Init(void)
{
}

uint16_t Pot_ReadPercent(void)
{
    return ADC_ReadPercent_Filtered(ADC_CH_POT, 17, 20);  /* α=0.85 */
}

