#include "bsp_spi.h"

void SPI2_Init(void)
{
    GPIO_InitTypeDef gpio;
    SPI_InitTypeDef  spi;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

    /* CS (PB12) 推挽输出，初始拉高 */
    gpio.GPIO_Pin   = GPIO_Pin_12;
    gpio.GPIO_Mode  = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &gpio);
    GPIO_SetBits(GPIOB, GPIO_Pin_12);

    /* SCK (PB13) 复用推挽 */
    gpio.GPIO_Pin  = GPIO_Pin_13;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &gpio);

    /* MISO (PB14) 浮空输入 */
    gpio.GPIO_Pin  = GPIO_Pin_14;
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &gpio);

    /* MOSI (PB15) 复用推挽 */
    gpio.GPIO_Pin  = GPIO_Pin_15;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &gpio);

    spi.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;
    spi.SPI_Mode              = SPI_Mode_Master;
    spi.SPI_DataSize          = SPI_DataSize_8b;
    spi.SPI_CPOL              = SPI_CPOL_Low;          // CPOL=0
    spi.SPI_CPHA              = SPI_CPHA_1Edge;        // CPHA=0
    spi.SPI_NSS               = SPI_NSS_Soft;           // 软件控制 CS
    spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256; // 降低速率，先求稳定
    spi.SPI_FirstBit          = SPI_FirstBit_MSB;
    SPI_Init(SPI2, &spi);
    SPI_Cmd(SPI2, ENABLE);
}

uint8_t SPI2_SendByte(uint8_t data)
{
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI2, data);
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
    return (uint8_t)SPI_I2S_ReceiveData(SPI2);
}
