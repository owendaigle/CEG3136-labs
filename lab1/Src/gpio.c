/*
 * gpio.c
 *
 *  Created on: Sep 22, 2025
 *      Author: odaig041
 */

// General-purpose input/output driver
#include "gpio.h"

// --------------------------------------------------------
// Initialization
// --------------------------------------------------------

// Enable the GPIO port peripheral clock for the specified pin
void GPIO_Enable(Pin_t pin) { //WORKING
	RCC->AHB2ENR |= (1 << GPIO_PORT_NUM(pin.port)); //shifts left by the number
}

// Set the operating mode of a GPIO pin:
// Input (IN), Output (OUT), Alternate Function (AF), or Analog (ANA)
void GPIO_Mode(Pin_t pin, PinMode_t mode) { //BROKEN
	pin.port->MODER = (pin.port->MODER & ~(0b11 << 2*pin.bit)) | (mode << 2*pin.bit);

}

// --------------------------------------------------------
// Pin observation and control
// --------------------------------------------------------

// Observe the value of an input pin
PinState_t GPIO_Input(const Pin_t pin) { //WORKING
	int a = ((pin.port->IDR) & (0x1UL << pin.bit)); // 8192 if pressed, 0 if not pressed
	if (a == 8192) {
		return HIGH;
	}
	else {
		return LOW;
	}
	//return ((pin.port->IDR) & (0x1UL << pin.bit));

}

void GPIO_Output(Pin_t pin, const PinState_t state) { //WORKING
	if (state == HIGH) {
		pin.port->BSRR = (0x1UL << pin.bit);
	}
	else if (state == LOW) {
		pin.port->BSRR = (0x1UL << (16+pin.bit));
	}
}

void GPIO_Toggle(Pin_t pin) { // IN PROGRESS
	int currState = pin.port->IDR; //gets value of whole reg -- 1001 1000
	currState &= 1 << pin.bit; //masks it to just get value of this bit -- 1000 0000
	currState = currState >> pin.bit; //moves back over to get 1 or 0 -- 1
	//PinState_t currState = GPIO_Input(pin);
	if (currState == 1) {
		GPIO_Output(pin, LOW);
	}
	else if (currState == 0) {
		GPIO_Output(pin, HIGH);
	}
}

// --------------------------------------------------------
// Interrupt handling
// --------------------------------------------------------

// Array of callback function pointers
// Bits 0 to 15 (each can select one port GPIOA to GPIOH)
// Rising and falling edge triggers for each
static void (*callbacks[16][2])(void);

// Register a function to be called when an interrupt occurs
void GPIO_Callback(Pin_t pin, void (*func)(void), PinEdge_t edge) {
callbacks[pin.bit][edge] = func;
// Enable interrupt generation
if (edge == RISE)
	EXTI->RTSR1 |= 1 << pin.bit;
else
	// FALL
	EXTI->FTSR1 |= 1 << pin.bit;
EXTI->EXTICR[pin.bit / 4] |= GPIO_PORT_NUM(pin.port) << 8 * (pin.bit % 4);
EXTI->IMR1 |= 1 << pin.bit;
// Enable interrupt vector
NVIC->IPR[EXTI0_IRQn + pin.bit] = 0;
__COMPILER_BARRIER();
NVIC->ISER[(EXTI0_IRQn + pin.bit) / 32] = 1 << ((EXTI0_IRQn + pin.bit) % 32);
__COMPILER_BARRIER();
}

// Interrupt handler for all GPIO pins
void GPIO_IRQHandler(int i) {
// Clear pending IRQ
NVIC->ICPR[(EXTI0_IRQn + i) / 32] = 1 << ((EXTI0_IRQn + i) % 32);
// Detect rising edge
if (EXTI->RPR1 & (1 << i)) {
	EXTI->RPR1 = (1 << i); // Service interrupt
	callbacks[i][RISE](); // Invoke callback function
}

// Detect falling edge
if (EXTI->FPR1 & (1 << i)) {
	EXTI->FPR1 = (1 << i); // Service interrupt
	callbacks[i][FALL](); // Invoke callback function
}
}
// Dispatch all GPIO IRQs to common handler function
void EXTI0_IRQHandler() { GPIO_IRQHandler( 0); }
void EXTI1_IRQHandler() { GPIO_IRQHandler( 1); }
void EXTI2_IRQHandler() { GPIO_IRQHandler( 2); }
void EXTI3_IRQHandler() { GPIO_IRQHandler( 3); }
void EXTI4_IRQHandler() { GPIO_IRQHandler( 4); }
void EXTI5_IRQHandler() { GPIO_IRQHandler( 5); }
void EXTI6_IRQHandler() { GPIO_IRQHandler( 6); }
void EXTI7_IRQHandler() { GPIO_IRQHandler( 7); }
void EXTI8_IRQHandler() { GPIO_IRQHandler( 8); }
void EXTI9_IRQHandler() { GPIO_IRQHandler( 9); }
void EXTI10_IRQHandler() { GPIO_IRQHandler(10); }
void EXTI11_IRQHandler() { GPIO_IRQHandler(11); }
void EXTI12_IRQHandler() { GPIO_IRQHandler(12); }
void EXTI13_IRQHandler() { GPIO_IRQHandler(13); }
void EXTI14_IRQHandler() { GPIO_IRQHandler(14); }
void EXTI15_IRQHandler() { GPIO_IRQHandler(15); }
