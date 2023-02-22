#ifndef __CCS811_H
#define __CCS811_H

#include "stm32l4xx_hal.h"
#include "gpio.h"
#include "usart3.h"

#define CCS811_Add      		0x5A<<1

#define DRIVE_MODE_1SEC 		0x10  // 1秒间隔测量
#define DRIVE_MODE_10SEC		0x20  // 10s间隔测量
#define DRIVE_MODE_60SEC 		0x30  // 每分钟测量
#define INTERRUPT_DRIVEN 		0x8   // 强力测量250ms

extern unsigned short eco2,tvoc;
void CCS811_Init(void);
int CCS811GetData(void);

#endif

