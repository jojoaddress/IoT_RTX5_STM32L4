#ifndef __BSP__WIFI__H__
#define __BSP__WIFI__H__

#include "stm32l4xx_hal.h"
#include "comm_config.h"

#define SERVER_IP "iot-as-mqtt.cn-shanghai.aliyuncs.com"
#define SERVER_PORT 1883
         

#define WIFI_ACCOUNT  "Rainwork"      
#define WIFI_PASSWORD "3.1415926"    

#define AP_ACCOUNT  	"Rainwork"     
#define AP_PASSWORD 	"3.1415926"      



uint8_t __exit_transmission();
uint8_t wifi_init();
uint8_t wifi_sta_mode();
uint8_t wifi_ap_mode();
uint8_t wifi_sta_mode_prgam(char *account,char *password);
#endif 