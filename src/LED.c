#include "LED.h"

/* GPIO Pin identifier */
typedef struct _GPIO_PIN {
  GPIO_TypeDef *port;
  uint16_t      pin;
} GPIO_PIN;

/* LED GPIO definitions */
const GPIO_PIN Pin_LED[] = {
//	{ GPIOC, GPIO_PIN_8  },		//LED1_RED
	{ GPIOC, GPIO_PIN_9  },		//LED1_GREEN
	{ GPIOC, GPIO_PIN_12 },		//LED2_BLUE
};

#define LED_COUNT (sizeof(Pin_LED)/sizeof(GPIO_PIN))


void LED_On (int num) 
{
	HAL_GPIO_WritePin(Pin_LED[num].port, Pin_LED[num].pin, GPIO_PIN_RESET);
}

void LED_Off (int num) 
{
	HAL_GPIO_WritePin(Pin_LED[num].port, Pin_LED[num].pin, GPIO_PIN_SET);
}
void LED_Toggle (int num) 
{
	HAL_GPIO_TogglePin(Pin_LED[num].port, Pin_LED[num].pin);;
}

void LED_SetOut (int val) {
	int n;

	for (n = 0; n < LED_COUNT; n++) {
		if (val & (1 << n)) 
			LED_On(n);
		else                
			LED_Off(n);
	}
}

int LED_GetCount (void) 
{
	return LED_COUNT;
}
void LED_Uninitialize (void) 
{
	int n;

	for (n = 0; n < LED_COUNT; n++) 
		HAL_GPIO_DeInit(Pin_LED[n].port, Pin_LED[n].pin);
}

void LED_Initialize (void) 
{
	int n;
	GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	for (n = 0; n < LED_COUNT; n++) 
	{
		GPIO_InitStruct.Pin   = Pin_LED[n].pin ;
		HAL_GPIO_Init(Pin_LED[n].port, &GPIO_InitStruct);	
		LED_Off(n);
	}
}

