// Motor controller app
#include <stdbool.h>
#include <math.h>
#include <stdio.h>

#include "motor.h"
#include "display.h"
#include "touchpad.h"
#include "gpio.h"
#include "timer.h"
#include "adc.h"
#include "sysclk.h"
#include "touchpad.h"

// GPIO pins
// Refer to Lab User’s Guide
static const Pin_t AI1 = {GPIOB, 10}; // Pin P??? -> Motor driver
static const Pin_t AI2 = {GPIOB, 11}; // Pin P??? -> Motor driver
static const Pin_t STBY = {GPIOE, 15}; // Pin P??? -> Motor driver
static const Pin_t EncA = {GPIOB, 0}; // Pin P??? <- Rotary encoder A
static const Pin_t EncB = {GPIOB, 1}; // Pin P??? <- Rotary encoder B

// Analog-to-digital converter for potentiometer
// Refer to Lab User’s Guide
static const ADCInput_t Pot = {ADC1, 1, {GPIOC, 0}}; // ADC1 Chan1 <- P???

// Timer channels
// Refer to Lab User’s Guide
static const TimerIO_t Motor = {TIM1, 1, {GPIOE, 9}, 1}; // Timer? Chan? -> P??? AF?

// Timer period
#define PWM_PSC (100 - 1) // Pre-scaler
#define PWM_ARR (240 - 1) // Auto-reload
#define PWM_RCR ( 50 - 1) // Repetition count

#define MAX_SPEED 360.0
static enum { CW=0, CCW=1 } direction = CCW; // Clockwise or counter-clockwise
static enum { OL=0, CL=1, CLT=2 } loopMode = OL; // Open/closed loop (w/ tuning)

static float rpmScalingFactor;
static float desiredRPM = 0;
static float errorRpm;
static float error = 0;
static float errorRpmSum = 0;
static float errorSum = 0;

// PI controller parameters
// Note: Update defaults for Np and Ni after tuning
// Kp = Np * dp
int Np = 8;
float dp = 0.1;
// Ki = Ni * di
int Ni = 6;
float di = 0.001;

uint32_t pulsesA = 0;
uint32_t pulsesB = 0;
uint32_t timerCount = 0;
uint32_t totalPulses = 0;
uint32_t prevTimerCount = 0;

// Interrupt callback functions
static void CallbackMotor(void);
static void CallbackEncA(void);
static void CallbackEncB(void);
// Change motor direction
// Refer to motor driver datasheet
void MotorDirection(int dir) {
	if (dir == CW) {
	// Clockwise
	GPIO_Output(AI1, HIGH);
	GPIO_Output(AI2, LOW);
	direction = CW;
	}
	else {
	// Counter-clockwise
	GPIO_Output(AI1, LOW);
	GPIO_Output(AI2, HIGH);
	direction = CCW;
	}
}

// Initialize app
void Init_Motor (void) {
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
	SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));
