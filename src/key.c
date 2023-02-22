#include "key.h"
#include "LED.h"
#include "cmsis_os2.h"

uint8_t Key_busy = 0;
int Key_Row = 0;
int Key_Col = 0;
//uint8_t isHandUP = 1; //1表示松手，0表示按下 
uint8_t KeyNum = 0;

osSemaphoreId_t sid_Key_Semaphore,sid_KeyPress_Semaphore;
osThreadId_t key_Thread;

typedef struct _GPIO_PIN {
  GPIO_TypeDef *port;
  uint16_t      pin;
} GPIO_PIN;

const GPIO_PIN Pin_Row[] = {        //行
	{ GPIOD, GPIO_PIN_2 },			//第1行	
	{ GPIOB, GPIO_PIN_3 },			//第2行
};
const GPIO_PIN Pin_Col[] = {        //列
	{ GPIOB, GPIO_PIN_5 },			//第1列
	{ GPIOB, GPIO_PIN_4 },			//第2列
};

#define KEY	HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11)
#define PinRow_COUNT    (sizeof(Pin_Row)/sizeof(GPIO_PIN))
#define PinCol_COUNT    (sizeof(Pin_Col)/sizeof(GPIO_PIN))
	
static void Key_Row_scan(int row);
static uint8_t Pin_Col_Read(int col);

static void Key_Row_scan(int row)
{
    int i;
	if(row < 0)
    {
        for(i = 0;i < PinRow_COUNT; i++)
        { 
            HAL_GPIO_WritePin(Pin_Row[i].port, Pin_Row[i].pin,GPIO_PIN_RESET);
        }
        return;
    }
    for(i = 0;i < PinRow_COUNT; i++)
    {
        if(i == row)
            HAL_GPIO_WritePin(Pin_Row[i].port, Pin_Row[i].pin,GPIO_PIN_RESET);
        else 
            HAL_GPIO_WritePin(Pin_Row[i].port, Pin_Row[i].pin,GPIO_PIN_SET);
    }
}
static uint8_t Pin_Col_Read(int col)
{
    return HAL_GPIO_ReadPin(Pin_Col[col].port, Pin_Col[col].pin);   
}

int Key_Scan(void)
{
    int key,i,j;
    osDelay(10);
    if(Pin_Col_Read(Key_Col) == 1) return 0;
        
    Key_Row_scan(0);
    if(Pin_Col_Read(Key_Col) == 0) Key_Row = 0;
    
    Key_Row_scan(1);
    if(Pin_Col_Read(Key_Col) == 0) Key_Row = 1;
    
    key=0;
    for(i = 0; i <Key_Row; i++)
    {
        key += 2;
    }
    for(j = 0; j <Key_Col+1; j++)
    {
        key++;
    }
    return key;
}

void Key_Callback(int col)
{
	if(Key_busy == 1) return;
    else{
        Key_busy = 1;
        Key_Col = col;
        osSemaphoreRelease(sid_Key_Semaphore);
    }
}

void Matrix_Keyboard_Init(void)  //矩阵键盘
{
	int i;
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_TypeDef *port;
	uint16_t      pin;
    
    __HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
    
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	for (i = 0; i < PinRow_COUNT; i++) 
	{
		port = Pin_Row[i].port;
		pin	 = Pin_Row[i].pin;
		GPIO_InitStruct.Pin = pin ;
		HAL_GPIO_Init(port, &GPIO_InitStruct);	
		HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
	}
    
    GPIO_InitStruct.Mode  = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	for (i = 0; i < PinCol_COUNT; i++) 
	{
		port = Pin_Col[i].port;
		pin	 = Pin_Col[i].pin;
		GPIO_InitStruct.Pin = pin ;
		HAL_GPIO_Init(port, &GPIO_InitStruct);	
	}
	
	 //第1列
    HAL_NVIC_SetPriority(EXTI4_IRQn,2,3);   	
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);
    //第2列
    HAL_NVIC_SetPriority(EXTI9_5_IRQn,2,3);   	
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
	
	Key_Row_scan(-1);
}

uint16_t NowKeyNum = 0;
uint8_t  isHandUP = 0; //1表示松手，0表示按下

void Key_Thread (void *argument) 
{
    
	while (1) {
        osSemaphoreAcquire (sid_KeyPress_Semaphore, osWaitForever);
        switch(NowKeyNum){
			case 1: LED_Toggle(1);break;
			case 2: LED_Toggle(2);break;
			case 3: LED_On(1);LED_On(2);break;
			case 4: LED_Off(1);LED_Off(2);break;
            default:
                break;
        }
        osDelay(10);
	}
}

void app_main_key (void *argument)
{
	int key;
	sid_Key_Semaphore = osSemaphoreNew(1, 0, NULL);
	sid_KeyPress_Semaphore = osSemaphoreNew(1, 0, NULL);
	Matrix_Keyboard_Init();
	Key_Row_scan(-1);
	key_Thread = osThreadNew (Key_Thread, NULL, NULL);
	while(1)
	{
		osSemaphoreAcquire(sid_Key_Semaphore, osWaitForever);
		key = Key_Scan();
		if(key >0){
			NowKeyNum = key;
//			isHandUP = 0;
			osSemaphoreRelease(sid_KeyPress_Semaphore);
		}
		Key_Row_scan(-1);
		
		//松手
		while(1){
			if(Pin_Col_Read(Key_Col)){
				Key_busy = 0;
				isHandUP = 1;
				break;
			}
			osDelay(1);
		}
	}
}

//板载按键

void Key_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}


int isPress(int mode)
{
	static uint8_t key_up=1;     //按键松开标志
	if(mode == 1)key_up=1;
	if(key_up && (KEY == 0))
	{
		osDelay(10);	//防抖
		key_up=0;
		if(KEY == 0) return 1;
	}
	else if(KEY == 1) key_up = 1;
	return 0;  //无按键按下
}
