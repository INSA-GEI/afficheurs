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

#define PROTOCOL_SEPARATOR			'|'

int PROTOCOL_SendCommand(char* cmd) {

}

int PROTOCOL_GetAnswer(char* ans) {

}

int PROTOCOL_Init(void) {
	uint8_t status;
	uint16_t panid;
	uint8_t rssi;
	uint64_t uid;

	/* First init XBEE */
	if (XBEE_Init() != XBEE_OK)
		return PROTOCOL_XBEE_INIT_ERROR;

//	/* test la com avec le XBEE */
//	if (XBEE_SendData(0x00FFFF, 1, SET, "Salut gars!", &status) != XBEE_OK)
//		return PROTOCOL_RX_TIMEOUT;
//
//	if (XBEE_SetPanID(0x9876) != XBEE_OK)
//		return PROTOCOL_RX_TIMEOUT;
//
//	if (XBEE_GetATValueU16("ID", &panid, &status) != XBEE_OK)
//		return PROTOCOL_RX_TIMEOUT;
//
//	rssi = 0;
//	if (XBEE_GetRSSI(&rssi) != XBEE_OK)
//		return PROTOCOL_RX_TIMEOUT;
//
//	uid = 0;
//	if (XBEE_GetUID(&uid) != XBEE_OK)
//		return PROTOCOL_RX_TIMEOUT;

	return PROTOCOL_OK;
}


