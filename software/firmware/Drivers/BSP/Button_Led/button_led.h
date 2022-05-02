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

typedef void (*BUTTON_Callback)(BUTTON_Event event);

void BUTTON_Init(void);
void BUTTON_SetCallback(BUTTON_Callback callback);

#endif /* BSP_BUTTON_LED_BUTTON_LED_H_ */
