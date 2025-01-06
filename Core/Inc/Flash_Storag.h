/*
 * Flash_Storage.c
 *
 *  Created on: Mar 25, 2022
 *      Author: RASEL_EEE
 */

#ifndef INC_FLASH_STORAGE_H_
#define INC_FLASH_STORAGE_H_

#include "main.h"
#include "Flash_Address.h"

#define FLASH_MAX_STORAGE_SIZE			FSA_MAX //byte
#define FLASH_SECTOR_START_ADDRESS 			0x08004000U
#define FLASH_SECTOR_SIZE					0x4000U		//16KByte

typedef enum{
	FLASH_ERR_NONE = 0,
	FLASH_ERR_ERASE,
	FLASH_ERR_PROGRAM,
	FLASH_ERR_NULL_PTR,
	FLASH_ERR_INVALID_ADDRESS,
	FLASH_ERR_LENGTH_OUTOFRANGE,
}FLASH_Error_te;




uint8_t FLASH_Download(void);
uint8_t FLASH_Upload(void);
void FLASH_CheckForChanges(void);
uint8_t FLASH_WriteInt(uint32_t address, uint32_t pData);
uint8_t FLASH_ReadInt(uint32_t address, uint32_t *pData);
uint8_t FLASH_Read(uint32_t startAddress, uint32_t *pData, uint16_t nData);
uint8_t FLASH_Write(uint32_t startAddress, uint32_t *pData, uint16_t nData);
uint8_t FLASH_Erase(void);
#endif /* INC_FLASH_STORAGE_C_ */
