/*
 * protocol.c
 *
 *  Created on: 12 avr. 2022
 *      Author: dimercur
 */

#include "protocol.h"
#include "xbee.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define PROTOCOL_CMD_JOIN			"JOIN"
#define PROTOCOL_CMD_SETUP			"SETUP"
#define PROTOCOL_CMD_GET_CALENDAR	"CAL"
#define PROTOCOL_CMD_GET_UPDATE		"UPDATE"
#define PROTOCOL_CMD_REPORT			"REPORT"

#define PROTOCOL_ANS_OK				"OK"
#define PROTOCOL_ANS_ERR			"ERR"
#define PROTOCOL_ANS_REJECT			"REJECT"
#define PROTOCOL_ANS_ACCEPT			"ACCEPT"
#define PROTOCOL_ANS_END			"END"

#define PROTOCOL_SEPARATOR			"|"

#define PROTOCOL_CONNECT_TIMEOUT	200 // step channel every 200 ms in case of no answer

#define DEBUG_PROTOCOL				2
#define DEBUG_CHANNEL				0x12
#define DEBUG_PANID					0x1337
#define DEBUG_PROTOCOL_TIMEOUT	    2000 // Timeout of around 2s

PROTOCOL_Status PROTOCOL_Init(void) {

	/* First init XBEE */
	if (XBEE_Init() != XBEE_OK)
		return PROTOCOL_XBEE_INIT_ERROR;

	return PROTOCOL_OK;
}

/*
 * PROTOCOL_Connect
 *
 * Scan each channel and broadcast JOIN command every 200 ms
 * Return PROTOCOL_OK if an ACCEPT answer is received. panid and channel field are updated in conf struct
 * Return PROTOCOL_NOT_CONNECTED if no answer or REJECT answers are received on all channels
 * Return PROTOCOL_RX_HW_ERROR in case of an hardware failure
 */
#if DEBUG_PROTOCOL!=0
PROTOCOL_Status PROTOCOL_Connect(PROTOCOL_ConfigurationTypedef* conf) {
	int status=PROTOCOL_OK;

	if (XBEE_SetChannel(DEBUG_CHANNEL) != XBEE_OK) {
		status= PROTOCOL_RX_HW_ERROR;
	}

	if (XBEE_SetPanID(DEBUG_PANID) != XBEE_OK) {
		status= PROTOCOL_RX_HW_ERROR;
	}

	HAL_Delay(50);

	conf->rf_channel = 0x12;
	conf->rf_panid = 0x1337;
	conf->gw_address = 0x13A20041C16E2B;

	return status;
}
#else
PROTOCOL_Status PROTOCOL_Connect(PROTOCOL_ConfigurationTypedef* conf) {
	unsigned long int panid=0;
	FlagStatus server_found=RESET;
	PROTOCOL_Status status=PROTOCOL_OK;
	int com_status;
	uint8_t transmit_status;
	XBEE_GENERIC_FRAME *rx_frame;

	char* data;
	uint64_t gw;

	for (int channel=XBEE_CHANNEL_FIRST; channel<=XBEE_CHANNEL_LAST; channel++) {
		/* Configure channel*/
		if (XBEE_SetChannel(channel) != XBEE_OK) {
			status= PROTOCOL_RX_HW_ERROR;
			break;
		}

		HAL_Delay(50);

		/* Send a broadcast "JOIN" command */
		if (XBEE_SendData(XBEE_BROADCAST_ADDRESS, 1, XBEE_PANID_BROADCAST, PROTOCOL_CMD_JOIN, &transmit_status)!= XBEE_OK) {
			status= PROTOCOL_RX_HW_ERROR;
			break;
		}

		/* Wait for ACCEPT, REJECT or no answer */
		//com_status = XBEE_GetData(&rx_frame, PROTOCOL_CONNECT_TIMEOUT);
		com_status = XBEE_GetData(&rx_frame, 2000);
		if (com_status == XBEE_RX_ERROR) {
			status= PROTOCOL_RX_HW_ERROR;
			break;
		}

		if ((com_status != XBEE_INVALID_FRAME) && (com_status != XBEE_RX_TIMEOUT)) {
			// a frame has been received, check its content
			if ((rx_frame->type == XBEE_RX_PACKET_TYPE) || (rx_frame->type == XBEE_RX_EXPLICIT_TYPE)) {
				data = ((XBEE_RX_PACKET_FRAME*)rx_frame)->data;
				gw = ((XBEE_RX_PACKET_FRAME*)rx_frame)->source_addr;

				if (strstr((const char*)data, PROTOCOL_ANS_ACCEPT)!= NULL) {
					/* retrieve panid */
					char *ptr = strtok(data, PROTOCOL_SEPARATOR); // ptr is on first word
					ptr = strtok(NULL, PROTOCOL_SEPARATOR);

					if (ptr!=NULL) {
						// should have found panid
						server_found = SET;
						panid = strtoul(ptr, NULL, 16);
						conf->gw_address = gw;
						conf->rf_channel = channel;
						conf->rf_panid = (uint16_t)panid;

						free (rx_frame);

						XBEE_SetPanID(conf->rf_panid);
						HAL_Delay(50);
						break;
					}
				}
			}

			free (rx_frame);
		}
	}

	if (server_found != SET)
		status = PROTOCOL_NOT_CONNECTED;

	return status;
}
#endif /* #if DEBUG_PROTOCOL!=1 */

