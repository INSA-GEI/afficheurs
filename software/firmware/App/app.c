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
#include "main.h"
#include <stdio.h>


/*
 *
 * RTOS TASK
 *
 */

StaticTask_t xTaskBufferWakeUpButton;
StaticTask_t xTaskBufferInit;
StaticTask_t xTaskBufferWakeUpRTC;
StaticTask_t xTaskBufferFirstWakeUp;


StackType_t xStackWakeUpButton[ STACK_SIZE ];
StackType_t xStackInit[ STACK_SIZE ];
StackType_t xStackWakeUpRTC[ STACK_SIZE ];
StackType_t xStackFirstWakeUp[ STACK_SIZE ];



TaskHandle_t xHandleWakeUpButton = NULL;
TaskHandle_t xHandleInit = NULL;
TaskHandle_t xHandleWakeUpRTC = NULL;
TaskHandle_t xHandleFirstWakeUp = NULL;

/*
 *
 * INIT
 *
 */


#define INIT_DISPLAY	0x00000001
#define INIT_XBEE		0x00000002
#define INIT_RTC		0x00000004
#define INIT_BUTTON		0x00000008
#define INIT_BATTERY	0x00000016

/*
 *
 * WAKE UP
 *
 */

extern uint8_t firstWakeUp;
extern uint8_t WakeUpButton;
extern uint8_t WakeUpRTC;
extern uint8_t WakeUpDebug;

/*
 *
 * PORTOCOL STATUS...
 *
 */

PROTOCOL_ConfigurationTypedef configuration;
PROTOCOL_Status status;
CALENDAR_Status update_status;
DISPLAY_Status display_status;
uint32_t peripheralsToInit;

/*
 *
 * RTC
 *
 */


uint8_t alarmHour = 0;
uint8_t alarmMin = 0;
uint8_t alarmSec = 0;
uint8_t hour = 0;
uint8_t min = 0;
uint8_t sec = 0;
uint8_t weekday = 0;
uint32_t timemin = 0;
uint32_t diff = 0;
extern RTC_HandleTypeDef hrtc;

/*
 *
 * DISPLAY
 *
 */

uint8_t GetLastPrintDisplay = 0; //Normal view = 0, day view = 1, week end view = 2
uint8_t BackUpNormalView = 0;
uint8_t is_connectDisplayShown = 0;
extern uint8_t ChangeCourseOnStandardView;

/*
 *
 * BATTERY
 *
 */

char val[4];
uint8_t mode = 0; //MODE NORMAL = 0 //MODE FAIBLE  = 1 //  MODE CRITIQUE = 2
uint32_t valBattery;

/*
 *
 * UART
 *
 */


uint8_t InitText[] = "Run vTaskInit!!!\r\n";
uint8_t WakeUpButtonText[] = "Run vTaskWakeUpButton!!!\r\n";
uint8_t FirstWakeUpText[] = "Run FirstWakeUp!!!\r\n";
uint8_t WakeUpRTCText[] = "Run vTaskWakeUpRTC!!!\r\n";
uint8_t FirstWakeUpOfTheDayText[] = "Run FirstWakeUpOfTheDay!!!\r\n";
uint8_t StandbyModeText[] = "StandBy Mode!!!\r\n";
uint8_t ConnectText[] = "Coordinator Connect!!!\r\n";
uint8_t ConfigText[] = "Coordinator Config!!!\r\n";
uint8_t CalendarXbeeText[] = "Loading Calendar XBEE!!!\r\n";
uint8_t CalendarFlashText[] = "Loading Calendar FLASH!!!\r\n";
uint8_t DisplayText[] = "Coordinator Display!!!\r\n";
uint8_t UpdateText[] = "Coordinator Update!!!\r\n";
uint8_t NightModeText[] = "Night Mode!!!\r\n";
uint8_t WeekModeText[] = "Week Mode!!!\r\n";
extern UART_HandleTypeDef huart2;

/*
 *
 * PROTOTYPE
 *
 */

