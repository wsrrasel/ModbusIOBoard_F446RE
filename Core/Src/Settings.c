/*
 * Settings.c
 *
 *  Created on: Oct 13, 2022
 *      Author: wsrra
 */


#include <Callbacks.h>
#include "Settings.h"
#include "main.h"
#include "Scheduler.h"
#include "Config.h"
#include "Debug.h"
#include "TimeStamp.h"
#include "Debounce.h"
#include "Flash_Address.h"
#include "Flash_Storag.h"
#include "Filters.h"

extern GlobalVar gVar;
extern Debug_ts debug;
extern TimeStamp timStamp;


extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart6;




/*
 * initial settings for the all kind of variables
 * */
void InitialSettings(void){
	// Utilities-----------------------

	/*Modbus RTU Serial line----------------------*/
	gVar.mbSerial.isDataReceived 	= 0U;
	gVar.mbSerial.isReceiving 		= 0U;
	gVar.mbSerial.isSending 		= 0U;
	gVar.mbSerial.frameOkFlag		= 0U;
	gVar.mbSerial.baudRate			= CONF_DEF_MB_SERIAL_BAUDRATE;
	gVar.mbSerial.dataBits			= CONF_DEF_MB_SERIAL_DATABIT;
	gVar.mbSerial.parityBit			= CONF_DEF_MB_SERIAL_PARITY;
	gVar.mbSerial.stopBit			= CONF_DEF_MB_SERIAL_STOPBIT;
	gVar.mbSerial.rcvByte 			= &CB_MB_RcvByte;
	gVar.mbSerial.send 				= &CB_MB_Send;
	gVar.mbSerial.setUpstream 		= &CB_MB_SetUpstream;
	gVar.mbSerial.setDownstream 	= &CB_MB_SetDownstream;
	HAL_GPIO_WritePin(RS485_RX_LED_GPIO_Port, RS485_RX_LED_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(RS485_TX_LED_GPIO_Port, RS485_TX_LED_Pin, GPIO_PIN_RESET);

	/*DIs----------------------------------------*/
	for(uint8_t i = DI_ID_0; i < DI_ID_MAX; i++){
		gVar.di[i].id 		= i;
		gVar.di[i].state 	= GPIO_PIN_RESET;
		Debounce_Init(&gVar.di[i].debounce, CONF_DEF_DI_DEBOUNCE_DELAY);
	}
	gVar.di[DI_ID_0].port 	= DI_0_GPIO_Port;
	gVar.di[DI_ID_0].pin 	= DI_0_Pin;
	gVar.di[DI_ID_1].port 	= DI_1_GPIO_Port;
	gVar.di[DI_ID_1].pin 	= DI_1_Pin;
	gVar.di[DI_ID_2].port 	= DI_2_GPIO_Port;
	gVar.di[DI_ID_2].pin 	= DI_2_Pin;
	gVar.di[DI_ID_3].port 	= DI_3_GPIO_Port;
	gVar.di[DI_ID_3].pin 	= DI_3_Pin;

	/*DOs----------------------------------------*/
	for(uint8_t i = DO_ID_0; i < DO_ID_MAX; i++){
		gVar.do_[i].id 		= i;
		gVar.do_[i].state 	= GPIO_PIN_RESET;
	}
	gVar.do_[DO_ID_0].port 	= DO_0_GPIO_Port;
	gVar.do_[DO_ID_0].pin 	= DO_0_Pin;
	gVar.do_[DO_ID_1].port 	= DO_1_GPIO_Port;
	gVar.do_[DO_ID_1].pin 	= DO_1_Pin;
	gVar.do_[DO_ID_2].port 	= DO_2_GPIO_Port;
	gVar.do_[DO_ID_2].pin 	= DO_2_Pin;
	gVar.do_[DO_ID_3].port 	= DO_3_GPIO_Port;
	gVar.do_[DO_ID_3].pin 	= DO_3_Pin;



	/*Push button-------------------------------*/
	gVar.pushButton.pressCount = 0;
	gVar.pushButton.port = SW_0_GPIO_Port;
	gVar.pushButton.pin = SW_0_Pin;
	gVar.pushButton.state = 0;
	Debounce_Init(&gVar.pushButton.debounce, DEF_RSTBTN_DEBOUNCE_DELAY);


}


/*Update settings from the storage*/
void UpdateSetting(void){
	uint8_t status = FLASH_Download();
	if( status != FLASH_ERR_NONE){
		DEBUG_SPRINT_NL("Storage: Error[%d]->Download Failed!",status);
	}else{
		DEBUG_SPRINT_NL("Storage: Download Successful!");
	}
	for(uint16_t address = 0; address < FSA_MAX; address++){
		uint32_t value = 0;
		FLASH_ReadInt(address, &value);
		switch (address) {
			/*DIs---------------------------*/
			case FSA_DI0_DEBOUNCE_DELAY:
				if(!(value < DEF_MAX_U16)){
					value = CONF_DEF_DI_DEBOUNCE_DELAY;
				}
				gVar.di[DI_ID_0].debounce.delay = value;
				break;
			case FSA_DI1_DEBOUNCE_DELAY:
				if(!(value < DEF_MAX_U16)){
					value = CONF_DEF_DI_DEBOUNCE_DELAY;
				}
				gVar.di[DI_ID_1].debounce.delay = value;
				break;
			case FSA_DI2_DEBOUNCE_DELAY:
				if(!(value < DEF_MAX_U16)){
					value = CONF_DEF_DI_DEBOUNCE_DELAY;
				}
				gVar.di[DI_ID_2].debounce.delay = value;
				break;
			case FSA_DI3_DEBOUNCE_DELAY:
				if(!(value < DEF_MAX_U16)){
					value = CONF_DEF_DI_DEBOUNCE_DELAY;
				}
				gVar.di[DI_ID_3].debounce.delay = value;
				break;

			/*Modbus Serial---------------------------*/
			case FSA_MB_SERIAL_BAUDRATE:
				if(!(value < DEF_MAX_U16)){
					value = CONF_DEF_MB_SERIAL_BAUDRATE;
				}
				gVar.mbSerial.baudRate = value;
				break;
			case FSA_MB_SERIAL_DATABIT:
				if(!(value == MBS_DB_7 || value == MBS_DB_8)){
					value = CONF_DEF_MB_SERIAL_DATABIT;
				}
				gVar.mbSerial.dataBits = value;
				break;
			case FSA_MB_SERIAL_PARITY:
				if(!(value == MBS_PB_NONE || value == MBS_PB_ODD || value == MBS_PB_EVEN)){
					value = CONF_DEF_MB_SERIAL_PARITY;
				}
				gVar.mbSerial.parityBit = value;
				break;
			case FSA_MB_SERIAL_STOPBIT:
				if(!(value == MBS_SB_1 || value == MBS_SB_2)){
					value = CONF_DEF_MB_SERIAL_STOPBIT;
				}
				gVar.mbSerial.stopBit = value;
				break;


				/*Debug---------------------*/
			case FSA_DEBUG_ENABLE:
				if(!(value == 0 || value == 1)){
					value = CONF_DEF_DEBUG_ENABLE;
				}
				debug.enable = value;
				break;
			default:
				break;
		}
	}
}

/**
 * @brief Resets the system to its factory settings by erasing the flash memory.
 *
 * This function performs a factory reset by erasing all data stored in the flash memory.
 * After execution, the flash memory will be completely cleared, and any stored data will
 * be lost.
 *
 * @note This function only erases the flash memory. Any other steps required to reset
 *       settings (such as restoring default values) should be handled separately.
 *
 * @attention Ensure that critical data is backed up before calling this function, as the
 *            flash memory will be irreversibly cleared.
 */
void FactorySettings(void){
	FLASH_Erase();
}


