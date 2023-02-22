#include "GPIO_EXIT.h"
#include "stm32l4xx_hal.h"

extern void Key_Callback(uint8_t col);
//col 2
void EXTI4_IRQHandler(void)
{
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_4);
    Key_Callback(1);
}
//col 1
void EXTI9_5_IRQHandler(void)
{
	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_5) != RESET)
    {		
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_5);
		Key_Callback(0);
    }
}