void vTaskInit(void* params);
void vTaskWakeUpButton(void* params);
void vTaskWakeUpRTC(void* params);
void vTaskFirstWakeUp(void* params);
void vTaskFirstWakeUpOfTheDay(void* params);
void Paint_Clear(uint16_t Color);
void clear_flag();
void set_flag();
void go_sleep();
void show_standard_view();
void next_alarm_rtc();
TickType_t msToTicks(TickType_t ms);

/*
 * ***********************************************************************************************************
 * ***********************************************************************************************************
 */

void TASKS_Create(void)
{
	xHandleInit = xTaskCreateStatic(
			vTaskInit,
			"Init",
			STACK_SIZE,
			NULL,
			PriorityInit,
			xStackInit,
			&xTaskBufferInit);

	xHandleWakeUpRTC = xTaskCreateStatic(
			vTaskWakeUpRTC,
			"WakeUpRTC",
			STACK_SIZE,
			NULL,
			PriorityWakeUpRTC,
			xStackWakeUpRTC,
			&xTaskBufferWakeUpRTC);

	xHandleWakeUpButton = xTaskCreateStatic(
			vTaskWakeUpButton,
			"WakeUpButton",
			STACK_SIZE,
			NULL,
			PriorityWakeUpButton,
			xStackWakeUpButton,
			&xTaskBufferWakeUpButton);

	xHandleFirstWakeUp = xTaskCreateStatic(
			vTaskFirstWakeUp,
			"FirstWakeUp",
			STACK_SIZE,
			NULL,
			PriorityFirstWakeUp,
			xStackFirstWakeUp,
			&xTaskBufferFirstWakeUp);


	vTaskSuspend(xHandleInit);
	vTaskSuspend(xHandleWakeUpButton);
	vTaskSuspend(xHandleWakeUpRTC);
	vTaskSuspend(xHandleFirstWakeUp);
}

/*
 * ***********************************************************************************************************
 * ***********************************************************************************************************
 */

void TASKS_Run(void)
{
	vTaskResume(xHandleInit);
}

/*
 * ***********************************************************************************************************
 * ***********************************************************************************************************
 */

// PIN 6 to simulate battery voltage
// PIN 0 to simulate a button press

