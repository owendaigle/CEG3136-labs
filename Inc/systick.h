/*
 * systick.h
 *
 *  Created on: Sep 22, 2025
 *      Author: odaig041
 *
 *		This is the header to the systick driver.
 *
 *      This driver configures the CPU system timer to generate a periodic
 *      interrupt and provices functions for accurately measuring delays
 *      and time between events.
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_

#include "stm32l5xx.h"

typedef unsigned int Time_t;
#define TIME_MAX (Time_t)(-1)

void StartSysTick(); //Initializes the system timer, enables interrupt
void WaitForSysTick(); //Puts CPU to sleep and waits for next interrupt
void msDelay(int t); //Uses systick to measure delay in ms.
Time_t TimeNow(); //returns current system time
Time_t TimePassed(Time_t since); //calculates elapsed time between previous time and current time.


#endif /* SYSTICK_H_ */
