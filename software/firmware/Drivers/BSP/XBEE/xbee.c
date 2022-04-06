/*
 * xbee.c
 *
 *  Created on: 4 avr. 2022
 *      Author: dimercur
 */


#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "xbee.h"
#include "xbee_ll.h"

uint16_t XBEE_panID;
uint8_t XBEE_chanID;
uint64_t XBEE_uid;

int XBEE_EnterCommandMode() {
	char buffer[5];
	uint8_t index;

	for (index=0; index < 5; index++)
		buffer[index]=0;

	/* Enter AT command mode */
	if (XBEE_LL_SendData("+++",strlen("+++")) != XBEE_LL_OK)
		return XBEE_CONFIG_ERROR;

	if (XBEE_LL_ReceiveData(buffer, 3) != XBEE_LL_OK)
		return XBEE_CONFIG_ERROR;

	if (strcmp(buffer,"OK\r")!=0) // error setting configuration mode
		return XBEE_CONFIG_ERROR;

	return XBEE_OK;
}

int XBEE_SetATCommand(char* atcmd) {
	char buffer[5];
	uint8_t index;

	for (index=0; index < 5; index++)
		buffer[index]=0;

	/* Send AT command  */
	if (XBEE_LL_SendData(atcmd,strlen(atcmd)) != XBEE_LL_OK)
		return XBEE_AT_CMD_ERROR;

	if (XBEE_LL_ReceiveData(buffer, 3) != XBEE_LL_OK)
		return XBEE_AT_CMD_ERROR;

	if (strcmp(buffer,"OK\r")!=0) // error setting configuration mode
		return XBEE_AT_CMD_ERROR;

	return XBEE_OK;
}

int XBEE_GetATValue(char* atcmd, char* value) {
	char tmp;
	uint8_t index;

	for (index=0; index < 20; index++)
		value[index]=0;

	/* Send AT Cmd */
	if (XBEE_LL_SendData(atcmd,strlen(atcmd)) != XBEE_LL_OK)
		return XBEE_AT_CMD_ERROR;

	/* Problem. We don't know the length of received data, only that answer end with \n
	 * So, we must loop until we have received all data
	 */
	tmp = 0;
	index=0;

	while (tmp!='\r') {
		if (XBEE_LL_ReceiveData(&tmp, 1) != XBEE_LL_OK)
			return XBEE_AT_CMD_ERROR;

		if (index >=20 ) return XBEE_AT_CMD_ERROR; // too much data received
		value[index] = (char)tmp;
		index++;
	}

	return XBEE_OK;
}

int XBEE_ConfigureDevice() {
#define RXBUFFERSIZE 30
	char aRxBuffer[RXBUFFERSIZE];
	uint32_t uid_sh, uid_sl;
	char*endPtr;

	if (XBEE_EnterCommandMode()!=XBEE_OK)
		return XBEE_CONFIG_ERROR;

	/* Now configure the device
	 * 1 - Enable API mode (AP)
	 * 2 - Change baudrate to 57600 (BD)
	 * 3 - Get UID of Device (SH+SL)
	 * 4 - End AT command mode (CN)
	 */

	// Set API mode
	if (XBEE_SetATCommand("ATAP=1\r")!=XBEE_OK)
		return XBEE_CONFIG_ERROR;

	// Set baudrate to 115200
	if (XBEE_SetATCommand("ATBD=7\r")!=XBEE_OK)
		return XBEE_CONFIG_ERROR;

	// Get device UID
	if (XBEE_GetATValue("ATSH\r", aRxBuffer)!=XBEE_OK)
		return XBEE_CONFIG_ERROR;

	// Data should contains upper part of 64-bit UID, in hex format
	uid_sh=strtol( aRxBuffer, &endPtr, 16);
	XBEE_uid = ((uint64_t)uid_sh)<<32;

	// Get device UID
	if (XBEE_GetATValue("ATSL\r", aRxBuffer)!=XBEE_OK)
		return XBEE_CONFIG_ERROR;

	// Data should contains upper part of 64-bit UID, in hex format
	uid_sl=strtol( aRxBuffer, &endPtr, 16);
	XBEE_uid = XBEE_uid+ uid_sl;

	return XBEE_OK;
}

int XBEE_Init (UART_HandleTypeDef* uart) {
	/* First, init GPIO */
	XBEE_LL_ConfigureGPIO();

	/* Then, setup usart at 9600 bauds and configure the device */
	XBEE_LL_ConfigureUart(XBEE_USART, 9600);
	if (XBEE_ConfigureDevice() != XBEE_OK)
	{
		return XBEE_CONFIG_ERROR;
	}

	/* If it is OK, reconf USART to 115200 bauds */
	XBEE_LL_ConfigureUart(XBEE_USART, 115200);

	/* Xbee module is ready to be used */
	return XBEE_OK;
}