void vTaskInit(void* params)
{
	HAL_UART_Transmit(&huart2,InitText,sizeof(InitText),10);

	peripheralsToInit = INIT_DISPLAY | INIT_XBEE | INIT_RTC | INIT_BATTERY | INIT_BUTTON;

	while (1)
	{

		/*
		 *
		 * BLOCK : INIT BATTERY
		 *
		 */


		if (peripheralsToInit & INIT_BATTERY)
		{
			if (BATTERY_Init() != BATTERY_OK)
			{
				PANIC_Handler(PANIC_EVT_RTC_CONFIG_ERROR);
			}
		}


		BATTERY_ModeVoltage(&mode); //Retrieve battery mode

		mode = 1; //Debug


		/*
		 *
		 * BLOCK : INIT RTC
		 *
		 */


		if (peripheralsToInit & INIT_RTC)
		{
			if (RTC_Init() != RTC_OK)
				PANIC_Handler(PANIC_EVT_RTC_CONFIG_ERROR);
		}

		/*
		 *
		 * BLOCK : CLEAR FLAG RTC
		 *
		 */


		clear_flag(); //Clearing the flag is mandatory for the proper functioning of RTC and button wake-ups.


		/*
		 *
		 * BLOCK : INIT BUTTON
		 *
		 */


		if (peripheralsToInit & INIT_BUTTON)
		{
				BUTTON_Init();
		}


		/*
		 *
		 * BLOCK : INIT DISPLAY
		 *
		 */

		if (peripheralsToInit & INIT_DISPLAY)
		{
			if (DISPLAY_Init() != DISPLAY_OK)
			{
				DISPLAY_EnterPowerOff();
				PANIC_Handler(PANIC_EVT_DISPLAY_CONFIG_ERROR);
			}
		}


		/*
		 *
		 * BLOCK : CHECK NIGHT MODE OR WEEK END MODE
		 *
		 */


		// Choice to program RTC alarms with a mask that does not consider days. Alarms trigger every day at the hours we choose regardless of the date.
		// Example: On Saturdays, it won't go to sleep until Monday morning at 6:00 AM, but until Sunday 6:00 AM.
		// Adding +2 to the RTC alarm date may cause issues for month-end dates, potentially setting alarms on the 32nd of the month and not waking up the RTC.


		if (WakeUpRTC == 1 || WakeUpButton == 1)
		{
			if (RTC_CheckWakeUpNight() == 1) // Check the RTC time, if it's night time, return 1.
			{
				DISPLAY_ShowNight();
				DISPLAY_Update();

				if (RTC_SetFirstAlarmOfTheDay() != RTC_OK) // Set the RTC alarm to wake up at 6:00 AM the next day.
					PANIC_Handler(PANIC_EVT_RTC_CONFIG_ERROR);

				go_sleep();
			}
			else if (RTC_CheckWakeUpWeekEnd() == 1) // Check the RTC date, if it's Saturday or Sunday, return 1
			{
				DISPLAY_ShowWeekEnd();
				DISPLAY_Update();

				if (RTC_SetFirstAlarmOfTheDay() != RTC_OK) // Set the RTC alarm to wake up at 6:00 AM the next day.
					PANIC_Handler(PANIC_EVT_RTC_CONFIG_ERROR);

				go_sleep();
			}
		}


		/*
		 *
		 * BLOCK : CHECK WAKE UP AFTER BUTTON PRESS AND DISPLAY MODIFICATION
		 *
		 */


		// This mode was designed after the STM32 was put to sleep following a button press. When the button is pressed, the display will be modified.
		// Therefore, a mode has been implemented that puts the RTC to sleep for 1 minute, allowing the user to view their display.
		// This allows for the identification of this wake-up event and triggers the 'buttonWakeUp' task to handle this specific case and switch back to the standard view.

		GetLastPrintDisplay = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0);

		if(WakeUpRTC == 1 && (GetLastPrintDisplay == CALENDAR_WEEK || GetLastPrintDisplay == CALENDAR_DAY))
		{
			WakeUpRTC = 0;
			WakeUpButton = 1;
			BackUpNormalView = 1;
		}


		/*
		 *
		 * BLOCK : INIT XBEE
		 *
		 */


		// XBEE Initialization Scenarios:
		// 1. First wake-up of the STM32 to load the calendar for the first time.
		// 2. When in debug mode.
		// 3. In battery mode 0 (normal mode) to request a calendar update.
		// 4. In battery mode 1 on the first Monday morning wake-up to update the calendar.

		if (WakeUpRTC == 1 || firstWakeUp == 1 || WakeUpDebug == 1 || mode == 0 || (mode == 1 && RTC_isFirstWakeUpOfTheDay() == 1 && RTC_GetWeekDay() == 1))
		{
				if (peripheralsToInit & INIT_XBEE)
				{
					if (PROTOCOL_Init(&configuration) != PROTOCOL_OK)
						PANIC_Handler(PANIC_EVT_XBEE_CONFIG_ERROR);
				}
		}


		/*
		 *
		 * BLOCK : CHECK BATTERY MODE CRITICAL
		 *
		 */


		if (mode == 2)
		{
			DISPLAY_ShowCritiqueMode();
			DISPLAY_Update();

			if (PROTOCOL_SendMessageBattery(&configuration) != PROTOCOL_OK)
				PANIC_Handler(PANIC_EVT_XBEE_CONFIG_ERROR);

			go_sleep();
		}


		/*
		 *
		 * BLOCK : PROTOCOL CONNECT
		 *
		 */


		// Choice made to send the configuration to the display every week on Monday morning at 6:00 AM.


		if(firstWakeUp == 1 || WakeUpDebug == 1 || (RTC_isFirstWakeUpOfTheDay() == 1 && RTC_GetWeekDay() == 1))
		{
			HAL_UART_Transmit(&huart2,ConnectText,sizeof(ConnectText),10);

			while ((status = PROTOCOL_Connect(&configuration))!=PROTOCOL_OK)
			{
				if (status == PROTOCOL_RX_HW_ERROR)
					PANIC_Handler(PANIC_EVT_XBEE_CONFIG_ERROR);
				else if  (status == PROTOCOL_NOT_CONNECTED)
				{
					if (!is_connectDisplayShown)
					{
						is_connectDisplayShown=1;
						DISPLAY_ShowWaitToConnect(configuration.device_uid);
						DISPLAY_Update();
					}
				vTaskDelay(msToTicks(1000*10));
				}
			}


			/*
			 *
			 * BLOCK : PROTOCOL CONFIG
			 *
			 */


			HAL_UART_Transmit(&huart2,ConfigText,sizeof(ConfigText),10);


			status = PROTOCOL_GetConfiguration(&configuration);

			if (status != PROTOCOL_OK)
			{
				if (status == PROTOCOL_RX_HW_ERROR)
					PANIC_Handler(PANIC_EVT_XBEE_CONFIG_ERROR);
			}


			/*
			 *
			 * BLOCK : RTC SET TIME AND DATE
			 *
			 */


			RTC_SetTime(configuration.init_time_hours,configuration.init_time_minutes,configuration.init_time_seconds);
			RTC_SetDate(configuration.init_week_day, configuration.init_date_day, configuration.init_date_month, configuration.init_date_year);
		}


		/*
		 *
		 * BLOCK : START RTOS TASKS
		 *
		 */


		if(firstWakeUp == 1 || WakeUpDebug == 1)
		{
			vTaskResume(xHandleFirstWakeUp);
		}
		else if (WakeUpButton == 1)
		{
			vTaskResume(xHandleWakeUpButton);
		}
		else if (WakeUpRTC == 1)
		{
				vTaskResume(xHandleWakeUpRTC);
		}
		else
		{
			PANIC_Handler(PANIC_EVT_WAKE_UP);
		}

		ulTaskNotifyTake( pdTRUE, portMAX_DELAY);
	}
}

