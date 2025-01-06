/*
 * Config.h
 *
 *  Created on: Oct 13, 2022
 *      Author: wsrra
 */

#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_


/*Modbus RTU Slave--------------------*/
#define CONF_DEF_MBRTUS_SLAVE_ADDRESS			0x01
#define CONF_DEF_MB_RTU_MODE			 		MB_RTU_MODE_SLAVE //MB_RTU_MODE_MASTER

/*Modbus RTU Serial-------------------*/
#define CONF_DEF_MB_SERIAL_BAUDRATE				19200U //user define baud rate
#define CONF_DEF_MB_SERIAL_DATABIT				MBS_DB_8
#define CONF_DEF_MB_SERIAL_PARITY				MBS_PB_EVEN
#define CONF_DEF_MB_SERIAL_STOPBIT				MBS_SB_1

#define CONF_DEF_MB_SERIAL_BR_INDEX				MBS_BR19200_INDEX // 0 position is the user define baud rate
#define CONF_DEF_MB_SERIAL_CONFIG				((CONF_DEF_MB_SERIAL_BR_INDEX<<12) | (MBS_DB_8<<8) | (MBS_PB_EVEN<<4) | MBS_SB_1)//MBS_BR2000000_INDEX

/*Timestamp---------------------------*/
#define CONF_DEF_TIMESTAMP_TIME_SOURCE			TS_TIM_TYP_SYSTICK


/*DIs---------------------------------*/
#define CONF_DEF_DI_DEBOUNCE_DELAY				100U	//us


/*Debug-------------------------------*/
#define CONF_DEF_DEBUG_ENABLE					0		//us

/*PTC---------------------------------*/
/*This configuration for non-linear triple PTC*/
#define CONF_DEF_PTC_SET_TEMP					DEF_PTC_SWITCHING_TEMP 	// C,
#define CONF_DEF_PTC_TARGET_TEMP				155U	// C
#define CONF_DEF_PTC_RES_AT_25C					300U 	//ohm
#define CONF_DEF_PTC_RES_AT_TARGET_MINUS_20C	750U 	//ohm
#define CONF_DEF_PTC_RES_AT_TARGET_MINUS_5C		1600U 	//ohm
#define CONF_DEF_PTC_RES_AT_TARGET_PLUS_5C		4000U 	//ohm
#define CONF_DEF_PTC_RES_AT_TARGET_PLUS_15C		12000U 	//ohm
#define CONF_DEF_PTC_TEMP_AT_TARGET_MINUS_20C	(CONF_DEF_PTC_TARGET_TEMP - 20)
#define CONF_DEF_PTC_TEMP_AT_TARGET_MINUS_5C	(CONF_DEF_PTC_TARGET_TEMP - 5)
#define CONF_DEF_PTC_TEMP_AT_TARGET_PLUS_5C		(CONF_DEF_PTC_TARGET_TEMP + 5)
#define CONF_DEF_PTC_TEMP_AT_TARGET_PLUS_15C	(CONF_DEF_PTC_TARGET_TEMP + 15)


#endif /* INC_CONFIG_H_ */
