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
	PROTOCOL_TX_ERROR,
	PROTOCOL_NOT_CONNECTED
} PROTOCOL_Status;

typedef struct {
	uint8_t rssi_val[32];
	uint8_t index;
	uint8_t nb_items;
	uint8_t no_ack;
} PROTOCOL_RSSI;

#define PROTOCOL_ROOM_NAME_LENGTH	8
#define PROTOCOL_ROOM_TYPE_LENGTH	22
#define PROTOCOL_DATE_STRING_MAX_LENGTH	8
#define PROTOCOL_TITLE_MAX_LENGTH	30
#define PROTOCOL_TRAINEE_MAX_LENGTH	10
#define PROTOCOL_TRAINER_MAX_LENGTH	18

typedef struct {
	uint64_t device_uid;
	uint8_t rf_channel;
	uint16_t rf_panid;
	uint64_t gw_address;
	uint32_t init_time;
	uint8_t init_day;
	char room_name[PROTOCOL_ROOM_NAME_LENGTH+1]; // room name length + space for '0' ending
	char room_type[PROTOCOL_ROOM_TYPE_LENGTH+1]; // room type length + space for '0' ending
	uint16_t update_starting_time;
	uint16_t update_ending_time;
	uint16_t update_delay;
	uint16_t last_error;
	uint8_t order_nbr;
	uint8_t battery_level;
	PROTOCOL_RSSI rssi;
} PROTOCOL_ConfigurationTypedef;

typedef char PROTOCOL_Day[PROTOCOL_DATE_STRING_MAX_LENGTH+1];

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
PROTOCOL_Status PROTOCOL_GetCalendar(PROTOCOL_ConfigurationTypedef* conf, PROTOCOL_CalendarTypedef* calendar);
PROTOCOL_Status PROTOCOL_GetCalendarUpdateStatus(PROTOCOL_ConfigurationTypedef* conf, uint8_t* update_status);
PROTOCOL_Status PROTOCOL_SendReport(PROTOCOL_ConfigurationTypedef* conf);

#endif /* PROTOCOL_H_ */
