/**
 * @file    bsp_wdg.c
 * @brief   IWDG 独立看门狗 ~4 秒超时复位
 * @note    LSI ≈ 40kHz, Prescaler=128 → 312.5Hz, Reload=1250 → 4s
 */
#include "bsp_wdg.h"

void IWDG_Init(void)
{
    /* 使能 LSI 并等待就绪 */
    RCC_LSICmd(ENABLE);
    while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);

    /* 解除写保护 */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    IWDG_SetPrescaler(IWDG_Prescaler_128);    // 40kHz / 128 = 312.5 Hz
    IWDG_SetReload(1250);                     // 1250 / 312.5 = 4 秒超时

    IWDG_ReloadCounter();                     // 加载初值
    IWDG_Enable();                            // 启动看门狗
}

void IWDG_Feed(void)
{
    IWDG_ReloadCounter();
}
