/*
 * rtc.h
 *
 *  Created on: 29 avr. 2022
 *      Author: dimercur
 */

#ifndef BSP_RTC_RTC_H_
#define BSP_RTC_RTC_H_

#include "stm32l4xx_hal.h"
#include "config.h"

typedef enum {
	RTC_OK=0,
	RTC_HW_Error,
	RTC_Invalid_Parameter
} RTC_Status;

typedef enum {
	RTC_AlarmEvent_WeekStart=1,
	RTC_AlarmEvent_OtherEvent
} RTC_AlarmEvent;

typedef void (*RTC_AlarmCallback)(RTC_AlarmEvent event);

RTC_Status RTC_Init(void);

/** Set the Time
 */
RTC_Status RTC_SetTime(uint8_t hour, uint8_t min, uint8_t sec) ;

/** Get current Time
 */
RTC_Status RTC_GetTime(uint8_t *hour, uint8_t *min, uint8_t *sec);

/** Get current Time
 */
RTC_Status RTC_GetWeekDay(uint8_t *weekday);

/** Set the Date
 */
RTC_Status RTC_SetDate(uint8_t weekday, uint8_t day, uint8_t month, uint8_t year);

void RTC_SetAlarmCallback(RTC_AlarmCallback callback);
/** Program a futur event (in minute)
 */
RTC_Status RTC_SetNextEvent (uint16_t min);

RTC_Status RTC_StopEvent (void);

RTC_Status RTC_EnableWeekStartEvent (uint8_t order_nbr);

uint32_t RTC_UnitTests(void);
#endif /* BSP_RTC_RTC_H_ */
