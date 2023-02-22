#include "HTS221.h"

extern I2C_HandleTypeDef hi2c1;
#define pHTSi2c &hi2c1

int HTS221_IIC_Init(void)
{
	uint8_t myID;

	MX_I2C1_Init();
	HTS221_ReadReg(HTS221_WHO_AM_I, &myID, 1);	//读出的myID值若等于0xBC表示IIC通信正确
	if(myID!= 0xBC)
		return -1;								//初始化失败，请检查IIC线路，或HTS221故障
	
	HTS221_WriteReg(HTS221_CTRL_REG1, 0x81);	//0x81设置为开机、输出频率1Hz
	HTS221_WriteReg(HTS221_AV_CONF, 0x3f);		//0x3f 设置温度采样精度为0.007、湿度采样精度为0.03
	return 0;
}

void HTS221_WriteReg(uint8_t regAddr, uint8_t data)
{
	uint8_t tt[2];
	tt[0] = regAddr;
	tt[1] = data;
	HAL_I2C_Master_Transmit(pHTSi2c, HTS221_Address,tt,2,100);
}

void HTS221_ReadReg(uint8_t regAddr, uint8_t *pData, int len)
{
	// HTS221寄存器地址的MSB为1 表示地址自动递增，否则地址不变，读再多也是同一个地址
	if(len>1)
		regAddr |= 0x80;
	HAL_I2C_Master_Transmit(pHTSi2c, HTS221_Address, &regAddr, 1, 100);
	HAL_I2C_Master_Receive (pHTSi2c, HTS221_Address, pData,  len, 100);
}

int HTS221_Device_Get_Temperature_Humidity(HTTS221_REG *pRegs)
{
	uint8_t status;
	
	HTS221_ReadReg(HTS221_STATUS_REG, &status, 1);	//判断是否有新采样数据
	if(status!=3)
		return 0;									// 没有新采样数据

	HTS221_ReadReg(HTS221_H0_RH_X2, (uint8_t *)&pRegs->H0_rH_x2, 16);
	HTS221_ReadReg(HTS221_HR_OUT_L, (uint8_t *)&pRegs->H_OUT, 4);	//HTS221读温湿度寄存器必须放在后面，读完后将立刻进入下一轮采样，前面那些寄存器都会变化

	pRegs->H0_rH_x2 /=2;
	pRegs->H1_rH_x2 /=2;
	pRegs->T0_degC_x8 = ((pRegs->T1_T0_msb&0x03)<<8 | pRegs->T0_degC_x8 ) / 8;
	pRegs->T1_degC_x8 = ((pRegs->T1_T0_msb&0x0c)<<6 | pRegs->T1_degC_x8 ) / 8;

	pRegs->Humidity = (float)(pRegs->H_OUT - pRegs->H0_T0_OUT)*(pRegs->H1_rH_x2 - pRegs->H0_rH_x2)
							/(pRegs->H1_T0_OUT - pRegs->H0_T0_OUT) + pRegs->H0_rH_x2;
	pRegs->Temperature = (float)(pRegs->T_OUT - pRegs->T0_OUT)*(pRegs->T1_degC_x8 - pRegs->T0_degC_x8)
							/(pRegs->T1_OUT - pRegs->T0_OUT) + pRegs->T0_degC_x8;
	return 1;

}