#define PROTOCOL_TIME_STRING_MAX_LENGTH	8

static PROTOCOL_Status PROTOCOL_ParseConfiguration(PROTOCOL_ConfigurationTypedef* conf, char* ptr) {
	PROTOCOL_Status status=PROTOCOL_OK;

	char currenttime[PROTOCOL_TIME_STRING_MAX_LENGTH+1];
	char starttime[PROTOCOL_TIME_STRING_MAX_LENGTH+1];
	char endtime[PROTOCOL_TIME_STRING_MAX_LENGTH+1];
	char* parsePtr;

	ptr = strtok(NULL, PROTOCOL_SEPARATOR); // ptr should be on current time (in seconds)
	if (ptr == NULL)
		return PROTOCOL_INVALID_FRAME;
	else
		// parse time of the frame
		strncpy(currenttime, ptr, PROTOCOL_TIME_STRING_MAX_LENGTH); // Copy time string in a separate buffer for parsing

	ptr = strtok(NULL, PROTOCOL_SEPARATOR); // ptr should be on current day
	if (ptr == NULL)
		return PROTOCOL_INVALID_FRAME;
	else
		conf->init_day=(uint8_t)strtoul(ptr,NULL,10);

	ptr = strtok(NULL, PROTOCOL_SEPARATOR); // ptr should be on room name
	if (ptr == NULL)
		return PROTOCOL_INVALID_FRAME;
	else {
		strncpy(conf->room_name, ptr, PROTOCOL_ROOM_NAME_LENGTH);
	}

	ptr = strtok(NULL, PROTOCOL_SEPARATOR); // ptr should be on room type
	if (ptr == NULL)
		return PROTOCOL_INVALID_FRAME;
	else {
		strncpy(conf->room_type, ptr, PROTOCOL_ROOM_TYPE_LENGTH);
	}

	ptr = strtok(NULL, PROTOCOL_SEPARATOR); // ptr should be on start hour (in min)
	if (ptr == NULL)
		return PROTOCOL_INVALID_FRAME;
	else
		strncpy(starttime, ptr, PROTOCOL_TIME_STRING_MAX_LENGTH);

	ptr = strtok(NULL, PROTOCOL_SEPARATOR); // ptr should be on end hour (in min)
	if (ptr == NULL)
		return PROTOCOL_INVALID_FRAME;
	else
		strncpy(endtime, ptr, PROTOCOL_TIME_STRING_MAX_LENGTH);

	ptr = strtok(NULL, PROTOCOL_SEPARATOR); // ptr should be on update delay (in min)
	if (ptr == NULL)
		return PROTOCOL_INVALID_FRAME;
	else
		conf->update_delay=(uint16_t)strtoul(ptr,NULL,10);

	ptr = strtok(NULL, PROTOCOL_SEPARATOR); // ptr should be on order number
	if (ptr == NULL)
		return PROTOCOL_INVALID_FRAME;
	else
		conf->order_nbr=(uint8_t)strtoul(ptr,NULL,10);

	/* finally, parse time string */
	// Current time
	parsePtr = strtok(currenttime, ":"); // hour part of the time
	conf->init_time=(uint32_t)(strtoul(parsePtr,NULL,10)*60*60);
	parsePtr = strtok(NULL, ":"); // min part of the time
	conf->init_time+=(uint32_t)(strtoul(parsePtr,NULL,10)*60);
	parsePtr = strtok(NULL, ":"); // second part of the time
	conf->init_time+=(uint32_t)strtoul(parsePtr,NULL,10);

	// Current time
	parsePtr = strtok(starttime, ":"); // hour part of the time
	conf->update_starting_time=(uint16_t)(strtoul(parsePtr,NULL,10)*60);
	parsePtr = strtok(NULL, ":"); // min part of the time
	conf->update_starting_time+=(uint16_t)strtoul(parsePtr,NULL,10);

	// Current time
	parsePtr = strtok(endtime, ":"); // hour part of the time
	conf->update_ending_time=(uint16_t)(strtoul(parsePtr,NULL,10)*60);
	parsePtr = strtok(NULL, ":"); // min part of the time
	conf->update_ending_time+=(uint16_t)strtoul(parsePtr,NULL,10);

	return status;
}

