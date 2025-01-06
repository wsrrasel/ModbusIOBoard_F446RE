/*
 * Scheduler.h
 *
 *  Created on: Oct 7, 2022
 *      Author: wsrra
 */

#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_
#include "MB_Serial.h"
#include "MB_RTUSlave.h"
#include "Config.h"
#include "Debounce.h"



/*DIs---------------------------*/
/*DI id*/
typedef enum DI_Id{
	DI_ID_0 = 0,
	DI_ID_1,
	DI_ID_2,
	DI_ID_3,
	DI_ID_MAX
}DI_Id_te;


/*DI Structure*/
typedef struct DI{
	uint8_t id;
	Debounce_ts debounce;
	GPIO_TypeDef *port;
	uint16_t pin;
	GPIO_PinState state;
}DI_ts;



/*DOs---------------------------*/
/*DO id*/
typedef enum DO_Id{
	DO_ID_0 = 0,
	DO_ID_1,
	DO_ID_2,
	DO_ID_3,
	DO_ID_MAX
}DO_Id_te;


/*DI Structure*/
typedef struct DO{
	uint8_t id;
	GPIO_TypeDef *port;
	uint16_t pin;
	GPIO_PinState state;
	GPIO_PinState lastState;
}DO_ts;


/*Push Button-------------------*/
typedef struct PushButton{
	Debounce_ts debounce;
	GPIO_TypeDef *port;
	uint16_t pin;
	GPIO_PinState state;
	uint32_t pressCount;
}PushButton_ts;



typedef struct StatusLed{
	uint8_t state;
	uint16_t blinkFreq;
	uint32_t tick;
	GPIO_TypeDef *port;
	uint16_t pin;
	GPIO_PinState status;
}StatusLed_ts;


/*Global variables structure-----*/
typedef struct GlobalVar{
	/*Modbus---------------------*/
	MBS_Serial mbSerial;
	MB_RTUSlave mbRTUSlave;

	/*DIs------------------------*/
	DI_ts di[DI_ID_MAX];

	/*DOs------------------------*/
	DO_ts do_[DO_ID_MAX];

	/*PushButton-----------------*/
	PushButton_ts pushButton;

}GlobalVar;


void SchedulerInit(void);
void Scheduler(void); //Scheduler A is used for the  blocking task
void PrintVersion(void);
void PrintBoardInfo(void);


#endif /* INC_SCHEDULER_H_ */
