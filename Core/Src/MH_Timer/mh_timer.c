/*
 * mh_timer.c
 *
 *  Created on: Jan 7, 2025
 *      Author: wsrra
 */

#include "mh_timer.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


/**
 * @brief Initializes the timer with a timeout value and time source function.
 *
 * @param timer Pointer to the MH_Timer structure.
 * @param timeout Timeout period in milliseconds.
 * @param getTime Function pointer to fetch the current time in milliseconds.
 */
void MH_Timer_Init(MH_Timer *timer, uint32_t timeout, uint32_t (*getTime)(void)) {
    if (timer == NULL || getTime == NULL) return;
    timer->enable = 0;         // Timer is initially disabled
    timer->time = getTime();   // Initialize time from the time source
    timer->timeout = timeout;
    timer->isTimeout = 0;
    timer->isTimeRst = 0;
    timer->getTime = getTime;  // Set the time source
}

/**
 * @brief Enables the timer.
 *
 * @param timer Pointer to the MH_Timer structure.
 */
void MH_Timer_Enable(MH_Timer *timer) {
    if (timer == NULL || timer->getTime == NULL) return;
    timer->enable = 1;
    timer->isTimeout = 0;      // Clear timeout flag
    timer->isTimeRst = 0;      // Clear reset flag
    timer->time = timer->getTime(); // Reset the reference time
}

/**
 * @brief Disables the timer.
 *
 * @param timer Pointer to the MH_Timer structure.
 */
void MH_Timer_Disable(MH_Timer *timer) {
    if (timer == NULL) return;
    timer->enable = 0;
}

/**
 * @brief Updates the timer status. Call this periodically.
 *
 * @param timer Pointer to the MH_Timer structure.
 */
void MH_Timer_Update(MH_Timer *timer) {
    if (timer == NULL || !timer->enable || timer->getTime == NULL) return;

    uint32_t currentTime = timer->getTime(); // Get the current time from the source
    uint32_t elapsedTime;

    // Handle overflow of the time source
    if (currentTime >= timer->time) {
        elapsedTime = currentTime - timer->time;
    } else {
        // Assuming a 32-bit counter that overflows
        elapsedTime = (UINT32_MAX - timer->time) + currentTime + 1;
    }

    // Check if timeout has occurred
    if (elapsedTime >= timer->timeout) {
        timer->isTimeout = 1; // Set timeout flag
        timer->enable = 0;    // Optionally disable the timer after timeout
    }
}

/**
 * @brief Checks if the timer has timed out.
 *
 * @param timer Pointer to the MH_Timer structure.
 * @return 1 if timeout occurred, 0 otherwise.
 */
uint8_t MH_Timer_IsTimeout(MH_Timer *timer) {
    if (timer == NULL) return 0;
    return timer->isTimeout;
}

/**
 * @brief Resets the timer to its initial state.
 *
 * @param timer Pointer to the MH_Timer structure.
 */
void MH_Timer_Reset(MH_Timer *timer) {
    if (timer == NULL || timer->getTime == NULL) return;
    timer->time = timer->getTime(); // Reset reference time
    timer->isTimeout = 0;          // Clear the timeout flag
    timer->isTimeRst = 1;          // Set the reset flag
}

/**
 * @brief Clears the reset flag after it has been acknowledged.
 *
 * @param timer Pointer to the MH_Timer structure.
 */
void MH_Timer_ClearResetFlag(MH_Timer *timer) {
    if (timer == NULL) return;
    timer->isTimeRst = 0;
}
