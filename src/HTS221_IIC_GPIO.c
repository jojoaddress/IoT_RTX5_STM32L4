#include "HTS221.h"

extern void usSleep(uint32_t us);
#define IIC_SCL(a)   HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,(GPIO_PinState)a)
#define IIC_SDA(a)   HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,(GPIO_PinState)a)
#define READ_SDA     HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_9)  

void SDA_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin=GPIO_PIN_9;
    GPIO_InitStruct.Mode=GPIO_MODE_INPUT;
    HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);
}
void SDA_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin=GPIO_PIN_9;
    GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);
}

void IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    GPIO_InitStruct.Pin=GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);
    
    IIC_SDA(1);
    IIC_SCL(1);
}

void IIC_Starts(void)
{
    SDA_OUT();
    IIC_SCL(1);
    IIC_SDA(1);
    usSleep(2);
    IIC_SDA(0);
    usSleep(2);
    IIC_SCL(0);
}

void IIC_Stop(void)
{
    SDA_OUT();
    IIC_SCL(0);
    IIC_SDA(0);
    usSleep(2);
    IIC_SCL(1);
    usSleep(2);
    IIC_SDA(1);
}

uint8_t IIC_Wait_ACK(void)
{
    uint8_t ucErrTime= 0;
    SDA_IN();
//    IIC_SDA(1);usSleep(1);
    IIC_SCL(1);usSleep(1);
    while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
    IIC_SCL(0);
    return 0;
}

void IIC_Ack(void)
{
    IIC_SCL(0);
    SDA_OUT();
    IIC_SDA(0);
    usSleep(2);
    IIC_SCL(1);
    usSleep(2);
    IIC_SCL(0);
}

void IIC_NAck(void)
{
    IIC_SCL(0);
    SDA_OUT();
    IIC_SDA(1);
    usSleep(2);
    IIC_SCL(1);
    usSleep(2);
    IIC_SCL(0);
}

void IIC_Send_Byte(uint8_t data)
{
    uint8_t i;
    SDA_OUT();
    IIC_SCL(0);
    for(i = 0;i < 8; i++)
    {
        if((data&0x80)>>7)
            IIC_SDA(1);
        else
            IIC_SDA(0);
        data <<= 1;
        usSleep(2);
        IIC_SCL(1);
        usSleep(2);
        IIC_SCL(0);
        usSleep(2);
    }
}

uint8_t IIC_Read_Byte(uint8_t ack)
{
    uint8_t i,ret = 0;
    SDA_IN();
    for(i = 0;i < 8;i++)
    {
        IIC_SCL(0);
        usSleep(2);
        IIC_SCL(1);
        usSleep(2);
        ret <<= 1;
        if(READ_SDA)
            ret++;
        usSleep(2);
    }
    if (!ack)
        IIC_NAck();        //发送nACK
    else
        IIC_Ack();         //发送ACK   
    return ret;
}
//---------------------------------------------------------//\

void HTS221_Reg_Write(uint8_t reg,uint8_t data)
{
    IIC_Starts();
    IIC_Send_Byte(HTS221_Address);
    IIC_Wait_ACK();
    IIC_Send_Byte(reg);
    IIC_Wait_ACK();
    IIC_Send_Byte(data);
    IIC_Wait_ACK();
    IIC_Stop();
}

uint8_t HTS221_Reg_Read(uint8_t reg)
{
    uint8_t data_temp;
    IIC_Starts();
    IIC_Send_Byte(HTS221_Address);
    IIC_Wait_ACK();
    IIC_Send_Byte(reg);
    IIC_Wait_ACK();
    
    IIC_Starts();
    IIC_Send_Byte(HTS221_Address+1);
    IIC_Wait_ACK();
    data_temp = IIC_Read_Byte(0);
    IIC_Stop();
    return data_temp;
}

