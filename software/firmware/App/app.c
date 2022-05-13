/*
 * app.c
 *
 *  Created on: 12 avr. 2022
 *      Author: dimercur
 */

#include "stm32l4xx_hal.h"
#include "cmsis_os.h"

#include "app.h"
#include "panic.h"

#include "protocol.h"
#include "calendar.h"
#include "display.h"
#include "rtc.h"
#include "battery.h"
#include "button_led.h"

/* Structure that will hold the TCB of the task being created. */
StaticTask_t xTaskBufferDrawDisplay;
StaticTask_t xTaskBufferInit;
StaticTask_t xTaskBufferPeriodicWakeup;
StaticTask_t xTaskBufferEvents;

/* Buffer that the task being created will use as its stack.  Note this is
    an array of StackType_t variables.  The size of StackType_t is dependent on
    the RTOS port. */
StackType_t xStackDrawDisplay[ STACK_SIZE ];
StackType_t xStackInit[ STACK_SIZE ];
StackType_t xStackPeriodicWakeup[ STACK_SIZE ];
StackType_t xStackEvents[ STACK_SIZE ];

TaskHandle_t xHandleDrawDisplay = NULL;
TaskHandle_t xHandleInit = NULL;
TaskHandle_t xHandlePeriodicWakeup = NULL;
TaskHandle_t xHandleEvents = NULL;

PROTOCOL_ConfigurationTypedef configuration;

#define INIT_DISPLAY	0x00000001
#define INIT_XBEE		0x00000002
#define INIT_RTC		0x00000004
#define INIT_BUTTON		0x00000008
#define INIT_BATTERY	0x00000010

uint32_t peripheralsToInit;

void vTaskDrawDisplay(void* params);
/* Function that implements the task being created. */
void vTaskInit(void* params);
/* Function that implements the task being created. */
void vTaskPeriodicWakeup(void* params);
void vTaskEvents(void* params);

TickType_t msToTicks(TickType_t ms) {
	TickType_t tmp = ms;

	if (ms<(1000/configTICK_RATE_HZ))
		tmp = 1;
	else {
		tmp = ms/(1000/configTICK_RATE_HZ);

		if (ms % (1000/configTICK_RATE_HZ)) // if the delay is not true multiple of tick rate, increase by one (delay longer)
			tmp ++;
	}
	return tmp;
}

/* Function that creates a task. */
void TASKS_Create(void) {
	//defaultTaskHandle = osThreadNew(DefaultTask, NULL, &defaultTask_attributes);
	//initTaskHandle = osThreadNew(InitApplicationTask, NULL, &initTask_attributes);
	/* Create the task without using any dynamic memory allocation. */
	xHandleInit = xTaskCreateStatic(
			vTaskInit,       /* Function that implements the task. */
			"Init",          /* Text name for the task. */
			STACK_SIZE,      /* Number of indexes in the xStack array. */
			NULL,    /* Parameter passed into the task. */
			PriorityInit,/* Priority at which the task is created. */
			xStackInit,          /* Array to use as the task's stack. */
			&xTaskBufferInit);  /* Variable to hold the task's data structure. */

	xHandleDrawDisplay = xTaskCreateStatic(
			vTaskDrawDisplay,       /* Function that implements the task. */
			"DrawDisplay",          /* Text name for the task. */
			STACK_SIZE,      /* Number of indexes in the xStack array. */
			NULL,    /* Parameter passed into the task. */
			PriorityDrawDisplay,/* Priority at which the task is created. */
			xStackDrawDisplay,          /* Array to use as the task's stack. */
			&xTaskBufferDrawDisplay);  /* Variable to hold the task's data structure. */

	xHandlePeriodicWakeup = xTaskCreateStatic(
			vTaskPeriodicWakeup,       /* Function that implements the task. */
			"PeriodicWakeup",          /* Text name for the task. */
			STACK_SIZE,      /* Number of indexes in the xStack array. */
			NULL,    /* Parameter passed into the task. */
			PriorityPeriodicWakeup,/* Priority at which the task is created. */
			xStackPeriodicWakeup,          /* Array to use as the task's stack. */
			&xTaskBufferPeriodicWakeup);  /* Variable to hold the task's data structure. */

	xHandleEvents = xTaskCreateStatic(
			vTaskEvents,       /* Function that implements the task. */
			"Events",          /* Text name for the task. */
			STACK_SIZE,      /* Number of indexes in the xStack array. */
			NULL,    /* Parameter passed into the task. */
			PriorityEvents,/* Priority at which the task is created. */
			xStackEvents,          /* Array to use as the task's stack. */
			&xTaskBufferEvents);  /* Variable to hold the task's data structure. */

	/* puxStackBuffer and pxTaskBuffer were not NULL, so the task will have
        been created, and xHandleDrawDisplay will be the task's handle.  Use the handle
        to suspend the task. */
	vTaskSuspend(xHandleInit);
	vTaskSuspend(xHandleDrawDisplay);
	vTaskSuspend(xHandlePeriodicWakeup);
	vTaskSuspend(xHandleEvents);
}

