/*
 * panic.h
 *
 *  Created on: 11 avr. 2022
 *      Author: dimercur
 */

#ifndef PANIC_H_
#define PANIC_H_

#include "stm32l4xx_hal.h"
#include "config.h"

typedef enum {
	PANIC_EVT_XBEE_CONFIG_ERROR=1,
	PANIC_EVT_DISPLAY_CONFIG_ERROR,
	PANIC_EVT_RTC_CONFIG_ERROR,
	PANIC_EVT_RTC_MSP_CONFIG_ERROR,
	PANIC_EVT_ADC_CONFIG_ERROR,
	PANIC_EVT_ADC_MSP_CONFIG_ERROR
} PANIC_Events;

void PANIC_Handler(PANIC_Events event);

#endif /* PANIC_H_ */
