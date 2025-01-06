/*
 * OD_Callback.h
 *
 *  Created on: May 16, 2022
 *      Author: RASEL_EEE
 */

#ifndef INC_OD_CALLBACK_H_
#define INC_OD_CALLBACK_H_
#include <stdint.h>


void CB_MB_InitCallback(void);
uint8_t CB_MB_RcvByte(void);
uint8_t CB_MB_Send(uint8_t *buff, uint16_t size);
void CB_MB_SetUpstream(void);
void CB_MB_SetDownstream(void);
#endif /* INC_OD_CALLBACK_H_ */
