#include "cmsis_os2.h"
#include "LED.h"
#include "usart.h"
#include "stdio.h"
#include "string.h"
#include "HTS221.h"
#include "beep.h"

#include "bsp_wifi.h"
#include "drv_serial.h"
#include "delay/delay.h"
#include "mqtt_init.h"
#include "sign_api.h"
#include "transport.h"
#include "MQTTPacket.h"
#include "string.h"
#include "my_wifi.h"
#include "Timer.h"
#include "CCS811.h"
osThreadId_t tid_Thread1, tid_Thread2, tid_Thread3, tid_Thread4;
 

void __task_before_init();
int property_post_create_packet(char *buf,int state);
void infrared_gpio_init();
void Data_reporting(int size);
void aliyun_gettopic();
void updata();


const char *DeviceName = "test_01";
const char *ProductKey = "gn6kcczR3Re";
const char *DeviceSecret = "2b88c1b346a185ada49c36f0ec73a73b";

HTTS221_REG regs;
uint8_t link_buff[256]={0};
int link_len=0;
uint8_t BUFF[256]={0};
uint16_t LEN=0;
int TempThreshold=100;		//
int range_threshold=40;
int ledcontrol=0;
uint8_t mqtt_ping_fifo[10] = {0};
uint8_t mqtt_ping_fifo_len = 0;

	char publish_buff[256]={0};
	char publish_topic[128]; 
	char get_topic[128];
	char mqtt_fifo[300]={0};
	iotx_dev_meta_info_t dev_meta = {0};
	iotx_sign_mqtt_t sign_mqtt = {0};
	int msgid = 1;
	int req_qos = 0;//QOS
	uint32_t packid = 0;
	

	int len_temp=0;
	int mqtt_fifo_len=0;
	int cnt_ping=0;
	int send_time=0;

	MQTTString topic_string = MQTTString_initializer;
	MQTTPacket_connectData mqttdata = MQTTPacket_connectData_initializer;
	
	int infred_read=0,infred_send_flag=0,fire=0;

