#ifndef __TIMER_H
#define __TIMER_H
#include <stdint.h>
#include "stm32l4xx_hal.h"

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;

void TIM2_Init(void);
void TIM3_PWM_Init(void);
void HAL_TIM_MspPostInit(TIM_HandleTypeDef* timHandle);
void TIM_SetTIM3Compare4(uint32_t compare);
void TIM_SetTIM3Compare3(uint32_t compare);
#endif  /* __LED_H */
