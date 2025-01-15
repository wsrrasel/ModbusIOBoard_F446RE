/*
 * OD_Callback.c
 *
 *  Created on: May 16, 2022
 *      Author: RASEL_EEE
 */

#include <Callbacks.h>
#include "MB_Addresses.h"
#include "Scheduler.h"
#include "MB_Serial.h"
#include <stdio.h>
#include "Debug.h"
#include "Flash_Address.h"
#include "Flash_Storag.h"
#include "TimeStamp.h"
#include "TaskHandler.h"
#include "mh_timer.h"

extern GlobalVar gVar;
extern Debug_ts debug;
extern TimeStamp timStamp;
extern UART_HandleTypeDef huart6;


/*
 * Callback functions for the modbus register
 * */
uint8_t CB_MB_readDICallback(MB_DIAddrEnum address, uint8_t *value);
uint8_t CB_MB_writeCOCallback( MB_CoilAddrEnum address, uint8_t value);
uint8_t CB_MB_readCOCallback( MB_CoilAddrEnum address, uint8_t *value);
uint8_t CB_MB_readIRCallback( MB_IRAddrEnum address, uint16_t *value);
uint8_t CB_MB_writeHRCallback( MB_HRAddrEnum address, uint16_t value);
uint8_t CB_MB_readHRCallback( MB_HRAddrEnum address, uint16_t *value);


static void PrintMBRequest(uint16_t address, uint16_t value, uint8_t errorStatus );


/*internal services---------*/
static void PrintMBRequest(uint16_t address, uint16_t value, uint8_t errorStatus ){
	if(errorStatus == MB_OK){
		DEBUG_SPRINT_NL("MB Reg: %d -> %d",address, value);
	}else{
		DEBUG_SPRINT_NL("MB Err: [%d] Invalid Request!",address);
	}

}



/**
  * @brief callback initialize
  */
void CB_MB_InitCallback(void){

	MB_attachDICallback(&CB_MB_readDICallback);
	MB_attachCOCallback(&CB_MB_writeCOCallback, &CB_MB_readCOCallback);
	MB_attachIRCallback(&CB_MB_readIRCallback);
	MB_attachHRCallback(&CB_MB_writeHRCallback, &CB_MB_readHRCallback);

}


uint8_t CB_MB_RcvByte(void){
	return (uint8_t)huart6.Instance->DR;
}
uint8_t CB_MB_Send(uint8_t *buff, uint16_t size){
	return HAL_UART_Transmit(&huart6, buff, size, (size+10));
}

void CB_MB_SetUpstream(void){
	HAL_GPIO_WritePin(RS485_DIR_GPIO_Port, RS485_DIR_Pin, GPIO_PIN_SET);
}
void CB_MB_SetDownstream(void){
	HAL_GPIO_WritePin(RS485_DIR_GPIO_Port, RS485_DIR_Pin, GPIO_PIN_RESET);
}


/**
  * @brief calback function for discrete input
  * @param reg: pointer to modbus registers
  * @param address: address of a register
  * @retval return error if an exception happen
  */
uint8_t CB_MB_readDICallback(MB_DIAddrEnum address, uint8_t *value){
	uint8_t errorStatus = MB_OK;
	switch (address) {
		case MB_DIA_DI1_STATUS:
			*value = gVar.di[DI_ID_0].state;
			break;
		case MB_DIA_DI2_STATUS:
			*value = gVar.di[DI_ID_1].state;
			break;
		case MB_DIA_DI3_STATUS:
			*value = gVar.di[DI_ID_2].state;
			break;
		case MB_DIA_DI4_STATUS:
			*value = gVar.di[DI_ID_3].state;
			break;
		case MB_DIA_DI5_STATUS:
			*value = gVar.di[DI_ID_4].state;
			break;
		case MB_DIA_DI6_STATUS:
			*value = gVar.di[DI_ID_5].state;
			break;
		case MB_DIA_DI7_STATUS:
			*value = gVar.di[DI_ID_6].state;
			break;
		case MB_DIA_DI8_STATUS:
			*value = gVar.di[DI_ID_7].state;
			break;
		default:
			errorStatus = MB_ERROR;
			break;
	}

	PrintMBRequest(address, *value, errorStatus);
	return errorStatus;
}




