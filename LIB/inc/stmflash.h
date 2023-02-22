#ifndef __STMFLASH_H
#define __STMFLASH_H
#include "sys.h"


typedef enum 
{
	FLASH_OK  = 0x0,
	FLASH_ERR = 0x1,
	FLASH_CHECK_ERR=0x3
	
}flash_status;

flash_status Flash_If_Erase(uint32_t Add);
flash_status Flash_If_Write( uint32_t dest_addr,uint8_t *src, uint32_t Len);  
flash_status Flash_If_Read(uint8_t* buff, uint32_t dest_addr, uint32_t Len);     





#endif
