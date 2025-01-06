/*
 * TimeStamp.c
 *
 *  Created on: Mar 28, 2023
 *      Author: Mahmudul Hasan (Russell)
 *      
 */

#include "TimeStamp.h"


TIM_HandleTypeDef *htim;
static uint32_t mcuSysClk;
static double mcuSysClkFacktor;
static uint64_t lastCount;		// total count from the last power up

void TS_Init(TimeStamp *ts, TIM_HandleTypeDef *_htim){
	htim = _htim;
	mcuSysClk = HAL_RCC_GetSysClockFreq();
	mcuSysClkFacktor = (double)(((htim->Init.Prescaler+1.0f)*1000000.0f)/HAL_RCC_GetSysClockFreq());
	lastCount = 0;

	ts->currentTime 	= 0;
	ts->ovfCount 		= 0;
	ts->timerType 		= TS_TIM_TYP_SYSTICK;
	ts->sec 			= 0;
	ts->us 			= 0;
	ts->ms 			= 0;
}
/* This function start the timer
 * */
void TS_StartTimer(TimeStamp *ts){
	if(ts->timerType == TS_TIM_TYP_TIMER){
		HAL_TIM_Base_Start_IT(htim);
	}
}

/* This function start the timer
 * */
void TS_StopTimer(TimeStamp *ts){
	if(ts->timerType == TS_TIM_TYP_TIMER){
		HAL_TIM_Base_Stop_IT(htim);
	}
}

void TS_SetTimerType(TimeStamp *ts, TS_TimerType type){
	ts->timerType = type;
}


void TS_UpdateTime(TimeStamp *ts){

//	ts->currentTime = (uint64_t)htim->Instance->CNT + (uint64_t)((uint64_t)htim->Init.Period * (uint64_t)ts->ovfCount);
//	ts->currentTime = ts->currentTime*(((htim->Init.Prescaler+1)*1000000U)/mcuSysClk);
	ts->currentTime = ((uint64_t)htim->Instance->CNT + lastCount)*mcuSysClkFacktor;
}

/* This function returns total microsecond
 * */
uint64_t TS_GetUS(TimeStamp *ts){
//	if(ts->timerType == TS_TIM_TYP_SYSTICK){
//		ts->currentTime = (uint64_t)(HAL_GetTick()*1000);
//	}else{
//		ts->currentTime = (uint64_t)htim->Instance->CNT + (uint64_t)((uint64_t)htim->Init.Period * (uint64_t)ts->ovfCount);
//		ts->currentTime = ts->currentTime*(((htim->Init.Prescaler+1)*1000000U)/mcuSysClk);
//	}
//	return ts->currentTime;

	ts->currentTime = (uint64_t)(( ts->timerType == TS_TIM_TYP_SYSTICK) ? ((HAL_GetTick()*1000)) : (htim->Instance->CNT + lastCount)*mcuSysClkFacktor);
	return ts->currentTime;

}

/* This function returns total millisecond
 * */
uint64_t TS_GetMS(TimeStamp *ts){
	return (TS_GetUS(ts)/1000);

}

void TS_GetTime(TimeStamp *ts){
	TS_GetUS(ts);
	ts->sec = ts->currentTime/1000000;
	ts->ms = (ts->currentTime - ((uint64_t)ts->sec*1000000))/1000;
	ts->us = (ts->currentTime)%1000;

}



void TS_CatchOVF(TimeStamp *ts){

	ts->ovfCount++;
	lastCount =  (uint64_t)((uint64_t)htim->Init.Period * (uint64_t)ts->ovfCount);

}
