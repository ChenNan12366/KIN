/**
 * @file    dht11.c
 * @brief   DHT11 温湿度传感器驱动（单线通信，PA5）
 */

#include "stm32f10x.h"
#include "delay.h"
#include "dht11.h"

static uint8_t dht11_temp_filtered = 25;
static uint8_t dht11_humi_filtered = 60;

static void DHT11_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStruct.GPIO_Pin   = DHT11_PIN;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_OD;      /* 开漏输出 */
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DHT11_PORT, &GPIO_InitStruct);

    GPIO_SetBits(DHT11_PORT, DHT11_PIN);                /* 释放总线（初始拉高） */
}

static void DHT11_Set_Output(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin   = DHT11_PIN;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_OD;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
}

static void DHT11_Set_Input(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin   = DHT11_PIN;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
}

static uint8_t DHT11_Read_Bit(void)
{
    uint8_t retry = 0;

    /* 等待总线拉低（主机拉低后释放） */
    while (GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN) && (retry++ < 100))
        delay_us(1);
    retry = 0;

    /* 等待总线拉高 */
    while (!GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN) && (retry++ < 100))
        delay_us(1);

    delay_us(30);                                        /* 延迟 30us 区分 0 和 1 */

    if (GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN))
        return 1;
    else
        return 0;
}

uint8_t DHT11_Read(uint8_t *temp, uint8_t *humi)
{
    uint8_t i, data[5];
    uint8_t checksum;

    if (temp == NULL || humi == NULL)
        return 1;

    DHT11_GPIO_Init();
    DHT11_Set_Output();

    /* 发送启动信号：拉低至少 18ms */
    GPIO_ResetBits(DHT11_PORT, DHT11_PIN);
    delay_ms(20);
    GPIO_SetBits(DHT11_PORT, DHT11_PIN);
    delay_us(30);

    DHT11_Set_Input();

    /* 等待 DHT11 响应（拉低 80us） */
    uint8_t retry = 0;
    while (GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN) && (retry++ < 100))
        delay_us(1);

    if (retry >= 100)
        return 1;  /* 无响应 */

    /* 等待 DHT11 拉高 */
    retry = 0;
    while (!GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN) && (retry++ < 100))
        delay_us(1);

    /* 读取 5 字节数据（40 bits） */
    for (i = 0; i < 5; i++)
    {
        uint8_t j;
        data[i] = 0;
        for (j = 0; j < 8; j++)
        {
            data[i] <<= 1;
            data[i] |= DHT11_Read_Bit();
        }
    }

    DHT11_Set_Output();
    GPIO_SetBits(DHT11_PORT, DHT11_PIN);                /* 释放总线 */

    /* 校验和检验：前 4 字节的和等于第 5 字节 */
    checksum = data[0] + data[1] + data[2] + data[3];
    if (checksum != data[4])
        return 1;

    *humi = data[0];                                    /* 湿度整数部分 */
    *temp = data[2];                                    /* 温度整数部分 */

    return 0;
}

uint8_t DHT11_ReadFiltered(uint8_t *temp, uint8_t *humi)
{
    uint8_t t, h;
    uint8_t ret = 1;  /* 默认失败 */

    if (DHT11_Read(&t, &h) == 0)
    {
        /* 一阶低通滤波（α=0.8） */
        dht11_temp_filtered = (4 * dht11_temp_filtered + t) / 5;
        dht11_humi_filtered = (4 * dht11_humi_filtered + h) / 5;
        ret = 0;  /* 读取成功 */
    }

    *temp = dht11_temp_filtered;
    *humi = dht11_humi_filtered;

    return ret;  /* 0=成功, 1=读取失败(返回旧值) */
}