#endif
	ADC_Enable(Pot); // Speed control potentiometer
	// Configure GPIO pins
	// Refer to motor controller datasheet for outputs in stop mode
	// Register callbacks for rotary encoder inputs
	GPIO_Enable(AI1);
	GPIO_Enable(AI2);
	GPIO_Enable(STBY);
	GPIO_Enable(EncA);
	GPIO_Enable(EncB);

	GPIO_Mode(AI1, OUTPUT);
	GPIO_Mode(AI2, OUTPUT);
	GPIO_Mode(STBY, OUTPUT);
	GPIO_Mode(EncA, INPUT);
	GPIO_Mode(EncB, INPUT);

	GPIO_Output(AI1, LOW);
	GPIO_Output(AI2, LOW);
	GPIO_Output(STBY, HIGH);

	GPIO_Callback(EncA, CallbackEncA, RISE);
	GPIO_Callback(EncB, CallbackEncB, RISE);
	//GPIO_Callback(Motor.pin, CallbackMotor, RISE);




	TimerEnable(Motor);
	TimerPeriod(Motor, PWM_PSC, PWM_ARR, PWM_RCR);
	TimerMode(Motor, OUTCMP, PWM1);
	TimerCallback(Motor, CallbackMotor, UP);
	TimerStart(Motor, OUTCMP);

	MotorDirection(direction);

	rpmScalingFactor = SYSCLK_FREQ/(PWM_PSC+1) / (PWM_ARR+1)/ (PWM_RCR+1)/(374 * 2)*60; // should be 3.20855614973
}
// Execute app
void Task_Motor (void) {
	float motorDrivePercentage = (float)ADC_Read(Pot) / 4096.0;
	desiredRPM = motorDrivePercentage * MAX_SPEED;

	if (timerCount-prevTimerCount > 0) {
		float measuredRPM = (float)totalPulses*rpmScalingFactor;
		if (loopMode == OL) {
		TimerOutput(Motor, (uint16_t) (motorDrivePercentage * (float)(PWM_ARR + 1)));
		// Display status for Open Loop mode
		DisplayPrint(MOTOR, 0, " OL %3d %%", (int)(motorDrivePercentage*100));
		DisplayPrint(MOTOR, 1, "%cCW %3d RPM", direction == CCW ? 'C' : ' ', (int)measuredRPM);
	}
	else {
		// Closed Loop control
		float Kp = (float)Np * dp;
		float Ki = (float)Ni * di;

		errorRpm = desiredRPM - measuredRPM;
		errorRpmSum += errorRpm;

		// Convert errorRpm to change on TIM1->CCR1
		error = errorRpm / MAX_SPEED * (float)(PWM_ARR + 1);
		errorSum = errorRpmSum / MAX_SPEED * (float)(PWM_ARR + 1);

		TimerOutput(Motor,round((Kp*error + Ki*errorSum)));
		if (loopMode == CLT) {
			// Display status for Closed Loop mode /w tuning enabled
			DisplayPrint(MOTOR, 0, "Kp:%1.1f  %4d RPM",Kp , (int)desiredRPM);
			DisplayPrint(MOTOR, 1, "Ki:%1.3f %3d RPM", Ki, (int)measuredRPM);
		}
		else {
			// Display status for normal Closed Loop mode
			DisplayPrint(MOTOR, 0, " CL T: %3d RPM", (int)desiredRPM);
			DisplayPrint(MOTOR, 1, "%cCW A: %3d RPM", direction == CCW ? 'C' : ' ', (int)measuredRPM);
		}
	}

	totalPulses = 0;
	prevTimerCount = timerCount;
	}
	// Touchpad controls
	switch (TouchInput(MOTOR)) {
	// Motor direction: update and apply
	case 1:
		MotorDirection(CW);
		break;
	case 4:
		MotorDirection(CCW);
		break;

	// Controller operating mode
	case 7:
		loopMode = OL;
		break;
	case 8:
		loopMode = CL;
		break;
	case 9:
		loopMode = CLT;
		break;

	// Controller tuning (must be in CLT mode to take effect)
	case 2:
		if (loopMode == CLT) {
			Np += 1;
		}
		break;
	case 3:
		if (loopMode == CLT) {
			Ni += 1;
		}
		break;
	case 5:
		if (loopMode == CLT) {
			Np -= 1;
		}
		break;
	case 6:
		if (loopMode == CLT) {
			Ni -= 1;
		}
		break;
	case 0:
		if (loopMode == CLT) {
			Np = 0;
			Ni = 0;
		}
		break;


	default: break;
	}
}
// Timer 1 update
void CallbackMotor (void) {
	timerCount++;
	totalPulses = pulsesA+pulsesB;
	pulsesA = 0;
	pulsesB = 0;
}
// Rotary encoder A rising edge
void CallbackEncA (void) {
	pulsesA++;
}
// Rotary encoder B rising edge
void CallbackEncB (void) {
	pulsesB++;
}
