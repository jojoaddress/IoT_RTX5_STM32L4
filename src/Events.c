#include "cmsis_os2.h"
#include "LED.h"

osThreadId_t tid_Thread_EventSender, tid_Thread_EventReceiver0, tid_Thread_EventReceiver1;

osEventFlagsId_t evt_Controler;

#define FLAGS_MSK1 0x00000003ul
#define FLAGS_LED0 1
#define FLAGS_LED1 2

void Thread_EventSender (void *argument)
{
	osDelay(4000);
	while (1) 
	{    
		osEventFlagsSet(evt_Controler, FLAGS_LED0);
		osDelay(1000);
		osEventFlagsSet(evt_Controler, FLAGS_LED1);
		osDelay(2000);
		
//		osEventFlagsSet(evt_Controler, FLAGS_LED0);
//		osEventFlagsSet(evt_Controler, FLAGS_LED1);
//		osDelay(1000);
	}
}
 
void Thread_EventReceiver0 (void *argument)
{
	while (1) 
	{
		osEventFlagsClear(evt_Controler,FLAGS_LED0);
		osEventFlagsWait (evt_Controler,FLAGS_LED0,osFlagsWaitAll, osWaitForever);
//�ȵ��¼���ͬʱҲ���Զ�����¼���־��
//		����Thread_EventStatus0һ�ζ����ܵȵ���
//		Thread_EventStatus1����Thread_EventReceiver0��Thread_EventReceiver1���¼���־��
		LED_On(0);
		osDelay(400); 
		LED_Off(0);
		osDelay(400);
	}
}
void Thread_EventReceiver1 (void *argument)
{
	while (1) 
	{
		osEventFlagsClear(evt_Controler,FLAGS_LED1);
		osEventFlagsWait (evt_Controler,FLAGS_LED1,osFlagsWaitAll, osWaitForever);

		LED_On(1);
		osDelay(400); 
		LED_Off(1);
		osDelay(400);
	}
}
void Thread_EventStatus0 (void *argument)
{
	while (1) 
	{
		osEventFlagsWait (evt_Controler,FLAGS_LED0|FLAGS_LED1,osFlagsWaitAll, osWaitForever);
//�ȴ������¼�������ż�������������¼���־
		LED_On(1);
		osDelay(100); 
		LED_Off(1);
		osDelay(100);
	}
}
void Thread_EventStatus1 (void *argument)
{
	while (1) 
	{
		osEventFlagsWait (evt_Controler,FLAGS_LED0|FLAGS_LED1,osFlagsWaitAny, osWaitForever);
//�ȴ������¼�����һ������ż����������������һ���¼���־
		LED_On(2);
		osDelay(100); 
		LED_Off(2);
		osDelay(100);
	}
}

void app_main_Events (void *argument) 
{
	evt_Controler = osEventFlagsNew(NULL);

	tid_Thread_EventSender = osThreadNew (Thread_EventSender, NULL, NULL);
	tid_Thread_EventReceiver0 = osThreadNew (Thread_EventReceiver0, NULL, NULL);
	tid_Thread_EventReceiver1 = osThreadNew (Thread_EventReceiver1, NULL, NULL);
	osThreadNew (Thread_EventStatus0, NULL, NULL);
	osThreadNew (Thread_EventStatus1, NULL, NULL);
}

