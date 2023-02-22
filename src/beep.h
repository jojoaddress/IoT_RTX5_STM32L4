#ifndef __BEEP_H
#define __BEEP_H

#define BEEP(n)	(n?HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET))
#define BEEP_TogglePin	HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_7)


void BEEP_Init(void);




#endif


