/*
 * app.c
 *
 *  Created on: 12 avr. 2022
 *      Author: dimercur
 */

#include "app.h"
#include "panic.h"

#include "protocol.h"

PROTOCOL_ConfigurationTypedef configuration;

void APP_Init(void) {
	PROTOCOL_Status status;

	/* Init RF layer */
	if (PROTOCOL_Init()!=PROTOCOL_OK)
		PANIC_Handler(PANIC_EVT_XBEE_CONFIG_ERROR);

	// Try connect to server
	status = PROTOCOL_Connect(&configuration);

	if (status != PROTOCOL_OK) {
		if (status == PROTOCOL_RX_HW_ERROR)
			PANIC_Handler(PANIC_EVT_XBEE_CONFIG_ERROR);
		else if  (status == PROTOCOL_NOT_CONNECTED)
			// things to do here, try again later
			while (1);
	}

	// Get screen configuration
	status = PROTOCOL_GetConfiguration(&configuration);

	if (status != PROTOCOL_OK) {
		if (status == PROTOCOL_RX_HW_ERROR)
			PANIC_Handler(PANIC_EVT_XBEE_CONFIG_ERROR);
		else if  (status == PROTOCOL_NOT_CONNECTED)
			// things to do here, try again later
			while (1);
	}

	// everything worked fine, but no code here yet
	while(1);
}
