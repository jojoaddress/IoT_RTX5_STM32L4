#ifndef __KEY_H
#define __KEY_H

#include "stm32l4xx_hal.h"

//extern uint8_t isHandUP;
extern uint8_t KeyNum;
extern uint8_t Key_busy;

void Key_Init(void);
void Matrix_Keyboard_Init(void);

void app_main_key (void *argument);
void Key_Thread (void *argument);


//°åÔØ°´¼ü
#define KEY	HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11)
void Key_Init(void);
int isPress(int mode);
#endif