/*
 * ***********************************************************************************************************
 * ***********************************************************************************************************
 */

void vTaskFirstWakeUp(void* params)
{
	while(1)
	{

		HAL_UART_Transmit(&huart2,FirstWakeUpText,sizeof(FirstWakeUpText),10);


		/*
		 *
		 * BLOCK : GET CALENDAR FROM XBEE
		 *
	     */


		HAL_UART_Transmit(&huart2,CalendarXbeeText,sizeof(CalendarXbeeText),10);

		status = PROTOCOL_GetCalendar(&configuration);

		if (status != PROTOCOL_OK)
		{
			if (status == PROTOCOL_RX_HW_ERROR)
				PANIC_Handler(PANIC_EVT_XBEE_CONFIG_ERROR);
		}

		/*
		 *
		 * BLOCK : SHOW STANDARD VIEW
		 *
	     */

		// Displays the standard view. Note that it is mandatory to call the RTC_CurrentTimeToSec function.
		// This function interprets the RTC time, e.g., 8:00 AM, as 480 minutes, which is used by the following functions.
		// Remaining work: Modify the CAL_GetNextReservation function to return both the next reservation time and its title.

		show_standard_view();

		/*
		 *
		 * BLOCK : SET ALARM RTC
		 *
	     */

		next_alarm_rtc();

		/*
		 *
		 * BLOCK : STANDBY MODE
		 *
	     */

		go_sleep();
	}
}

/*
 * ***********************************************************************************************************
 * ***********************************************************************************************************
 */

