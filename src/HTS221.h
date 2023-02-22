#ifndef __HTS221_H
#define __HTS221_H

#include <stdint.h>
#include "stm32l4xx_hal.h"

#define IIC_CMSIS_MODE 1 //0 GPIO;  1 Device;  2 Driver 
typedef struct {
	int16_t H_OUT;
	int16_t T_OUT;
	int reserved1;
	uint8_t H0_rH_x2;
	uint8_t H1_rH_x2;
	uint8_t T0_degC_x8;
	uint8_t T1_degC_x8;
	uint8_t reserved2;
	uint8_t T1_T0_msb;
	int16_t H0_T0_OUT;
	uint16_t reserved3;
	int16_t H1_T0_OUT;
	int16_t T0_OUT;
	int16_t T1_OUT;
	float Humidity;
	float Temperature;
}HTTS221_REG;


#define HTS221_Address 			0xBE
#define HTS221_WHO_AM_I 		0x0F
#define HTS221_AV_CONF			0x10
#define HTS221_CTRL_REG1    	0x20
#define HTS221_CTRL_REG2    	0x21
#define HTS221_CTRL_REG3    	0x22
#define HTS221_STATUS_REG   	0x27
#define HTS221_HR_OUT_L     	0x28
#define HTS221_HR_OUT_H     	0x29
#define HTS221_TEMP_OUT_L  		0x2A
#define HTS221_TEMP_OUT_H  		0x2B

#define HTS221_H0_RH_X2      	0x30
#define HTS221_H1_RH_X2      	0x31
#define HTS221_T0_DEGC_X8    	0x32
#define HTS221_T1_DEGC_X8    	0x33
#define HTS221_T0_T1_DEGC_H2 	0x35
#define HTS221_H0_T0_OUT_L   	0x36
#define HTS221_H0_T0_OUT_H    	0x37
#define HTS221_H1_T0_OUT_L    	0x3A
#define HTS221_H1_T0_OUT_H    	0x3B
#define HTS221_T0_OUT_L       	0x3C
#define HTS221_T0_OUT_H       	0x3D
#define HTS221_T1_OUT_L       	0x3E
#define HTS221_T1_OUT_H       	0x3F

//GPIO模拟 方式
void IIC_Init(void);
void SDA_IN(void);
void SDA_OUT(void);
void IIC_Starts(void);
void IIC_Stop(void);
uint8_t IIC_Wait_ACK(void);
void IIC_Ack(void);
void IIC_NAck(void);
void IIC_Send_Byte(uint8_t data);
uint8_t IIC_Read_Byte(uint8_t ack);

uint8_t HTS221_Reg_Read(uint8_t reg);
void HTS221_Reg_Write(uint8_t reg,uint8_t data);

int  HTS221_Init(void);
float HTS221_Get_Temperature(void);
float HTS_221_Get_Humidity(void);



//Device 方式
void MX_I2C1_Init(void);

int  HTS221_IIC_Init(void);
void HTS221_WriteReg(uint8_t regAddr, uint8_t data);
void HTS221_ReadReg(uint8_t regAddr, uint8_t *pData, int len);
int  HTS221_Device_Get_Temperature_Humidity(HTTS221_REG *pRegs);

//Driver 方式
int  HTS221_IIC_Driver_Init(void);
int  HTS221_Driver_Get_Temperature_Humidity(HTTS221_REG *pRegs);

#endif

