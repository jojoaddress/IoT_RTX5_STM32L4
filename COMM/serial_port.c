#include "drv_serial.h"
#include "stm32l4xx_hal.h"
#include "my_wifi.h"
//#include "key/key.h"
#include "stmflash.h"
#include "wifi_analy.h"
//#include "task_config.h"


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{

	drv_serial_usart_irhandle(UartHandle);
}
	
	
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	static uint8_t led_cnt=0;
	static uint16_t key_cnt=0;
	
	drv_serial_tim_irhandle(htim);
		#if EXAMPLE_WIFI_CONFIG_TASK
	if(htim==&htim7)
	{

		if(my_wifi.wifi_config_sta==1)
		{
			LED2_ON;
		}
		else
		{
			led_cnt++;
			if(led_cnt==3)
			{
				led_cnt=0;
				LED2_Tog;
			}
		
		}
		
		
		if(!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2))
		{
			key_cnt++;
			if(key_cnt>=250)
			{
				printf("long key......\r\n");
				key_cnt=0;
				__set_FAULTMASK(1);
				my_wifi.wifi_config_sta=0;
				my_wifi.wifi_sta_state=0; //要开启热点，ap模式，防止中途中断操作
			
				HAL_TIM_Base_Stop_IT(&htim7);
				wifi_write(FLASH_SAVE_ADDR,(uint8_t*)&my_wifi,sizeof(my_wifi));
			
				
				NVIC_SystemReset();
			}
		}
		else key_cnt=0;
	}
	#endif
}
