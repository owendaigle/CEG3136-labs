/*
 * gpio.h
 *
 *  Created on: Sep 22, 2025
 *      Author: odaig041
 */

#ifndef GPIO_H_
#define GPIO_H_

#include "stm32l5xx.h"

//this preprocessor macro converts a GPIO reg base address
//(GPIOA, GPIOB, GPIOC) to a zero based port index (0,1,2)
//The address pattern can be found from the device header file
#define GPIO_PORT_NUM(addr) (((unsigned)(addr) & 0xFC00) / 0x400)

//structure representing a single GPIO pin
typedef struct {
	GPIO_TypeDef *port; //GPIOA, GPIOB, etc
	int bit; //bit index 0..15
} Pin_t;

typedef enum {INPUT=0b00, OUTPUT=0b01, ALTFUNC=0b10, ANALOG=0b11} PinMode_t;
typedef enum {LOW=0, HIGH=1} PinState_t;
typedef enum {FALL=0, RISE=1} PinEdge_t;

void GPIO_Enable(Pin_t pin); //enables the system clock via RCC_AHB2ENR
void GPIO_Mode(Pin_t pin, PinMode_t mode); //configures the mode of GPIO pin via GPIOx_MODER
PinState_t GPIO_Input(Pin_t pin); //Returns state of GPIO pin thru reading GPIO_x_BSRR
void GPIO_Output(Pin_t pin, PinState_t state); // Drives GPIO output pin using GPIOx_BSRR
void GPIO_Toggle(Pin_t pin); //Toggles state of GPIO output pin using GPIOx_ODR
void GPIO_Callback(Pin_t pin, void (*func)(void), PinEdge_t edge); //registers a callback function for a GPIO input pin and edge direction.

#endif /* GPIO_H_ */
