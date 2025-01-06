/*
 * Flash_Storage.c
 *
 *  Created on: Mar 25, 2022
 *      Author: RASEL_EEE
 */
#include "Flash_Storag.h"
#include "string.h"
#include "stdio.h"
#include "Debug.h"
#include "main.h"


uint32_t FLASH_LOCAL_BUFF[FLASH_MAX_STORAGE_SIZE] = {0xFFFFFFFF};
uint8_t isUnsaved = 0;



/**
 * @brief Downloads the contents of the flash memory into the local buffer.
 *
 * This function reads the contents of the flash memory starting from address 0 and stores
 * the data into the local buffer (`FLASH_LOCAL_BUFF`). It downloads the entire flash memory
 * contents up to `FLASH_MAX_STORAGE_SIZE`.
 *
 * @retval FLASH_ERR_NONE          The operation was successful.
 * @retval FLASH_ERR_NULL_PTR      The pointer to the data is `NULL`.
 * @retval FLASH_ERR_LENGTH_OUTOFRANGE  The data length exceeds the maximum allowed storage size.
 *
 * @note This function is typically used to sync the local buffer with the current data stored in
 *       flash memory.
 *
 * @attention Ensure that the flash memory is unlocked before invoking this function.
 */
uint8_t FLASH_Download(void){
	uint8_t status = FLASH_Read(0, FLASH_LOCAL_BUFF, FLASH_MAX_STORAGE_SIZE);
	if(status != FLASH_ERR_NONE){
		memset(FLASH_LOCAL_BUFF, 0xFF, sizeof(FLASH_LOCAL_BUFF[0])*FLASH_MAX_STORAGE_SIZE);
	}
	return status;
}

/**
 * @brief Uploads the local buffer contents to the flash memory.
 *
 * This function writes the contents of the local buffer (`FLASH_LOCAL_BUFF`) to the flash memory
 * starting from address 0. It uploads the entire buffer, which has a maximum size defined
 * by `FLASH_MAX_STORAGE_SIZE`.
 *
 * @retval FLASH_ERR_NONE          The operation was successful.
 * @retval FLASH_ERR_NULL_PTR      The pointer to the data is `NULL`.
 * @retval FLASH_ERR_LENGTH_OUTOFRANGE  The data length exceeds the maximum allowed storage size.
 * @retval FLASH_ERR_ERASE         An error occurred while erasing the flash memory.
 * @retval FLASH_ERR_PROGRAM       An error occurred while programming the flash memory.
 *
 * @note The function first reads the current contents of the flash memory, modifies it with the
 *       new data, erases the flash sector, and then writes the modified data back to the flash memory.
 *
 * @attention Ensure that the flash memory is unlocked before invoking this function.
 */
uint8_t FLASH_Upload(void){
	return FLASH_Write(0, FLASH_LOCAL_BUFF, FLASH_MAX_STORAGE_SIZE);
}

/**
 * @brief Checks for unsaved changes and uploads to flash memory if necessary.
 *
 * This function checks if there are unsaved changes in memory by evaluating the `isUnsaved` flag.
 * If changes exist, it attempts to upload the changes to flash memory using the `FLASH_Upload()` function.
 * Upon successful upload, it logs a success message; otherwise, it logs an error message.
 */
void FLASH_CheckForChanges(void){
	if(isUnsaved){
		isUnsaved = 0;
		uint8_t status = FLASH_Upload();
		if(status != FLASH_ERR_NONE){
			DEBUG_SPRINT_NL("FLASH: Upload failed! [Error-%d] ",status);
		}else{
			DEBUG_SPRINT_NL("FLASH: Upload successful!");
		}
	}
}


/**
 * @brief Reads a 32-bit integer from a specific address in flash memory.
 *
 * This function reads a single 32-bit word from flash memory at the specified address
 * and stores the result in the provided `pData` buffer.
 *
 * @param address The address in flash memory (in words) to read from.
 * @param pData Pointer to a 32-bit variable where the read data will be stored.
 *
 * @return uint8_t Status of the flash read operation:
 * 		   - 'FLASH_ERR_INVALID_ADDRESS'
 *         - `FLASH_ERR_NONE` on successful operation.
 */
uint8_t FLASH_ReadInt(uint32_t address, uint32_t *pData){
	if(address > FLASH_MAX_STORAGE_SIZE){
		return FLASH_ERR_INVALID_ADDRESS;
	}
	*pData = FLASH_LOCAL_BUFF[address];
	return FLASH_ERR_NONE;
}



/**
 * @brief Writes a 32-bit integer value to a specified address in the flash buffer.
 *
 * This function writes the value pointed to by `pData` into the local buffer (`FLASH_LOCAL_BUFF`)
 * at the specified address. It also sets the `isUnsaved` flag to `1`, indicating that the buffer
 * has unsaved changes that need to be written to flash memory.
 *
 * @param[in] address  The index in the flash buffer where the value will be written. The value should
 *                     be within the bounds of the buffer size.
 * @param[in] pData    Pointer to the 32-bit integer data to be written into the buffer.
 *
 * @retval error status
 *
 * @note Ensure that the `address` is within the valid range of `FLASH_LOCAL_BUFF` to avoid buffer
 *       overflow issues.
 *
 * @attention After writing, the `isUnsaved` flag is set to signal that the buffer has been modified
 *            and will need to be written back to flash memory at a later time.
 */
