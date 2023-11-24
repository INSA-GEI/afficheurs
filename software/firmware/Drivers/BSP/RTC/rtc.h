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

RTC_Status RTC_Init(void);
RTC_Status RTC_SetTime(uint8_t hour, uint8_t min, uint8_t sec) ;
RTC_Status RTC_GetTime(uint8_t *hour, uint8_t *min, uint8_t *sec);
RTC_Status RTC_SetDate(uint8_t weekday, uint8_t day, uint8_t month, uint8_t year);
RTC_AlarmEvent RTC_WaitForEvent(void);
RTC_Status RTC_SetNextEvent(uint16_t min);
RTC_Status RTC_StopEvent(void);
RTC_Status RTC_EnableWeekStartEvent(uint8_t order_nbr);
RTC_Status RTC_SetAlarm(uint8_t hour, uint8_t min, uint8_t sec);

uint8_t RTC_GetLastPrintDisplay();
uint32_t RTC_UnitTests(void);
RTC_Status RTC_GetAlarm(uint8_t *hour, uint8_t *min, uint8_t *sec);
RTC_Status RTC_CurrentTimeToSec(uint32_t *sec);


uint8_t RTC_isWakeUpMode2();
uint8_t RTC_isWakeUpMode3();

RTC_Status RTC_SetPeriodicAlarm45();
RTC_Status RTC_SetPeriodicAlarm10(uint32_t diff);
RTC_Status RTC_SetPeriodicAlarm1();


/* ------*/
uint8_t RTC_GetWeekDay();
uint8_t RTC_isFirstWakeUpOfTheDay();



//USEFULL
RTC_Status UART_Debug(uint8_t *hourAlarm, uint8_t *minAlarm, uint8_t *secAlarm, uint8_t *hourRTC, uint8_t *minRTC, uint8_t *secRTC);
RTC_Status RTC_SetFirstAlarmOfTheDay();
RTC_Status RTC_SetPeriodicAlarm15();
uint8_t RTC_CheckWakeUpNight();
uint8_t RTC_CheckWakeUpWeekEnd();

#endif /* BSP_RTC_RTC_H_ */
