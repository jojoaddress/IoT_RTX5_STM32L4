#include "HTS221.h"

extern I2C_HandleTypeDef hi2c1;
#define pHTSi2c &hi2c1

int HTS221_IIC_Init(void)
{
	uint8_t myID;

	MX_I2C1_Init();
	HTS221_ReadReg(HTS221_WHO_AM_I, &myID, 1);	//������myIDֵ������0xBC��ʾIICͨ����ȷ
	if(myID!= 0xBC)
		return -1;								//��ʼ��ʧ�ܣ�����IIC��·����HTS221����
	
	HTS221_WriteReg(HTS221_CTRL_REG1, 0x81);	//0x81����Ϊ���������Ƶ��1Hz
	HTS221_WriteReg(HTS221_AV_CONF, 0x3f);		//0x3f �����¶Ȳ�������Ϊ0.007��ʪ�Ȳ�������Ϊ0.03
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
	// HTS221�Ĵ�����ַ��MSBΪ1 ��ʾ��ַ�Զ������������ַ���䣬���ٶ�Ҳ��ͬһ����ַ
	if(len>1)
		regAddr |= 0x80;
	HAL_I2C_Master_Transmit(pHTSi2c, HTS221_Address, &regAddr, 1, 100);
	HAL_I2C_Master_Receive (pHTSi2c, HTS221_Address, pData,  len, 100);
}

int HTS221_Device_Get_Temperature_Humidity(HTTS221_REG *pRegs)
{
	uint8_t status;
	
	HTS221_ReadReg(HTS221_STATUS_REG, &status, 1);	//�ж��Ƿ����²�������
	if(status!=3)
		return 0;									// û���²�������

	HTS221_ReadReg(HTS221_H0_RH_X2, (uint8_t *)&pRegs->H0_rH_x2, 16);
	HTS221_ReadReg(HTS221_HR_OUT_L, (uint8_t *)&pRegs->H_OUT, 4);	//HTS221����ʪ�ȼĴ���������ں��棬��������̽�����һ�ֲ�����ǰ����Щ�Ĵ�������仯

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