uint8_t FLASH_WriteInt(uint32_t address, uint32_t pData){
	if(address > FLASH_MAX_STORAGE_SIZE){
		return FLASH_ERR_INVALID_ADDRESS;
	}
	FLASH_LOCAL_BUFF[address] = pData;
	isUnsaved = 1;
	return FLASH_ERR_NONE;
}


/**
 * @brief Reads data from flash memory.
 *
 * This function reads a specified number of 32-bit words from flash memory, starting at the
 * provided address, and stores the data into the provided buffer.
 *
 * @param startAddress The starting address in flash memory (in words) to read from.
 * @param pData Pointer to the buffer where the read data will be stored.
 * @param nData Number of 32-bit words to be read.
 *
 * @return uint8_t Status of the flash read operation:
 *         - `FLASH_ERR_NULL_PTR` if the data pointer is NULL.
 *         - `FLASH_ERR_LENGTH_OUTOFRANGE` if the requested data size exceeds the flash memory limits.
 *         - `FLASH_ERR_NONE` on successful operation.
 */
uint8_t FLASH_Read(uint32_t startAddress, uint32_t *pData, uint16_t nData){
	if(pData == NULL){
		return FLASH_ERR_NULL_PTR;
	}if(startAddress+nData > FLASH_MAX_STORAGE_SIZE){
		return FLASH_ERR_LENGTH_OUTOFRANGE;
	}
	for(uint32_t i = 0; i < nData; i++){
		pData[i] = *(__IO uint32_t *)(FLASH_SECTOR_START_ADDRESS+((startAddress+i)*4));
	}
	return FLASH_ERR_NONE;
}

/**
 * @brief Writes data to flash memory.
 *
 * This function writes a given array of data to flash memory starting at the specified
 * address. It reads the current contents of flash memory into a buffer, modifies the buffer
 * with new data, erases the relevant sector(s), and writes the modified buffer back to flash memory.
 *
 * @param startAddress The starting address in flash memory to write to.
 * @param pData Pointer to the data to be written to flash memory.
 * @param nData Number of 32-bit words to be written.
 *
 * @return uint8_t Status of the flash write operation:
 *         - `FLASH_ERR_NULL_PTR` if the data pointer is NULL.
 *         - `FLASH_ERR_LENGTH_OUTOFRANGE` if the length of data exceeds flash memory size.
 *         - `FLASH_ERR_ERASE` if the flash erase operation fails.
 *         - `FLASH_ERR_PROGRAM` if the flash programming operation fails.
 *         - `FLASH_ERR_NONE` on successful operation.
 */
uint8_t FLASH_Write(uint32_t startAddress, uint32_t *pData, uint16_t nData){
	if(pData == NULL){
		return FLASH_ERR_NULL_PTR;
	}if(startAddress+nData > FLASH_MAX_STORAGE_SIZE){
		return FLASH_ERR_LENGTH_OUTOFRANGE;
	}
	uint16_t buffSize = FLASH_MAX_STORAGE_SIZE;
	uint32_t buff[FLASH_MAX_STORAGE_SIZE] = {0};
	uint8_t status = 0;

	/*Read*/
	status = FLASH_Read(0, buff, buffSize);
	if(status != FLASH_ERR_NONE){
		return status;
	}

	/*Modify*/
	memcpy(&buff[startAddress], pData, sizeof(pData[0])*nData);


	/*Erase*/
	status = FLASH_Erase();
	if(status != HAL_OK){
		return FLASH_ERR_ERASE;
	}

	/*Write*/
	HAL_FLASH_Unlock();
	for(uint16_t i = 0; i < FLASH_MAX_STORAGE_SIZE; i++){
		status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, (FLASH_SECTOR_START_ADDRESS+(4*i)), buff[i]);
		if(status != HAL_OK){
			return FLASH_ERR_PROGRAM;
		}

	}

	HAL_FLASH_Lock();

	return FLASH_ERR_NONE;
}


/**
 * @brief Erases a sector of flash memory.
 *
 * This function erases one sector of flash memory (sector 1 in this case)
 * on bank 1. It unlocks the flash, clears any relevant error flags, performs the
 * erase operation, and locks the flash afterward.
 *
 * @note The erase operation is configured for sector 1 on bank 1. The voltage
 *       range is set to `VOLTAGE_RANGE_3`. Adjust the sector or bank as needed.
 *
 * @return uint8_t Returns the status of the flash erase operation:
 *         - `HAL_OK` on success.
 *         - `HAL_ERR` on failure.
 */
uint8_t FLASH_Erase(void){
	HAL_StatusTypeDef status = 0;
	static FLASH_EraseInitTypeDef flasEraseInit;
	uint32_t pageError;
	flasEraseInit.TypeErase = FLASH_TYPEERASE_SECTORS;
	flasEraseInit.Banks =  FLASH_BANK_1;
	flasEraseInit.Sector =  FLASH_SECTOR_1;
	flasEraseInit.NbSectors = 1U;
	flasEraseInit.VoltageRange = VOLTAGE_RANGE_3;

	HAL_FLASH_Unlock();

    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP);
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPERR);
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_WRPERR);
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PGAERR);
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PGPERR);
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PGSERR);

    status = HAL_FLASHEx_Erase(&flasEraseInit, &pageError);

	HAL_FLASH_Lock();

	if(status != HAL_OK){
		memset(FLASH_LOCAL_BUFF, 0xFF, sizeof(FLASH_LOCAL_BUFF[0])*FLASH_MAX_STORAGE_SIZE);
	}

	return status;
}
