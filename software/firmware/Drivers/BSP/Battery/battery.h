/*
 * battery.h
 *
 *  Created on: Apr 29, 2022
 *      Author: dimercur
 */

#ifndef BSP_BATTERY_BATTERY_H_
#define BSP_BATTERY_BATTERY_H_

#include "stm32l4xx_hal.h"
#include "config.h"

typedef enum {
	BATTERY_OK=1,
	BATTERY_HW_ERROR,
	BATTERY_TIMEOUT
} BATTERY_Status;

BATTERY_Status BATTERY_Init(void);
BATTERY_Status BATTERY_GetVoltage(uint16_t *val);

#endif /* BSP_BATTERY_BATTERY_H_ */
