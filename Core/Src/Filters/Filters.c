/*
 * Filters.c
 *
 *  Created on: Mar 9, 2022
 *      Author: RASEL_EEE
 */

#include "Filters.h"
#include "stdio.h"
#include "string.h"
#include  "math.h"

#define pi 3.1416f
static unsigned int winBufTemp[FLTR_MED_WINDOW_SIZE_MAX] = {0};

int CompFunc (const void * a, const void * b);



const uint16_t fltrMedWindowMidPoint = ((FLTR_MED_WINDOW_SIZE_MAX-1U)/2U);

/**
  * @brief  Median filter
  * @param  mParams: median parameters
  * @retval filtered value
  */
FLTR_Error FLTR_ApplyMedian(FLTR_Median *mParams){
	if(mParams == NULL){
		return FLTR_ERROR_NULL_PTR;
	}else if(!__FLTR_CHECK_MED_WIN_SIZE(mParams)){
		return FLTR_ERROR_INVALID_WINDOW_SIZE;
	}

	mParams->window[mParams->position++] = mParams->nextValue;					// set the new sample to the window buffer
	memcpy(winBufTemp, mParams->window, mParams->windowSize*sizeof(winBufTemp[0]));		// copy the samples from the window buffer to the local buffer
	qsort(winBufTemp, mParams->windowSize, sizeof(winBufTemp[0]), CompFunc);				// Arrange the local buffer elements in ascending order
	if(mParams->position >= mParams->windowSize)	mParams->position = 0;			// if window buffer is full, set the position to zero for adding new sample to the buffer
	mParams->value = winBufTemp[((mParams->windowSize-1U)/2U)];									// getting the center value of the buffer and return
	return FLTR_ERROR_NONE;
}

/**
  * @brief  Moving Average filter
  * @param  maParams: Moving average parameters
  * @retval filtered value
  */
FLTR_Error FLTR_ApplyMovAvg(FLTR_MovingAverage *maParams){
	if(maParams == NULL){
		return FLTR_ERROR_NULL_PTR;
	}else if(!__FLTR_CHECK_MOVAVG_WIN_SIZE(maParams)){
		return FLTR_ERROR_INVALID_WINDOW_SIZE;
	}
	maParams->sum = maParams->sum - maParams->window[maParams->position] + maParams->nextValue;		// add the new value and subtract the old value from the sum of the elements
	maParams->window[maParams->position++] = maParams->nextValue;									// set the new sample to the window buffer
	if(maParams->position >=  maParams->windowSize)	maParams->position = 0;								// if window buffer is full, set the position to zero for adding new sample to the buffer
	maParams->value = maParams->sum/ maParams->windowSize;//(uint16_t)ceil((float)maParams->sum/(float)FLTR_MOV_WINDOW_SIZE);															// calculate the average and return
	return FLTR_ERROR_NONE;
}


/**
  * @brief  Clears median filter
  * @param  mParams: median filter parameters
  * @retvalnone
  */
void FLTR_ClearMedian(FLTR_Median *mParams){
	mParams->value = 0;
	mParams->nextValue = 0;
	mParams->position = 0;
	memset(mParams->window, 0, FLTR_MED_WINDOW_SIZE_MAX*sizeof(mParams->window[0]));
}

/**
  * @brief  Clears moving average filter
  * @param  mParams: moving average filter parameters
  * @retvalnone
  */
void FLTR_ClearMovAvg(FLTR_MovingAverage *maParams){
	maParams->value = 0;
	maParams->nextValue = 0;
	maParams->position = 0;
	maParams->sum = 0;
	memset(maParams->window, 0, FLTR_MOV_WINDOW_SIZE_MAX*sizeof(maParams->window[0]));
}


/**
  * @brief  debounce filter to remove the bounce of the digital inputs
  * @param  debounce: debounce filter struct
  * @retval none
  */
uint8_t FLTR_ApplyDebounce(FLTR_Debounce *debounce){

	if((debounce->finalState == debounce->currentState) && (debounce->counter > 0U)){
		debounce->counter --;											// resets the bounce counter if the input state is not changed
	}
	if(debounce->finalState != debounce->currentState){
		debounce->counter++;												// increments the bounce counter value if the input state is changed
	}

	if(debounce->counter >= debounce->countMax){						// if debounce counter is exceed the max limit
		debounce->isInterrupt = 0U;
		debounce->counter = 0U;											// reset the counter
		debounce->finalState = debounce->currentState;										// update the last state variable
		return 1U;
	}
	return 0U;
}


void FLTR_StartDebounceExti(FLTR_Debounce *debounce){
	debounce->isInterrupt = 1U;
}

//void FLTR_DebounceTimerExit(FLTR_Debounce *debounce){
//		debounce->counter ++;											// reset the counter
//}
//
//void FLTR_CheckDebouncedExit(FLTR_Debounce *debounce){
//	if(debounce->counter >= debounce->countMax){						// if debounce counter is exceed the max limit
//		debounce->counter = 0U;											// reset the counter
//		debounce->finalState = debounce->currentState;										// update the last state variable
//	}
//}
/**
  * @brief  debounce filter for external interrupt
  * @param  debounce: debounce filter struct
  * @retval none
  */
uint8_t FLTR_ApplyDebounceExti(FLTR_Debounce *debounce){
	if(debounce->isInterrupt > 0U){
		return FLTR_ApplyDebounce(debounce);
	}
	return 0U;
}


/**
  * @brief  clear debounce filter parameter
  * @param  debounce: debounce filter struct
  * @retval none
  */
void FLTR_ClearDebounce(FLTR_Debounce *debounce){
	debounce->counter = 0;
	debounce->countMax = 0;
	debounce->currentState = 0;
	debounce->finalState = 0;
	debounce->isInterrupt = 0U;
}

/**
  * @brief  compare to numbers
  * @param a,b: two numbers
  * @retval return value can be positive or negative, if positive a is grater else b is grater
  */
int CompFunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}


