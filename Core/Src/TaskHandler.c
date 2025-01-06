/*
 * TaskHandler.c
 *
 *  Created on: Oct 2, 2024
 *      Author: Lenovo X1 yoga
 */

#include "TaskHandler.h"
#include "MB_Serial.h"
#include "Scheduler.h"
#include "Debounce.h"
#include "TimeStamp.h"
#include "Debug.h"
#include "Flash_Address.h"
#include "Flash_Storag.h"
#include "Settings.h"

extern GlobalVar gVar;
extern Debug_ts debug;
extern TimeStamp timStamp;


extern TIM_HandleTypeDef htim3;


/**
 * @brief Monitors and updates the Modbus slave address if it changes.
 *
 * This function checks the hardware pins associated with the Modbus slave address
 * at regular intervals. If a change in the slave address is detected, the new
 * address is updated in the system and the change is logged.
 *
 * @note The slave address is read from GPIO pins (SA_0, SA_1, SA_2, SA_3).
 * The function only updates the address if the new address is non-zero.
 *
 * @param None
 *
 * @return void
 */
void TH_MB_ChecktSlaveAddressChange(void){

	uint32_t tick = 0;
	if((HAL_GetTick() - tick)>= DEF_MB_SLAVEADDR_CHECKING_INTERVAL){
		union{
			uint8_t byte;
			struct{
				uint8_t SA_0 : 1;
				uint8_t SA_1 : 1;
				uint8_t SA_2 : 1;
				uint8_t SA_3 : 1;
				uint8_t notUsed : 4;
			}bit;
		}slvAddr;

		slvAddr.byte = 0;

		slvAddr.bit.SA_0 = (uint8_t)HAL_GPIO_ReadPin(SA_0_GPIO_Port, SA_0_Pin);
		slvAddr.bit.SA_1 = (uint8_t)HAL_GPIO_ReadPin(SA_1_GPIO_Port, SA_1_Pin);
		slvAddr.bit.SA_2 = (uint8_t)HAL_GPIO_ReadPin(SA_2_GPIO_Port, SA_2_Pin);
		slvAddr.bit.SA_3 = (uint8_t)HAL_GPIO_ReadPin(SA_3_GPIO_Port, SA_3_Pin);


		if((gVar.mbRTUSlave.slave_address != slvAddr.byte) && (slvAddr.byte > 0)){
			DEBUG_SPRINT_APPEND(debug.str,"Slave Address: [Changed] [%d(Old)->", gVar.mbRTUSlave.slave_address);
			gVar.mbRTUSlave.slave_address = slvAddr.byte;
			DEBUG_SPRINT_APPEND(debug.str,"%d(New)]", gVar.mbRTUSlave.slave_address);
			DEBUG_SPRINT_NL(debug.str);
		}
	}
}



void TH_MB_CheckSerialConfigChanges(void){

	/* Check the save command to save and apply
	 * the serial configuration changes
	 * */
	if(!gVar.mbSerial.saveSrlCnfFlag) {return;}
	gVar.mbSerial.saveSrlCnfFlag = 0;
	/*Apply the changes*/
	MBS_Config(&gVar.mbSerial);

	/*Save the changes*/
	FLASH_WriteInt(FSA_MB_SERIAL_BAUDRATE, gVar.mbSerial.baudRate);
	FLASH_WriteInt(FSA_MB_SERIAL_DATABIT, gVar.mbSerial.dataBits);
	FLASH_WriteInt(FSA_MB_SERIAL_PARITY, gVar.mbSerial.parityBit);
	FLASH_WriteInt(FSA_MB_SERIAL_STOPBIT, gVar.mbSerial.stopBit);
}




/*DIs-------------------------------------*/
/**
 * @brief Reads and debounces digital input states.
 *
 * This function iterates through all defined digital input IDs, reads their
 * current state from the corresponding GPIO pins, and applies a debounce
 * algorithm to each input. The debounced state is then updated in the
 * corresponding global variable structure.
 *
 * @note The debounce function is responsible for filtering out noise and
 *       providing stable digital input states.
 *
 * @param None
 *
 * @return void
 */
void TH_DI(void){
	uint32_t currentTime = 0;
	uint8_t inputState = 0;

	for(uint8_t i = DI_ID_0; i < DI_ID_MAX; i++){
		currentTime = (uint32_t)TS_GetUS(&timStamp);;
		inputState = HAL_GPIO_ReadPin(gVar.di[i].port, gVar.di[i].pin);
		uint8_t status = Debounce(&gVar.di[i].debounce, currentTime, inputState);
		gVar.di[i].state = gVar.di[i].debounce.state;
		if(status){
//			DEBUG_SPRINT_NL("DI_%d: {State: %d}",i, gVar.di[i].state);
		}
	}
}



/*DOs-------------------------------------*/
/**
 * @brief Monitors and logs the state changes of digital output (DO) pins.
 *
 * This function checks the state of each digital output pin. If the state of a pin
 * has changed since the last check, the new state is logged. The function loops
 * through all digital outputs defined by `DO_ID_0` to `DO_ID_MAX`.
 *
 * @note The states of the digital output pins are stored in the global `gVar` structure.
 * The function compares the current state with the last known state and updates it accordingly.
 *
 * @param None
 *
 * @return void
 */
void TH_DO(void){
	for(uint8_t i = DO_ID_0; i < DO_ID_MAX; i++){
		gVar.do_[i].state = HAL_GPIO_ReadPin(gVar.do_[i].port, gVar.do_[i].pin);
		if(gVar.do_[i].state != gVar.do_[i].lastState){
			gVar.do_[i].lastState = gVar.do_[i].state;
			DEBUG_SPRINT_NL("DO_%d: {State: %d}",i, gVar.do_[i].state);
		}
	}
}




/*Pudh Button------------------------------*/
/**
 * @brief Initiates a factory reset if a button is held down for a specified duration.
 *
 * This function monitors a designated button (`SW_0_Pin`). If the button is pressed and held
 * for the duration defined by `DEF_FACTORY_RESET_DELAY`, it triggers a factory reset by calling
 * `FactorySettings()`, clearing all data in flash memory. The system will then restart.
 *
 * @details
 * - The function tracks the button's state using a static `tick` variable.
 * - Once the button is held for the required duration, it performs the factory reset,
 *   prints status messages, and finally calls `NVIC_SystemReset()` to reset the MCU.
 * - If the button is released before the time elapses, the timer is reset.
 *
 * @note Ensure that this function is not called unintentionally, as the factory reset will
 *       erase all stored data.
 *
 * @attention This function causes an MCU reset, and all unsaved data will be lost.
 */
void TH_PushButton(void){
	static uint32_t tick = 0U;

	uint32_t currentTime = (uint32_t)TS_GetUS(&timStamp);;
	uint8_t inputState = HAL_GPIO_ReadPin(SW_0_GPIO_Port, SW_0_Pin);
	Debounce(&gVar.pushButton.debounce, currentTime, inputState);
	gVar.pushButton.state = gVar.pushButton.debounce.state;
	if(gVar.pushButton.state){
		/*Factory reset*/
		if((HAL_GetTick() - tick) >= DEF_RSTBTN_FACT_RST_DELAY){
			DEBUG_SPRINT_NL("Factory Reset: Started");
			FactorySettings();
			DEBUG_SPRINT_NL("Factory Reset: Done!");
			HAL_Delay(1);
			DEBUG_SPRINT_NL("Device Restarting...");
			NVIC_SystemReset();		// to reset the mcu
		}
	}else{
		tick = HAL_GetTick();
	}
}

