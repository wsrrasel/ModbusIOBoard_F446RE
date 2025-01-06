/*
 * Debug.h
 *
 *  Created on: Mar 10, 2022
 *      Author: RASEL_EEE
 */

#ifndef OD_LIB_DEBUG_INC_DEBUG_H_
#define OD_LIB_DEBUG_INC_DEBUG_H_

#include "main.h"

#define NEW_LINE 			"\r\n"
#define DBUG_STR_SIZE  		512
#define DBUG_RX_BUFF_SIZE  	128




typedef enum EnableState{
	DBUG_DISBLED = 0U,
	DBUG_ENABLED,
}EnableState;

typedef struct Debug{
	EnableState enable;
	uint16_t len;
	char str[DBUG_STR_SIZE];
	uint8_t rxBuff[DBUG_RX_BUFF_SIZE];
	uint8_t rxBuffSize;
	uint8_t isDataRcvd;
}Debug_ts;


void DEBUG_INIT(UART_HandleTypeDef *_huart, Debug_ts *_debug);
//HAL_StatusTypeDef DEBUG_RcvDMAStart(uint8_t *buffer, uint16_t size);
void DEBUG_ENABLE(void);
void DEBUG_DISABLE(void);
void DEBUG_PRINT(const char *pData);
int DEBUG_SPRINT_APPEND( char *pData, char *format, ...);
int DEBUG_SPRINT(char *format, ...);
int DEBUG_SPRINT_NL(char *format, ...);
void DEBUG_PRINT_INT(const char *str_h, uint32_t number, const char *str_t);
void DEBUG_PRINT_FLOAT(const char *str_h, float f_number, const char *str_t);

#endif /* OD_LIB_DEBUG_INC_DEBUG_H_ */
