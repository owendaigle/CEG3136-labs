#ifndef I2C_H_
#define I2C_H_

#include <stdbool.h>
#include "stm32l5xx.h"
#include "gpio.h"

//i2c bus connection
typedef struct {
	I2C_TypeDef *iface; //interface registers I2C1 - I2C3
	Pin_t pinSDA;
	Pin_t pinSCL;
} I2C_Bus_t;

extern I2C_Bus_t LeafyI2C; //I2C bus on leafy mainboard

//I2C transfer record
typedef struct I2C_Xfer_t {
	I2C_Bus_t *bus; //pointer to I2C bus structure
	uint8_t addr; //7 bit target address and read/write bit
	uint8_t *data; //pointer to data buffer
	int size; //total number of bytes in transfer
	bool stop;
	bool busy; //busy indicator (queued or in progress)
	struct I2C_Xfer_t *next; //pointer to next transfer in queue
} I2C_Xfer_t;

void I2C_Enable(I2C_Bus_t bus); //enable I2C bus connection
void I2C_Request(I2C_Xfer_t *p); //request a new transfer

void ServiceI2CRequests(void); //called from main loop

#endif /* I2C_H_ */
