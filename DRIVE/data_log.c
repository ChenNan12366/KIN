#include "data_log.h"
#include "w25q64.h"
#include <string.h>

static uint32_t write_addr  = 0;        // 当前写入地址
static uint16_t log_count   = 0;        // 已存储条数
static uint32_t sys_seconds = 0;        // 系统运行秒数
static uint8_t  unsaved_cnt = 0;        // 未保存到头部的日志计数

#define HEADER_SAVE_INTERVAL  10        // 每 N 条日志才写一次头部，大幅减少 Flash 磨损

static uint16_t CRC16_Simple(uint8_t *data, uint8_t len)
{
    uint16_t crc = 0xFFFF;
    for (uint8_t i = 0; i < len; i++)
    {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++)
        {
            if (crc & 0x0001)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc >>= 1;
        }
    }
    return crc;
}

void Log_Init(void)
{
    W25Q64_Init();
    uint8_t header[4];
    W25Q64_ReadData(0, header, 4);
    log_count  = (header[0] << 8) | header[1];
    write_addr = (header[2] << 8) | header[3];
    if (log_count > 10000) log_count = 0;
    if (write_addr < 4) write_addr = 4;
}

void Log_Save(uint8_t temp, uint8_t humi, uint8_t soil, uint8_t rain, uint8_t smoke, uint8_t pot)
{
    uint8_t  buf[LOG_SIZE];
    uint16_t crc;

    sys_seconds += 30;

    buf[0] = temp;  buf[1] = humi;
    buf[2] = soil;  buf[3] = rain;
    buf[4] = smoke; buf[5] = pot;
    buf[6] = (sys_seconds >> 24) & 0xFF;
    buf[7] = (sys_seconds >> 16) & 0xFF;
    buf[8] = (sys_seconds >> 8) & 0xFF;
    buf[9] = sys_seconds & 0xFF;

    crc = CRC16_Simple(buf, 10);
    buf[10] = crc & 0xFF;
    buf[11] = (crc >> 8) & 0xFF;
    buf[12] = 0;
    buf[13] = 0;

    /* 超出 8MB 范围则回卷 */
    if (write_addr + LOG_SIZE > 8 * 1024 * 1024)
    {
        write_addr = 4;
        W25Q64_SectorErase(0);
        log_count = 0;
    }

    W25Q64_PageProgram(write_addr, buf, LOG_SIZE);
    write_addr += LOG_SIZE;
    log_count++;
    unsaved_cnt++;

    /* 每 N 条日志才更新一次头部，减少扇区擦写（10万次寿命保护） */
    if (unsaved_cnt >= HEADER_SAVE_INTERVAL)
    {
        uint8_t header[4];
        header[0] = (log_count >> 8) & 0xFF;
        header[1] = log_count & 0xFF;
        header[2] = (write_addr >> 8) & 0xFF;
        header[3] = write_addr & 0xFF;
        W25Q64_SectorErase(0);
        W25Q64_PageProgram(0, header, 4);
        unsaved_cnt = 0;
    }
}

/**
 * @brief  强制刷新头部到 Flash（掉电前调用，预防丢失最后几条记录）
 */
void Log_Flush(void)
{
    if (unsaved_cnt > 0)
    {
        uint8_t header[4];
        header[0] = (log_count >> 8) & 0xFF;
        header[1] = log_count & 0xFF;
        header[2] = (write_addr >> 8) & 0xFF;
        header[3] = write_addr & 0xFF;
        W25Q64_SectorErase(0);
        W25Q64_PageProgram(0, header, 4);
        unsaved_cnt = 0;
    }
}

uint16_t Log_Count(void)
{
    return log_count;
}

void Log_Read(uint16_t index, uint8_t *buf)
{
    W25Q64_ReadData(4 + index * LOG_SIZE, buf, LOG_SIZE);
}
