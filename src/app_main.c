#include "cmsis_os2.h" 
#include "LED.h"
#include "key.h"
#include "Usart.h"
//#include "HTS221.h"
//#include "Timer.h"

void app_main (void *argument);
void app_main_2task (void *argument);
void app_main_Events (void *argument);
void app_main_Semaphore (void *argument);
void app_main_Mutex (void *argument);

osThreadId_t TaskID_2task, TaskID_Sem, TaskID_Event, TaskID_Mutex,TaskID_Key,TaskID_USART2;

uint32_t HAL_GetTick (void) 
{
	static uint32_t ticks = 0U;
	uint32_t i;
	if (osKernelGetState () == osKernelRunning) 
	{
		return ((uint32_t)osKernelGetTickCount ());
	} 
	/* If Kernel is not running wait approximately 1 ms then increment and return auxiliary tick counter value */
	for (i = (SystemCoreClock >> 14U); i > 0U; i--) 
	{ 
		__NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); 
		__NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); 
	}
	return ++ticks; 
}

void usSleep(uint32_t us)
{
//注意：微秒级的延时仅仅用于SysTimer32位计数器从0到溢出时间内的延时
//RTX5的系统tick是uint64_t，时间间隔设置为1ms时，tick计数从0到溢出将耗时5.8亿年
//但是osKernelGetSysTimerCount在计算时强制转换为(uint32_t)，所以存在32位计数值溢出情况。
	unsigned int SysTimerCount0, us_Count;
	us_Count = osKernelGetSysTimerFreq()/1000000 * us;//微秒计数值
	SysTimerCount0 = osKernelGetSysTimerCount();
	while(osKernelGetSysTimerCount() - SysTimerCount0 < us_Count);//已经考虑过溢出情况
}

void app_main (void *argument) 
{
	
//	Key_Init();
	LED_Initialize();
//	TIM2_Init();	//TIM2用蓝灯闪烁
	
	TaskID_2task 	= osThreadNew(app_main_2task, NULL, NULL);
//	TaskID_USART2	= osThreadNew(USART_1_Thread, NULL, NULL);
//	TaskID_USART2	= osThreadNew(USART_2_Thread, NULL, NULL);
	
//	TaskID_Sem 		= osThreadNew(app_main_Semaphore, NULL, NULL);    
//	TaskID_Mutex	= osThreadNew(app_main_Mutex, NULL, NULL); 
//	TaskID_Event	= osThreadNew(app_main_Events, NULL, NULL);    
//	TaskID_Key      = osThreadNew(app_main_key, NULL, NULL);
}