/**
  * @brief calback function for coil output
  * @param reg: pointer to modbus registers
  * @param address: address of a register
  * @retval return error if an exception happen
  */
uint8_t CB_MB_writeCOCallback(MB_CoilAddrEnum address, uint8_t value){
	uint8_t errorStatus = MB_OK;
	/*------Write your code here------*/
	switch (address) {
		case MB_COA_DO1_ENABLE:
			HAL_GPIO_WritePin(DO_0_GPIO_Port, DO_0_Pin, value>0);
			break;
		case MB_COA_DO2_ENABLE:
			HAL_GPIO_WritePin(DO_1_GPIO_Port, DO_1_Pin, value>0);
			break;
		case MB_COA_DO3_ENABLE:
			HAL_GPIO_WritePin(DO_2_GPIO_Port, DO_2_Pin, value>0);
			break;
		case MB_COA_DO4_ENABLE:
			HAL_GPIO_WritePin(DO_3_GPIO_Port, DO_3_Pin, value>0);
			break;
		case MB_COA_DO5_ENABLE:
			HAL_GPIO_WritePin(DO_4_GPIO_Port, DO_4_Pin, value>0);
			break;
		case MB_COA_DO6_ENABLE:
			HAL_GPIO_WritePin(DO_5_GPIO_Port, DO_5_Pin, value>0);
			break;
		case MB_COA_DO7_ENABLE:
			HAL_GPIO_WritePin(DO_6_GPIO_Port, DO_6_Pin, value>0);
			break;
		case MB_COA_DO8_ENABLE:
			HAL_GPIO_WritePin(DO_7_GPIO_Port, DO_7_Pin, value>0);
			break;

			/*Modbus------------*/
		case MB_COA_MB_SERIAL_SAVE_CONFIG:
			gVar.mbSerial.saveSrlCnfFlag = 1U;
			break;
		case MB_COA_MB_KEEP_ALIVE_ENABLE:
				(value>0) ?
				MH_Timer_Enable(&gVar.mbKpAlvTimer) :
				MH_Timer_Disable(&gVar.mbKpAlvTimer);
				FLASH_WriteInt(FSA_MB_KEEP_ALIVE_ENABLE, (value>0));
			break;

			/*Debug---------------------*/
		case MB_COA_DEBUG_ENABLE:
			debug.enable = value>0;
			FLASH_WriteInt(FSA_DEBUG_ENABLE, (value>0));
			break;
		default:
			errorStatus = MB_ERROR;
			break;
	}
	PrintMBRequest(address, value, errorStatus);
	return errorStatus;
}


/**
  * @brief calback function for reading coil output
  * @param reg: pointer to modbus registers
  * @param address: address of a register
  * @retval return error if an exception happen
  */
uint8_t CB_MB_readCOCallback( MB_CoilAddrEnum address, uint8_t *value){
	uint8_t errorStatus = MB_OK;
	switch (address) {
		case MB_COA_DO1_ENABLE:
			*value = HAL_GPIO_ReadPin(DO_0_GPIO_Port, DO_0_Pin);
			break;
		case MB_COA_DO2_ENABLE:
			*value = HAL_GPIO_ReadPin(DO_1_GPIO_Port, DO_1_Pin);
			break;
		case MB_COA_DO3_ENABLE:
			*value = HAL_GPIO_ReadPin(DO_2_GPIO_Port, DO_2_Pin);
			break;
		case MB_COA_DO4_ENABLE:
			*value = HAL_GPIO_ReadPin(DO_3_GPIO_Port, DO_3_Pin);
			break;
		case MB_COA_DO5_ENABLE:
			*value = HAL_GPIO_ReadPin(DO_4_GPIO_Port, DO_4_Pin);
			break;
		case MB_COA_DO6_ENABLE:
			*value = HAL_GPIO_ReadPin(DO_5_GPIO_Port, DO_5_Pin);
			break;
		case MB_COA_DO7_ENABLE:
			*value = HAL_GPIO_ReadPin(DO_6_GPIO_Port, DO_6_Pin);
			break;
		case MB_COA_DO8_ENABLE:
			*value = HAL_GPIO_ReadPin(DO_7_GPIO_Port, DO_7_Pin);
			break;
		case MB_COA_MB_SERIAL_SAVE_CONFIG:
			*value = gVar.mbSerial.saveSrlCnfFlag;
			break;

			/*Modbus------------*/
		case MB_COA_MB_KEEP_ALIVE_ENABLE:
			*value = gVar.mbKpAlvTimer.enable;
			break;

			/*Debug---------------------*/
		case MB_COA_DEBUG_ENABLE:
			*value = debug.enable;
			break;


		default:
				errorStatus = MB_ERROR;
			break;
	}
	PrintMBRequest(address, *value, errorStatus);
	return errorStatus;
}


