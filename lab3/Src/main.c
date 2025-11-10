// System headers
#include <stdio.h>
#include "systick.h"
#include "i2c.h"
#include "gpio.h"
#include "touchpad.h"

// App headers
#include "alarm.h"
#include "game.h"
#include "calc.h"
#include "bin.h"

int main(void)
{
	// Initialize apps

	Init_Alarm();
	Init_Game();
	Init_Calc();
	Init_Bing();

	// Enable system services
	StartSysTick();

	while (1) {
		// Run apps
		Task_Alarm();
		Task_Game();
		Task_Calc();
		Task_Bing();

		// Housekeeping
		UpdateIOExpanders();
		UpdateDisplay();
		ScanTouchpad();
		ServiceI2CRequests();
		WaitForSysTick();
	}
}
