/*
 * TaskHandler.h
 *
 *  Created on: Oct 2, 2024
 *      Author: Lenovo X1 yoga
 */

#ifndef INC_TASKHANDLER_H_
#define INC_TASKHANDLER_H_
#include "stdint.h"
#include "Scheduler.h"

/*Modbus-----------------*/
void TH_MB_ChecktSlaveAddressChange(void);
void TH_MB_CheckSerialConfigChanges(void);
void TH_MB_CheckMBKeepAlive(void);

/*DIOs-------------------*/
void TH_DI(void);
void TH_DO(void);

/*Factory reset-----------*/
void TH_PushButton(void);

/*PTC---------------------*/
void TH_PTC(void);
void TH_PTCAction(void);
void TH_SetStatusLEDState(StatusLed_ts *statusLed, uint8_t state);

/*Relay-------------------*/
void TH_RelayOn(void);
void TH_RelayOff(void);

/*Alarm-------------------*/
void TH_AlarmSet(void);
void TH_AlarmClear(void);




#endif /* INC_TASKHANDLER_H_ */
