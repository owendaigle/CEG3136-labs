// System headers
#include <stdio.h>
#include "systick.h"
#include "i2c.h"
#include "gpio.h"
#include "touchpad.h"
#include "spi.h"

// App headers
#include "alarm.h"
#include "game.h"
#include "calc.h"
#include "bin.h"
#include "enviro.h"
#include "motor.h"
#include "clock.h"

int main(void)
{
	// Initialize apps

	Init_Alarm();
	Init_Game();
	Init_Calc();
	Init_Bing();
	Init_Enviro();
	Init_Motor();
	Init_Clock();

	// Enable system services
	StartSysTick();

	while (1) {
		// Run apps
		Task_Alarm();
		Task_Game();
		Task_Calc();
		Task_Bing();
		Task_Enviro();
		Task_Motor();
		Task_Clock();

		// Housekeeping
		UpdateIOExpanders();
		UpdateDisplay();
		ScanTouchpad();
		ServiceI2CRequests();
		ServiceSPIRequests();
		WaitForSysTick();
	}
}
