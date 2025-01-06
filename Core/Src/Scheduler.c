/*
 * Scheduler.c
 *
 *  Created on: Oct 7, 2022
 *      Author: wsrra
 */

#include <Callbacks.h>
#include "Scheduler.h"
#include "main.h"
#include <stdio.h>
#include <math.h>
#include "Settings.h"
#include "Define.h"
#include "stm32f4xx_hal_rcc.h"
#include "TimeStamp.h"
#include "Debug.h"
#include "TaskHandler.h"
#include "Flash_Storag.h"
#include "Flash_Address.h"

extern GlobalVar gVar;
extern Debug_ts debug;
extern TimeStamp timStamp;

extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim9;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart6;




/*
 * Schedules
 * */
typedef enum{
	SCHEDULE_VERY_FAST = 0U,		// 100ms Schedule
	SCHEDULE_FAST,		// 100ms Schedule
	SCHEDULE_MEDIUM,		// 100ms Schedule
	SCHEDULE_SLOW,			// 500ms Schedule
	SCHEDULE_VERY_SLOW,			// 500ms Schedule
	MAX_SCHEDULE,
}OD_Schedules;


/*
 * non-blocking Schedule interval time using SysTick
 * */
typedef enum{
	INTERVAL_5MS = 5U,			// 5ms delay time
	INTERVAL_20MS = 20U,			// 20ms delay time
	INTERVAL_50MS = 50U,			// 50ms delay time
	INTERVAL_100MS = 100U,		// 500ms delay time
	INTERVAL_200MS = 200U,		// 500ms delay time
	INTERVAL_250MS = 250U,		// 500ms delay time
	INTERVAL_500MS = 500U,		// 500ms delay time
	INTERVAL_1000MS = 1000U,		// 1020ms delay time
	INTERVAL_5000MS = 5000U,		// 2080ms delay time
	INTERVAL_10000MS = 10000U,		// 10000ms delay time
}OD_ScheduleInterval;



uint32_t LastScheduleTime[MAX_SCHEDULE] = {0U};
void SetLastScheduleTime(uint8_t schedule);
uint8_t TimeReached(uint8_t schedule, uint16_t interval);

/*Loop functions*/
void SupperLoop(void);
void MediumLoop(void);
void VerySlowLoop(void);


void SchedulerInit(void){
	HAL_Delay(1000);
	/*Power LED-------------------------------*/
	HAL_GPIO_WritePin(PWR_LED_GPIO_Port, PWR_LED_Pin, GPIO_PIN_SET);

	/*Time stame init-------------------------*/
	TS_Init(&timStamp, &htim9);
	TS_SetTimerType(&timStamp, TS_TIM_TYP_TIMER);
	TS_StartTimer(&timStamp);
	DEBUG_INIT(&huart2, &debug);
	DEBUG_ENABLE();

	DEBUG_SPRINT("\n\n");
	DEBUG_SPRINT_NL("---------------Started---------------");




	/*Print HW & FW Version-------------------*/
	PrintVersion();
	PrintBoardInfo();






	/*Initialize all the settings-------------*/
	InitialSettings();
	UpdateSetting();

	/*Initialize the modbus serial line-------*/
	MBRS_Init(&gVar.mbRTUSlave, CONF_DEF_MBRTUS_SLAVE_ADDRESS);
	MBS_Init(&gVar.mbSerial, &huart6, &htim4, &htim5);		// initialize modbus serial
	MBS_Config(&gVar.mbSerial);								// to configure the modbus serial and 3.5 and 1.5 char timer


	/*Callback--------------------------------*/
	CB_MB_InitCallback();

	/*Print debug status*/
	uint8_t temp = debug.enable;
	debug.enable = 1;
	DEBUG_SPRINT_NL( "Debug: %sabled!", (temp? "En":"Dis"));
	debug.enable = temp;
}


/**
  * @brief Scheduler A is used for the non blocking task
  *
  */