PROTOCOL_Status PROTOCOL_GetConfiguration(PROTOCOL_ConfigurationTypedef* conf) {
	int com_status;
	PROTOCOL_Status status=PROTOCOL_OK;
	XBEE_GENERIC_FRAME *rx_frame;
	uint8_t transmit_status;
	char* data;
	char *ptr;

	/* Send a "SETUP" command */
	if (XBEE_SendData(conf->gw_address, 1, XBEE_NO_PANID_BROADCAST, PROTOCOL_CMD_SETUP, &transmit_status)!= XBEE_OK) {
		status= PROTOCOL_RX_HW_ERROR;
	}

	if (transmit_status !=XBEE_TX_STATUS_SUCCESS ) {
		// here we should increment no ack counter for report
		// not good to have either no ack or CCA failure
		conf->rssi.no_ack++;
	}

	// Wait for answer
	/* Wait for OK, ERR or no answer */
#if DEBUG_PROTOCOL != 0
	com_status = XBEE_GetData(&rx_frame, DEBUG_PROTOCOL_TIMEOUT);
#else
	com_status = XBEE_GetData(&rx_frame, PROTOCOL_CONNECT_TIMEOUT);
#endif /* #if DEBUG_PROTOCOL==1 */

	if (com_status == XBEE_RX_ERROR) {
		status= PROTOCOL_RX_HW_ERROR;
	} else if (com_status == XBEE_RX_ERROR) {
		status = PROTOCOL_RX_TIMEOUT;
	}

	if ((rx_frame->type == XBEE_RX_PACKET_TYPE) || (rx_frame->type == XBEE_RX_EXPLICIT_TYPE)) {
		data = ((XBEE_RX_PACKET_FRAME*)rx_frame)->data;

		ptr = strtok(data, PROTOCOL_SEPARATOR); // ptr is on first word

		//check if it is OK or ERR
		if (strstr (ptr, PROTOCOL_ANS_OK)!= NULL) { // answer is "OK"
			status = PROTOCOL_ParseConfiguration(conf, ptr);
		} else if (strstr (ptr, PROTOCOL_ANS_ERR)!= NULL) { // answer is "ERR"
			ptr = strtok(NULL, PROTOCOL_SEPARATOR);
			conf->last_error = (uint16_t)strtol(ptr,NULL, 10);
			status = PROTOCOL_RX_CMD_ERROR;
		} else { // unknown answer
			status= PROTOCOL_INVALID_FRAME;
		}
	}

	free (rx_frame);
	return status;
}

