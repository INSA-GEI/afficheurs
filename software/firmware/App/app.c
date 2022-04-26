/*
 * app.c
 *
 *  Created on: 12 avr. 2022
 *      Author: dimercur
 */

#include "app.h"
#include "panic.h"

#include "protocol.h"
#include "calendar.h"
#include "display.h"

PROTOCOL_ConfigurationTypedef configuration;

void APP_Init(void) {
	PROTOCOL_Status status;
	DISPLAY_StatusTypedef display_status;

	//uint8_t update_status;
	/* Init screen */
	display_status = DISPLAY_Init();
	if (display_status != DISPLAY_OK)
		PANIC_Handler(PANIC_EVT_XBEE_CONFIG_ERROR);

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
	}

	// Get calendar
	//status = PROTOCOL_GetCalendar(&configuration, &calendar);
	status = PROTOCOL_GetCalendar(&configuration);

	if (status != PROTOCOL_OK) {
		if (status == PROTOCOL_RX_HW_ERROR)
			PANIC_Handler(PANIC_EVT_XBEE_CONFIG_ERROR);
	}

	//	// Get calendar update status
	//	status = PROTOCOL_GetCalendarUpdateStatus(&configuration, &update_status);
	//
	//	if (status != PROTOCOL_OK) {
	//		if (status == PROTOCOL_RX_HW_ERROR)
	//			PANIC_Handler(PANIC_EVT_XBEE_CONFIG_ERROR);
	//	}

	//	// send rssi report configuration
	//	status = PROTOCOL_SendReport(&configuration);
	//
	//	if (status != PROTOCOL_OK) {
	//		if (status == PROTOCOL_RX_HW_ERROR)
	//			PANIC_Handler(PANIC_EVT_XBEE_CONFIG_ERROR);
	//	}

	// SHow first reservation
	//DISPLAY_ShowReservation(&configuration, CAL_GetFirst(), "Test", DISPLAY_PromptIconNext);
	DISPLAY_ShowDayReservation(&configuration, 1);

	// everything worked fine, but no code here yet
	while(1);
}
