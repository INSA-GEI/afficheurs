/*
 * protocol.c
 *
 *  Created on: 12 avr. 2022
 *      Author: dimercur
 */

#include "protocol.h"
#include "xbee.h"

#include "stdlib.h"
#include "string.h"

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

#define DEBUG_PROTOCOL				1
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
#if DEBUG_PROTOCOL==1
PROTOCOL_Status PROTOCOL_Connect(PROTOCOL_ConfigurationTypedef* conf) {
	int status=PROTOCOL_OK;

	if (XBEE_SetChannel(DEBUG_CHANNEL) != XBEE_OK) {
		status= PROTOCOL_RX_HW_ERROR;
	}

	if (XBEE_SetPanID(DEBUG_PANID) != XBEE_OK) {
		status= PROTOCOL_RX_HW_ERROR;
	}

	HAL_Delay(50);

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
#endif /* #if DEBUG_PROTOCOL==1 */

PROTOCOL_Status PROTOCOL_ParseConfiguration(PROTOCOL_ConfigurationTypedef* conf, char* ptr) {
	PROTOCOL_Status status=PROTOCOL_OK;
	char time[9];
	int i;

	ptr = strtok(NULL, PROTOCOL_SEPARATOR); // ptr should be on current time

	i=0;
	while (ptr[i]!='|') {
		time[i]=ptr[i];
		i++;
	}
	time[i]=0;



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
	if (XBEE_SendData(XBEE_BROADCAST_ADDRESS, 1, XBEE_NO_PANID_BROADCAST, PROTOCOL_CMD_SETUP, &transmit_status)!= XBEE_OK) {
		status= PROTOCOL_RX_HW_ERROR;
	}

	if (transmit_status !=XBEE_TX_STATUS_SUCCESS ) {
		// here we should increment no ack counter for report
		// not good to have iether no ack or CCA failure
	}

	// Wait for answer
	/* Wait for OK, ERR or no answer */
#if DEBUG_PROTOCOL == 1
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

PROTOCOL_Status PROTOCOL_GetCalendar(PROTOCOL_CalendarTypedef* calendar) {

	return PROTOCOL_OK;
}

//PROTOCOL_Status PROTOCOL_GetCalendarUpdateStatus(uint8_t* status) {
//
//	return PROTOCOL_OK;
//}

PROTOCOL_Status PROTOCOL_SendReport(void) {

	return PROTOCOL_OK;
}



