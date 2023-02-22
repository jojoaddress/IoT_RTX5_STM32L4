#include "cmsis_os2.h"                                        // CMSIS RTOS header file
#include "LED.h"
 
/*----------------------------------------------------------------------------
 *      Mutex creation & usage
 *---------------------------------------------------------------------------*/
 
void Thread_Mutex (void *argument);                           // thread function
osThreadId_t tid_Thread_Mutex;                                // thread id
 
osMutexId_t mid_Thread_Mutex;                                 // mutex id
  
void Thread_Mutex (void *argument)
{
	while (1) 
	{
		osMutexAcquire (mid_Thread_Mutex, osWaitForever);

		LED_Off(1);
		LED_On(0);
		osDelay(500);
		
		osMutexRelease (mid_Thread_Mutex);
		osThreadYield ();                                         // suspend thread
	}
}

void app_main_Mutex (void *argument) 
{	
	mid_Thread_Mutex = osMutexNew (NULL);
	if (!tid_Thread_Mutex) {
		; // Mutex object not created, handle failure
	}

	tid_Thread_Mutex = osThreadNew (Thread_Mutex, NULL, NULL);
	while(1)
	{
		osMutexAcquire (mid_Thread_Mutex, osWaitForever);

		LED_Off(0);
		LED_On(1);
		osDelay(500);
		
		osMutexRelease (mid_Thread_Mutex);
		osThreadYield ();                                         // suspend thread
	}
}