/* Function that creates a task. */
void TASKS_Run(void) {
	vTaskResume(xHandleInit);
}

void vTaskInit(void* params) {
	PROTOCOL_Status status;
	uint8_t is_connectDisplayShown =0;

	//uint32_t val;

	//uint8_t update_status;
	// First init, after power up
	// Init every peripheral, in order to check if there are hardware issues
	peripheralsToInit = INIT_DISPLAY | INIT_XBEE | INIT_RTC | INIT_BUTTON | INIT_BATTERY;

	while (1) {
		/* Init screen */
		if (peripheralsToInit & INIT_DISPLAY) {
			if (DISPLAY_Init() != DISPLAY_OK) {
				DISPLAY_EnterPowerOff();

				PANIC_Handler(PANIC_EVT_DISPLAY_CONFIG_ERROR);
			}
		}

		if (peripheralsToInit & INIT_RTC) {
			if (RTC_Init() != RTC_OK)
				PANIC_Handler(PANIC_EVT_RTC_CONFIG_ERROR);
		}

		if (peripheralsToInit & INIT_BATTERY) {
			if (BATTERY_Init() != BATTERY_OK)
				PANIC_Handler(PANIC_EVT_ADC_CONFIG_ERROR);
		}

		if (peripheralsToInit & INIT_BUTTON) {
			BUTTON_Init();
		}

		if (peripheralsToInit & INIT_XBEE) {
#if DEBUG_PROTOCOL_FAKE_CONFIG!=1
			/* Init RF layer */
			if (PROTOCOL_Init(&configuration)!=PROTOCOL_OK)
				PANIC_Handler(PANIC_EVT_XBEE_CONFIG_ERROR);

			// Try connect to server
			while ((status = PROTOCOL_Connect(&configuration))!=PROTOCOL_OK) {
				if (status == PROTOCOL_RX_HW_ERROR)
					PANIC_Handler(PANIC_EVT_XBEE_CONFIG_ERROR);
				else if  (status == PROTOCOL_NOT_CONNECTED) {
					if (!is_connectDisplayShown) {
						is_connectDisplayShown=1;
						DISPLAY_ShowWaitToConnect(configuration.device_uid);
						DISPLAY_Update();
					}

					vTaskDelay(msToTicks(1000*10)); // toutes les 10s
				}
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

			/* Show first reservation (a revoir) */
			DISPLAY_ShowReservation(&configuration, CAL_GetFirst(), "Hello", DISPLAY_PromptIconInfo);
			DISPLAY_Update();
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
		}

		DISPLAY_EnterPowerOff();
		RTC_SetDate(2, 10, 5, 22);
		RTC_SetTime(14, 8, 00);

		peripheralsToInit =0;

		/* Init has complete, so we can start others tasks */
		vTaskResume(xHandleDrawDisplay);
		vTaskResume(xHandlePeriodicWakeup);
		vTaskResume(xHandleEvents);

		//wait for next activation of init (after a deep sleep)
		ulTaskNotifyTake( pdTRUE, portMAX_DELAY); // infinite timeout
		// wake up this task with xTaskNotifyGive(xHandleInit);
	}
}

/* Function that implements the task being created. */
void vTaskDrawDisplay(void* params) {
	uint16_t val=0;

	while(1) {
		vTaskDelay(msToTicks(1000));

		BATTERY_GetVoltage(&val);
	}
}

/* Function that implements the task being created. */
void vTaskPeriodicWakeup(void* params) {
	RTC_AlarmEvent evt;

	while(1) {
		RTC_SetNextEvent(1);
		evt = RTC_WaitForEvent();

		switch (evt) {
		case RTC_AlarmEvent_WeekStart:
			break;
		case RTC_AlarmEvent_OtherEvent:
			break;
		}
	}
}

/* Function that implements the task being created. */
void vTaskEvents(void* params) {
	BUTTON_Event evt;

	while(1) {
		evt=BUTTON_WaitForEvent();

		if (DISPLAY_Init() != DISPLAY_OK)
			PANIC_Handler(PANIC_EVT_DISPLAY_CONFIG_ERROR);

		switch (evt) {
		case BUTTON_ShortPress:
			DISPLAY_ShowWaitToConnect(configuration.device_uid);
			break;
		case BUTTON_LongPress:
			DISPLAY_ShowConfiguration(&configuration);
		}

		DISPLAY_Update();
		DISPLAY_EnterPowerOff();
	}
}