/**
  * @brief calback function for input register
  * @param reg: pointer to modbus registers
  * @param address: address of a register
  * @retval return error if an exception happen
  */
uint8_t CB_MB_readIRCallback( MB_IRAddrEnum address, uint16_t *value){
	uint8_t errorStatus = MB_OK;

	switch (address) {
	/*Modbus------------*/
		case MB_IRA_SLAVE_ADDRESS:
			*value = gVar.mbRTUSlave.slave_address;
			break;
		default:
				errorStatus = MB_ERROR;
			break;
	}
	PrintMBRequest(address, *value, errorStatus);
	return errorStatus;
}

/**
  * @brief calback function for write holding register
  * @param reg: pointer to modbus registers
  * @param address: address of a register
  * @retval return error if an exception happen
  *
  */
uint8_t CB_MB_writeHRCallback( MB_HRAddrEnum address, uint16_t value){
	uint8_t errorStatus = MB_OK;
	switch (address) {
		/*DIs---------------------------*/
		case MB_HRA_DI0_DEBOUNCE_DELAY:
			gVar.di[DI_ID_0].debounce.delay = value;
			if(FLASH_WriteInt(FSA_DI0_DEBOUNCE_DELAY, value) != FLASH_ERR_NONE){
				errorStatus = MB_ERROR;
			}
			break;
		case MB_HRA_DI1_DEBOUNCE_DELAY:
			gVar.di[DI_ID_1].debounce.delay = value;
			if( FLASH_WriteInt(FSA_DI1_DEBOUNCE_DELAY, value) != FLASH_ERR_NONE){
				errorStatus = MB_ERROR;
			}
			break;
		case MB_HRA_DI2_DEBOUNCE_DELAY:
			gVar.di[DI_ID_2].debounce.delay = value;
			if( FLASH_WriteInt(FSA_DI2_DEBOUNCE_DELAY, value) != FLASH_ERR_NONE){
				errorStatus = MB_ERROR;
			}
			break;
		case MB_HRA_DI3_DEBOUNCE_DELAY:
			gVar.di[DI_ID_3].debounce.delay = value;
			if( FLASH_WriteInt(FSA_DI3_DEBOUNCE_DELAY, value) != FLASH_ERR_NONE){
				errorStatus = MB_ERROR;
			}
			break;
		case MB_HRA_DI4_DEBOUNCE_DELAY:
			gVar.di[DI_ID_4].debounce.delay = value;
			if( FLASH_WriteInt(FSA_DI4_DEBOUNCE_DELAY, value) != FLASH_ERR_NONE){
				errorStatus = MB_ERROR;
			}
			break;
		case MB_HRA_DI5_DEBOUNCE_DELAY:
			gVar.di[DI_ID_5].debounce.delay = value;
			if( FLASH_WriteInt(FSA_DI5_DEBOUNCE_DELAY, value) != FLASH_ERR_NONE){
				errorStatus = MB_ERROR;
			}
			break;
		case MB_HRA_DI6_DEBOUNCE_DELAY:
			gVar.di[DI_ID_6].debounce.delay = value;
			if( FLASH_WriteInt(FSA_DI6_DEBOUNCE_DELAY, value) != FLASH_ERR_NONE){
				errorStatus = MB_ERROR;
			}
			break;
		case MB_HRA_DI7_DEBOUNCE_DELAY:
			gVar.di[DI_ID_7].debounce.delay = value;
			if( FLASH_WriteInt(FSA_DI7_DEBOUNCE_DELAY, value) != FLASH_ERR_NONE){
				errorStatus = MB_ERROR;
			}
			break;

		/*Modbus Serial---------------------------*/
		case MB_HRA_MB_SERIAL_BAUDRATE:
			gVar.mbSerial.baudRate = value;
			break;
		case MB_HRA_MB_SERIAL_DATABIT:
			if( value == MBS_DB_7 || value == MBS_DB_8){
				gVar.mbSerial.dataBits = value;
			}else{
				errorStatus = MB_ERROR;
			}
			break;
		case MB_HRA_MB_SERIAL_PARITY:
			if( value == MBS_PB_NONE || value == MBS_PB_ODD || value == MBS_PB_EVEN){
				gVar.mbSerial.parityBit = value;
			}else{
				errorStatus = MB_ERROR;
			}
			break;
		case MB_HRA_MB_SERIAL_STOPBIT:
			if( value == MBS_SB_1 || value == MBS_SB_2){
				gVar.mbSerial.stopBit = value;
			}else{
				errorStatus = MB_ERROR;
			}
			break;
		case MB_HRA_MB_KEEP_ALIVE_TIMEOUT:
			if(value >= DEF_MB_SERIAL_KEEP_ALIVE_TIMEOUT_MIN && value <= UINT16_MAX){
				gVar.mbKpAlvTimer.timeout = value;
				if( FLASH_WriteInt(FSA_MB_KEEP_ALIVE_TIMEOUT, value) != FLASH_ERR_NONE){
					errorStatus = MB_ERROR;
				}
			}else{
				errorStatus = MB_ERROR;
			}
			break;
		case MB_HRA_MB_KEEP_ALIVE:
			MH_Timer_Reset(&gVar.mbKpAlvTimer);
			break;
		default:
				errorStatus = MB_ERROR;
			break;
	}
	PrintMBRequest(address, value, errorStatus);
	return errorStatus;
}