void Scheduler(void){



	SupperLoop();

	if(TimeReached(SCHEDULE_SLOW, INTERVAL_250MS)){
		SetLastScheduleTime(SCHEDULE_SLOW);
		MediumLoop();
	}

	if(TimeReached(SCHEDULE_VERY_SLOW, INTERVAL_5000MS)){
		SetLastScheduleTime(SCHEDULE_VERY_SLOW);
		VerySlowLoop();
	}

}





/**
 * @brief Main loop of the system.
 *
 * The SupperLoop function serves as the core loop in the application, typically
 * executed after initialization. It continuously runs, handling various tasks
 * such as processing inputs, managing peripherals, and updating system states.
 *
 * @note This function is usually intended to be called repeatedly or placed
 *       within an infinite loop to keep the system running.
 *
 * @param None
 *
 * @return void
 */
void SupperLoop(void){


	/*DIs---------------------*/
	TH_DI();

	/*DOs---------------------*/
	TH_DO();

}


/**
 * @brief Executes tasks that need to run at a medium frequency.
 *
 * This function is intended to handle tasks that do not require high-speed
 * execution but still need to be checked or executed periodically.
 *
 * @note The actual tasks and functionality to be implemented in this loop
 * will depend on the specific application requirements. This loop is expected
 * to be called within the main loop of the application at regular intervals.
 *
 * @param None
 *
 * @return void
 */
void MediumLoop(void){
	/*Factory reset-------------*/
	TH_MB_ChecktSlaveAddressChange();
	TH_MB_CheckSerialConfigChanges();

	/*Factory reset-------------*/
	FLASH_CheckForChanges();

	/*Factory reset-------------*/
	TH_PushButton();

}

/**
 * @brief Loop for handling infrequent tasks.
 *
 * The VerySlowLoop function is intended for tasks that do not need to be executed frequently.
 * This may include background operations like system diagnostics, logging, or low-priority
 * maintenance tasks. It should be called at a much slower rate compared to the main loop.
 *
 * @note This loop can be useful for saving resources by offloading less urgent tasks.
 *
 * @param None
 *
 * @return void
 */
void VerySlowLoop(void){

	/*Periodic logging----------*/
	if(!debug.enable) {return;}

	DEBUG_PRINT("\r\n");
	/*Modbus logging----------*/
	DEBUG_SPRINT_NL("Modbus: {SA: %d, SrlConfig:{BR:%d, DB:%d, PB:%d, SB:%d}}",
					gVar.mbRTUSlave.slave_address,
					gVar.mbSerial.baudRate, gVar.mbSerial.dataBits,
					gVar.mbSerial.parityBit, gVar.mbSerial.stopBit);

	/*Dis logging----------------*/
	DEBUG_SPRINT_NL("DI 0 to 4: { State[%d, %d, %d, %d], "
					"DbuncDelay[%ld, %ld, %ld, %ld]us }",
					gVar.di[DI_ID_0].state, gVar.di[DI_ID_1].state,
					gVar.di[DI_ID_2].state, gVar.di[DI_ID_3].state,
					gVar.di[DI_ID_0].debounce.delay,gVar.di[DI_ID_1].debounce.delay,
					gVar.di[DI_ID_2].debounce.delay,gVar.di[DI_ID_3].debounce.delay);
	DEBUG_SPRINT_NL(debug.str);
	/*Dis logging----------------*/
	DEBUG_SPRINT_NL("DO 0 to 4: { State[%d, %d, %d, %d] }",
					gVar.do_[DO_ID_0].state, gVar.do_[DO_ID_1].state,
					gVar.do_[DO_ID_2].state, gVar.do_[DO_ID_3].state);
	DEBUG_SPRINT_NL(debug.str);
}






/**
  * @brief if timer reached to next schedule time it will allow to execute the fast loop
  * @param schedule: schedule task id
  * @param interval: interval between two schedule task
  * @retval if time reached it will return true otherwise false
  *
  */
uint8_t TimeReached(uint8_t schedule, uint16_t interval){

	return (((HAL_GetTick()-LastScheduleTime[schedule]) >= interval) || ((int32_t)(HAL_GetTick() - LastScheduleTime[schedule]) < 0));

}


/**
  * @brief Sets the last time of the executed schedule task
  * @param schedule: schedule task id
  * @retval none
  */