#if DEBUG_PROTOCOL == 2
XBEE_RX_PACKET_FRAME frame1={XBEE_RX_PACKET_TYPE, 0x13A20041C16E2B, 0, 14, "OK|abcdefghijk"};
XBEE_RX_PACKET_FRAME frame2={XBEE_RX_PACKET_TYPE, 0x13A20041C16E2B, 0, 14, "OK|lmnopqrstuv"};
XBEE_RX_PACKET_FRAME frame3={XBEE_RX_PACKET_TYPE, 0x13A20041C16E2B, 0, 7, "OK|wxyz"};
XBEE_RX_PACKET_FRAME frame4={XBEE_RX_PACKET_TYPE, 0x13A20041C16E2B, 0, 3, "END"};

int PROTOCOL_GetDataSim (XBEE_GENERIC_FRAME** frame, int timeout) {
	int com_status =XBEE_OK;
	static int index=0;
	XBEE_RX_PACKET_FRAME* framelist[] = {&frame1, &frame2, &frame3, &frame4};

	if (index < 4) {
		*frame = (XBEE_GENERIC_FRAME*)framelist[index];
		index++;
	}

	return com_status;
}
#endif /* DEBUG_PROTOCOL == 2 */

typedef struct pCalEventList {
	char* event;
	struct pCalEventList* nextframe;
} PROTOCOL_CalEventList;