/**
  * @brief calback function for read holding register
  * @param reg: pointer to modbus registers
  * @param address: address of a register
  * @retval return error if an exception happen
  *
  */
uint8_t CB_MB_readHRCallback( MB_HRAddrEnum address, uint16_t *value){
	uint8_t errorStatus = MB_OK;
	switch (address) {
		/*DIs---------------------------*/
		case MB_HRA_DI0_DEBOUNCE_DELAY:
			*value = (uint16_t)gVar.di[DI_ID_0].debounce.delay;
			break;
		case MB_HRA_DI1_DEBOUNCE_DELAY:
			*value = (uint16_t)gVar.di[DI_ID_1].debounce.delay;
			break;
		case MB_HRA_DI2_DEBOUNCE_DELAY:
			*value = (uint16_t)gVar.di[DI_ID_2].debounce.delay;
			break;
		case MB_HRA_DI3_DEBOUNCE_DELAY:
			*value = (uint16_t)gVar.di[DI_ID_3].debounce.delay;
			break;
		case MB_HRA_DI4_DEBOUNCE_DELAY:
			*value = (uint16_t)gVar.di[DI_ID_4].debounce.delay;
			break;
		case MB_HRA_DI5_DEBOUNCE_DELAY:
			*value = (uint16_t)gVar.di[DI_ID_5].debounce.delay;
			break;
		case MB_HRA_DI6_DEBOUNCE_DELAY:
			*value = (uint16_t)gVar.di[DI_ID_6].debounce.delay;
			break;
		case MB_HRA_DI7_DEBOUNCE_DELAY:
			*value = (uint16_t)gVar.di[DI_ID_7].debounce.delay;
			break;

		/*Modbus Serial---------------------------*/
		case MB_HRA_MB_SERIAL_BAUDRATE:
			*value = gVar.mbSerial.baudRate;
			break;
		case MB_HRA_MB_SERIAL_DATABIT:
			*value = gVar.mbSerial.dataBits;
			break;
		case MB_HRA_MB_SERIAL_PARITY:
			*value = gVar.mbSerial.parityBit;
			break;
		case MB_HRA_MB_SERIAL_STOPBIT:
			*value = gVar.mbSerial.stopBit;
			break;
		case MB_HRA_MB_KEEP_ALIVE_TIMEOUT:
			*value = gVar.mbKpAlvTimer.timeout;
			break;
		case MB_HRA_MB_KEEP_ALIVE:
			*value = 0;
			break;

		default:
				errorStatus = MB_ERROR;
			break;
	}
	PrintMBRequest(address, *value, errorStatus);
	return errorStatus;
}