void SetLastScheduleTime(uint8_t schedule){
	LastScheduleTime[schedule] = HAL_GetTick();
}


/* Print theFirmware and hardware version number
 * */
void PrintVersion(void){
	DEBUG_SPRINT_NL("Hardware Version: %02d.%02d", HW_VIRSION_MAJOR, HW_VIRSION_MINOR);
	DEBUG_SPRINT_NL("Firmware Version: %02d.%02d", FW_VIRSION_MAJOR, FW_VIRSION_MINOR);
}


/* Print the MCU information
 * */
void PrintBoardInfo(void){
	union {
		uint32_t value;
		struct{
			uint32_t LSION : 1;
			uint32_t LSIRDY : 1;
			uint32_t RSRVD : 22;
			uint32_t RMVF : 1;
			uint32_t BORRSTF  : 1;
			uint32_t PINRSTF  : 1;
			uint32_t PORRSTF  : 1;
			uint32_t SFTRSTF  : 1;
			uint32_t IWDGRSTF : 1;
			uint32_t WWDGRSTF : 1;
			uint32_t LPWRRSTF : 1;

		}bit;
	}rstReg;

	rstReg.value = RCC->CSR;
	char str[64] = {0};
	DEBUG_SPRINT_APPEND(str, "Restart Reason: ");
	if(rstReg.bit.LPWRRSTF) DEBUG_SPRINT_APPEND(str,  "Low power, ");
	if(rstReg.bit.WWDGRSTF) DEBUG_SPRINT_APPEND(str, "W-Watchdog, ");
	if(rstReg.bit.IWDGRSTF) DEBUG_SPRINT_APPEND( str, "I-Watchdog, ");
	if(rstReg.bit.SFTRSTF) DEBUG_SPRINT_APPEND(str, "Software, ");
	if(rstReg.bit.PORRSTF) DEBUG_SPRINT_APPEND( str, "POR/PDR, ");
	if(rstReg.bit.PINRSTF) DEBUG_SPRINT_APPEND( str, "Hardware, ");
	if(rstReg.bit.BORRSTF) DEBUG_SPRINT_APPEND( str, "BOR, ");
	DEBUG_SPRINT_NL(str);
	__HAL_RCC_CLEAR_RESET_FLAGS();

	FLASH_Download();
	/*Read Slave address*/
	TH_MB_ChecktSlaveAddressChange();

	/*Read Serial COnfig*/
	uint32_t value = 0;
	FLASH_ReadInt(FSA_MB_SERIAL_BAUDRATE, &value);
	if(!(value < DEF_MAX_U16)){
		value = CONF_DEF_MB_SERIAL_BAUDRATE;
	}
	gVar.mbSerial.baudRate = value;

	FLASH_ReadInt(FSA_MB_SERIAL_DATABIT, (uint32_t*)&value);
	if(!(value == MBS_DB_7 || value == MBS_DB_8)){
		value = CONF_DEF_MB_SERIAL_DATABIT;
	}
	gVar.mbSerial.dataBits = value;

	FLASH_ReadInt(FSA_MB_SERIAL_PARITY, (uint32_t*)&value);
	if(!(value == MBS_PB_NONE || value == MBS_PB_ODD || value == MBS_PB_EVEN)){
		value = CONF_DEF_MB_SERIAL_PARITY;
	}
	gVar.mbSerial.parityBit = value;

	FLASH_ReadInt(FSA_MB_SERIAL_STOPBIT, (uint32_t*)&value);
	if(!(value == MBS_SB_1 || value == MBS_SB_2)){
		value = CONF_DEF_MB_SERIAL_STOPBIT;
	}
	gVar.mbSerial.stopBit = value;



	DEBUG_SPRINT_NL("Modbus: {SA: %d, SrlConfig:{BR:%d, DB:%d, PB:%d, SB:%d}}",
					gVar.mbRTUSlave.slave_address,
					gVar.mbSerial.baudRate, gVar.mbSerial.dataBits,
					gVar.mbSerial.parityBit, gVar.mbSerial.stopBit);


}

