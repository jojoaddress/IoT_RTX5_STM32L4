
#ifndef WIFI_ANS
#define WIFI_ANS
#include "stm32l4xx_hal.h"


uint8_t wifi_parsing(char *buff,char *account,char *password);
int wifi_write(int addr,uint8_t *buff,uint32_t len);

#endif