/*
 * panic.c
 *
 *  Created on: 11 avr. 2022
 *      Author: dimercur
 */

#include "panic.h"

void PANIC_Handler(PANIC_Events event) {

	switch (event) {
	case PANIC_EVT_XBEE_CONFIG_ERROR:
		break;
	case PANIC_EVT_DISPLAY_CONFIG_ERROR:
		break;
	}

	while (1);
}
