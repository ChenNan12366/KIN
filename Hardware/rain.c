#include "rain.h"
#include "bsp_adc.h"

static uint16_t rain_filtered = 0;

void Rain_Init(void)
{
    rain_filtered = adc_values[2];
}

uint16_t Rain_ReadPercent(void)
{
    uint16_t raw = adc_values[2];
    rain_filtered = (19 * rain_filtered + raw) / 20;
    return (rain_filtered * 100) / 4095;
}
