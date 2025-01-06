/*
 * Debounce.h
 *
 *  Created on: Apr 14, 2024
 *      Author: wsrra
 */

#ifndef SRC_DEBOUNCE_DEBOUNCE_H_
#define SRC_DEBOUNCE_DEBOUNCE_H_
#include <stdint.h>



typedef struct Debounce{
	uint32_t currentTime;
	uint32_t lastTime;
	uint32_t delay;
	uint8_t state;
	uint8_t lastState;
}Debounce_ts;

void Debounce_Init(Debounce_ts *debounce, uint32_t delay);
void Debounce_Clear(Debounce_ts *debounce);
uint8_t Debounce(Debounce_ts *debounce, uint32_t currentTime, uint8_t inputState);


#endif /* SRC_DEBOUNCE_DEBOUNCE_H_ */