void Thread1 (void *argument) 
{
	
	while (1) 
	{
		
		if(range_threshold<regs.Temperature){
			LED_On(1);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
			osDelay(500);
		}
		else{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
			LED_Off(1);
		}
	}
}
void Thread2 (void *argument) 
{
	while (1) 
	{
		osDelay(500);
		printf("lklklklklklklkllllllllklklkllllllllllll:   %d",ledcontrol);
	 	if(ledcontrol==0){
			TIM_SetTIM3Compare3(0);
			
		}else if(ledcontrol==1){
			TIM_SetTIM3Compare3(10);
		}else if(ledcontrol==2){
			TIM_SetTIM3Compare3(100);
		}else if(ledcontrol==3){
			TIM_SetTIM3Compare3(200);
		}else{
			TIM_SetTIM3Compare3(0);
		}
			
	
	}
}
void Thread3 (void *argument) 
{
	
	uint8_t str[50],status;
	MX_GPIO_Init();
	MX_USART1_UART_Init();
	MX_USART2_UART_Init();
	MX_USART3_UART_Init();
	MX_TIM6_Init();
	MX_TIM7_Init();
	TIM3_PWM_Init();
	BEEP_Init();
	printf("Rnuning....\r\n");
	delay_init(80);
	drv_serial_init();

	#if IIC_CMSIS_MODE==0
		HTS221_Init();
	#elif IIC_CMSIS_MODE==1
		HTS221_IIC_Init();
	#else
		HTS221_IIC_Driver_Init();
	#endif
	
//	CCS811_Init();
	infrared_gpio_init();
	__task_before_init();
	aliyun_gettopic();
	printf("example wifi link aliyun task running....\r\n");
	strcpy(dev_meta.product_key,ProductKey);
	strcpy(dev_meta.device_name, DeviceName);
	strcpy(dev_meta.device_secret, DeviceSecret);
	IOT_Sign_MQTT(IOTX_CLOUD_REGION_SHANGHAI, &dev_meta, &sign_mqtt);
	mqttdata.keepAliveInterval = keepalive_time;
	mqttdata.cleansession = 1;
	mqttdata.clientID.cstring = sign_mqtt.clientid;
	mqttdata.username.cstring = sign_mqtt.username;
	mqttdata.password.cstring = sign_mqtt.password;
	
	do{
		memset(link_buff, 0, sizeof(link_buff));
		memset(BUFF, 0, sizeof(BUFF));	
		link_len = MQTTSerialize_connect(link_buff, sizeof(link_buff), &mqttdata);
		transport_sendPacketBuffer(0, link_buff, link_len);
		printf("等待连接阿里云\r\n");
		HAL_Delay(1000);
		drv_serial_read(BUFF,&LEN);	
	}while(!(BUFF[0] == 0x20 && BUFF[1] == 0x02 && BUFF[2] == 0x00 && 
		BUFF[3] == 0x00));

	printf("连接阿里云成功\r\n");
	while (1) 
	{		
		while(1)
		{
			updata();
			if(!CCS811GetData())
			{
				printf("\neCO2=%dppm,TVOC=%dmg/m3\r\n\n",eco2,tvoc);
			}
			#if IIC_CMSIS_MODE==0
				regs.Humidity = HTS_221_Get_Humidity();
				regs.Temperature = HTS221_Get_Temperature();
				break;
			#elif IIC_CMSIS_MODE==1
				if(HTS221_Device_Get_Temperature_Humidity(&regs)) break;//判断是否有新采样数据
			#else
				if(HTS221_Driver_Get_Temperature_Humidity(&regs)) break;//判断是否有新采样数据
			#endif
			osDelay(10);
		}
		printf("temperature=%.1f ℃  humidity=%.1f  %%\r\n\n ", regs.Temperature,regs.Humidity);
		
		//infred_read=HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13);                  //红外感应
		//printf("infred_read=%d\r\n",infred_read);
//		if(infred_read)
//			infred_send_flag=1;
//		else
//			infred_send_flag=0;
//		if(infred_send_flag)
//			printf("Alarm\r\n");
//		else 
//			printf("OK\r\n");
		cnt_ping++;
		send_time++;
		
		len_temp=property_post_create_packet(publish_buff,2);   //上报警报数据
		Data_reporting(len_temp);
		
		if(send_time==5)
		{
				send_time=0;	
				len_temp=property_post_create_packet(publish_buff,1);
				Data_reporting(len_temp);
		}
		delay_ms(1000);
		if(cnt_ping==100)
		{
			cnt_ping=0;
			mqtt_ping_fifo_len = MQTTSerialize_pingreq(mqtt_ping_fifo, sizeof(mqtt_ping_fifo));
			transport_sendPacketBuffer(0, mqtt_ping_fifo, mqtt_ping_fifo_len);
		}
	}
}

void __task_before_init()
{
	u8 temperature;  	    
	u8 humidity; 
	uint8_t buff[50];
	while(wifi_init());
	wifi_sta_mode();
}

