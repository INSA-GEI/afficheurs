/*
 * protocol.h
 *
 *  Created on: 12 avr. 2022
 *      Author: dimercur
 */

#include "stm32l4xx_hal.h"

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

typedef enum {
	PROTOCOL_OK=0,
	PROTOCOL_XBEE_INIT_ERROR,
	PROTOCOL_INVALID_FRAME,
	PROTOCOL_RX_TIMEOUT,
	PROTOCOL_RX_HW_ERROR,
	PROTOCOL_RX_CMD_ERROR,
	PROTOCOL_NOT_CONNECTED
} PROTOCOL_Status;

typedef struct {
	uint64_t device_uid;
	uint8_t rf_channel;
	uint16_t rf_panid;
	uint64_t gw_address;
	uint16_t init_time;
	uint8_t init_day;
	char* room_name;
	char* room_type;
	uint16_t update_starting_time;
	uint16_t update_ending_time;
	uint16_t update_delay;
	uint8_t order_nbr;
	uint16_t last_error;
} PROTOCOL_ConfigurationTypedef;

typedef char PROTOCOL_Day[9];

typedef struct pReservation {
	uint16_t start_time;
	uint16_t end_time;
	uint8_t day_nbr;
	char* title;
	char* trainees[4];
	char* trainers[4];
	struct pReservation* next_reservation;
} PROTOCOL_Reservation;

typedef struct {
	PROTOCOL_Day week[7];
	PROTOCOL_Reservation* first_reservation;
} PROTOCOL_CalendarTypedef;

PROTOCOL_Status PROTOCOL_Init(void);
PROTOCOL_Status PROTOCOL_Connect(PROTOCOL_ConfigurationTypedef* conf);
PROTOCOL_Status PROTOCOL_GetConfiguration(PROTOCOL_ConfigurationTypedef* conf);
PROTOCOL_Status PROTOCOL_GetCalendar(PROTOCOL_CalendarTypedef* calendar);
//PROTOCOL_Status PROTOCOL_GetCalendarUpdateStatus(uint8_t* status);
PROTOCOL_Status PROTOCOL_SendReport(void);

#endif /* PROTOCOL_H_ */
