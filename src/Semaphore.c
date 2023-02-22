#include "cmsis_os2.h"                                        // CMSIS RTOS header file
#include "LED.h"
#include "key.h"

void Thread_Semaphore (void *argument);                       // thread function
osThreadId_t tid_Thread_Semaphore;                            // thread id
 
osSemaphoreId_t sid_Thread_Semaphore;                         // semaphore id
 
void Thread_Semaphore (void *argument)
{
	int32_t val;

	while (1) {
//下面两条语句分别注释一句，开启一句，看看实验效果
		val = osSemaphoreAcquire (sid_Thread_Semaphore, osWaitForever); // 永远等待
//		val = osSemaphoreAcquire (sid_Thread_Semaphore, 100);      	// wait 100 mSec
//		val = osSemaphoreAcquire (sid_Thread_Semaphore, 0);      	// 不等待立刻返回，实测val=osErrorResource

		switch (val) {
			case osOK:
				LED_Toggle(2);
				break;
			case osErrorTimeout://waittime为100时进入这里
				LED_Toggle(1);
				break;
			case osErrorResource://waittime为0时进入这里
				LED_Toggle(1);
				osDelay(100);
				break;
			case osErrorParameter:
				LED_Toggle(1);
				break;
			default:
				LED_Toggle(0);
				break;
		}

		osThreadYield ();                                         // suspend thread
	}
}

void app_main_Semaphore (void *argument) 
{	
	Key_Init();
	sid_Thread_Semaphore = osSemaphoreNew(2, 2, NULL);
	if (!sid_Thread_Semaphore) {
		 // Semaphore object not created, handle failure
	}

	tid_Thread_Semaphore = osThreadNew (Thread_Semaphore, NULL, NULL); 
	while(1)
	{
//通过按键发送信号量
		if(KEY == 0){
			osDelay  (10); 
			if(KEY == 0)			//防抖处理，但是一次按键有10ms以上保持低电平，所以会导致多发信号量
			{
				while(KEY == 0) osDelay  (1); //循环等待按键抬起
				osSemaphoreRelease (sid_Thread_Semaphore); 
			}
		}
		osDelay(1); //这行必须有，否则在无按键时，一直不停地做按键判断。也可以使用osThreadYield ();
		
//每间隔2秒发一次信号量		
//		osSemaphoreRelease (sid_Thread_Semaphore); 
//		osDelay(2000);
	}
}
