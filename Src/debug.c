/*
 * @AUTHOR: OWEN DAIGLE
 * @COURSE: CEG3136
 */

#include "stm32l5xx.h"

//function to be able to use printf using the serial wire viewer (SWV)
// of the ARM cortex M family. AKA Instrumentation Trace Macrocell (ITM)
int __io_putchar(int c) {
	ITM_SendChar(c);
	return c;
}
//some processors do not support SWV such as cortex M0+ series, there we can use uart.
