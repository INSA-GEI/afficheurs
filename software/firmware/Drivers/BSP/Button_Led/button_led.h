/*
 * button_led.h
 *
 *  Created on: Apr 29, 2022
 *      Author: dimercur
 */

#ifndef BSP_BUTTON_LED_BUTTON_LED_H_
#define BSP_BUTTON_LED_BUTTON_LED_H_

#include "stm32l4xx_hal.h"
#include "config.h"

typedef enum {
	BUTTON_ShortPress=1,
	BUTTON_LongPress
} BUTTON_Event;

void BUTTON_Init(void);
BUTTON_Event BUTTON_WaitForEvent(void);

#endif /* BSP_BUTTON_LED_BUTTON_LED_H_ */