float HTS221_Get_Temperature(void)
{
	int16_t T0_out,T1_out,T_out,T0_degC, T1_degC;
	int16_t T0_degC_x8,T1_degC_x8;
	uint8_t buff[2],T0_T1_msb;
	float Temp;
	
	buff[0] = HTS221_Reg_Read(HTS221_T0_DEGC_X8);
	buff[1] = HTS221_Reg_Read(HTS221_T1_DEGC_X8);
	T0_T1_msb = HTS221_Reg_Read(HTS221_T0_T1_DEGC_H2);
	
	T0_degC_x8 = ((uint16_t)(T0_T1_msb & 0x03) << 8)| (uint16_t)buff[0];
	T1_degC_x8 = ((uint16_t)(T0_T1_msb & 0x0C) << 6)| (uint16_t)buff[1];
	T0_degC = T0_degC_x8 >> 3;
	T1_degC = T1_degC_x8 >> 3;
	
	buff[0] = HTS221_Reg_Read(HTS221_T0_OUT_L);
	buff[1] = HTS221_Reg_Read(HTS221_T0_OUT_H);
	T0_out = ((uint16_t)buff[1] << 8) | (uint16_t)buff[0];
	buff[0] = HTS221_Reg_Read(HTS221_T1_OUT_L);
	buff[1] = HTS221_Reg_Read(HTS221_T1_OUT_H);
	T1_out = ((uint16_t)buff[1] << 8) | (uint16_t)buff[0];
	
	buff[0] = HTS221_Reg_Read(HTS221_TEMP_OUT_L);
	buff[1] = HTS221_Reg_Read(HTS221_TEMP_OUT_H);
	T_out = ((uint16_t)buff[1] << 8) | (uint16_t)buff[0];
	
	Temp = (float)(T_out-T0_out)*(float)(T1_degC-T0_degC)/(float)(T1_out-T0_out)+T0_degC;
	return Temp;
}

float HTS_221_Get_Humidity(void)
{
	int16_t H0_T0_out,H1_T0_out,H_T_out;
	int16_t H0_RH,H1_RH;
	uint8_t buff[2];
	float HR;
	
	buff[0] = HTS221_Reg_Read(HTS221_H0_RH_X2);
	buff[1] = HTS221_Reg_Read(HTS221_H1_RH_X2);
	H0_RH = buff[0] >> 1;
	H1_RH = buff[1] >> 1;
	
	buff[0] = HTS221_Reg_Read(HTS221_H0_T0_OUT_L);
	buff[1] = HTS221_Reg_Read(HTS221_H0_T0_OUT_H);
	H0_T0_out = ((uint16_t)buff[1] << 8) | (uint16_t)buff[0];
	buff[0] = HTS221_Reg_Read(HTS221_H1_T0_OUT_L);
	buff[1] = HTS221_Reg_Read(HTS221_H1_T0_OUT_H);
	H1_T0_out = ((uint16_t)buff[1] << 8) | (uint16_t)buff[0];
	
	buff[0] = HTS221_Reg_Read(HTS221_HR_OUT_L);
	buff[1] = HTS221_Reg_Read(HTS221_HR_OUT_H);
	H_T_out = ((uint16_t)buff[1] << 8) | (uint16_t)buff[0];
	
	HR = (float)(H_T_out-H0_T0_out)*(float)(H1_RH-H0_RH)/(float)(H1_T0_out-H0_T0_out)+H0_RH;
	return HR;
}

int HTS221_Init(void)
{
	uint8_t myID;
	IIC_Init();
	myID = HTS221_Reg_Read(HTS221_WHO_AM_I);	//读出的myID值若等于0xBC表示IIC通信正确
	if(myID!= 0xBC)
		return -1;								//初始化失败，请检查IIC线路，或HTS221故障

	HTS221_Reg_Write(HTS221_CTRL_REG1, 0x81);	//0x81设置为开机、输出频率1Hz
	HTS221_Reg_Write(HTS221_AV_CONF, 0x3f);		//0x3f 设置温度采样精度为0.007、湿度采样精度为0.03
	return 0;
}

