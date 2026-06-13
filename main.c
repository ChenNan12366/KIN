/**
 * @file    main.c
 * @brief   智能环境监测终端 - 主程序
 * @note    OLED 三页显示、按键交互、报警、Flash 存储、串口打印
 *          PA1 在 TIM2 初始化后被手动恢复为 GPIO，确保 RGB 正常。
 */

#include "stm32f10x.h"
#include "delay.h"
#include "oled.h"
#include "key.h"
#include "bsp_adc.h"
#include "bsp_usart.h"
#include "bsp_tim.h"
#include "soil.h"
#include "pot.h"
#include "rain.h"
#include "mq2.h"
#include "dht11.h"
#include "rgb.h"
#include "buzzer.h"
#include "w25q64.h"
#include "data_log.h"
#include <stdio.h>

#define PAGE_COUNT  3                    /* OLED 页面总数 */

int main(void)
{
    uint8_t  temp = 25, humi = 60;       /* 温湿度默认值 */
    uint16_t soil_pct, pot_pct, rain_pct, mq2_pct;
    uint8_t  page = 0;                   /* 当前显示页面 */
    uint32_t last_save_time    = 0;      /* 上次存储时间戳 */
    uint32_t last_print_time   = 0;      /* 上次串口打印时间戳 */
    uint32_t last_display_time = 0;      /* 上次 OLED 刷新时间戳 */
    uint8_t  flash_ok = 0;               /* Flash 是否正常 */
    char     buf[50];                     /* 通用字符串缓冲区 */

    /* ---------- 系统初始化 ---------- */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  /* 2位抢占，2位子优先级 */
    delay_init();
    BSP_TIM_Init();                       /* 启动 1ms 系统节拍 */

    /* ===== 修复 PA1 被 TIM2 占用的问题 ===== */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef gpio_fix;
    gpio_fix.GPIO_Pin   = GPIO_Pin_1;
    gpio_fix.GPIO_Mode  = GPIO_Mode_Out_PP;
    gpio_fix.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio_fix);
    GPIO_SetBits(GPIOA, GPIO_Pin_1);      /* 初始拉高，RGB 红灯灭 */

    ADC_DMA_Init();                       /* 启动 ADC + DMA 四通道扫描 */
    USART1_Init(115200);                  /* 串口1（DMA 发送） */

    OLED_Init();
    Key_Init();
    RGB_Init();
    Buzzer_Init();

    /* 强制拉高蜂鸣器 PA8，防止上电误响 */
    GPIO_InitTypeDef gpio_init;
    gpio_init.GPIO_Pin   = GPIO_Pin_8;
    gpio_init.GPIO_Mode  = GPIO_Mode_Out_PP;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio_init);
    GPIO_SetBits(GPIOA, GPIO_Pin_8);

    /* ---------- W25Q64 Flash 初始化 ---------- */
    printf("\r\n=== System Start ===\r\n");
    W25Q64_Init();
    delay_ms(10);
    uint32_t id = W25Q64_ReadID();
    printf("W25Q64 ID: 0x%08X\r\n", (unsigned int)id);
    flash_ok = (id == 0x00EF4017);
    if (flash_ok)
    {
        printf("SPI Flash OK!\r\n");
        Log_Init();
        printf("Log count: %d\r\n", Log_Count());
    }
    else
    {
        printf("SPI Flash ERROR!\r\n");
    }

    /* ---------- 传感器模块初始化 ---------- */
    Soil_Init();
    Pot_Init();
    Rain_Init();
    MQ2_Init();

    /* ---------- 启动画面 ---------- */
    OLED_Clear();
    OLED_ShowString(0, 0, "SYSTEM START");
    delay_ms(1000);
    OLED_Clear();

    /* ==================== 主循环 ==================== */
    while (1)
    {
        /* ---- 按键处理 ---- */
        if (Key_GetFlag())                      // PC13 短按
        {
            page = (page + 1) % PAGE_COUNT;
            OLED_Clear();
            printf("KEY: page=%d\r\n", page);
        }
        if (Key_LongPressed())                  // PA0 长按
        {
            W25Q64_SectorErase(0);
            Log_Init();
            flash_ok = 1;
            OLED_Clear();
            OLED_ShowString(0, 0, "Log Cleared!");
            delay_ms(1000);
            OLED_Clear();
            printf("LOG: cleared!\r\n");
        }

        /* ---- 传感器数据读取 ---- */
        if (DHT11_ReadFiltered(&temp, &humi) != 0)
        {
            temp = 25;
            humi = 60;
        }
        soil_pct = Soil_ReadPercent();
        pot_pct  = Pot_ReadPercent();
        rain_pct = Rain_ReadPercent();
        mq2_pct  = MQ2_ReadPercent();

        /* ---- 报警判断 ---- */
        if (mq2_pct > 70)
        {
            RGB_SetColor(RGB_RED);
            GPIO_ResetBits(GPIOA, GPIO_Pin_8);   /* 蜂鸣器响 */
        }
        else
        {
            RGB_SetColor(RGB_GREEN);
            GPIO_SetBits(GPIOA, GPIO_Pin_8);     /* 蜂鸣器停 */
        }

        /* ---- OLED 显示刷新（每 500ms）---- */
        if (timer_tick_ms - last_display_time >= 500)
        {
            last_display_time = timer_tick_ms;

            if (page == 0)                         /* 第0页：环境数据 */
            {
                sprintf(buf, "T:%dC H:%d%%  ", temp, humi);
                OLED_ShowString(0, 0, buf);
                sprintf(buf, "Soil: %3d%%      ", soil_pct);
                OLED_ShowString(0, 2, buf);
                sprintf(buf, "Rain: %3d%%      ", rain_pct);
                OLED_ShowString(0, 4, buf);
                sprintf(buf, "Log:%4d        ", flash_ok ? Log_Count() : 0);
                OLED_ShowString(0, 6, buf);
            }
            else if (page == 1)                    /* 第1页：控制与安全 */
            {
                sprintf(buf, "Pot : %3d%%      ", pot_pct);
                OLED_ShowString(0, 0, buf);
                sprintf(buf, "Smoke:%3d%%      ", mq2_pct);
                OLED_ShowString(0, 2, buf);
                sprintf(buf, "Security: OK    ");
                OLED_ShowString(0, 4, buf);
                sprintf(buf, "Log:%4d        ", flash_ok ? Log_Count() : 0);
                OLED_ShowString(0, 6, buf);
            }
            else if (page == 2)                    /* 第2页：系统运行时间 */
            {
                uint32_t seconds = timer_tick_ms / 1000;
                uint8_t  h = seconds / 3600;
                uint8_t  m = (seconds % 3600) / 60;
                uint8_t  s = seconds % 60;
                sprintf(buf, "UP:%02d:%02d:%02d", h, m, s);
                OLED_ShowString(0, 0, buf);
                OLED_ShowString(0, 2, "System Run Time");
                OLED_ShowString(0, 6, "Page 3/3        ");
            }
        }

        /* ---- 30 秒自动存储 ---- */
        if (flash_ok && timer_tick_ms - last_save_time >= 30000)
        {
            last_save_time = timer_tick_ms;
            Log_Save(temp, humi, soil_pct, rain_pct, mq2_pct, pot_pct);
            printf("LOG: count=%d\r\n", Log_Count());
        }

        /* ---- 串口打印（每 2 秒）---- */
        if (timer_tick_ms - last_print_time >= 2000)
        {
            last_print_time = timer_tick_ms;
            printf("T:%d H:%d | Soil:%d Rain:%d Pot:%d Smoke:%d | Page:%d\r\n",
                   temp, humi, soil_pct, rain_pct, pot_pct, mq2_pct, page);
        }
				
    }
}