void vTaskWakeUpButton(void* params)
{
	while(1)
	{
		HAL_UART_Transmit(&huart2,WakeUpButtonText,sizeof(WakeUpButtonText),10);


		HAL_UART_Transmit(&huart2,CalendarFlashText,sizeof(CalendarFlashText), 10);


		/*
		 *
		 * BLOCK : GET CONFIG FROM FLASH
		 *
	     */


		PROTOCOL_GetFlashConfig(&configuration); //Parse the configuration from flash memory.
		if (status != PROTOCOL_OK)
		{
			if (status == PROTOCOL_RX_HW_ERROR)
				PANIC_Handler(PANIC_EVT_XBEE_CONFIG_ERROR);
		}


		/*
		 *
		 * BLOCK : GET CALENDAR FROM FLASH
		 *
	     */


		PROTOCOL_GetFlashCalendar(); //Parse the calendar from flash memory.
		if (status != PROTOCOL_OK)
		{
			if (status == PROTOCOL_RX_HW_ERROR)
				PANIC_Handler(PANIC_EVT_XBEE_CONFIG_ERROR);
		}


		/*
		 *
		 * BLOCK : SHOW APPROPRIOATE VIEW
		 *
		 */


		// Switches to the appropriate view based on the currently displayed view.
		// Initial user action transitions from the standard view to the daily view with approximately a one-minute timer.
		// Subsequent user action before reaching the daily view transitions from the daily view to the weekly view.

		if (BackUpNormalView == 1) //Wake-up after one minute of user inactivity (no additional button press).
		{
			show_standard_view();
		}
		else
		{
			GetLastPrintDisplay = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0);

			if(GetLastPrintDisplay == CALENDAR_NORMAL) //First press from standard view.
			{
				DISPLAY_ShowDayReservation(&configuration, RTC_GetWeekDay());

			    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, 1);
			}
			else if (GetLastPrintDisplay == CALENDAR_DAY) //Second press from daily view.
			{
				DISPLAY_ShowWeekReservation(&configuration);
			    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, 2);
			}
			else if (GetLastPrintDisplay == CALENDAR_WEEK) //Third press from weekly view.
			{
				DISPLAY_ShowDayReservation(&configuration, RTC_GetWeekDay());

			    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, 1);
			}
			else
			{
				PANIC_Handler(PANIC_EVT_SHOW_LASTED_CONFIGURATION);
			}

		}

		DISPLAY_Update();


		/*
		 *
		 * BLOCK : SET ALARM RTC
		 *
	     */


		if(BackUpNormalView == 1)
		{
			next_alarm_rtc();
		}
		else
		{
			// Set the next RTC wake-up in 1 minute, allowing the user to view the daily or weekly display
			// before returning to the standard view.

			if (RTC_SetPeriodicAlarm1() != RTC_OK)
				PANIC_Handler(PANIC_EVT_RTC_CONFIG_ERROR);
		}


		/*
		 *
		 * BLOCK : STANDBY MODE
		 *
		 */


		go_sleep();
	}
}

/*
 * ***********************************************************************************************************
 * ***********************************************************************************************************
 */

