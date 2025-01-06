/*
 * Debounce.c
 *
 *  Created on: Apr 14, 2024
 *      Author: wsrra
 */

#include "Debounce.h"

/*@Brief Debounce main function*/
void Debounce_Init(Debounce_ts *debounce, uint32_t delay){
	Debounce_Clear(debounce);
	debounce->delay = delay;
}

void Debounce_Clear(Debounce_ts *debounce){
	debounce->lastState = 0;
	debounce->state = 0;
	debounce->lastTime = 0;
	debounce->currentTime = 0;
}

/*@Brief Debounce main function*/
uint8_t Debounce(Debounce_ts *debounce, uint32_t currentTime, uint8_t inputState){
	uint8_t ret = 0;
	if(debounce->state == inputState) {	return ret;	}
	else { debounce->currentTime = currentTime;}

	/*Timeout Check*/
	if((debounce->currentTime - debounce->lastTime) >= debounce->delay){
		debounce->state = inputState;
		if(debounce->state != debounce->lastState){
			// Input state has changed
			debounce->lastState = debounce->state;
			ret = 1;
		}else{
			ret = 0;
		}
		debounce->lastTime = debounce->currentTime;
	}

	return ret;
}
