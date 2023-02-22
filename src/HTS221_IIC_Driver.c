#include "Driver_I2C.h"
#include "cmsis_os2.h"
#include "HTS221.h"

osSemaphoreId_t sem_IIC;
static volatile char bI2C_Error;
extern ARM_DRIVER_I2C Driver_I2C1;
static ARM_DRIVER_I2C * I2Cdrv = &Driver_I2C1;

void IIC_callback(uint32_t event)
{
//ARM_I2C_EVENT_TRANSFER_INCOMPLETE  	Less data was transferred than requested
//ARM_I2C_EVENT_TRANSFER_DONE  			Transfer or receive is finished
//ARM_I2C_EVENT_ADDRESS_NACK  			Slave address was not acknowledged
//ARM_I2C_EVENT_ARBITRATION_LOST 	 	Master lost bus arbitration
//ARM_I2C_EVENT_BUS_ERROR 				Invalid start/stop position detected
//ARM_I2C_EVENT_BUS_CLEAR 				Bus clear operation completed
//ARM_I2C_EVENT_GENERAL_CALL			Slave was addressed with a general call address
//ARM_I2C_EVENT_SLAVE_RECEIVE			Slave addressed as receiver but SlaveReceive operation is not started
//ARM_I2C_EVENT_SLAVE_TRANSMIT			Slave addressed as transmitter but SlaveTransmit operation is not started
	uint32_t mask;
	mask = 	ARM_I2C_EVENT_TRANSFER_INCOMPLETE  	|
			ARM_I2C_EVENT_ADDRESS_NACK 			|
			ARM_I2C_EVENT_ARBITRATION_LOST 		|
			ARM_I2C_EVENT_BUS_ERROR;

	if (event & mask) {
		bI2C_Error = 1;
		osSemaphoreRelease(sem_IIC);
		return ;
	}
	if (event & ARM_I2C_EVENT_TRANSFER_DONE) {
		bI2C_Error = 0;
		osSemaphoreRelease(sem_IIC);
		return ;
	}
	if (event & ARM_I2C_EVENT_TRANSFER_INCOMPLETE) {
		bI2C_Error = 1;
		osSemaphoreRelease(sem_IIC);
		return ;
	}
	if (event & ARM_I2C_EVENT_BUS_CLEAR) {
		bI2C_Error = 0;
		return ;
	}
}

int IIC_Write (uint16_t addr, uint8_t *buf, uint32_t len, int timeout) 
{
	osStatus_t ret;
	addr >>=1;	//Driver��ʽ��оƬ��ַҪ�۳�d0λ����дλ��
	I2Cdrv->MasterTransmit (addr, buf, len, false);    
	ret = osSemaphoreAcquire (sem_IIC, timeout);
	if (ret != osOK || (ret == osOK && bI2C_Error)) return -1;
	return 0;
}

int IIC_Read (uint16_t addr, uint8_t regAddr, uint8_t *buf, uint32_t len, int timeout) 
{
	osStatus_t ret;
	addr >>=1;	//Driver��ʽ��оƬ��ַҪ�۳�d0λ����дλ��
	
	// HTS221�Ĵ�����ַ��MSBΪ1 ��ʾ��ַ�Զ������������ַ���䣬���ٶ�Ҳ��ͬһ����ַ
	if(len>1)
		regAddr |= 0x80;
	
	I2Cdrv->MasterTransmit (addr, &regAddr, 1, false);    
	ret = osSemaphoreAcquire (sem_IIC, timeout);
	if (ret != osOK || (ret == osOK && bI2C_Error)) return -1;

	I2Cdrv->MasterReceive (addr, buf, len, false);
	ret = osSemaphoreAcquire (sem_IIC, timeout);
	if (ret != osOK || (ret == osOK && bI2C_Error)) return -1;
	return 0;
}

const uint8_t regs_init_value[][2]={
	{HTS221_CTRL_REG1,	0x81},	//0x81 ����Ϊ���������Ƶ��1Hz
	{HTS221_AV_CONF, 	0x3f}	//0x3f �����¶Ȳ�������Ϊ0.007��ʪ�Ȳ�������Ϊ0.03
};
int HTS221_IIC_Driver_Init(void)
{
	uint8_t myID;

	sem_IIC = osSemaphoreNew(1, 0, NULL);

	I2Cdrv->Initialize   (IIC_callback);
	I2Cdrv->PowerControl (ARM_POWER_FULL);
	I2Cdrv->Control      (ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_FAST);// ARM_I2C_BUS_SPEED_STANDARD
	I2Cdrv->Control      (ARM_I2C_BUS_CLEAR, 0);

	IIC_Read (HTS221_Address, HTS221_WHO_AM_I, &myID, 1, 100);	//HTS221_WHO_AM_I ������Ϊ0xBC���ʾIICͨ����ȷ
	if(myID!= 0xBC)
		return -1;								//��ʼ��ʧ�ܣ�����IIC��·����HTS221����
	
	IIC_Write(HTS221_Address, (uint8_t *)regs_init_value[0], 2, 100);
	IIC_Write(HTS221_Address, (uint8_t *)regs_init_value[1], 2, 100);
	return 0;
}

int HTS221_Driver_Get_Temperature_Humidity(HTTS221_REG *pRegs)
{
	uint8_t status;
	
	if(IIC_Read (HTS221_Address, HTS221_STATUS_REG, &status, 1, 100)==-1 || status!=3)	//HTS221_STATUS_REG ����״ֵ̬Ϊ3��ʾ�¶Ⱥ�ʪ�����²�������
		return 0;
	
	if(IIC_Read (HTS221_Address, HTS221_H0_RH_X2, (uint8_t*)&pRegs->H0_rH_x2, 16, 500)==-1)
		return 0;
	//HTS221����ʪ�ȼĴ���������ں��棬��������̽�����һ�ֲ�����ǰ����Щ�Ĵ�������仯
	if(IIC_Read (HTS221_Address, HTS221_HR_OUT_L, (uint8_t*)&pRegs->H_OUT, 4, 500)==-1)
		return 0;
	
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

