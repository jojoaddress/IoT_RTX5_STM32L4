#ifndef __MQTT_INIT__H__
#define __MQTT_INIT__H__

#include "stm32l4xx_hal.h"

//#define DeviceName = "ihgMrQekwQ1zrU2oar1i";
//#define ProductKey = "a1ShB7yHwq9";
//#define DeviceSecret = "9xGKjRoXr7d4vSzvSJNyJrJ2LeahMxSy"

#define ali_port 1883
#define keepalive_time 120

#define DEBUG_MQTT_INFO 1

//extern uint8_t RX_BUFF[256];
//extern uint16_t RX_LEN;


//typedef enum
//{
//	MQTT_PUBLISH_PROPERTY = 0x00,   //�����ϱ�
//	MQTT_PUBLISH_ALARM,             //�¼��ϱ�
//	MQTT_PING,
//	NOTASK = 0xFF,
//}Mqtt_StateTypedef;

#endif