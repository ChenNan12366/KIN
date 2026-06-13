#include "w25q64.h"
#include "bsp_spi.h"

#define W25Q64_TIMEOUT  1000000   /* 超时计数器最大值 */

void W25Q64_Init(void)
{
    SPI2_Init();
    W25Q64_CS_H();
}

uint32_t W25Q64_ReadID(void)
{
    uint32_t id = 0;
    W25Q64_CS_L();
    SPI2_SendByte(W25Q64_CMD_JEDEC_ID);
    id |= (uint32_t)SPI2_SendByte(0xFF) << 16;
    id |= (uint32_t)SPI2_SendByte(0xFF) << 8;
    id |= SPI2_SendByte(0xFF);
    W25Q64_CS_H();
    return id;
}

static void WriteEnable(void)
{
    W25Q64_CS_L();
    SPI2_SendByte(W25Q64_CMD_WRITE_ENABLE);
    W25Q64_CS_H();
}

/**
 * @brief  等待芯片空闲（带超时保护）
 * @return 0=就绪, 1=超时
 */
static uint8_t WaitBusy(void)
{
    uint32_t timeout = W25Q64_TIMEOUT;
    uint8_t status;
    do {
        W25Q64_CS_L();
        SPI2_SendByte(W25Q64_CMD_READ_STATUS);
        status = SPI2_SendByte(0xFF);
        W25Q64_CS_H();
        if (--timeout == 0) return 1;  /* 超时，返回错误 */
    } while (status & 0x01);
    return 0;
}

void W25Q64_ReadData(uint32_t addr, uint8_t *buf, uint32_t len)
{
    uint32_t i;
    W25Q64_CS_L();
    SPI2_SendByte(W25Q64_CMD_READ_DATA);
    SPI2_SendByte((addr >> 16) & 0xFF);
    SPI2_SendByte((addr >> 8) & 0xFF);
    SPI2_SendByte(addr & 0xFF);
    for (i = 0; i < len; i++)
        buf[i] = SPI2_SendByte(0xFF);
    W25Q64_CS_H();
}

uint8_t W25Q64_PageProgram(uint32_t addr, uint8_t *buf, uint16_t len)
{
    uint16_t i;
    if (len > 256) len = 256;
    WriteEnable();
    W25Q64_CS_L();
    SPI2_SendByte(W25Q64_CMD_PAGE_PROGRAM);
    SPI2_SendByte((addr >> 16) & 0xFF);
    SPI2_SendByte((addr >> 8) & 0xFF);
    SPI2_SendByte(addr & 0xFF);
    for (i = 0; i < len; i++)
        SPI2_SendByte(buf[i]);
    W25Q64_CS_H();
    return WaitBusy();  /* 0=成功, 1=超时 */
}

uint8_t W25Q64_SectorErase(uint32_t addr)
{
    WriteEnable();
    W25Q64_CS_L();
    SPI2_SendByte(W25Q64_CMD_SECTOR_ERASE);
    SPI2_SendByte((addr >> 16) & 0xFF);
    SPI2_SendByte((addr >> 8) & 0xFF);
    SPI2_SendByte(addr & 0xFF);
    W25Q64_CS_H();
    return WaitBusy();  /* 0=成功, 1=超时 */
}
