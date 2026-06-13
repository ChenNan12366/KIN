#ifndef __POT_H
#define __POT_H
#include "stm32f10x.h"

void     Pot_Init(void);
uint16_t Pot_ReadPercent(void);

#endif