void vTaskWakeUpRTC(void* params)
{
	while(1)
	{

		HAL_UART_Transmit(&huart2,WakeUpRTCText,sizeof(WakeUpRTCText),10);
		HAL_UART_Transmit(&huart2,UpdateText,sizeof(UpdateText),10);


		// Mode 0 --> I check for updates. If updates are available, I send the new calendar; otherwise, I parse the calendar from flash memory.

		/*
		 *
		 * BLOCK : MODE 0
		 *
	     */

		if(mode == 0)
		{


			/*
			 *
			 * BLOCK : GET UPDATE CALENDAR
			 *
		     */


			status = PROTOCOL_GetCalendarUpdateStatus(&configuration, &update_status);
			if (status != PROTOCOL_OK)
			{
				if (status == PROTOCOL_RX_HW_ERROR)
					PANIC_Handler(PANIC_EVT_XBEE_CONFIG_ERROR);
			}

			if(update_status == CALENDAR_UPDATE)
			{
					/*
					 *
					 * --> UPDATE
					 *
					 */


					HAL_UART_Transmit(&huart2,CalendarXbeeText,sizeof(CalendarXbeeText),10);


					/*
					 *
					 * BLOCK : GET CALENDAR FROM XBEE
					 *
				     */


					status = PROTOCOL_GetCalendar(&configuration);

					if (status != PROTOCOL_OK)
					{
						if (status == PROTOCOL_RX_HW_ERROR)
						PANIC_Handler(PANIC_EVT_XBEE_CONFIG_ERROR);
					}


					/*
					 *
					 * BLOCK : UDPATE DISPLAY
					 *
				     */


					if(ChangeCourseOnStandardView == 0)
					{

							// This part simply helps to avoid consuming energy
							// to update the screen when there's nothing to update.
							// I use a backup register in which I store the value as 1
							// when I've set an RTC alarm 10 minutes before the class change.
							// In all cases, I reset this value to 0 after performing the update.

					}
					else if(ChangeCourseOnStandardView == 1)
					{
						HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0);


						/*
						 *
						 * BLOCK : GET CALENDAR FROM FLASH
						 *
						 */


						 PROTOCOL_GetFlashCalendar();
						 if (status != PROTOCOL_OK)
						 {
							   if (status == PROTOCOL_RX_HW_ERROR)
										PANIC_Handler(PANIC_EVT_XBEE_CONFIG_ERROR);
						 }

						/*
						 *
						 * BLOCK : SHOW STANDARD VIEW
						 *
						 */


						 show_standard_view();
					}

			}
			else if(update_status == CALENDAR_NO_UPDATE)
			{
				   /*
					*
					* --> NO UPDATE
					*
					*/

					ChangeCourseOnStandardView = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1);

					//ChangeCourseOnStandardView is set to 1 during alarm scheduling.
					// If I schedule an alarm before going to sleep to change the current course display, typically 10 minutes before,
					// then it's used to modify the display; otherwise, no action is taken.


					if(ChangeCourseOnStandardView == 0)
					{

					}
					else if(ChangeCourseOnStandardView == 1)
					{
						/*
						 *
						 * BLOCK : GET CALENDAR FROM FLASH
						 *
						 */


						PROTOCOL_GetFlashCalendar();
						if (status != PROTOCOL_OK)
						{
							if (status == PROTOCOL_RX_HW_ERROR)
									PANIC_Handler(PANIC_EVT_XBEE_CONFIG_ERROR);
						}

						HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0);


						/*
						 *
						 * BLOCK : GET CALENDAR FROM FLASH
						 *
						 */


						 PROTOCOL_GetFlashCalendar();
						 if (status != PROTOCOL_OK)
						 {
							   if (status == PROTOCOL_RX_HW_ERROR)
										PANIC_Handler(PANIC_EVT_XBEE_CONFIG_ERROR);
						 }


						/*
						 *
						 * BLOCK : SHOW STANDARD VIEW
						 *
						 */


						 show_standard_view();
					}
			}

			else
			{
				//panic mode ?
			}
		}


		// Mode 1 --> I check if it's the first wake-up of the week. If it's the first wake-up, I update the calendar; otherwise, I parse the calendar from flash memory.


		//This battery-saving mode only wakes up to modify the current display.
		//Therefore, there's no need to inquire whether an update is necessary, as it always is.


		/*
		 *
		 * BLOCK : MODE 1
		 *
	     */


		else if (mode == 1)
		{

				/*
				 *
				 * BLOCK : FIRST WAKE OF THE DAY
				 *
			     */


				if (RTC_isFirstWakeUpOfTheDay() == 1 && RTC_GetWeekDay() == 1)
				{


					/*
					 *
					 * BLOCK : GET CALENDAR FROM XBEE
					 *
					 */


					 HAL_UART_Transmit(&huart2,CalendarXbeeText,sizeof(CalendarXbeeText),10);

				     status = PROTOCOL_GetCalendar(&configuration);

					 if (status != PROTOCOL_OK)
					 {
						 	if (status == PROTOCOL_RX_HW_ERROR)
								PANIC_Handler(PANIC_EVT_XBEE_CONFIG_ERROR);
					 }
				}
				else
				{


					/*
					 *
					 * BLOCK : GET CONFIG FROM FLASH
					 *
				     */


					HAL_UART_Transmit(&huart2,CalendarFlashText,sizeof(CalendarFlashText),10);

					PROTOCOL_GetFlashConfig(&configuration);
					if (status != PROTOCOL_OK)
					{
							if (status == PROTOCOL_RX_HW_ERROR)
									PANIC_Handler(PANIC_EVT_XBEE_CONFIG_ERROR);
					}
				}


		/*
		 *
		 * BLOCK : GET CALENDAR FROM FLASH
		 *
		 */


		 PROTOCOL_GetFlashCalendar();
		 if (status != PROTOCOL_OK)
		 {
			   if (status == PROTOCOL_RX_HW_ERROR)
						PANIC_Handler(PANIC_EVT_XBEE_CONFIG_ERROR);
		 }

		/*
		 *
		 * BLOCK : SHOW STANDARD VIEW
		 *
		 */


		show_standard_view();

		}
		else
		{
			//panic mode ?
		}


		/*
		 *
		 * BLOCK : SET ALARM RTC
		 *
	     */


		next_alarm_rtc();


		/*
		 *
		 * BLOCK : STANDBY MODE
		 *
		 */


		go_sleep();
	}
}

