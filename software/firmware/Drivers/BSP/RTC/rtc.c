/*
 * rtc.c
 *
 *  Created on: 29 avr. 2022
 *      Author: dimercur
 */

#include <panic.h>
#include <rtc.h>
#include <stddef.h>
#include <stm32l432xx.h>
#include <stm32l4xx_hal_cortex.h>
#include <stm32l4xx_hal_def.h>
#include <stm32l4xx_hal_rcc.h>
#include <stm32l4xx_hal_rcc_ex.h>
#include <stm32l4xx_hal_rtc.h>
#include <sys/_stdint.h>

RTC_HandleTypeDef hrtc;
static RTC_AlarmCallback alarm_callback= {0};

static uint8_t dec2bcd(uint8_t num)
{
	uint8_t ones = 0;
	uint8_t tens = 0;
	uint8_t temp = 0;

	if (num > 99)
		num = 99;

	ones = num%10;
	temp = num/10;
	tens = temp<<4;
	return (tens + ones);
}

static uint8_t bcd2dec(uint8_t num)
{
	uint8_t ones = 0;
	uint8_t tens = 0;

	ones = num & 0x0F;
	tens = num & 0xF0;
	tens = (tens>>4)*10;
	return (tens + ones);
}

/**
 * @brief RTC Initialization Function
 * @param None
 * @retval None
 */
RTC_Status RTC_Init(void) {
	RTC_Status status = RTC_OK;

	/** Initialize RTC Only
	 */
	hrtc.Instance = RTC;
	hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
	hrtc.Init.AsynchPrediv = 127;
	hrtc.Init.SynchPrediv = 255;
	hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
	hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
	hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	if (HAL_RTC_Init(&hrtc) != HAL_OK)
		status= RTC_HW_Error;

	return status;
}

/** Set the Time
 */
RTC_Status RTC_SetTime(uint8_t hour, uint8_t min, uint8_t sec) {
	RTC_TimeTypeDef sTime = {0};

	if (hour<24)
		//sTime.Hours = dec2bcd(hour);
		sTime.Hours = hour;
	else
		return RTC_Invalid_Parameter;

	if (min<60)
		//sTime.Minutes = dec2bcd(min);
		sTime.Minutes = min;
	else
		return RTC_Invalid_Parameter;

	if (sec<60)
		//sTime.Seconds = dec2bcd(sec);
		sTime.Seconds = sec;
	else
		return RTC_Invalid_Parameter;

	sTime.TimeFormat = RTC_HOURFORMAT_24;
	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
		return RTC_HW_Error;

	return RTC_OK;
}

/** Get current Time
 */
RTC_Status RTC_GetTime(uint8_t *hour, uint8_t *min, uint8_t *sec) {
	RTC_TimeTypeDef sTime = {0};

	if (HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN)!= HAL_OK)
		return RTC_HW_Error;

	*hour = sTime.Hours;
	*min = sTime.Minutes;
	*sec = sTime.Seconds;

	return RTC_OK;
}

/** Get current Time
 */
RTC_Status RTC_GetWeekDay(uint8_t *weekday) {
	RTC_DateTypeDef sDate = {0};

	if (HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN)!= HAL_OK)
		return RTC_HW_Error;

	*weekday = sDate.WeekDay;

	return RTC_OK;
}

/** Set the Date
 */
RTC_Status RTC_SetDate(uint8_t weekday, uint8_t day, uint8_t month, uint8_t year) {
	RTC_DateTypeDef sDate = {0};

	if ((weekday>=RTC_WEEKDAY_MONDAY) && (weekday<=RTC_WEEKDAY_SUNDAY))
		sDate.WeekDay = weekday;
	else
		return RTC_Invalid_Parameter;

	if ((month>=1) && (month<=12))
		//sDate.Month = dec2bcd(month);
		sDate.Month = month;
	else
		return RTC_Invalid_Parameter;

	if ((day>=1) && (day<=31))
		//sDate.Date = dec2bcd(day);
		sDate.Date = day;
	else
		return RTC_Invalid_Parameter;

	//sDate.Year = dec2bcd(year);
	sDate.Year = year;

	if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
		return RTC_HW_Error;

	return RTC_OK;
}

void RTC_SetAlarmCallback(RTC_AlarmCallback callback) {
	alarm_callback = callback;
}

/** Program a futur event (in minute)
 */
RTC_Status RTC_EnableWeekStartEvent (uint8_t order_nbr) {
	RTC_AlarmTypeDef sAlarm = {0};
	uint8_t hours =0;
	uint8_t minutes =0;
	uint8_t sec =0;

	sAlarm.AlarmTime.SubSeconds = 0x0;
	sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
	sAlarm.AlarmTime.TimeFormat = RTC_HOURFORMAT_24;

	sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
	sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
	sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY;
	sAlarm.Alarm = RTC_ALARM_B;

	sec = order_nbr*10; // spread calendar request every 10 seconds
	minutes = sec/60;
	hours = 1+ minutes/60; // start of calendar update is 1:00, on monday

	sec %= 60;
	minutes %=60;
	hours %=24;

	sAlarm.AlarmDateWeekDay = RTC_WEEKDAY_MONDAY;
//	sAlarm.AlarmTime.Hours = dec2bcd(hours);
//	sAlarm.AlarmTime.Minutes = dec2bcd(minutes);
//	sAlarm.AlarmTime.Seconds = dec2bcd(sec);

	sAlarm.AlarmTime.Hours = hours;
	sAlarm.AlarmTime.Minutes = minutes;
	sAlarm.AlarmTime.Seconds = sec;

	if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
		return RTC_HW_Error;

	return RTC_OK;
}


