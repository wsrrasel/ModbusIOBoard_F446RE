
/*
 * Define.h
 *
 *  Created on: Oct 17, 2022
 *      Author: wsrra
 */

#ifndef INC_DEFINE_H_
#define INC_DEFINE_H_


#define HW_VIRSION								0x0100
#define HW_VIRSION_MAJOR						0x01
#define HW_VIRSION_MINOR						0x00

#define FW_VIRSION								0x0100
#define FW_VIRSION_MAJOR						0x01
#define FW_VIRSION_MINOR						0x01


#define DEF_MAX_U8								256U
#define DEF_MAX_U16								65536U
#define DEF_MAX_U32								4294967296U

/*Modbus Slave----------------------*/
#define DEF_MB_SLAVEADDR_CHECKING_INTERVAL		1U


/*PTC-------------------------------*/
#define DEF_PTC_SWITCHING_TEMP					150 //ohm
#define DEF_PTC_MIN_RES_IN_NORMAL				50u //ohom
#define DEF_PTC_MAX_RES_IN_SHORT_CIRCUIT		10	//ohm
#define DEF_PTC_MIN_RES_IN_OPEN_CIRCUIT			100000//ohm

/*Factory reset----------------------*/
#define DEF_RSTBTN_FACT_RST_DELAY				10000U //in ms
#define DEF_RSTBTN_DEBOUNCE_DELAY				100U	//us


/*Status LED-------------------------*/
#define DEF_STATUS_LED_BLINK_FREQ_1HZ			1 // in Hz
#define DEF_STATUS_LED_BLINK_FREQ_4HZ			4 // in Hz

#define DEF_ALARM_RESET_TEMP					40 //C

#endif /* INC_DEFINE_H_ */
