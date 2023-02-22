#include "Timer.h"
#include "LED.h"

TIM_HandleTypeDef htim3;

TIM_HandleTypeDef htim2;

void TIM2_IRQHandler(void)		
{
    __HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE);
    LED_Toggle(2);			
}

void TIM2_Init(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 8000-1;	//虽大0xFFFF
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 5000-1;	//虽大0xFFFF       500ms
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	HAL_TIM_Base_Init(&htim2);
	
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig);
	
//	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
//	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
//	HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig);
	
	HAL_TIM_Base_Start_IT(&htim2);
}



void TIM_SetTIM3Compare4(uint32_t compare)
{
	TIM3->CCR4=compare; 
}
void TIM_SetTIM3Compare3(uint32_t compare)
{
	TIM3->CCR3=compare; //设置比较值，调节PWM输出波形的占空比
}
void TIM3_PWM_Init(void)
{
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_OC_InitTypeDef sConfigOC = {0};

	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 80-1;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = 500-1;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	HAL_TIM_PWM_Init(&htim3);
	
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) ;
  
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = (500-1)/2;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3);
  
	HAL_TIM_MspPostInit(&htim3);
	
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* tim_pwmHandle)
{

	if(tim_pwmHandle->Instance==TIM3)
	{
		__HAL_RCC_TIM3_CLK_ENABLE();
	}
}
void HAL_TIM_MspPostInit(TIM_HandleTypeDef* timHandle)
{

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(timHandle->Instance==TIM3)
	{
		__HAL_RCC_TIM3_CLK_ENABLE();
		__HAL_RCC_GPIOC_CLK_ENABLE();
	  
		GPIO_InitStruct.Pin = GPIO_PIN_8;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	}
}

void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef* tim_pwmHandle)
{

	if(tim_pwmHandle->Instance==TIM3)
	{
		__HAL_RCC_TIM3_CLK_DISABLE();
	}
}