static int PROTOCOL_ParseCalendar(PROTOCOL_CalendarTypedef* calendar, char* str) {
	char* datelist =NULL;
	char* traineesptr=NULL;
	char* trainersptr = NULL;
	char* ptr;
	int i;
	int length;

	PROTOCOL_CalEventList* headptr = NULL;
	PROTOCOL_CalEventList* indexptr=NULL;
	PROTOCOL_Reservation* indexreservationptr=NULL;

	ptr = strtok(str, PROTOCOL_SEPARATOR); // ptr should be on date list
	if (ptr == NULL)
		return PROTOCOL_INVALID_FRAME;
	else
		// copy date list for further analysis
		datelist = ptr;

	// 1st step: split string in reservation string
	do {
		ptr = strtok(NULL, PROTOCOL_SEPARATOR); // ptr should be on next event

		if (ptr != NULL) {
			if (headptr == NULL) {
				headptr = (PROTOCOL_CalEventList*)malloc(sizeof(PROTOCOL_CalEventList));
				indexptr = headptr;
			} else {
				indexptr->nextframe = (PROTOCOL_CalEventList*)malloc(sizeof(PROTOCOL_CalEventList));
				indexptr = indexptr->nextframe;
			}

			indexptr->nextframe = NULL;
			indexptr->event = ptr;
		}
	} while (ptr!=NULL);

	// 2nd step: split week date
	ptr = strtok(datelist, ";"); // ptr should be on first date

	for (i=0; i<7 ; i++) {
		if (ptr != NULL)
			strncpy(calendar->week[i], ptr, PROTOCOL_DATE_STRING_MAX_LENGTH);

		ptr = strtok(NULL, ";"); // ptr should be on next date
	}

	// 3rd step: split events
	indexptr =headptr;

	while (headptr!=NULL) {
		if (calendar->first_reservation == NULL) {
			calendar->first_reservation = (PROTOCOL_Reservation*)malloc(sizeof(PROTOCOL_Reservation));
			indexreservationptr =calendar->first_reservation;
		} else {
			indexreservationptr->next_reservation = (PROTOCOL_Reservation*)malloc(sizeof(PROTOCOL_Reservation));
			indexreservationptr = indexreservationptr->next_reservation;
		}

		memset(indexreservationptr, 0, sizeof(PROTOCOL_Reservation)); // init all field to '0'

		ptr = strtok(headptr->event, ";"); // ptr should be on event day
		if (ptr!=NULL)
			indexreservationptr->day_nbr = (uint8_t)strtoul(ptr,NULL,10);

		ptr = strtok(NULL, ";"); // ptr should be on start time
		if (ptr!=NULL)
			indexreservationptr->start_time = (uint16_t)strtoul(ptr,NULL,10);

		ptr = strtok(NULL, ";"); // ptr should be on end time
		if (ptr!=NULL)
			indexreservationptr->end_time = (uint16_t)strtoul(ptr,NULL,10);

		ptr = strtok(NULL, ";"); // ptr should be on title
		if (ptr!=NULL) {
			length = strlen(ptr);
			if (length > PROTOCOL_TITLE_MAX_LENGTH)
				length = PROTOCOL_TITLE_MAX_LENGTH;

			indexreservationptr->title = (char*)malloc(length+1); // for '0' ending
			strncpy(indexreservationptr->title, ptr, PROTOCOL_TITLE_MAX_LENGTH);
		}

		ptr = strtok(NULL, ";"); // ptr should be on trainees
		traineesptr= ptr;
		ptr = strtok(NULL, ";"); // ptr should be on trainers
		trainersptr= ptr;

		if (traineesptr!=NULL) {
			ptr = strtok(traineesptr, ",");// ptr should be on 1st trainee
			i=0;
			while ((ptr != NULL) && (i<4)) {
				length = strlen(ptr);
				if (length> PROTOCOL_TRAINEE_MAX_LENGTH)
					length = PROTOCOL_TRAINEE_MAX_LENGTH;

				indexreservationptr->trainees[i]=(char*)malloc(length);
				strncpy(indexreservationptr->trainees[i], ptr, PROTOCOL_TRAINEE_MAX_LENGTH);

				i++;
				ptr = strtok(NULL, ",");// ptr should be on next trainee
			}
		}

		if (trainersptr!=NULL) {
			ptr = strtok(trainersptr, ",");// ptr should be on 1st trainer
			i=0;
			while ((ptr != NULL) && (i<4)) {
				length = strlen(ptr);
				if (length> PROTOCOL_TRAINER_MAX_LENGTH)
					length = PROTOCOL_TRAINER_MAX_LENGTH;

				indexreservationptr->trainers[i]=(char*)malloc(length);
				strncpy(indexreservationptr->trainers[i], ptr, PROTOCOL_TRAINER_MAX_LENGTH);

				i++;
				ptr = strtok(NULL, ",");// ptr should be on next trainer
			}
		}

		// 4th step: free the main string
		// All the main string is now split, we can deallocate it.
		free((void*)str);
	}

	return PROTOCOL_OK;
}

typedef struct pCalFrameList {
	XBEE_RX_PACKET_FRAME* frame;
	struct pCalFrameList* nextframe;
} PROTOCOL_CalFrameList;

