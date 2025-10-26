// System headers
#include <stdio.h>
#include "systick.h"
#include "i2c.h"
#include "gpio.h"

// App headers
#include "alarm.h"
#include "game.h"

int main(void)
{
	// Initialize apps
	Init_Alarm();
	Init_Game();

	// Enable system services
	StartSysTick();

	while (1) {
		// Run apps
		Task_Alarm();
		Task_Game();
		// Housekeeping
		UpdateIOExpanders();
		ServiceI2CRequests();
		WaitForSysTick();
	}
}