void infrared_gpio_init()
{
	  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOC_CLK_ENABLE();


  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

int property_post_create_packet(char *buf,int state)   
{
	int p=0;	
	if(buf==NULL)
	{
		return 0;
	}
	memset(buf,0,sizeof(&buf));
	p+=sprintf(buf+p,"{\"id\":\"1\",\"method\":\"thing.event.property.post\",");
	p+=sprintf(buf+p,"%s","\"version\":\"1.0\",");
	p+=sprintf(buf+p,"\"params\":{");
	if(state==1){
		p+=sprintf(buf+p,"%s%f","\"CurrentTemperature\":",regs.Temperature);
		p+=sprintf(buf+p,"%s%f",",\"CurrentHumidity\":",regs.Humidity);
//		p+=sprintf(buf+p,"%s%d",",\"carbon_dioxide\":",eco2);
//		p+=sprintf(buf+p,"%s%d",",\"tvoc\":",tvoc);
	}
	else{
		p+=sprintf(buf+p,"%s%d","\"ledcontrol\":",ledcontrol);
		p+=sprintf(buf+p,"%s%d",",\"TempThreshold\":",range_threshold);
	}
	p+=sprintf(buf+p,"%s","}}");
	return p;
}

void Data_reporting(int size){
	printf("len_temp=%d\r\n",size);
	printf("publish_buff:\r\n%s\r\n",publish_buff);
	packid++;
	memset(publish_topic,0,sizeof(publish_topic));
	sprintf(publish_topic,"/sys/%s/%s/thing/event/property/post",ProductKey,DeviceName);  //发布属性上报Topic
	printf("属性上报Topic:%s\r\n",publish_topic);
	topic_string.cstring=(char*)publish_topic;
	mqtt_fifo_len = MQTTSerialize_publish((uint8_t*)mqtt_fifo, sizeof(mqtt_fifo), 0, req_qos, 0, 
	packid, topic_string, (uint8_t*)publish_buff, size);	
	transport_sendPacketBuffer(0, (uint8_t*)mqtt_fifo, mqtt_fifo_len);
}

char* findstr(char* src,const char* dest,int len1,int len2) {
	int i,j;
	for(i=0;i<len1-len2;i++)
	{
		for(j=0;j<sizeof(dest);j++)
		{
			if(*(src+i+j)!=*(dest+j))
				break;
		}
		if(j==sizeof(dest))
			return src+i;
	}
	return NULL;
}

void updata()
{
	uint8_t RX_BUFF_TEMP[256]={0};
	uint16_t RX_LEN=0;
	char* buff;
	char* key1="TempThreshold";
	char* key2="ledcontrol";
	int len1=sizeof(RX_BUFF_TEMP),len2=sizeof("TempThreshold"),len3=sizeof("ledcontrol");

	memset(RX_BUFF_TEMP,0,sizeof(RX_BUFF_TEMP));
	drv_serial_read(RX_BUFF_TEMP,&RX_LEN);
	if( NULL!=findstr((char*)RX_BUFF_TEMP,key1,len1,len2) )
	{
		buff=findstr((char*)RX_BUFF_TEMP,key1,len1,len2);
		printf("\nbuff: %s\n",buff);
		range_threshold=0;
		for(int i=1;;i++){
			range_threshold=range_threshold*10+(buff[len2+i]-'0');
			if(buff[len2+i+1]<'0'||buff[len2+i+1]>'9'){
				break;
			}
		}
		printf("range_threshold: %d\n\n",range_threshold);
	}
	
	if( NULL!=findstr((char*)RX_BUFF_TEMP,key2,len1,len3) )
	{
		buff=findstr((char*)RX_BUFF_TEMP,key2,len1,len3);
		printf("\nbuff: %s\n",buff);
		ledcontrol=0;
		for(int i=1;;i++){
			ledcontrol=ledcontrol*10+(buff[len3+i]-'0');
			if(buff[len3+i+1]<'0'||buff[len3+i+1]>'9'){
				break;
			}
		}
		printf("ledcontrol: %d\n\n",ledcontrol);
	}
}

void aliyun_gettopic()
{
	memset(get_topic,0,sizeof(get_topic));
  sprintf(get_topic,"/sys/%s/%s/thingrvice/propertyt",ProductKey,DeviceName);
	
	topic_string.cstring=(char*)publish_topic;
	mqtt_fifo_len = MQTTSerialize_publish((uint8_t*)mqtt_fifo, sizeof(mqtt_fifo), 0, req_qos, 0, packid, topic_string, (uint8_t*)publish_buff, len_temp);	
	transport_sendPacketBuffer(0, (uint8_t*)mqtt_fifo, mqtt_fifo_len);
}

void app_main_2task (void *argument) 
{
	tid_Thread1 = osThreadNew (Thread1, NULL, NULL);
	tid_Thread2 = osThreadNew (Thread2, NULL, NULL);
	tid_Thread3 = osThreadNew (Thread3, NULL, NULL);
}
