#include "Driver_USART.h"
#include "cmsis_os2.h"
#include "stdio.h"
#include "string.h"

#include "Usart.h"
/*
˵����
	��RTE_Device.h��������Usart�����ź�DMA

�����NXP��˾��LPCϵ�У�
1���䴮�ڳ���DMA���ֻ�δ�о��ã��ر�DMA��������ջ��쳣��
2����IDE��C���뻷����Define�����������Щ����
	USART0_TRIG_LVL=USART_TRIG_LVL_14,USART1_TRIG_LVL=USART_TRIG_LVL_14,USART2_TRIG_LVL=USART_TRIG_LVL_14,USART3_TRIG_LVL=USART_TRIG_LVL_14
������RTE_Device.h�����������漸�ж���
	#define USART0_TRIG_LVL           USART_TRIG_LVL_14
	#define USART1_TRIG_LVL           USART_TRIG_LVL_14
	#define USART2_TRIG_LVL           USART_TRIG_LVL_14
	#define USART3_TRIG_LVL           USART_TRIG_LVL_14
*/

//���ڰ���USART_1�ӿ�
#define USART_Initialization		USART_1_Initialization
#define USART_Thread				USART_1_Thread
#define USART_Send					USART_1_Send
#define USART_Receive				USART_1_Receive
#define USART_Boudrate				USART_1_Boudrate
#define USART_RxBuffer				USART_1_RxBuffer
#define USART_TxBuffer				USART_1_TxBuffer

#define bSending_USART				bSending_USART_1
#define bReceiving_USART			bReceiving_USART_1
#define sem_USART_Send				sem_USART_1_Send
#define sem_USART_Receive			sem_USART_1_Receive
#define USART_callback				USART_1_callback
#define USART_drv					USART_1_drv

#define huart						huart1
#define USART_IRQHandler			USART1_IRQHandler


extern ARM_DRIVER_USART Driver_USART1;
static ARM_DRIVER_USART *USART_drv = &Driver_USART1;

uint32_t USART_Boudrate=115200;//;460800 

/******************************************************************/
//���´���������ڵ��������򶼱�����ͬ

uint8_t USART_RxBuffer  [RXBUFFERSIZE];
uint8_t USART_TxBuffer  [TXBUFFERSIZE];
char bSending_USART=0;
char bReceiving_USART=0;
osSemaphoreId_t sem_USART_Send;
osSemaphoreId_t sem_USART_Receive;
extern UART_HandleTypeDef huart;


void USART_callback(uint32_t event)
{
	uint32_t mask;
	mask = 	ARM_USART_EVENT_RX_OVERFLOW  		|
			ARM_USART_EVENT_TX_UNDERFLOW 		|
			ARM_USART_EVENT_RX_BREAK 			|
			ARM_USART_EVENT_RX_FRAMING_ERROR 	|
			ARM_USART_EVENT_RX_PARITY_ERROR  	;
	if (event & mask) {
// ���ֹ��ϣ���λ�����շ��ź�
		USART_drv->Control(ARM_USART_ABORT_RECEIVE, 0);
		osSemaphoreRelease(sem_USART_Send);
		osSemaphoreRelease(sem_USART_Receive);
		return ;
	}
	if (event & (ARM_USART_EVENT_TX_COMPLETE)) {
// ���ͽ���
		osSemaphoreRelease(sem_USART_Send);
		return ;
	}
	if (event & ARM_USART_EVENT_RX_TIMEOUT) {
// �յ�һ���µ�����
		osSemaphoreRelease(sem_USART_Receive);
		return ;
	}
}

void USART_Send (uint8_t *out, uint32_t cnt) 
{
	bSending_USART = 1;
	USART_drv->Send (out, cnt);
    
	osSemaphoreAcquire (sem_USART_Send, osWaitForever);
	bSending_USART = 0;
}

int USART_Receive (uint8_t *in, int timeout) 
{
	int len;
//����IDLE Line���߿��м�⹦�ܣ�ʵ��һ������֡�Ľ���
	bReceiving_USART = 1;
	USART_drv->Receive (in, RXBUFFERSIZE);
	
	osSemaphoreAcquire (sem_USART_Receive, timeout);
	bReceiving_USART = 0;
	len = RXBUFFERSIZE - huart.RxXferCount;
	USART_drv->Control(ARM_USART_ABORT_RECEIVE, 0);
	return len;
}

void  USART_Initialization (void) 
{
	static char init=0;
	if (init) return;
	init = 1;
	sem_USART_Send = osSemaphoreNew(1, 0, NULL);
	sem_USART_Receive = osSemaphoreNew(1, 0, NULL);
	
    /*Initialize the USART driver */
    USART_drv->Initialize(USART_callback);
    /*Power up the USART peripheral */
    USART_drv->PowerControl(ARM_POWER_FULL);
    /*Configure the USART */
    USART_drv->Control(ARM_USART_MODE_ASYNCHRONOUS |
                      ARM_USART_DATA_BITS_8 |
                      ARM_USART_PARITY_NONE |
                      ARM_USART_STOP_BITS_1 |
                      ARM_USART_FLOW_CONTROL_NONE, USART_Boudrate);
     
    /* Enable Receiver and Transmitter lines */
    USART_drv->Control (ARM_USART_CONTROL_TX, 1);
    USART_drv->Control (ARM_USART_CONTROL_RX, 1);
	
	__HAL_UART_ENABLE_IT(&huart,UART_IT_IDLE);//����IDLE�жϣ��жϲ�����stm32l4xx_it.c�Ĵ����ж���
}

void USART_Thread (void *argument) 
{
	uint8_t str[] = "Hello World!\n";
	int rxCount=0;
	
	USART_Initialization();
	
	USART_Send(str,strlen((char*)str));//�ַ���������0���÷���
	while(1)
	{
		rxCount = USART_Receive(USART_RxBuffer, osWaitForever);
		USART_Send(USART_RxBuffer,rxCount);
	}
}

void USART_IRQHandler(void)
{
	HAL_UART_IRQHandler(&huart);
	
	if(__HAL_UART_GET_FLAG(&huart,UART_FLAG_IDLE) != RESET)
	{
		__HAL_UART_CLEAR_IDLEFLAG(&huart);
		if(bReceiving_USART == 0) return ;

		osSemaphoreRelease(sem_USART_Receive);
	}
}
  
