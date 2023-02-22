#include "bsp_wifi.h"
#include "drv_serial.h"
#include "string.h"
#include "comm_config.h"
/**
  * @brief  
	* @param
  * @note   
	*/
	

	
#ifdef WIFI_EMW3080_ENABLED

/**
  * @brief  退出透传
	* @param
  * @note   
	*/
uint8_t __exit_transmission()
{
	if(!send_cmd_check(&drv_huart,(uint8_t*)"+++",(uint8_t*)"OK",20,1))
	{
		printf("退出透传模式\r\n");
		return 0;
	}
	return 1;
}

uint8_t wifi_init()
{
	for(uint8_t i=0;i<2;i++)
	{
		if(send_cmd_check(&drv_huart,(uint8_t*)"AT\r\n",(uint8_t*)"OK",10,5))//
		{
			__exit_transmission();
			HAL_Delay(500);
		}
		else
		{
			printf("检测到WIFI模块\r\n");

			return 0;
		}
	}
	return 1;
}

uint8_t wifi_sta_mode()
{
	uint8_t wifi_buff[70];
	memset(wifi_buff,0,70);
	if(send_cmd_check(&drv_huart,(uint8_t *)"AT+WSAPQ\r\n",(uint8_t*)"",200,5)){}
	if(send_cmd_check(&drv_huart,(uint8_t *)"AT+WJAPQ\r\n",(uint8_t*)"OK",200,5))return 2;
	HAL_Delay(1000);
	sprintf((char *)wifi_buff,"AT+WJAP=%s,%s\r\n",WIFI_ACCOUNT,WIFI_PASSWORD);
	if(send_cmd_check(&drv_huart,wifi_buff,(uint8_t*)"OK",2000,5))return 1;
	if(send_cmd_check(&drv_huart,(uint8_t *)"AT+CIPSTOP=1\r\n",(uint8_t*)"OK",200,5))return 4;
	memset(wifi_buff,0,70);
	sprintf((char *)wifi_buff,"AT+CIPSTART=1,tcp_client,%s,%d\r\n",SERVER_IP,SERVER_PORT);
	if(send_cmd_check(&drv_huart,wifi_buff,(uint8_t*)"OK",800,5))return 5;
	if(send_cmd_check(&drv_huart,(uint8_t *)"AT+CIPEVENT=OFF\r\n",(uint8_t*)"OK",200,5))return 6;
	if(send_cmd_check(&drv_huart,(uint8_t *)"AT+CIPRECVCFG=1\r\n",(uint8_t*)"OK",400,5))return 7;
	if(send_cmd_check(&drv_huart,(uint8_t *)"AT+CIPSENDRAW\r\n",(uint8_t*)"OK",200,5))return 8;

	return 0;
}

uint8_t wifi_sta_mode_prgam(char *account,char *password)
{
	uint8_t wifi_buff[70];
	memset(wifi_buff,0,70);
	if(send_cmd_check(&drv_huart,(uint8_t *)"AT+WSAPQ\r\n",(uint8_t*)"",200,5)){}
	if(send_cmd_check(&drv_huart,(uint8_t *)"AT+WJAPQ\r\n",(uint8_t*)"OK",200,5))return 2;
	HAL_Delay(1000);
	sprintf((char *)wifi_buff,"AT+WJAP=%s,%s\r\n",account,password);
	if(send_cmd_check(&drv_huart,wifi_buff,(uint8_t*)"OK",2000,5))return 1;
	if(send_cmd_check(&drv_huart,(uint8_t *)"AT+CIPSTOP=1\r\n",(uint8_t*)"OK",200,5))return 4;
	memset(wifi_buff,0,70);
	sprintf((char *)wifi_buff,"AT+CIPSTART=1,tcp_client,%s,%d\r\n",SERVER_IP,SERVER_PORT);
	if(send_cmd_check(&drv_huart,wifi_buff,(uint8_t*)"OK",800,5))return 5;
	if(send_cmd_check(&drv_huart,(uint8_t *)"AT+CIPEVENT=OFF\r\n",(uint8_t*)"OK",200,5))return 6;
	if(send_cmd_check(&drv_huart,(uint8_t *)"AT+CIPRECVCFG=1\r\n",(uint8_t*)"OK",400,5))return 7;
	if(send_cmd_check(&drv_huart,(uint8_t *)"AT+CIPSENDRAW\r\n",(uint8_t*)"OK",200,5))return 8;

	return 0;
}

uint8_t wifi_ap_mode()
{
	uint8_t wifi_buff[50];
	memset(wifi_buff,0,50);
	if(send_cmd_check(&drv_huart,(uint8_t *)"AT+WJAPQ\r\n",(uint8_t*)"OK",200,1)){}
	HAL_Delay(200);
	if(send_cmd_check(&drv_huart,(uint8_t *)"AT+WSAPQ\r\n",(uint8_t*)"OK",200,1)){}
	if(send_cmd_check(&drv_huart,(uint8_t *)"AT+CIPSTOP=0\r\n",(uint8_t*)"OK",200,1)){}
	HAL_Delay(500);
	sprintf((char *)wifi_buff,"AT+WSAP=%s,%s\r\n",AP_ACCOUNT,AP_PASSWORD);
	if(send_cmd_check(&drv_huart,wifi_buff,(uint8_t*)"OK",2000,5))return 1;
	HAL_Delay(500);
	if(send_cmd_check(&drv_huart,(uint8_t*)"AT+CIPSTART=0,tcp_server,8080\r\n",(uint8_t*)"OK",500,2)) return 2;
	HAL_Delay(400);
			if(send_cmd_check(&drv_huart,(uint8_t *)"AT+CIPEVENT=OFF\r\n",(uint8_t*)"OK",200,5))return 6;
		if(send_cmd_check(&drv_huart,(uint8_t *)"AT+CIPRECVCFG=1\r\n",(uint8_t*)"OK",400,5))return 7;
	if(send_cmd_check(&drv_huart,(uint8_t *)"AT+CIPSENDRAW\r\n",(uint8_t*)"OK",200,5))return 8;	
	return 0;	
}
#endif
