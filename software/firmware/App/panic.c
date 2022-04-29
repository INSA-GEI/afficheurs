/*
 * panic.c
 *
 *  Created on: 11 avr. 2022
 *      Author: dimercur
 */

#include "panic.h"
#include "display.h"

void PANIC_Handler(PANIC_Events event) {

	switch (event) {
	case PANIC_EVT_XBEE_CONFIG_ERROR:
		DISPLAY_ShowPanic(0x1);
		break;
	case PANIC_EVT_DISPLAY_CONFIG_ERROR:
		break;
	case PANIC_EVT_RTC_CONFIG_ERROR:
		DISPLAY_ShowPanic(0x2);
		break;
	case PANIC_EVT_RTC_MSP_CONFIG_ERROR:
		DISPLAY_ShowPanic(0x3);
		break;
	case PANIC_EVT_ADC_CONFIG_ERROR:
		DISPLAY_ShowPanic(0x4);
		break;
	case PANIC_EVT_ADC_MSP_CONFIG_ERROR:
		DISPLAY_ShowPanic(0x5);
		break;
	default:
		break;
	}

	while (1);
}
