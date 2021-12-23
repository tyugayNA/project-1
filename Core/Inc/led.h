/*
 * led.h
 *
 *  Created on: Dec 22, 2021
 *      Author: nikolay
 */

#ifndef INC_LED_H_
#define INC_LED_H_

#include "main.h"

typedef struct {
	GPIO_TypeDef* port;
	uint16_t pin;
	uint16_t count;
	uint16_t period;
} led_parameters;

#endif /* INC_LED_H_ */