/** Program a futur event (in minute)
 */
RTC_Status RTC_SetNextEvent (uint16_t min) {
	RTC_DateTypeDef sDate = {0};
	RTC_TimeTypeDef sTime = {0};
	RTC_AlarmTypeDef sAlarm = {0};
	uint8_t hours =0;
	uint8_t minutes =0;

	if (HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN)!= HAL_OK)
		return RTC_HW_Error;

	if (HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN)!= HAL_OK)
		return RTC_HW_Error;

	sAlarm.AlarmTime.Seconds = 0x0;
	sAlarm.AlarmTime.SubSeconds = 0x0;
	sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
	sAlarm.AlarmTime.TimeFormat = RTC_HOURFORMAT_24;

	sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
	sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
	sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY;
	sAlarm.AlarmDateWeekDay = sDate.WeekDay;
	sAlarm.Alarm = RTC_ALARM_A;

	hours = min/60;
	minutes = min - ((uint16_t)hours)*60;

//	hours += bcd2dec(sTime.Hours);
//	minutes += bcd2dec(sTime.Minutes);

	hours += sTime.Hours;
	minutes += sTime.Minutes;

	if (minutes >=60 ) {
		hours++;
		minutes-=60;
	}

	if (hours >=24 )
		hours-=24;

//	sAlarm.AlarmTime.Hours = dec2bcd(hours);
//	sAlarm.AlarmTime.Minutes = dec2bcd(minutes);

	sAlarm.AlarmTime.Hours = hours;
	sAlarm.AlarmTime.Minutes = minutes;

	if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
		return RTC_HW_Error;

	return RTC_OK;
}

RTC_Status RTC_StopEvent (void) {

	if (HAL_RTC_DeactivateAlarm(&hrtc, RTC_ALARM_A)!= HAL_OK)
		return RTC_HW_Error;

	return RTC_OK;
}

