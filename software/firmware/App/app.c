/*
 * app.c
 *
 *  Created on: 12 avr. 2022
 *      Author: dimercur
 */

#include "app.h"
#include "panic.h"

#include "protocol.h"

void APP_Init(void) {
	/* Init RF layer */
	if (PROTOCOL_Init()!=PROTOCOL_OK)
		PANIC_Handler(PANIC_EVT_XBEE_CONFIG_ERROR);
}
