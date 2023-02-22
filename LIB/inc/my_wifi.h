#ifndef __MY_WIFI_
#define __MY_WIFI_

#define FLASH_SAVE_ADDR  0X08020000

typedef struct 
{
	char wifi_account[50];    
	char wifi_password[50];
	char wifi_config_sta;	
	char wifi_sta_state;   //wifi «∑ÒΩ¯»Îsta
	
}WIFI_Dev_AP_Config_Typedef;

extern  WIFI_Dev_AP_Config_Typedef my_wifi;
#endif