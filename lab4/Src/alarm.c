// Alarm system app
#include <stdio.h>
#include "alarm.h"
#include "systick.h"
#include "gpio.h"
#include "display.h"

// GPIO pins
// ...
static const Pin_t RED_LED = {GPIOA, 9}; //PA9
static const Pin_t GREEN_LED = {GPIOC, 7}; //PC7
static const Pin_t BLUE_LED = {GPIOB, 7}; //PB7
static const Pin_t ESTOP_BUTTON = {GPIOB, 2}; //PB2
static const Pin_t MOTION = {GPIOB, 9}; //PB9
static const Pin_t BUZZ = {GPIOA, 0}; //PB9




static enum {DISARMED, ARMED, TRIGGERED} state;

// Constants
// ...
#define DEBUG 0
#define BUZZER_ENABLED 0

// Variables
// ...
static Time_t currLoopTime;
static Time_t buttonPressedStartTime; // for debouncing, when the button first gets pressed
static int buttonPressed = 0; //flag for button pressed.
static int buttonReleased = 0; //flag for button released and not dealt with yet.
static int motionDetected = 0; //flag for motion detected
static int longPressDetected = 0; //to prevent short button presses if long presses are detected

// Declare interrupt callback functions
static void CallbackMotionDetect();
static void CallbackButtonPress();
static void CallbackButtonRelease();

// Initialization code
void Init_Alarm (void) {
	// GPIO ENABLES
	GPIO_Enable(BLUE_LED);
	GPIO_Enable(RED_LED);
	GPIO_Enable(GREEN_LED);
	GPIO_Enable(ESTOP_BUTTON);
	GPIO_Enable(MOTION);
	GPIO_Enable(BUZZ);

	//GPIO MODES
	GPIO_Mode(BLUE_LED, OUTPUT);
	GPIO_Mode(RED_LED, OUTPUT);
	GPIO_Mode(GREEN_LED, OUTPUT);
	GPIO_Mode(ESTOP_BUTTON, INPUT);
	GPIO_Mode(MOTION, INPUT);
	GPIO_Mode(BUZZ, OUTPUT);

	//CALLBACKS
	GPIO_Callback(ESTOP_BUTTON, CallbackButtonPress, RISE);
	GPIO_Callback(ESTOP_BUTTON, CallbackButtonRelease, FALL);
	GPIO_Callback(MOTION, CallbackMotionDetect, RISE);

	//ENABLE DISPLAY
	DisplayEnable();
	DisplayColor(ALARM, WHITE);
	DisplayPrint(ALARM, 0, "DISARMED");

	//DEBUG STUFF
	if (DEBUG == 1) {
		printf("in debug mode\n");
		printf("testing LEDs\n");
		GPIO_Toggle(RED_LED);
		GPIO_Toggle(GREEN_LED);
		GPIO_Toggle(BLUE_LED);

		printf("all should be on\n");
	}

	currLoopTime = TimeNow();

	state = DISARMED;
	printf("DISARMED at time %u ALARM INITIALIZED\n", TimeNow());
	DisplayPrint(ALARM, 0, "DISARMED");
	DisplayColor(ALARM, WHITE);

}

// Task code (state machine)
void Task_Alarm (void) {

	if (buttonPressed == 1 && buttonReleased == 0 && longPressDetected == 0) {
		if (TimeNow() - buttonPressedStartTime > 3000) { //check for 3000 ms
			if (DEBUG) {printf("3s Button Detected\n");};
			state = DISARMED;
			DisplayPrint(ALARM, 0, "DISARMEDDDD");
			DisplayColor(ALARM, WHITE);
			printf("DISARMED at time %u BUTTON HELD\n", TimeNow());
			longPressDetected = 1;
		}
	}

	if (buttonReleased == 1 && buttonPressed == 0) { //for brief button press
		if (longPressDetected == 1) {
			longPressDetected = 0;
		}
		else {
			if (DEBUG) {printf("brief Button Detected\n");};
			motionDetected = 0;
			state = ARMED;
			DisplayPrint(ALARM, 0, "ARMED");
			DisplayColor(ALARM, YELLOW);
			printf("ARMED at time %u BUTTON PRESS\n", TimeNow());

		}
		buttonReleased = 0; //no longer need to process button released event
	}

	if (motionDetected == 1) {
		if (state == ARMED) {
			state = TRIGGERED;
			DisplayPrint(ALARM, 0, "TRIGGERED");
			DisplayColor(ALARM, RED);
			printf("TRIGGERED at time %u MOTION DETECTED\n", TimeNow());
			motionDetected = 0;
		}
	}

	switch (state) {
		case DISARMED:


			// ...
			motionDetected = 0;
			GPIO_Output(RED_LED,LOW);
			GPIO_Output(GREEN_LED,LOW);
			GPIO_Output(BLUE_LED,LOW);
			GPIO_Output(BUZZ, LOW);
			break;
		case ARMED:


			// ...
			GPIO_Output(BUZZ, LOW);
			if ((TimeNow() - currLoopTime) > 1000 && (TimeNow() - currLoopTime) < 2000) {
				GPIO_Output(RED_LED,LOW);
				GPIO_Output(GREEN_LED,HIGH);
				GPIO_Output(BLUE_LED,LOW);

			}

			//msDelay(1000);
			else if ((TimeNow() - currLoopTime) >= 2000) {
				GPIO_Output(GREEN_LED,LOW);
				GPIO_Output(BLUE_LED,HIGH);
				currLoopTime = TimeNow();
			}

			break;
		case TRIGGERED:


			// ...
			GPIO_Output(RED_LED,HIGH);
			GPIO_Output(GREEN_LED,LOW);
			GPIO_Output(BLUE_LED,LOW);

			int BUZZTIME = 200;
			if ((TimeNow() - currLoopTime) > BUZZTIME && (TimeNow() - currLoopTime) < 2*BUZZTIME) {
				if (BUZZER_ENABLED)
					GPIO_Output(BUZZ, HIGH);
				//if (DEBUG) {printf("BUZZ HIGH\n");};

			}
			else if ((TimeNow() - currLoopTime) >= 2*BUZZTIME) {
				GPIO_Output(BUZZ, LOW);
				currLoopTime = TimeNow();
				//if (DEBUG) {printf("BUZZ LOW\n");};
			}
			break;
		}
}

void CallbackMotionDetect (void) {
	// ...
	motionDetected = 1;
}

void CallbackButtonPress (void) {
	// ...
	buttonPressedStartTime = TimeNow();
	buttonPressed = 1;

}

void CallbackButtonRelease (void) {
	// ...
	if (TimePassed(buttonPressedStartTime) > 50) {
		buttonPressed = 0;
		buttonReleased = 1;
	}

}
