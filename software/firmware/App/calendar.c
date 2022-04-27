/*
 * calendar.c
 *
 *  Created on: 26 avr. 2022
 *      Author: dimercur
 */

#include "calendar.h"
#include "stdlib.h"
#include "string.h"

static CAL_CalendarTypedef calendar;
static CAL_Reservation* currentreservationptr=NULL;
static char hour_to_str_buffer[6];

const char* CAL_DayName[7] = {
		"Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi", "Samedi", "Dimanche"
};

void CAL_Init(void) {
	memset (&calendar, 0, sizeof(CAL_CalendarTypedef));
	currentreservationptr=NULL;
}

void CAL_Flush(void) {
	CAL_Reservation* indexreservationptr=calendar.first_reservation;
	CAL_Reservation* nextreservationptr=NULL;
	int i;

	if (indexreservationptr!=NULL) {
		if (indexreservationptr->title!=NULL)
			free (indexreservationptr->title);

		for (i=0; i<4; i++) {
			if (indexreservationptr->trainees[i]!= NULL)
				free (indexreservationptr->trainees[i]);

			if (indexreservationptr->trainers[i]!= NULL)
				free (indexreservationptr->trainers[i]);
		}

		nextreservationptr = indexreservationptr->next_reservation;
		free(indexreservationptr);
		indexreservationptr = nextreservationptr;
	}

	CAL_Init();
}

CAL_Day* CAL_GetDayArray(void) {
	return (calendar.week);
}

CAL_Reservation* CAL_NewReservation(void) {
	CAL_Reservation* indexreservationptr=NULL;

	if (calendar.first_reservation == NULL) {
		calendar.first_reservation = (CAL_Reservation*)malloc(sizeof(CAL_Reservation));
		indexreservationptr =calendar.first_reservation;
	} else {
		indexreservationptr =calendar.first_reservation;

		while (indexreservationptr->next_reservation != NULL)
			indexreservationptr = indexreservationptr->next_reservation;

		indexreservationptr->next_reservation = (CAL_Reservation*)malloc(sizeof(CAL_Reservation));
		indexreservationptr = indexreservationptr->next_reservation;
	}

	memset(indexreservationptr, 0, sizeof(CAL_Reservation)); // init all field to '0'

	return indexreservationptr;
}

CAL_Reservation* CAL_GetFirst(void) {
	currentreservationptr=calendar.first_reservation;
	return currentreservationptr;
}

CAL_Reservation* CAL_GetNext(void) {
	if (currentreservationptr != NULL)
		currentreservationptr = currentreservationptr->next_reservation;

	return currentreservationptr;
}

char* CAL_HourToStr(uint16_t hour) {
	memset(hour_to_str_buffer,0, 6);
	uint8_t h = hour/60;
	uint8_t m = hour - h*60;

	itoa(h,hour_to_str_buffer,10);

	if (hour_to_str_buffer[1]!=0) {
		hour_to_str_buffer[2]=':';

		if (m<10) {
			hour_to_str_buffer[3]='0';
			hour_to_str_buffer[4]='0'+m;
		} else
			itoa(m,&hour_to_str_buffer[3],10);
	} else {
		hour_to_str_buffer[1]=':';

		if (m<10) {
			hour_to_str_buffer[2]='0';
			hour_to_str_buffer[3]='0'+m;
		} else
			itoa(m,&hour_to_str_buffer[2],10);
	}

	return hour_to_str_buffer;
}

uint16_t CAL_HourToInt(int hour, int min) {
	return (hour*60)+min;
}
