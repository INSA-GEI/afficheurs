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
#include "rtc.h"
#include "battery.h"
#include "button_led.h"

PROTOCOL_ConfigurationTypedef configuration;

void APP_Init(void) {
	//PROTOCOL_Status status;

	//uint8_t update_status;
	/* Init screen */
	if (DISPLAY_Init() != DISPLAY_OK)
		PANIC_Handler(PANIC_EVT_DISPLAY_CONFIG_ERROR);

	if (RTC_Init() != RTC_OK)
		PANIC_Handler(PANIC_EVT_RTC_CONFIG_ERROR);

	if (BATTERY_Init() != BATTERY_OK)
		PANIC_Handler(PANIC_EVT_ADC_CONFIG_ERROR);

	BUTTON_Init();

#if DEBUG_PROTOCOL_FAKE_CONFIG!=1
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
#else
	PROTOCOL_FakeConfig(&configuration);
	PROTOCOL_FakeCalendar();
#endif /* DEBUG_PROTOCOL_FAKE_CONFIG */

	// Show first reservation
	//DISPLAY_ShowReservation(&configuration, CAL_GetFirst(), "Test", DISPLAY_PromptIconNext);
	DISPLAY_ShowDayReservation(&configuration, 1);
	//DISPLAY_ShowWaitToConnect(configuration.device_uid);
	//DISPLAY_ShowPanic(0x12345678);
	//DISPLAY_ShowConfiguration(&configuration);
	//DISPLAY_ShowWeekReservation(&configuration);

	// everything worked fine, but no code here yet
	// Switch off display and wait
	DISPLAY_EnterPowerOff();

	while(1);
}