uint32_t RTC_UnitTests(void) {
	uint32_t status=0;
	RTC_AlarmTypeDef sAlarm = {0};

	// test dec2bcd
	if (dec2bcd(1) != 1)
		status |= 0x1;

	if (dec2bcd(10) != 0x10)
		status |= 0x2;

	if (dec2bcd(99) != 0x99)
		status |= 0x4;

	if (dec2bcd(100) != 0x99)
		status |= 0x8;

	// test dec2bcd
	if (bcd2dec(0x01) != 1)
		status |= 0x10;

	if (bcd2dec(0x12) != 12)
		status |= 0x20;

	if (bcd2dec(0x99) != 99)
		status |= 0x40;

	/* Test setSetNextEvent */
	RTC_SetTime(0, 0, 0);
	RTC_SetNextEvent (1);
	HAL_RTC_GetAlarm(&hrtc, &sAlarm, RTC_ALARM_A, RTC_FORMAT_BIN);

	if ((sAlarm.AlarmTime.Hours != 0) ||
			(sAlarm.AlarmTime.Minutes != 1) ||
			(sAlarm.AlarmTime.Seconds != 0) ||
			(sAlarm.AlarmMask != RTC_ALARMMASK_DATEWEEKDAY))
		status |= 0x100;

	RTC_SetTime(0, 59, 0);
	RTC_SetNextEvent (1);
	HAL_RTC_GetAlarm(&hrtc, &sAlarm, RTC_ALARM_A, RTC_FORMAT_BIN);

	if ((sAlarm.AlarmTime.Hours != 1) ||
			(sAlarm.AlarmTime.Minutes != 0) ||
			(sAlarm.AlarmTime.Seconds != 0) ||
			(sAlarm.AlarmMask != RTC_ALARMMASK_DATEWEEKDAY))
		status |= 0x200;

	RTC_SetTime(23, 59, 0);
	RTC_SetNextEvent (1);
	HAL_RTC_GetAlarm(&hrtc, &sAlarm, RTC_ALARM_A, RTC_FORMAT_BIN);

	if ((sAlarm.AlarmTime.Hours != 0) ||
			(sAlarm.AlarmTime.Minutes != 0) ||
			(sAlarm.AlarmTime.Seconds != 0) ||
			(sAlarm.AlarmMask != RTC_ALARMMASK_DATEWEEKDAY))
		status |= 0x400;

	RTC_SetTime(0, 0, 0);
	RTC_SetNextEvent (60);
	HAL_RTC_GetAlarm(&hrtc, &sAlarm, RTC_ALARM_A, RTC_FORMAT_BIN);

	if ((sAlarm.AlarmTime.Hours != 1) ||
			(sAlarm.AlarmTime.Minutes != 0) ||
			(sAlarm.AlarmTime.Seconds != 0) ||
			(sAlarm.AlarmMask != RTC_ALARMMASK_DATEWEEKDAY))
		status |= 0x800;

	RTC_SetTime(0, 0, 0);
	RTC_SetNextEvent (24*60-1);
	HAL_RTC_GetAlarm(&hrtc, &sAlarm, RTC_ALARM_A, RTC_FORMAT_BIN);

	if ((sAlarm.AlarmTime.Hours != 23) ||
			(sAlarm.AlarmTime.Minutes != 59) ||
			(sAlarm.AlarmTime.Seconds != 0) ||
			(sAlarm.AlarmMask != RTC_ALARMMASK_DATEWEEKDAY))
		status |= 0x1000;

	RTC_SetTime(23, 59, 0);
	RTC_SetNextEvent (24*60-1);
	HAL_RTC_GetAlarm(&hrtc, &sAlarm, RTC_ALARM_A, RTC_FORMAT_BIN);

	if ((sAlarm.AlarmTime.Hours != 23) ||
			(sAlarm.AlarmTime.Minutes != 58) ||
			(sAlarm.AlarmTime.Seconds != 0) ||
			(sAlarm.AlarmMask != RTC_ALARMMASK_DATEWEEKDAY))
		status |= 0x2000;

	RTC_StopEvent();

	RTC_EnableWeekStartEvent(0);
	HAL_RTC_GetAlarm(&hrtc, &sAlarm, RTC_ALARM_B, RTC_FORMAT_BIN);
	if ((sAlarm.AlarmTime.Hours != 1) ||
			(sAlarm.AlarmTime.Minutes != 0) ||
			(sAlarm.AlarmTime.Seconds != 0) ||
			(sAlarm.AlarmMask != RTC_ALARMMASK_NONE) ||
			(sAlarm.AlarmDateWeekDay != 1)  ||
			(sAlarm.AlarmDateWeekDaySel != RTC_ALARMDATEWEEKDAYSEL_WEEKDAY))
		status |= 0x10000;

	RTC_EnableWeekStartEvent(1);
	HAL_RTC_GetAlarm(&hrtc, &sAlarm, RTC_ALARM_B, RTC_FORMAT_BIN);
	if ((sAlarm.AlarmTime.Hours != 1) ||
			(sAlarm.AlarmTime.Minutes != 0) ||
			(sAlarm.AlarmTime.Seconds != 10) ||
			(sAlarm.AlarmMask != RTC_ALARMMASK_NONE)  ||
			(sAlarm.AlarmDateWeekDay != 1)  ||
			(sAlarm.AlarmDateWeekDaySel != RTC_ALARMDATEWEEKDAYSEL_WEEKDAY))
		status |= 0x20000;

	RTC_EnableWeekStartEvent(6);
	HAL_RTC_GetAlarm(&hrtc, &sAlarm, RTC_ALARM_B, RTC_FORMAT_BIN);
	if ((sAlarm.AlarmTime.Hours != 1) ||
			(sAlarm.AlarmTime.Minutes != 1) ||
			(sAlarm.AlarmTime.Seconds != 0) ||
			(sAlarm.AlarmMask != RTC_ALARMMASK_NONE)  ||
			(sAlarm.AlarmDateWeekDay != 1)  ||
			(sAlarm.AlarmDateWeekDaySel != RTC_ALARMDATEWEEKDAYSEL_WEEKDAY))
		status |= 0x40000;

	return status;
}

/**
 * @brief RTC MSP Initialization
 * This function configures the hardware resources used in this example
 * @param hrtc: RTC handle pointer
 * @retval None
 */
void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc) {
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

	if(hrtc->Instance==RTC) {
		/** Initializes the peripherals clock
		 */
		PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
		PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
			PANIC_Handler(PANIC_EVT_RTC_MSP_CONFIG_ERROR);

		/* Peripheral clock enable */
		__HAL_RCC_RTC_ENABLE();
		/* RTC interrupt Init */
		HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 5, 0);
		HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
	}
}

/**
 * @brief RTC MSP De-Initialization
 * This function freeze the hardware resources used in this example
 * @param hrtc: RTC handle pointer
 * @retval None
 */
void HAL_RTC_MspDeInit(RTC_HandleTypeDef* hrtc) {
	if(hrtc->Instance==RTC) {
		/* Peripheral clock disable */
		__HAL_RCC_RTC_DISABLE();

		/* RTC interrupt DeInit */
		HAL_NVIC_DisableIRQ(RTC_Alarm_IRQn);
	}
}

/**
 * @brief This function handles RTC alarm interrupt through EXTI line 18.
 */
void RTC_Alarm_IRQHandler(void) {
	HAL_RTC_AlarmIRQHandler(&hrtc);
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc) {
	if (alarm_callback!=NULL)
		alarm_callback(RTC_AlarmEvent_OtherEvent);
}

void HAL_RTCEx_AlarmBEventCallback(RTC_HandleTypeDef *hrtc) {
	if (alarm_callback!=NULL)
		alarm_callback(RTC_AlarmEvent_WeekStart);
}
