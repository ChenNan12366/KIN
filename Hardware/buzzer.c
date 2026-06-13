#include "buzzer.h"

#define BUZZER_PORT GPIOA
#define BUZZER_PIN  GPIO_Pin_8

void Buzzer_Init(void)
{
    GPIO_InitTypeDef gpio;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    gpio.GPIO_Pin   = BUZZER_PIN;
    gpio.GPIO_Mode  = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BUZZER_PORT, &gpio);
    GPIO_SetBits(BUZZER_PORT, BUZZER_PIN);  // 初始拉高，不响
}

void Buzzer_On(void)
{
    GPIO_ResetBits(BUZZER_PORT, BUZZER_PIN);
}

void Buzzer_Off(void)
{
    GPIO_SetBits(BUZZER_PORT, BUZZER_PIN);
}
