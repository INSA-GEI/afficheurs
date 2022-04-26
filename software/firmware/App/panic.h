/*
 * panic.h
 *
 *  Created on: 11 avr. 2022
 *      Author: dimercur
 */

#ifndef PANIC_H_
#define PANIC_H_

typedef enum {
	PANIC_EVT_XBEE_CONFIG_ERROR=1,
	PANIC_EVT_DISPLAY_CONFIG_ERROR
} PANIC_Events;

void PANIC_Handler(PANIC_Events event);

#endif /* PANIC_H_ */