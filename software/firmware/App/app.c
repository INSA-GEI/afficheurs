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
StaticTask_t xTaskBufferProtocol;
StaticTask_t xTaskBufferPeriodicWakeup;

/* Buffer that the task being created will use as its stack.  Note this is
    an array of StackType_t variables.  The size of StackType_t is dependent on
    the RTOS port. */
StackType_t xStackDrawDisplay[ STACK_SIZE ];
StackType_t xStackProtocol[ STACK_SIZE ];
StackType_t xStackPeriodicWakeup[ STACK_SIZE ];

TaskHandle_t xHandleDrawDisplay = NULL;
TaskHandle_t xHandleProtocol = NULL;
TaskHandle_t xHandlePeriodicWakeup = NULL;

PROTOCOL_ConfigurationTypedef configuration;
void Alarm_Callback(RTC_AlarmEvent event);
void Button_Callback(BUTTON_Event event);
void DefaultTask(void *argument);
void InitApplicationTask(void *argument);

void vTaskDrawDisplay(void* params);
/* Function that implements the task being created. */
void vTaskProtocol(void* params);
/* Function that implements the task being created. */
void vTaskPeriodicWakeup(void* params);

///* Definitions for defaultTask */
//osThreadId_t defaultTaskHandle;
//const osThreadAttr_t defaultTask_attributes = {
//		.name = "defaultTask",
//		.stack_size = 128 * 4,
//		.priority = (osPriority_t) osPriorityNormal,
//};
//
//osThreadId_t initTaskHandle;
//const osThreadAttr_t initTask_attributes = {
//		.name = "initApplication",
//		.stack_size = 128 * 4,
//		.priority = (osPriority_t) osPriorityNormal,
//};

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
	xHandleDrawDisplay = xTaskCreateStatic(
			vTaskDrawDisplay,       /* Function that implements the task. */
			"DrawDisplay",          /* Text name for the task. */
			STACK_SIZE,      /* Number of indexes in the xStack array. */
			NULL,    /* Parameter passed into the task. */
			PriorityDrawDisplay,/* Priority at which the task is created. */
			xStackDrawDisplay,          /* Array to use as the task's stack. */
			&xTaskBufferDrawDisplay);  /* Variable to hold the task's data structure. */

	xHandleProtocol = xTaskCreateStatic(
			vTaskProtocol,       /* Function that implements the task. */
			"Protocol",          /* Text name for the task. */
			STACK_SIZE,      /* Number of indexes in the xStack array. */
			NULL,    /* Parameter passed into the task. */
			PriorityProtocol,/* Priority at which the task is created. */
			xStackProtocol,          /* Array to use as the task's stack. */
			&xTaskBufferProtocol);  /* Variable to hold the task's data structure. */

	xHandlePeriodicWakeup = xTaskCreateStatic(
			vTaskPeriodicWakeup,       /* Function that implements the task. */
			"PeriodicWakeup",          /* Text name for the task. */
			STACK_SIZE,      /* Number of indexes in the xStack array. */
			NULL,    /* Parameter passed into the task. */
			PriorityPeriodicWakeup,/* Priority at which the task is created. */
			xStackPeriodicWakeup,          /* Array to use as the task's stack. */
			&xTaskBufferPeriodicWakeup);  /* Variable to hold the task's data structure. */
	/* puxStackBuffer and pxTaskBuffer were not NULL, so the task will have
        been created, and xHandleDrawDisplay will be the task's handle.  Use the handle
        to suspend the task. */
	vTaskSuspend(xHandleDrawDisplay);
	vTaskSuspend(xHandleProtocol);
	vTaskSuspend(xHandlePeriodicWakeup);
}

/* Function that creates a task. */
void TASKS_Run(void) {
	vTaskResume(xHandleDrawDisplay);
	vTaskResume(xHandleProtocol);
	vTaskResume(xHandlePeriodicWakeup);
}

void APP_Init(void) {
	//PROTOCOL_Status status;
	//uint32_t val;
	uint16_t battery_val=0x00;

	//uint8_t update_status;
	/* Init screen */
	if (DISPLAY_Init() != DISPLAY_OK)
		PANIC_Handler(PANIC_EVT_DISPLAY_CONFIG_ERROR);
	DISPLAY_EnterPowerOff();

	if (RTC_Init() != RTC_OK)
		PANIC_Handler(PANIC_EVT_RTC_CONFIG_ERROR);
	RTC_SetAlarmCallback(Alarm_Callback);

	if (BATTERY_Init() != BATTERY_OK)
		PANIC_Handler(PANIC_EVT_ADC_CONFIG_ERROR);

	BUTTON_Init();
	BUTTON_SetCallback(Button_Callback);

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
	//DISPLAY_ShowDayReservation(&configuration, 1);
	//DISPLAY_ShowWaitToConnect(configuration.device_uid);
	//DISPLAY_ShowPanic(0x12345678);
	//DISPLAY_ShowConfiguration(&configuration);
	//DISPLAY_ShowWeekReservation(&configuration);

	// Test RTC
	//	RTC_SetAlarmCallback(Alarm_Callback);
	//
	//	if (RTC_SetTime(8, 0, 0) != RTC_OK)
	//		while (1);
	//
	//	if (RTC_SetDate(1, 2, 05, 22) != RTC_OK)
	//		while (1);
	//
	//	if (RTC_SetNextEvent(1) != RTC_OK)
	//		while (1);
	//
	//	if (RTC_EnableWeekStartEvent(0) != RTC_OK)
	//		while (1);
	//	if ((val=RTC_UnitTests()) != 0)
	//		while (1);

	// Test battery
	// if (BATTERY_GetVoltage(&battery_val) != BATTERY_OK)
	//	while (1);
}

void DefaultTask(void *argument) {
	while (1)
		vTaskDelay(msToTicks(1000));
}

void Button_Callback(BUTTON_Event event) {
	int i;
	if (event == BUTTON_ShortPress)
		i=0;
	else
		i=1;
}

void Alarm_Callback(RTC_AlarmEvent event) {
	/*uint8_t hour;
	uint8_t min;
	uint8_t sec;
	uint8_t weekday;

	if (RTC_GetTime(&hour, &min, &sec)!= RTC_OK)
		while (1);

	if (RTC_GetWeekDay(&weekday)!= RTC_OK)
		while (1);

	if (event == RTC_AlarmEvent_OtherEvent)
		RTC_StopEvent();

	while(1);*/
}

/* Function that implements the task being created. */
void vTaskDrawDisplay(void* params) {
	while(1) {

	}
}

/* Function that implements the task being created. */
void vTaskProtocol(void* params) {
	while(1) {

	}
}

/* Function that implements the task being created. */
void vTaskPeriodicWakeup(void* params) {
	while(1) {

	}
}

