/*
 * rtc.c
 *
 *  Created on: 29 avr. 2022
 *      Author: dimercur
 */

#include "stm32l4xx_hal.h"
#include "rtc.h"
#include "panic.h"

RTC_HandleTypeDef hrtc;
static RTC_AlarmCallback alarm_callback= {0};

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
		sTime.Hours = hour;
	else
		return RTC_Invalid_Parameter;

	if (min<60)
		sTime.Minutes = min;
	else
		return RTC_Invalid_Parameter;

	if (sec<60)
		sTime.Seconds = sec;
	else
		return RTC_Invalid_Parameter;

	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
		return RTC_HW_Error;

	return RTC_OK;
}

/** Get current Time
 */
RTC_Status RTC_GetTime(uint8_t *hour, uint8_t *min, uint8_t *sec) {
	RTC_TimeTypeDef sTime = {0};

	if (HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BCD)!= HAL_OK)
		return RTC_HW_Error;

	*hour = sTime.Hours;
	*min =sTime.Minutes;
	*sec = sTime.Seconds;

	return RTC_OK;
}

/** Get current Time
 */
RTC_Status RTC_GetWeekDay(uint8_t *weekday) {
	RTC_DateTypeDef sDate = {0};

	if (HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BCD)!= HAL_OK)
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

	if ((month>=RTC_MONTH_JANUARY) && (month<=RTC_MONTH_DECEMBER))
		sDate.Month = month;
	else
		return RTC_Invalid_Parameter;

	if ((day>=1) && (day<=31))
		sDate.Date = day;
	else
		return RTC_Invalid_Parameter;

	sDate.Year = year;

	if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
		return RTC_HW_Error;

	return RTC_OK;
}

void RTC_SetAlarmCallback(RTC_AlarmCallback callback){
	alarm_callback = callback;
}

/** Program a futur event (in minute)
 */
RTC_Status RTC_SetNextEvent (uint16_t min) {
	RTC_DateTypeDef sDate = {0};
	RTC_TimeTypeDef sTime = {0};
	RTC_AlarmTypeDef sAlarm = {0};
	uint8_t hours =0;
	uint8_t minutes =0;

	hours = min/60;
	minutes = min - ((uint16_t)hours)*60;

	if (HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BCD)!= HAL_OK)
		return RTC_HW_Error;

	if (HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BCD)!= HAL_OK)
		return RTC_HW_Error;

	sAlarm.AlarmTime.Seconds = 0x0;
	sAlarm.AlarmTime.SubSeconds = 0x0;
	sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
	sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
	sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
	sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
	sAlarm.AlarmDateWeekDay = sDate.WeekDay;
	sAlarm.Alarm = RTC_ALARM_A;

	sAlarm.AlarmTime.Minutes += minutes;
	if (sAlarm.AlarmTime.Minutes >=60 ) {
		hours++;
		sAlarm.AlarmTime.Minutes-=60;
	}

	sAlarm.AlarmTime.Hours += hours;
	if (sAlarm.AlarmTime.Hours >=24 ) {
		sAlarm.AlarmTime.Hours-=24;
	}

	if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK)
		return RTC_HW_Error;

	return RTC_OK;
}

RTC_Status RTC_StopEvent (void) {

	if (HAL_RTC_DeactivateAlarm(&hrtc, RTC_ALARM_A)!= HAL_OK)
		return RTC_HW_Error;

	return RTC_OK;
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

void HAL_RTCEx_AlarmBEventCallback(RTC_HandleTypeDef *hrtc) {
	if (alarm_callback!=NULL)
		alarm_callback(RTC_ALARM_B);
}
