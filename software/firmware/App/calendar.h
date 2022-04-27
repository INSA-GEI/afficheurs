/*
 * calendar.h
 *
 *  Created on: 26 avr. 2022
 *      Author: dimercur
 */

#ifndef CALENDAR_H_
#define CALENDAR_H_

#include "stm32l4xx_hal.h"
#include "config.h"

#define CAL_DATE_STRING_MAX_LENGTH	10
#define CAL_TITLE_MAX_LENGTH	30
#define CAL_TRAINEE_MAX_LENGTH	14
#define CAL_TRAINER_MAX_LENGTH	22

typedef char CAL_Day[CAL_DATE_STRING_MAX_LENGTH+1];

typedef struct pReservation {
	uint16_t start_time;
	uint16_t end_time;
	uint8_t day_nbr;
	char* title;
	char* trainees[4];
	char* trainers[4];
	struct pReservation* next_reservation;
} CAL_Reservation;

typedef struct {
	CAL_Day week[7];
	CAL_Reservation* first_reservation;
} CAL_CalendarTypedef;

extern const char* CAL_DayName[7];

void CAL_Init(void);
void CAL_Flush(void);
CAL_Day* CAL_GetDayArray(void);
CAL_Reservation* CAL_NewReservation(void);
void CAL_SetTimeAndDate(CAL_Reservation* cal, uint16_t start_time, uint16_t end_time, uint8_t day);
void CAL_SetTitle(CAL_Reservation* cal, const char* title);
void CAL_SetTrainee(CAL_Reservation* cal, const char* trainee);
void CAL_SetTrainer(CAL_Reservation* cal, const char* trainer);
CAL_Reservation* CAL_GetFirst(void);
CAL_Reservation* CAL_GetNext(void);
char* CAL_HourToStr(uint16_t hour);
uint16_t CAL_HourToInt(int hour, int min);
#endif /* CALENDAR_H_ */
