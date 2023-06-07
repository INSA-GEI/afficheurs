/*
 * protocol.c
 *
 *  Created on: 12 avr. 2022
 *      Author: dimercur
 */

#include "protocol.h"
#include "xbee.h"

#include "calendar.h"

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

PROTOCOL_Status PROTOCOL_Init(PROTOCOL_ConfigurationTypedef* conf) {

	/* First init XBEE */
	if (XBEE_Init() != XBEE_OK)
		return PROTOCOL_XBEE_INIT_ERROR;

//	if (XBEE_GetUID(&(conf->device_uid))!=XBEE_OK)
//		return PROTOCOL_XBEE_INIT_ERROR;

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
	PROTOCOL_Status status=PROTOCOL_OK;
	int com_status;
	uint8_t transmit_status;
	XBEE_GENERIC_FRAME *rx_frame;
	char* data;

	/* PAS DE CHANNEL A SET
	if (XBEE_SetChannel(DEBUG_CHANNEL) != XBEE_OK) {
		status= PROTOCOL_RX_HW_ERROR;
	}
	*/

	if (XBEE_SetPanID(DEBUG_PANID) != XBEE_OK) {
		status= PROTOCOL_RX_HW_ERROR;
	}

	//HAL_Delay(50);
	vTaskDelay(msToTicks(100));

	conf->rf_channel = 0x12;
	conf->rf_panid = DEBUG_PANID;
	conf->gw_address = 0x13A20041C16E2B;

#if DEBUG_PROTOCOL ==1
	/* Send a broadcast "JOIN" command */
	if (XBEE_SendData(XBEE_BROADCAST_ADDRESS, XBEE_DEST_ADDRESS, 1, XBEE_PANID_BROADCAST, PROTOCOL_CMD_JOIN, &transmit_status)!= XBEE_OK) {
		status= PROTOCOL_RX_HW_ERROR;
		return status;
	}

	/* Wait for ACCEPT, REJECT or no answer */
	com_status = XBEE_GetData(&rx_frame, PROTOCOL_CONNECT_TIMEOUT);
	//com_status = XBEE_GetData(&rx_frame, 0);

	if (com_status == XBEE_RX_ERROR) {
		status= PROTOCOL_RX_HW_ERROR;
		return status;
	} else if (com_status == XBEE_RX_TIMEOUT) {
		status= PROTOCOL_NOT_CONNECTED;
		return status;
	}

	if ((com_status != XBEE_INVALID_FRAME)) {
		// a frame has been received, check its content
		if ((rx_frame->type == XBEE_RX_PACKET_TYPE) || (rx_frame->type == XBEE_RX_EXPLICIT_TYPE)) {
			data = ((XBEE_RX_PACKET_FRAME*)rx_frame)->data;

			if (strcmp((const char*)data, PROTOCOL_ANS_ACCEPT)==0) {
				/* retrieve panid */
				char *ptr = strtok(data, PROTOCOL_SEPARATOR); // ptr is on first word
				ptr = strtok(NULL, PROTOCOL_SEPARATOR);

				if (ptr!=NULL)
					free (rx_frame);
			}
		}

		free (rx_frame);
	}
#endif /* DEBUG_PROTOCOL ==1 */
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

		//HAL_Delay(50);
		vTaskDelay(msToTicks(100));

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

				if (strcmp((const char*)data, PROTOCOL_ANS_ACCEPT)==0) {
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
						//HAL_Delay(50);
						vTaskDelay(msToTicks(100));
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
	else {
		// parse time of the frame
		strncpy(currenttime, ptr, PROTOCOL_TIME_STRING_MAX_LENGTH); // Copy time string in a separate buffer for parsing
		currenttime[PROTOCOL_TIME_STRING_MAX_LENGTH] = 0; // force '0' ending if ptr is long or longer than PROTOCOL_TIME_STRING_MAX_LENGTH
	}

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
		conf->room_name[PROTOCOL_ROOM_NAME_LENGTH]=0;// force '0' ending if ptr is long or longer than PROTOCOL_ROOM_NAME_LENGTH
	}

	ptr = strtok(NULL, PROTOCOL_SEPARATOR); // ptr should be on room type
	if (ptr == NULL)
		return PROTOCOL_INVALID_FRAME;
	else {
		strncpy(conf->room_type, ptr, PROTOCOL_ROOM_TYPE_LENGTH);
		conf->room_type[PROTOCOL_ROOM_TYPE_LENGTH]=0; // force '0' ending if ptr is long or longer than PROTOCOL_ROOM_NAME_LENGTH
	}

	ptr = strtok(NULL, PROTOCOL_SEPARATOR); // ptr should be on start hour (in min)
	if (ptr == NULL)
		return PROTOCOL_INVALID_FRAME;
	else {
		strncpy(starttime, ptr, PROTOCOL_TIME_STRING_MAX_LENGTH);
		starttime[PROTOCOL_TIME_STRING_MAX_LENGTH]=0; // force '0' ending if ptr is long or longer than PROTOCOL_TIME_STRING_MAX_LENGTH
	}

	ptr = strtok(NULL, PROTOCOL_SEPARATOR); // ptr should be on end hour (in min)
	if (ptr == NULL)
		return PROTOCOL_INVALID_FRAME;
	else {
		strncpy(endtime, ptr, PROTOCOL_TIME_STRING_MAX_LENGTH);
		endtime[PROTOCOL_TIME_STRING_MAX_LENGTH]=0; // force '0' ending if ptr is long or longer than PROTOCOL_TIME_STRING_MAX_LENGTH
	}

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
	if (XBEE_SendData(conf->gw_address, XBEE_DEST_ADDRESS, 1, XBEE_NO_PANID_BROADCAST, PROTOCOL_CMD_SETUP, &transmit_status)!= XBEE_OK) {
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
	com_status = XBEE_GetData(&rx_frame, 0);
#else
	com_status = XBEE_GetData(&rx_frame, PROTOCOL_CONNECT_TIMEOUT);
#endif /* #if DEBUG_PROTOCOL==1 */

	if (com_status == XBEE_RX_ERROR) {
		status= PROTOCOL_RX_HW_ERROR;
	} else if (com_status == XBEE_RX_TIMEOUT) {
		status = PROTOCOL_RX_TIMEOUT;
	}

	if ((rx_frame->type == XBEE_RX_PACKET_TYPE) || (rx_frame->type == XBEE_RX_EXPLICIT_TYPE)) {
		data = ((XBEE_RX_PACKET_FRAME*)rx_frame)->data;

		ptr = strtok(data, PROTOCOL_SEPARATOR); // ptr is on first word

		//check if it is OK or ERR
		if (strcmp (ptr, PROTOCOL_ANS_OK)==0) { // answer is "OK"
			status = PROTOCOL_ParseConfiguration(conf, ptr);
		} else if (strcmp (ptr, PROTOCOL_ANS_ERR)==0) { // answer is "ERR"
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
//XBEE_RX_PACKET_FRAME frame1={XBEE_RX_PACKET_TYPE, 0x13A20041C16E2B, 0, 14, "OK|abcdefghijk"};
//XBEE_RX_PACKET_FRAME frame2={XBEE_RX_PACKET_TYPE, 0x13A20041C16E2B, 0, 14, "OK|lmnopqrstuv"};
//XBEE_RX_PACKET_FRAME frame3={XBEE_RX_PACKET_TYPE, 0x13A20041C16E2B, 0, 7, "OK|wxyz"};
//XBEE_RX_PACKET_FRAME frame4={XBEE_RX_PACKET_TYPE, 0x13A20041C16E2B, 0, 3, "END"};

XBEE_RX_PACKET_FRAME frame1={XBEE_RX_PACKET_TYPE, 0x13A20041C16E2B, 0, 86, "OK|28/02/2022;01/03/2022;02/03/2022;03/03/2022;04/03/2022;05/03/2022;06/03/2022|2;900;"};
XBEE_RX_PACKET_FRAME frame2={XBEE_RX_PACKET_TYPE, 0x13A20041C16E2B, 0, 61, "OK|1020;Algo & programmation;4AE-SE,4AE-IR;S.DI MERCURIO,R.ST"};
XBEE_RX_PACKET_FRAME frame3={XBEE_RX_PACKET_TYPE, 0x13A20041C16E2B, 0, 41, "OK|AR#2;1035;1250;Reseaux;4IR;V.Nicomette"};
XBEE_RX_PACKET_FRAME frame4={XBEE_RX_PACKET_TYPE, 0x13A20041C16E2B, 0, 3, "END"};

int PROTOCOL_GetDataSim (XBEE_GENERIC_FRAME** frame, int timeout) {
	int com_status =XBEE_OK;
	static int index=0;
	int val;
	XBEE_RX_PACKET_FRAME* framelist[] = {&frame1, &frame2, &frame3, &frame4};

	if (index < 4) {
		val = sizeof(XBEE_RX_PACKET_FRAME)+framelist[index]->data_length+1;
		*frame = (XBEE_GENERIC_FRAME*)malloc(val);
		memcpy(*frame, framelist[index],val);
		index++;
	}

	return com_status;
}
#endif /* DEBUG_PROTOCOL == 2 */

typedef struct pCalEventList {
	char* event;
	struct pCalEventList* nextevent;
} PROTOCOL_CalEventList;

static int PROTOCOL_ParseCalendar(char* str) {
	char* datelist =NULL;
	char* traineesptr=NULL;
	char* trainersptr = NULL;
	char* ptr;
	int i;
	int length;

	PROTOCOL_CalEventList* headptr = NULL;
	PROTOCOL_CalEventList* indexptr=NULL;

	ptr = strtok(str, PROTOCOL_SEPARATOR); // ptr should be on date list
	if (ptr == NULL)
		return PROTOCOL_INVALID_FRAME;
	else
		// copy date list for further analysis
		datelist = ptr;

	// 1st step: split string in reservation string
	ptr = strtok(NULL, PROTOCOL_SEPARATOR); // ptr should be on first event
	ptr = strtok(ptr, "#"); // ptr still should be on first event

	while (ptr!=NULL) {
		if (headptr == NULL) {
			headptr = (PROTOCOL_CalEventList*)malloc(sizeof(PROTOCOL_CalEventList));
			indexptr = headptr;
		} else {
			indexptr->nextevent = (PROTOCOL_CalEventList*)malloc(sizeof(PROTOCOL_CalEventList));
			indexptr = indexptr->nextevent;
		}

		indexptr->nextevent = NULL;
		indexptr->event = ptr;


		ptr = strtok(NULL, "#"); // ptr should be on next event
	};

	// 2nd step: split week date
	ptr = strtok(datelist, ";"); // ptr should be on first date

	CAL_Day* days;
	days = CAL_GetDayArray();

	for (i=0; i<7 ; i++) {
		if (ptr != NULL) {
			strncpy(days[i], ptr, CAL_DATE_STRING_MAX_LENGTH);
			days[i][CAL_DATE_STRING_MAX_LENGTH]=0; // force '0' ending in case ptr string is long or longer than PROTOCOL_DATE_STRING_MAX_LENGTH
		}

		ptr = strtok(NULL, ";"); // ptr should be on next date
	}

	// 3rd step: split events
	CAL_Reservation* reservation;

	while (headptr!=NULL) {
		reservation = CAL_NewReservation();

		ptr = strtok(headptr->event, ";"); // ptr should be on event day
		if (ptr!=NULL)
			reservation->day_nbr = (uint8_t)strtoul(ptr,NULL,10);

		ptr = strtok(NULL, ";"); // ptr should be on start time
		if (ptr!=NULL)
			reservation->start_time = (uint16_t)strtoul(ptr,NULL,10);

		ptr = strtok(NULL, ";"); // ptr should be on end time
		if (ptr!=NULL)
			reservation->end_time = (uint16_t)strtoul(ptr,NULL,10);

		ptr = strtok(NULL, ";"); // ptr should be on title
		if (ptr!=NULL) {
			length = strlen(ptr);
			if (length > CAL_TITLE_MAX_LENGTH)
				length = CAL_TITLE_MAX_LENGTH;

			reservation->title = (char*)malloc(length+1); // for '0' ending
			strncpy(reservation->title, ptr, length);
			reservation->title[length]=0; // force '0' ending in case ptr string is long or longer than PROTOCOL_TITLE_MAX_LENGTH
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
				if (length> CAL_TRAINEE_MAX_LENGTH)
					length = CAL_TRAINEE_MAX_LENGTH;

				reservation->trainees[i]=(char*)malloc(length);
				strncpy(reservation->trainees[i], ptr, length);
				reservation->trainees[i][length]=0; // force '0' ending in case ptr string is long or longer than PROTOCOL_TRAINEE_MAX_LENGTH

				i++;
				ptr = strtok(NULL, ",");// ptr should be on next trainee
			}
		}

		if (trainersptr!=NULL) {
			ptr = strtok(trainersptr, ",");// ptr should be on 1st trainer
			i=0;
			while ((ptr != NULL) && (i<4)) {
				length = strlen(ptr);
				if (length> CAL_TRAINER_MAX_LENGTH)
					length = CAL_TRAINER_MAX_LENGTH;

				reservation->trainers[i]=(char*)malloc(length);
				strncpy(reservation->trainers[i], ptr, length);
				reservation->trainers[i][length]=0; // force '0' ending in case ptr string is long or longer than PROTOCOL_TRAINER_MAX_LENGTH

				i++;
				ptr = strtok(NULL, ",");// ptr should be on next trainer
			}
		}

		indexptr= headptr;
		headptr= headptr->nextevent;
		free((void*)indexptr);
	}

	// 4th step: free the main string
	// All the main string is now split, we can deallocate it.
	free((void*)str);

	return PROTOCOL_OK;
}

typedef struct pCalFrameList {
	XBEE_RX_PACKET_FRAME* frame;
	struct pCalFrameList* nextframe;
} PROTOCOL_CalFrameList;

PROTOCOL_Status PROTOCOL_GetCalendar(PROTOCOL_ConfigurationTypedef* conf) {
	int com_status;
	PROTOCOL_Status status=PROTOCOL_OK;
	XBEE_GENERIC_FRAME *rx_frame;
	uint8_t transmit_status;
	char* data;
	char *ptr;
	int end_rx=0;

	PROTOCOL_CalFrameList* framelisthead=NULL;
	PROTOCOL_CalFrameList* framelistindex=NULL;

#if DEBUG_PROTOCOL != 2
	/* Send a "CAL" command */
	if (XBEE_SendData(conf->gw_address, XBEE_DEST_ADDRESS, 1, XBEE_NO_PANID_BROADCAST, PROTOCOL_CMD_GET_CALENDAR, &transmit_status)!= XBEE_OK) {
		status= PROTOCOL_RX_HW_ERROR;
	}

	if (transmit_status !=XBEE_TX_STATUS_SUCCESS ) {
		// here we should increment no ack counter for report
		// not good to have either no ack or CCA failure
		conf->rssi.no_ack++;
	}
#endif /* #if DEBUG_PROTOCOL==1 */

	// Wait for answer
	/* Wait for OK, ERR, END or no answer */
	while (!end_rx) {
#if DEBUG_PROTOCOL == 2
		com_status = PROTOCOL_GetDataSim(&rx_frame, DEBUG_PROTOCOL_TIMEOUT);
#elif DEBUG_PROTOCOL == 1
		com_status = XBEE_GetData(&rx_frame, 0);
#else
		com_status = XBEE_GetData(&rx_frame, PROTOCOL_CONNECT_TIMEOUT);
#endif /* #if DEBUG_PROTOCOL==1 */
		if (com_status == XBEE_RX_ERROR) {
			status= PROTOCOL_RX_HW_ERROR;
			end_rx=1;
		} else if (com_status == XBEE_RX_TIMEOUT) {
			status = PROTOCOL_RX_TIMEOUT;
			end_rx=1;
		} else if (com_status == XBEE_INVALID_FRAME) {
			status = PROTOCOL_RX_CMD_ERROR;
		} else {
			if ((rx_frame->type == XBEE_RX_PACKET_TYPE) || (rx_frame->type == XBEE_RX_EXPLICIT_TYPE)) {
				data = ((XBEE_RX_PACKET_FRAME*)rx_frame)->data;

				ptr = strtok(data, PROTOCOL_SEPARATOR); // ptr is on first word
				if (strcmp (ptr, PROTOCOL_ANS_ERR)==0) { // answer is "ERR"
					ptr = strtok(NULL, PROTOCOL_SEPARATOR);
					conf->last_error = (uint16_t)strtol(ptr,NULL, 10);
					status = PROTOCOL_RX_CMD_ERROR;
					end_rx=1;

					free(rx_frame);
				} else if (strcmp (ptr, PROTOCOL_ANS_END)==0) { // answer is "END"
					end_rx=1;
					free(rx_frame);
				} else { // its an acceptable frame
					if (framelisthead == NULL) {
						framelisthead = (PROTOCOL_CalFrameList*)malloc(sizeof(PROTOCOL_CalFrameList));
						framelistindex=framelisthead;
					} else {
						framelistindex->nextframe = (PROTOCOL_CalFrameList*)malloc(sizeof(PROTOCOL_CalFrameList));
						framelistindex = framelistindex->nextframe;
					}

					framelistindex->frame=(XBEE_RX_PACKET_FRAME*)rx_frame;
					framelistindex->nextframe=NULL;
				}
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
			if (strstr(framelistindex->frame->data,"OK"))
				string_length += framelistindex->frame->data_length-3; // -3 to remove "OK\0" at start of data
			else
				string_length += framelistindex->frame->data_length; // -3 to remove "OK\0" at start of data
			framelistindex = framelistindex->nextframe;
		}

		string_length+=1; // for 0 ending
		data = (char*)malloc(string_length);
		memset(data, 0, string_length); // fill array with '0' to avoid strncpy missing '0' at end of copy
		ptr = data;

		framelistindex = framelisthead;

		// copy string chunks and free memory
		while (framelisthead!=NULL) {
			if (strstr(framelisthead->frame->data,"OK")) {
				ptr=strncpy(ptr,framelisthead->frame->data+3, framelisthead->frame->data_length-3);// move data 3 chars ahead, to step over "OK" and find start of PDU
				framelistindex = framelisthead;
				ptr= ptr+(framelisthead->frame->data_length-3); // go to end of string 0 ending
			} else {
				ptr=strncpy(ptr,framelisthead->frame->data, framelisthead->frame->data_length);// move data 3 chars ahead, to step over "OK" and find start of PDU
				framelistindex = framelisthead;
				ptr= ptr+(framelisthead->frame->data_length); // go to end of string 0 ending
			}

			free ((void*)framelisthead->frame);
			framelisthead = framelisthead->nextframe;
			free ((void*)framelistindex);
		}

		status=PROTOCOL_ParseCalendar(data);
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
	if (XBEE_SendData(conf->gw_address, XBEE_DEST_ADDRESS, 1, XBEE_NO_PANID_BROADCAST, PROTOCOL_CMD_GET_UPDATE, &transmit_status)!= XBEE_OK) {
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
	} else if (com_status == XBEE_RX_TIMEOUT) {
		status = PROTOCOL_RX_TIMEOUT;
	}

	if ((rx_frame->type == XBEE_RX_PACKET_TYPE) || (rx_frame->type == XBEE_RX_EXPLICIT_TYPE)) {
		data = ((XBEE_RX_PACKET_FRAME*)rx_frame)->data;

		ptr = strtok(data, PROTOCOL_SEPARATOR); // ptr is on first word

		//check if it is OK or ERR
		if (strcmp (ptr, PROTOCOL_ANS_OK)==0) { // answer is "OK"
			ptr = strtok(NULL, PROTOCOL_SEPARATOR);
			if (ptr==NULL)
				status = PROTOCOL_INVALID_FRAME;
			else
				*update_status = (uint8_t)strtoul(ptr, NULL, 10);
		} else if (strcmp (ptr, PROTOCOL_ANS_ERR)==0) { // answer is "ERR"
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
	if (XBEE_SendData(conf->gw_address, XBEE_DEST_ADDRESS, 1, XBEE_NO_PANID_BROADCAST, report_string, &transmit_status)!= XBEE_OK) {
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

#if DEBUG_PROTOCOL_FAKE_CONFIG==1
static const char* fake_conf="OK|16:31:26|2|GEI 109|Informatique|06:00:00|19:00:00|900|0";
static const char* fake_cal="15/11/2021;16/11/2021;17/11/2021;18/11/2021;19/11/2021;20/11/2021;21/11/2021|1;570;735;PROJET Integrateur - 2/7;5GEI_SDBD_A,5GEI_SDBD_B;FERRY Julien,HUGUET Marie-Jose,MARIGO Pierre,SIALA Mohamed,YANGUI Sami#1;840;915;SOA-TD(9) MOOC - I5IRIL12;5GEI_SDBD_A;#1;930;1095;TP Langage C - E;3_MIC_E;CAYRE Romain,CHABI SIKA BONI Abdel Kader#2;480;585;25A - I5TSSL11 - TP Vuln Logicielles 3//6;5GEI_TLS_SEC;ALATA Eric,NICOMETTE Vincent#2;615;720;25A -I5TSSL11 -  TP  Vuln Logicielles 4//6;5GEI_TLS_SEC;ALATA Eric,NICOMETTE Vincent#2;750;915;Analyse Exp. TP(5) - I5IRAD12;5GEI_SDBD_A1,5GEI_SDBD_A2;FEUILLET Laure,TREDAN Gilles#2;930;1095;Analyse Exp. TP(5) - I5IRAD12;5GEI_SDBD_B1,5GEI_SDBD_B2;FEUILLET Laure,HU Hao#3;480;585;25A - I5TSSL11 - TP  Vuln Logicielles 5/6;5GEI_TLS_SEC;ALATA Eric,NICOMETTE Vincent#3;615;720;25A - I5TSSL11 - TP  Vuln Logicielles 6/6;5GEI_TLS_SEC;ALATA Eric,NICOMETTE Vincent#3;750;915;Big Data Arch.-TP2 - I5IRBD12;5GEI_SDBD_A1,5GEI_SDBD_A2;GHARBI Ghada,YANGUI Sami#4;570;735;SoC EC11 (TP) du 9/11;5 IS - AE1,5 IS - AE2;DRAGOMIRESCU Daniela#5;570;735;AP-PLNE-TP4 (B1);5GEI_SDBD_B1;CAPELLE Mikael#5;840;915;TD Processus de developpement logiciel automatise 1/7 - IB;4IR_I_B;GUERMOUCHE Nawal#5;930;1005;TD Processus de developpement logiciel automatise 7/7 - IB;4IR_I_B;GUERMOUCHE Nawal";

PROTOCOL_Status PROTOCOL_FakeConfig(PROTOCOL_ConfigurationTypedef* conf) {
	PROTOCOL_Status status=PROTOCOL_OK;
	char* data;
	char* ptr;
	int len=strlen(fake_conf);

	data= (char*)malloc(len+1);
	memcpy(data, fake_conf, len);
	data[len]=0;

	ptr = strtok(data, PROTOCOL_SEPARATOR);
	status=PROTOCOL_ParseConfiguration(conf, ptr);
	free((void*) data);

	conf->device_uid=0x13a20041c02be8;
	conf->gw_address=0x13a20041c0bae6;
	conf->rf_channel=0x12;
	conf->rf_panid=0x1337;

	return status;
}

PROTOCOL_Status PROTOCOL_FakeCalendar(void) {
	PROTOCOL_Status status=PROTOCOL_OK;
	char* data;
	int len=strlen(fake_cal);

	data= (char*)malloc(len+1);
	memcpy(data, fake_cal, len);
	data[len]=0;

	status=PROTOCOL_ParseCalendar(data);
	//free((void*) data);
	return status;
}
#endif /* DEBUG_PROTOCOL_FAKE_CONFIG==1 */