PROTOCOL_Status PROTOCOL_GetCalendar(PROTOCOL_ConfigurationTypedef* conf, PROTOCOL_CalendarTypedef* calendar) {
	int com_status;
	PROTOCOL_Status status=PROTOCOL_OK;
	XBEE_GENERIC_FRAME *rx_frame;
	uint8_t transmit_status;
	char* data;
	char *ptr;
	int end_rx=0;

	PROTOCOL_CalFrameList* framelisthead=NULL;
	PROTOCOL_CalFrameList* framelistindex=NULL;

	/* Send a "CAL" command */
	if (XBEE_SendData(conf->gw_address, 1, XBEE_NO_PANID_BROADCAST, PROTOCOL_CMD_GET_CALENDAR, &transmit_status)!= XBEE_OK) {
		status= PROTOCOL_RX_HW_ERROR;
	}

	if (transmit_status !=XBEE_TX_STATUS_SUCCESS ) {
		// here we should increment no ack counter for report
		// not good to have either no ack or CCA failure
		conf->rssi.no_ack++;
	}

	// Wait for answer
	/* Wait for OK, ERR, END or no answer */
	while (!end_rx) {
#if DEBUG_PROTOCOL == 2
		com_status = PROTOCOL_GetDataSim(&rx_frame, DEBUG_PROTOCOL_TIMEOUT);
#elif DEBUG_PROTOCOL == 1
		com_status = XBEE_GetData(&rx_frame, DEBUG_PROTOCOL_TIMEOUT);
#else
		com_status = XBEE_GetData(&rx_frame, PROTOCOL_CONNECT_TIMEOUT);
#endif /* #if DEBUG_PROTOCOL==1 */
		if (com_status == XBEE_RX_ERROR) {
			status= PROTOCOL_RX_HW_ERROR;
			end_rx=1;
		} else if (com_status == XBEE_RX_ERROR) {
			status = PROTOCOL_RX_TIMEOUT;
			end_rx=1;
		} else {
			if ((rx_frame->type == XBEE_RX_PACKET_TYPE) || (rx_frame->type == XBEE_RX_EXPLICIT_TYPE)) {
				data = ((XBEE_RX_PACKET_FRAME*)rx_frame)->data;

				ptr = strtok(data, PROTOCOL_SEPARATOR); // ptr is on first word
				if (strstr (ptr, PROTOCOL_ANS_OK)!= NULL) { // answer is "OK"
					if (framelisthead == NULL) {
						framelisthead = (PROTOCOL_CalFrameList*)malloc(sizeof(PROTOCOL_CalFrameList));
						framelistindex=framelisthead;
					} else {
						framelistindex->nextframe = (PROTOCOL_CalFrameList*)malloc(sizeof(PROTOCOL_CalFrameList));
						framelistindex = framelistindex->nextframe;
					}

					framelisthead->frame=(XBEE_RX_PACKET_FRAME*)rx_frame;
					framelisthead->nextframe=NULL;
				} else if (strstr (ptr, PROTOCOL_ANS_ERR)!= NULL) { // answer is "ERR"
					ptr = strtok(NULL, PROTOCOL_SEPARATOR);
					conf->last_error = (uint16_t)strtol(ptr,NULL, 10);
					status = PROTOCOL_RX_CMD_ERROR;
					end_rx=1;

					free(rx_frame);
				} else if (strstr (ptr, PROTOCOL_ANS_END)!= NULL) // answer is "END"
					end_rx=1;
				free(rx_frame);
			} else {
				// do nothing, free memory frame and wait for an other frame
				free(rx_frame);
			}
		}
	}

	if ((status == PROTOCOL_OK) || (status == PROTOCOL_RX_TIMEOUT)) {
		// build the concatenated answer string
		int string_length=0;
		framelistindex = framelisthead;

		// compute necessary string size
		while (framelistindex!=NULL) {
			string_length += framelistindex->frame->data_length;
			framelistindex = framelistindex->nextframe;
		}

		string_length+=1; // for 0 ending
		data = (char*)malloc(string_length);
		ptr = data;

		framelistindex = framelisthead;

		// copy string chunks and free memory
		while (framelisthead!=NULL) {
			ptr=strncpy(ptr,framelisthead->frame->data, framelisthead->frame->data_length);
			framelistindex = framelisthead;
			ptr= ptr-1; //remove 0 ending

			free ((void*)framelisthead->frame);
			framelisthead = framelisthead->nextframe;
			free ((void*)framelistindex);
		}

		status=PROTOCOL_ParseCalendar(calendar, data);
	}

	return status;
}

