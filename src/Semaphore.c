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
//�����������ֱ�ע��һ�䣬����һ�䣬����ʵ��Ч��
		val = osSemaphoreAcquire (sid_Thread_Semaphore, osWaitForever); // ��Զ�ȴ�
//		val = osSemaphoreAcquire (sid_Thread_Semaphore, 100);      	// wait 100 mSec
//		val = osSemaphoreAcquire (sid_Thread_Semaphore, 0);      	// ���ȴ����̷��أ�ʵ��val=osErrorResource

		switch (val) {
			case osOK:
				LED_Toggle(2);
				break;
			case osErrorTimeout://waittimeΪ100ʱ��������
				LED_Toggle(1);
				break;
			case osErrorResource://waittimeΪ0ʱ��������
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
//ͨ�����������ź���
		if(KEY == 0){
			osDelay  (10); 
			if(KEY == 0)			//������������һ�ΰ�����10ms���ϱ��ֵ͵�ƽ�����Իᵼ�¶෢�ź���
			{
				while(KEY == 0) osDelay  (1); //ѭ���ȴ�����̧��
				osSemaphoreRelease (sid_Thread_Semaphore); 
			}
		}
		osDelay(1); //���б����У��������ް���ʱ��һֱ��ͣ���������жϡ�Ҳ����ʹ��osThreadYield ();
		
//ÿ���2�뷢һ���ź���		
//		osSemaphoreRelease (sid_Thread_Semaphore); 
//		osDelay(2000);
	}
}
