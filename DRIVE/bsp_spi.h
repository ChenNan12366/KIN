/**
 * @file    bsp_spi.h
 * @brief   SPI2 硬件驱动（用于 W25Q64）
 */

#ifndef __BSP_SPI_H
#define __BSP_SPI_H
#include "stm32f10x.h"

void SPI2_Init(void);
uint8_t SPI2_SendByte(uint8_t data);

#endif