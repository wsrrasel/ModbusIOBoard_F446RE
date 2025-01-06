/*
 * Debug.c
 *
 *  Created on: Mar 10, 2022
 *      Author: RASEL_EEE
 */

#include "Debug.h"
#include "stdio.h"
#include "string.h"
#include "math.h"
#include <stdarg.h>
#include "TimeStamp.h"
#include "Scheduler.h"

extern GlobalVar gVar;
extern TimeStamp timStamp;
UART_HandleTypeDef *huart;
extern DMA_HandleTypeDef hdma_usart2_rx;
uint16_t DBUG_TransTimeout(int16_t len);

Debug_ts *_dbg;

/**
  * @brief  Initializes Debug port
   * @param  huart Pointer to a UART_HandleTypeDef structure that contains
  *	               the configuration information for the specified UART module.
  * @retval HAL status
  */
void DEBUG_INIT(UART_HandleTypeDef *_huart, Debug_ts *_debug){
	huart = _huart;
	_dbg = _debug;
	_dbg->len = 0;
	_dbg->rxBuffSize = 0;
	_dbg->isDataRcvd = 0;
	memset(_dbg->str, 0, sizeof(_dbg->str));
	memset(_dbg->rxBuff, 0, DBUG_RX_BUFF_SIZE);
//	memset(_dbg->rxMsg, 0, DBUG_RX_BUFF_SIZE);
	__HAL_UART_ENABLE_IT(_huart, UART_IT_RXNE);		// enable uart receive interrupt
//	DEBUG_RcvDMAStart(_dbg->rxBuff, DBUG_RX_BUFF_SIZE);

}

/**
 * @brief Starts receiving data using DMA in idle mode for debugging.
 *
 * This function initiates UART reception using DMA and disables the
 * half-transfer interrupt. It allows the reception of data into the
 * specified buffer until the line is idle or the buffer is full.
 *
 * @param[in]  buffer Pointer to the buffer where received data will be stored.
 * @param[in]  size   Size of the buffer in bytes.
 *
 * @return HAL_StatusTypeDef Status of the DMA start operation.
 *         HAL_OK if successful, or an error status if unsuccessful.
 *
 * @note The half-transfer interrupt is disabled in this function.
 */
//HAL_StatusTypeDef DEBUG_RcvDMAStart(uint8_t *buffer, uint16_t size){
//
//	HAL_StatusTypeDef status;
//	status = HAL_UARTEx_ReceiveToIdle_DMA(huart, buffer, size);
//	__HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);
//
//	return status;
//}


/**
 * @brief Enables the debug mode.
 *
 * This function sets the `enable` flag of the `debug` structure
 * to indicate that debugging is enabled.
 *
 * @return void No return value.
 */
void DEBUG_ENABLE(void){
	_dbg->enable = DBUG_ENABLED;
}


/**
 * @brief Disables the debug mode.
 *
 * This function sets the `enable` flag of the `debug` structure
 * to indicate that debugging is disabled.
 *
 * @return void No return value.
 */
void DEBUG_DISABLE(void){
	_dbg->enable = DBUG_DISBLED;
}

/**
  * @brief  Prints string
  * @param  pData Pointer to data buffer (uint8_t or uint16_t data elements).
  * @retval HAL status
  */
void DEBUG_PRINT(const char *pData){
	if(_dbg->enable == DBUG_ENABLED){
		int len = strlen(pData);
		if(len > 0U){

			HAL_UART_Transmit(huart, (uint8_t *)pData, len, DBUG_TransTimeout(len));
			memset((char *)pData,0,len);
		}
	}
}

/**
  * @brief  Prints string
  * @param  pData Pointer to data buffer (uint8_t or uint16_t data elements).
  * @retval HAL status
  */
int DEBUG_SPRINT(char *format, ...){
	if(_dbg->enable != DBUG_ENABLED) return 0;
	va_list aptr;
	int32_t ret;

	va_start(aptr, format);
	ret = vsprintf(_dbg->str, format, aptr);
	va_end(aptr);
	if(ret>0){
		_dbg->len = ret;
		DEBUG_PRINT(_dbg->str);
	}

	return ret;
}

/**
  * @brief  Prints string
  * @param  pData Pointer to data buffer (uint8_t or uint16_t data elements).
  * @retval HAL status
  */
int DEBUG_SPRINT_NL(char *format, ...){
	if(_dbg->enable != DBUG_ENABLED) return 0;
	va_list aptr;
	int32_t ret;

	va_start(aptr, format);
	ret = vsprintf(_dbg->str, format, aptr);
	va_end(aptr);
	if(ret>0){
		_dbg->len = ret;
		TS_GetTime(&timStamp);
		char temp[50];
		sprintf(temp, "%lu.%03lu.%03lu : ", timStamp.sec, timStamp.ms, timStamp.us );

//		HAL_UART_Transmit(huart, (uint8_t *)temp, size, DBUG_TransTimeout(size));

		DEBUG_PRINT(temp);
		DEBUG_PRINT(_dbg->str);
		DEBUG_PRINT("\r\n");
	}

	return ret;
}



/**
  * @brief  Prints string
  * @param  pData Pointer to data buffer (uint8_t or uint16_t data elements).
  * @retval HAL status
  */
int DEBUG_SPRINT_APPEND(char *pData, char *format, ...){
	if(_dbg->enable != DBUG_ENABLED) return 0;
	va_list aptr;
	int32_t ret;
	char buff[DBUG_STR_SIZE];

	va_start(aptr, format);
	ret = vsprintf(buff, format, aptr);
	va_end(aptr);

	_dbg->len = ret+strlen(pData);
	if(ret>0 &&  (_dbg->len<= DBUG_STR_SIZE)){
		strcat((char *)pData,buff );
	}
	return (ret>0 ? _dbg->len : ret);
	//	DEBUG_PRINT();
}


/**
  * @brief  Prints string
  * @param  pData Pointer to data buffer (uint8_t or uint16_t data elements).
  * @retval HAL status
  */
void DEBUG_PRINT_INT(const char *str_h, uint32_t number, const char *str_t){
	if(_dbg->enable != DBUG_ENABLED) return;
	DEBUG_PRINT(str_h);
	char str[4];
	sprintf(str,"%ld",number);
	DEBUG_PRINT(str);
	DEBUG_PRINT(str_t);
}

/**
  * @brief  Prints floating point number
  * @param  f_number floting point number.
  * @param  precision number of precisions after point
  * @retval HAL status
  */
void DEBUG_PRINT_FLOAT(const char *str_h, float f_number,  const char *str_t){
	if(_dbg->enable != DBUG_ENABLED) return;
	DEBUG_PRINT(str_h);
	char str[6];
	sprintf(str,"%0.2f",f_number);
	DEBUG_PRINT(str);
	DEBUG_PRINT(str_t);
}

/**
  * @brief  read form the debug port
  * @param  f_number floting point number.
  * @param  precision number of precisions after point
  * @retval HAL status
  */
void DEBUG_READ(uint8_t *buff){

}

/**
  * @brief enable build-in printf() function to print in serial terminal
  * @param none
  * @retval None
  */
void DEBUG_ENABLE_PRINTF(void){
	//TODO: to implement later
}


/**
  * @brief to get the transmission rate according to the number of byte to be sent
  * @param none
  * @retval return timeout in ms
  */
uint16_t DBUG_TransTimeout(int16_t len){
	return ceil(len*0.086806f);		// for 115200 baud rate byte transmission rate is 1.05 ms (aprox.)
}