/*
 * ***********************************************************************************************************
 * ***********************************************************************************************************
 */

void clear_flag()
{
	if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET)
	{
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB); // PWR_FLAG_SB: Flag indicating a wake-up event of the STM32 after standby mode.
		HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1); // PWR_WAKEUP_PIN1: Wake-up pin via the button's port.
		HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
	}
}

/*
 * ***********************************************************************************************************
 * ***********************************************************************************************************
 */

void set_flag()
{
	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
	__HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc, RTC_FLAG_WUTF);
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
}

/*
 * ***********************************************************************************************************
 * ***********************************************************************************************************
 */

void go_sleep()
{
	UART_Debug(&alarmHour, &alarmMin, &alarmSec, &hour, &min, &sec);
	HAL_UART_Transmit(&huart2,StandbyModeText,sizeof(StandbyModeText),10);
	set_flag();
	HAL_PWR_EnterSTANDBYMode();
}

/*
 * ***********************************************************************************************************
 * ***********************************************************************************************************
 */

TickType_t msToTicks(TickType_t ms)
{
	TickType_t tmp = ms;

	if (ms<(1000/configTICK_RATE_HZ))
		tmp = 1;
	else {
		tmp = ms/(1000/configTICK_RATE_HZ);

		if (ms % (1000/configTICK_RATE_HZ))
			tmp ++;
	}
	return tmp;
}

/*
 * ***********************************************************************************************************
 * ***********************************************************************************************************
 */

// This function takes into account the need to display a class 10 minutes before its scheduled start time.
// It uses the 'diff' variable for this purpose.

void show_standard_view()
{
	BATTERY_ModeVoltage(&mode);

	mode = 1; //debug
	RTC_CurrentTimeToSec(&timemin);
	diff = CAL_Diff();

	if(diff <= 10)
	{
		if (mode )
		DISPLAY_ShowReservation(&configuration, CAL_GetNextReservation(timemin), CAL_GetTitleNextReservation(timemin), DISPLAY_PromptIconInfo, mode);
		DISPLAY_Update();
	}
	else if(diff > 10)
	{
		DISPLAY_ShowReservation(&configuration, CAL_GetCurrent(timemin), CAL_GetTitleNextReservation(timemin), DISPLAY_PromptIconInfo, mode);
		DISPLAY_Update();
	}
	else
	{
		//panic mode ?
	}

	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, 0); //Back up register
}

/*
 * ***********************************************************************************************************
 * ***********************************************************************************************************
 */

//This simple function sets the next alarm based on the battery mode:
// - Battery Mode 0: Set alarms every 15 or 10 minutes before the start of a class to trigger display changes.
// - Battery Mode 1: No RTC alarms, except 10 minutes before the start of a class to trigger display changes.


void next_alarm_rtc()
{
		BATTERY_ModeVoltage(&mode); //Retrieve battery mode

		diff = CAL_Diff();

		if (mode == 0)
		{
			if(diff >= 25)
			{
				RTC_SetPeriodicAlarm15();
			}
			else if(diff < 25)
			{
				RTC_SetPeriodicAlarm10(diff);
			}
		}
		else if (mode == 1)
		{
			RTC_SetPeriodicAlarm10(diff);
		}
		else
		{

			//panic mode ?
		}
}
