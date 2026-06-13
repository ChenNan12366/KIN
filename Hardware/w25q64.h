#ifndef __W25Q64_H
#define __W25Q64_H
#include "stm32f10x.h"

/* 指令表 */
#define W25Q64_CMD_WRITE_ENABLE    0x06
#define W25Q64_CMD_READ_STATUS     0x05
#define W25Q64_CMD_PAGE_PROGRAM    0x02
#define W25Q64_CMD_SECTOR_ERASE    0x20
#define W25Q64_CMD_READ_DATA       0x03
#define W25Q64_CMD_JEDEC_ID        0x9F

/* 片选宏 */
#define W25Q64_CS_L()  GPIO_ResetBits(GPIOB, GPIO_Pin_12)
#define W25Q64_CS_H()  GPIO_SetBits(GPIOB, GPIO_Pin_12)

void     W25Q64_Init(void);
uint32_t W25Q64_ReadID(void);
void     W25Q64_ReadData(uint32_t addr, uint8_t *buf, uint32_t len);
void     W25Q64_PageProgram(uint32_t addr, uint8_t *buf, uint16_t len);
void     W25Q64_SectorErase(uint32_t addr);

#endif