PROTOCOL_Status PROTOCOL_GetCalendarUpdateStatus(PROTOCOL_ConfigurationTypedef* conf, uint8_t* update_status) {
	int com_status;
	PROTOCOL_Status status=PROTOCOL_OK;
	XBEE_GENERIC_FRAME *rx_frame;
	uint8_t transmit_status;
	char* data;
	char *ptr;

	/* Send a "UPDATE" command */
	if (XBEE_SendData(conf->gw_address, 1, XBEE_NO_PANID_BROADCAST, PROTOCOL_CMD_GET_UPDATE, &transmit_status)!= XBEE_OK) {
		status= PROTOCOL_RX_HW_ERROR;
	}

	if (transmit_status !=XBEE_TX_STATUS_SUCCESS ) {
		// here we should increment no ack counter for report
		// not good to have either no ack or CCA failure
		conf->rssi.no_ack++;
	}

	// Wait for answer
	/* Wait for OK, ERR or no answer */
#if DEBUG_PROTOCOL != 0
	com_status = XBEE_GetData(&rx_frame, DEBUG_PROTOCOL_TIMEOUT);
#else
	com_status = XBEE_GetData(&rx_frame, PROTOCOL_CONNECT_TIMEOUT);
#endif /* #if DEBUG_PROTOCOL==1 */

	if (com_status == XBEE_RX_ERROR) {
		status= PROTOCOL_RX_HW_ERROR;
	} else if (com_status == XBEE_RX_ERROR) {
		status = PROTOCOL_RX_TIMEOUT;
	}

	if ((rx_frame->type == XBEE_RX_PACKET_TYPE) || (rx_frame->type == XBEE_RX_EXPLICIT_TYPE)) {
		data = ((XBEE_RX_PACKET_FRAME*)rx_frame)->data;

		ptr = strtok(data, PROTOCOL_SEPARATOR); // ptr is on first word

		//check if it is OK or ERR
		if (strstr (ptr, PROTOCOL_ANS_OK)!= NULL) { // answer is "OK"
			ptr = strtok(NULL, PROTOCOL_SEPARATOR);
			if (ptr==NULL)
				status = PROTOCOL_INVALID_FRAME;
			else
				*update_status = (uint8_t)strtoul(ptr, NULL, 10);
		} else if (strstr (ptr, PROTOCOL_ANS_ERR)!= NULL) { // answer is "ERR"
			ptr = strtok(NULL, PROTOCOL_SEPARATOR);
			conf->last_error = (uint16_t)strtol(ptr,NULL, 10);
			status = PROTOCOL_RX_CMD_ERROR;
		} else { // unknown answer
			status= PROTOCOL_INVALID_FRAME;
		}
	}

	free (rx_frame);
	return status;
}

PROTOCOL_Status PROTOCOL_SendReport(PROTOCOL_ConfigurationTypedef* conf) {
	PROTOCOL_Status status=PROTOCOL_OK;
	uint8_t transmit_status;
	char report_string[32] = {0};
	uint8_t max_rssi=0;
	uint8_t min_rssi=0xFF;
	uint8_t moy_rssi=0;
	uint8_t index;

	for (index=0; index<=conf->rssi.index; index++) {
		if (conf->rssi.rssi_val[index]<min_rssi)
			min_rssi = conf->rssi.rssi_val[index];

		if (conf->rssi.rssi_val[index]>max_rssi)
			max_rssi = conf->rssi.rssi_val[index];

		moy_rssi+=conf->rssi.rssi_val[index];
	}

	if (conf->rssi.index!=0)
		moy_rssi = moy_rssi / conf->rssi.index;

	snprintf(report_string, 32, "REPORT|%02X|%02X|%02X|%02X|%02X",min_rssi,max_rssi,moy_rssi,conf->rssi.no_ack,conf->battery_level);

	conf->rssi.index=0;
	conf->rssi.no_ack=0;

	/* Send a "REPORT" command */
	if (XBEE_SendData(conf->gw_address, 1, XBEE_NO_PANID_BROADCAST, report_string, &transmit_status)!= XBEE_OK) {
		status= PROTOCOL_RX_HW_ERROR;
	}

	if (transmit_status !=XBEE_TX_STATUS_SUCCESS ) {
		// here we should increment no ack counter for report
		// not good to have either no ack or CCA failure
		conf->rssi.no_ack++;
		status =  PROTOCOL_TX_ERROR;
	}

	return status;
}



