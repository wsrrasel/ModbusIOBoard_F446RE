/*
 * mh_timer.h
 *
 *  Created on: Jan 7, 2025
 *      Author: wsrra
 */

#ifndef SRC_MH_TIMER_MH_TIMER_H_
#define SRC_MH_TIMER_MH_TIMER_H_

#include <stdint.h>

/**
 * @brief Timer structure to manage timeout functionality.
 */
typedef struct MH_Timer {
    uint8_t enable;           /**< 1 = enable, 0 = disable */
    uint32_t time;            /**< Previous time in milliseconds */
    uint32_t timeout;         /**< Timeout period in milliseconds */
    uint8_t isTimeout;        /**< 1 = timeout occurred, 0 = no timeout */
    uint8_t isTimeRst;        /**< 1 = time reset occurred, 0 = no reset */
    uint32_t (*getTime)(void); /**< Function pointer to fetch the current time in milliseconds */
} MH_Timer;


void MH_Timer_Init(MH_Timer *timer, uint32_t timeout, uint32_t (*getTime)(void));
void MH_Timer_Enable(MH_Timer *timer);
void MH_Timer_Disable(MH_Timer *timer);
void MH_Timer_Update(MH_Timer *timer);
uint8_t MH_Timer_IsTimeout(MH_Timer *timer);
void MH_Timer_Reset(MH_Timer *timer);
void MH_Timer_ClearResetFlag(MH_Timer *timer);

#endif /* SRC_MH_TIMER_MH_TIMER_H_ */
