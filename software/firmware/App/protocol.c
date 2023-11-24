#include "protocol.h"
#include "xbee.h"

#include "calendar.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*
 * *****************************************
 * *****************************************
 */

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

#define MESSAGE_BATTERIE_HS         "BATTERIE HORS SERVICE"

#define PROTOCOL_SEPARATOR			"|"

#define CAL_UPDATE       			"UPDATE"
#define CAL_NO_UPDATE       		"NO_UPDATE"

#define CAL_DAY_CONFIG				"DAY"
#define CAL_WEEK_CONFIG 			"WEEK"

#define PROTOCOL_CONNECT_TIMEOUT	200

/*
 * *****************************************
 * *****************************************
 */

#define FLASH_USER_START_ADDR      ADDR_FLASH_PAGE_121
#define FLASH_USER_END_ADDR        ADDR_FLASH_PAGE_127 /* 12000*/
#define FLASH_USER_START_ADDR2     ADDR_FLASH_PAGE_120
#define FLASH_USER_END_ADDR2       ADDR_FLASH_PAGE_121 /* 2000 */


uint32_t FirstPage = 0;
uint32_t NbOfPages = 0;
uint32_t Address = 0;
uint32_t BankNumber = 0;
uint32_t PAGEError = 0;

static FLASH_EraseInitTypeDef EraseInitStruct;

static uint32_t GetPage(uint32_t Address);
static uint32_t GetBank(uint32_t Address);
void copy_u64(uint64_t *buf, char* data);


uint64_t buffer;
uint32_t length;

char* txt;
char* txtCopy;


char* txt2;
char* txtCopy2;


/*
 * *****************************************
 * *****************************************
 */

PROTOCOL_Status PROTOCOL_Init(PROTOCOL_ConfigurationTypedef* conf)
{
	if (XBEE_Init() != XBEE_OK)
		return PROTOCOL_XBEE_INIT_ERROR;

	return PROTOCOL_OK;
}

/*
 * *****************************************
 * *****************************************
 */

PROTOCOL_Status PROTOCOL_Connect(PROTOCOL_ConfigurationTypedef* conf)
{
	PROTOCOL_Status status = PROTOCOL_OK;
	int com_status;
	uint8_t transmit_status;
	XBEE_GENERIC_FRAME *rx_frame;
	char* data;

	vTaskDelay(msToTicks(100));

	if (XBEE_SendData_Zigbee(ZIGBEE_SERVER_ADDR, 1, PROTOCOL_CMD_JOIN, &transmit_status)!= XBEE_OK)
	{
		status= PROTOCOL_RX_HW_ERROR;
		return status;
	}

	com_status = XBEE_GetData(&rx_frame, 5000);

	if (com_status == XBEE_RX_ERROR)
	{
		status= PROTOCOL_RX_HW_ERROR;
		return status;
	}
	else if (com_status == XBEE_RX_TIMEOUT)
	{
		status= PROTOCOL_NOT_CONNECTED;
		return status;
	}

	if ((com_status != XBEE_INVALID_FRAME))
	{
		if ((rx_frame->type == XBEE_RX_PACKET_TYPE) || (rx_frame->type == XBEE_RX_EXPLICIT_TYPE))
		{
			data = ((XBEE_RX_PACKET_FRAME*)rx_frame)->data;

			if (strcmp((const char*)data, PROTOCOL_ANS_ACCEPT)==0)
			{
				char *ptr = strtok(data, PROTOCOL_SEPARATOR);
				ptr = strtok(NULL, PROTOCOL_SEPARATOR);

				if (ptr!=NULL)
					free (rx_frame);
			}
			else
			{
				status= PROTOCOL_NOT_CONNECTED;
			}
		}

		free (rx_frame);
	}

	return status;
}


/*
 * *****************************************
 * *****************************************
 */

static PROTOCOL_Status PROTOCOL_ParseConfiguration(PROTOCOL_ConfigurationTypedef* conf, char* ptr)
{
	PROTOCOL_Status status = PROTOCOL_OK;

	char currentTime[PROTOCOL_TIME_STRING_MAX_LENGTH+1];
	char currentDate[PROTOCOL_TIME_STRING_MAX_LENGTH+1];
	char starttime[PROTOCOL_TIME_STRING_MAX_LENGTH+1];
	char endtime[PROTOCOL_TIME_STRING_MAX_LENGTH+1];
	char* parsePtr;

	/* INFO 1 : TIME RTC */
	ptr = strtok(NULL, PROTOCOL_SEPARATOR);
	if (ptr == NULL)
		return PROTOCOL_INVALID_FRAME;
	else
	{
		strncpy(currentTime, ptr, PROTOCOL_TIME_STRING_MAX_LENGTH);
		currentTime[PROTOCOL_TIME_STRING_MAX_LENGTH] = 0;
	}

	/* INFO 2 : DATE RTC */
	ptr = strtok(NULL, PROTOCOL_SEPARATOR);
	if (ptr == NULL)
		return PROTOCOL_INVALID_FRAME;
	else
	{
		strncpy(currentDate, ptr, PROTOCOL_TIME_STRING_MAX_LENGTH);
		currentDate[PROTOCOL_TIME_STRING_MAX_LENGTH] = 0;
	}

	/* INFO 3 : DAY */
	ptr = strtok(NULL, PROTOCOL_SEPARATOR);
	if (ptr == NULL)
		return PROTOCOL_INVALID_FRAME;
	else
		conf->init_week_day=(uint8_t)strtoul(ptr,NULL,10);

	/* INFO 4 : ROOM NAME  */
	ptr = strtok(NULL, PROTOCOL_SEPARATOR);
	if (ptr == NULL)
		return PROTOCOL_INVALID_FRAME;
	else
	{
		strncpy(conf->room_name, ptr, PROTOCOL_ROOM_NAME_LENGTH);
		conf->room_name[PROTOCOL_ROOM_NAME_LENGTH]= 0;
	}

	/* INFO 5 : ROOM TYPE */
	ptr = strtok(NULL, PROTOCOL_SEPARATOR);
	if (ptr == NULL)
		return PROTOCOL_INVALID_FRAME;
	else
	{
		strncpy(conf->room_type, ptr, PROTOCOL_ROOM_TYPE_LENGTH);
		conf->room_type[PROTOCOL_ROOM_TYPE_LENGTH]=0;
	}

	/* INFO 7 : START TIME */
	ptr = strtok(NULL, PROTOCOL_SEPARATOR);
	if (ptr == NULL)
		return PROTOCOL_INVALID_FRAME;
	else
	{
		strncpy(starttime, ptr, PROTOCOL_TIME_STRING_MAX_LENGTH);
		starttime[PROTOCOL_TIME_STRING_MAX_LENGTH]=0;
	}


	/* INFO 8 : END TIME */
	ptr = strtok(NULL, PROTOCOL_SEPARATOR);
	if (ptr == NULL)
		return PROTOCOL_INVALID_FRAME;
	else
	{
		strncpy(endtime, ptr, PROTOCOL_TIME_STRING_MAX_LENGTH);
		endtime[PROTOCOL_TIME_STRING_MAX_LENGTH]=0;
	}

	/* INFO 9 : UPDATE DELAY */
	ptr = strtok(NULL, PROTOCOL_SEPARATOR);
	if (ptr == NULL)
		return PROTOCOL_INVALID_FRAME;
	else
		conf->update_delay=(uint16_t)strtoul(ptr,NULL,10);

	/* INFO 10 : ORDER NUMBER */
	ptr = strtok(NULL, PROTOCOL_SEPARATOR);
	if (ptr == NULL)
		return PROTOCOL_INVALID_FRAME;
	else
		conf->order_nbr=(uint8_t)strtoul(ptr,NULL,10);


	/* finally, parse time string */
	// Current time
	parsePtr = strtok(currentTime, ":");
	conf->init_time_hours=(uint32_t)(strtoul(parsePtr,NULL,10));
	parsePtr = strtok(NULL, ":");
	conf->init_time_minutes=(uint32_t)(strtoul(parsePtr,NULL,10));
	parsePtr = strtok(NULL, ":");
	conf->init_time_seconds=(uint32_t)strtoul(parsePtr,NULL,10);


	// Current date
	parsePtr = strtok(currentDate, "/");
	conf->init_date_day=(uint16_t)(strtoul(parsePtr,NULL,10));
	parsePtr = strtok(NULL, "/");
	conf->init_date_month=(uint16_t)strtoul(parsePtr,NULL,10);
	parsePtr = strtok(NULL, "/");
	conf->init_date_year=(uint16_t)strtoul(parsePtr,NULL,10);


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

/*
 * *****************************************
 * *****************************************
 */

PROTOCOL_Status PROTOCOL_SendMessageBattery(PROTOCOL_ConfigurationTypedef* conf)
{
	PROTOCOL_Status status = PROTOCOL_OK;
	uint8_t transmit_status;

	if (XBEE_SendData_Zigbee(ZIGBEE_SERVER_ADDR, 1, MESSAGE_BATTERIE_HS, &transmit_status)!= XBEE_OK)
	{
		status= PROTOCOL_RX_HW_ERROR;
	}

	return status;
}
/*
 * *****************************************
 * *****************************************
 */

PROTOCOL_Status PROTOCOL_GetConfiguration(PROTOCOL_ConfigurationTypedef* conf)
{
	int com_status;
	PROTOCOL_Status status=PROTOCOL_OK;
	XBEE_GENERIC_FRAME *rx_frame;
	uint8_t transmit_status;
	char* data;
	char* ptr;


	/* Send a "SETUP" command */
	if (XBEE_SendData_Zigbee(ZIGBEE_SERVER_ADDR, 1, PROTOCOL_CMD_SETUP, &transmit_status)!= XBEE_OK)
	{
		status= PROTOCOL_RX_HW_ERROR;
	}

	if (transmit_status !=XBEE_TX_STATUS_SUCCESS )
	{
		conf->rssi.no_ack++;
	}

	com_status = XBEE_GetData(&rx_frame, 5000);

	if (com_status == XBEE_RX_ERROR)
	{
		status= PROTOCOL_RX_HW_ERROR;
	}
	else if (com_status == XBEE_RX_TIMEOUT)
	{
		status = PROTOCOL_RX_TIMEOUT;
	}

	if ((rx_frame->type == XBEE_RX_PACKET_TYPE) || (rx_frame->type == XBEE_RX_EXPLICIT_TYPE))
	{
		data = ((XBEE_RX_PACKET_FRAME*)rx_frame)->data;

	    /*-------------------------------------------------------------------------------------------------------------------------------*/
		/*-------------------------------------------------------------------------------------------------------------------------------*/
		/*-------------------------------------------------------------------------------------------------------------------------------*/
		/*-------------------------------------------------------------------------------------------------------------------------------*/
		/*-------------------------------------------------------------------------------------------------------------------------------*/
		/*-------------------------------------------------------------------------------------------------------------------------------*/
		HAL_FLASH_Unlock();


		__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
		FirstPage = GetPage(FLASH_USER_START_ADDR2);
		NbOfPages = GetPage(FLASH_USER_END_ADDR) - FirstPage + 1;
		BankNumber = GetBank(FLASH_USER_START_ADDR2);

		EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
		EraseInitStruct.Banks       = BankNumber;
		EraseInitStruct.Page        = FirstPage;
		EraseInitStruct.NbPages     = NbOfPages;

		if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
		{
		    while (1)
		    {
		    }
		}


		length = strlen(data);

		if(length > 2000)
		{
			status = PROTOCOL_RX_HW_ERROR;
			return status;
		}

		Address = FLASH_USER_START_ADDR2;

		for (int i = 0; i <= length ;)
		{
				copy_u64(&buffer, data + i);
				if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Address, buffer) == HAL_OK)
				{
					Address = Address + 8;
					i = i + 8;
				}
				else
				{
					while (1)
					{

					}
				}
		}

	    HAL_FLASH_Lock();

		/*-------------------------------------------------------------------------------------------------------------------------------*/
		/*-------------------------------------------------------------------------------------------------------------------------------*/
		/*-------------------------------------------------------------------------------------------------------------------------------*/
		/*-------------------------------------------------------------------------------------------------------------------------------*/
		/*-------------------------------------------------------------------------------------------------------------------------------*/

		ptr = strtok(data, PROTOCOL_SEPARATOR);

		if (strcmp (ptr, PROTOCOL_ANS_OK)==0)
		{
			status = PROTOCOL_ParseConfiguration(conf, ptr);
		}
		else if (strcmp (ptr, PROTOCOL_ANS_ERR)==0)
		{
			ptr = strtok(NULL, PROTOCOL_SEPARATOR);
			conf->last_error = (uint16_t)strtol(ptr,NULL, 10);
			status = PROTOCOL_RX_CMD_ERROR;
		}
		else
		{
			status= PROTOCOL_INVALID_FRAME;
		}
	}

	free (rx_frame);
	return status;
}

/*
 * *****************************************
 * *****************************************
 */

PROTOCOL_Status PROTOCOL_GetFlashConfig(PROTOCOL_ConfigurationTypedef* conf)
{
	PROTOCOL_Status status=PROTOCOL_OK;

	Address = FLASH_USER_START_ADDR2;

	char *ptr;

	txt2 = ((char*)Address);
    length = strlen(txt2);

	txtCopy2 = (char *) malloc(length);
	strcpy(txtCopy2, txt2);

	ptr = strtok(txtCopy2, PROTOCOL_SEPARATOR);

	if (strcmp (ptr, PROTOCOL_ANS_OK)==0)
	{
		status = PROTOCOL_ParseConfiguration(conf, ptr);
	}
	else if (strcmp (ptr, PROTOCOL_ANS_ERR)==0)
	{
		ptr = strtok(NULL, PROTOCOL_SEPARATOR);
		conf->last_error = (uint16_t)strtol(ptr,NULL, 10);
		status = PROTOCOL_RX_CMD_ERROR;
	}
	else
	{
		status= PROTOCOL_INVALID_FRAME;
	}
	return status;
}

/*
 * *****************************************
 * *****************************************
 */

typedef struct pCalEventList
{
	char* event;
	struct pCalEventList* nextevent;
} PROTOCOL_CalEventList;

/*
 * *****************************************
 * *****************************************
 */

static int PROTOCOL_ParseCalendar(char* str)
{
	char* datelist =NULL;
	char* traineesptr=NULL;
	char* trainersptr = NULL;
	char* ptr;
	int i;
	int length;

	PROTOCOL_CalEventList* headptr = NULL;
	PROTOCOL_CalEventList* indexptr=NULL;

	ptr = strtok(str, PROTOCOL_SEPARATOR);
	if (ptr == NULL)
		return PROTOCOL_INVALID_FRAME;
	else
		datelist = ptr;

	// 1st step: split string in reservation string
	ptr = strtok(NULL, PROTOCOL_SEPARATOR);
	ptr = strtok(ptr, "#");

	while (ptr!=NULL) {
		if (headptr == NULL)
		{
			headptr = (PROTOCOL_CalEventList*)malloc(sizeof(PROTOCOL_CalEventList));
			indexptr = headptr;
		}
		else
		{
			indexptr->nextevent = (PROTOCOL_CalEventList*)malloc(sizeof(PROTOCOL_CalEventList));
			indexptr = indexptr->nextevent;
		}

		indexptr->nextevent = NULL;
		indexptr->event = ptr;


		ptr = strtok(NULL, "#");
	};

	// 2nd step: split week date
	ptr = strtok(datelist, ";");

	CAL_Day* days;
	days = CAL_GetDayArray();

	for (i=0; i<7 ; i++)
	{
		if (ptr != NULL)
		{
			strncpy(days[i], ptr, CAL_DATE_STRING_MAX_LENGTH);
			days[i][CAL_DATE_STRING_MAX_LENGTH]=0;
		}

		ptr = strtok(NULL, ";");
	}

	// 3rd step: split events
	CAL_Reservation* reservation;

	while (headptr!=NULL) {
		reservation = CAL_NewReservation();

		ptr = strtok(headptr->event, ";");
		if (ptr!=NULL)
			reservation->day_nbr = (uint8_t)strtoul(ptr,NULL,10);

		ptr = strtok(NULL, ";");
		if (ptr!=NULL)
			reservation->start_time = (uint16_t)strtoul(ptr,NULL,10);

		ptr = strtok(NULL, ";");
		if (ptr!=NULL)
			reservation->end_time = (uint16_t)strtoul(ptr,NULL,10);

		ptr = strtok(NULL, ";");
		if (ptr!=NULL) {
			length = strlen(ptr);
			if (length > CAL_TITLE_MAX_LENGTH)
				length = CAL_TITLE_MAX_LENGTH;

			reservation->title = (char*)malloc(length+1);
			strncpy(reservation->title, ptr, length);
			reservation->title[length]=0;
		}

		ptr = strtok(NULL, ";");
		traineesptr= ptr;
		ptr = strtok(NULL, ";");
		trainersptr= ptr;

		if (traineesptr!=NULL) {
			ptr = strtok(traineesptr, ",");
			i=0;
			while ((ptr != NULL) && (i<4))
			{
				length = strlen(ptr);
				if (length> CAL_TRAINEE_MAX_LENGTH)
					length = CAL_TRAINEE_MAX_LENGTH;

				reservation->trainees[i]=(char*)malloc(length);
				strncpy(reservation->trainees[i], ptr, length);
				reservation->trainees[i][length]=0;

				i++;
				ptr = strtok(NULL, ",");
			}
		}

		if (trainersptr!=NULL) {
			ptr = strtok(trainersptr, ",");
			i=0;
			while ((ptr != NULL) && (i<4)) {
				length = strlen(ptr);
				if (length> CAL_TRAINER_MAX_LENGTH)
					length = CAL_TRAINER_MAX_LENGTH;

				reservation->trainers[i]=(char*)malloc(length);
				strncpy(reservation->trainers[i], ptr, length);
				reservation->trainers[i][length]=0;

				i++;
				ptr = strtok(NULL, ",");
			}
		}

		indexptr= headptr;
		headptr= headptr->nextevent;
		free((void*)indexptr);
	}

	// 4th step: free the main string
	free((void*)str);

	return PROTOCOL_OK;
}

/*
 * *****************************************
 * *****************************************
 */

typedef struct pCalFrameList
{
	XBEE_RX_PACKET_FRAME* frame;
	struct pCalFrameList* nextframe;
} PROTOCOL_CalFrameList;

/*
 * *****************************************
 * *****************************************
 */

PROTOCOL_Status PROTOCOL_GetCalendar(PROTOCOL_ConfigurationTypedef* conf)
{
	int com_status;	PROTOCOL_Status status=PROTOCOL_OK;
	XBEE_GENERIC_FRAME *rx_frame;
	uint8_t transmit_status;
	char* data;
	char *ptr;
	int end_rx=0;

	PROTOCOL_CalFrameList* framelisthead=NULL;
	PROTOCOL_CalFrameList* framelistindex=NULL;

	/* Send a "CAL" command */
	if (XBEE_SendData_Zigbee(ZIGBEE_SERVER_ADDR, 1, PROTOCOL_CMD_GET_CALENDAR, &transmit_status)!= XBEE_OK)
	{
		status= PROTOCOL_RX_HW_ERROR;
	}

	if (transmit_status !=XBEE_TX_STATUS_SUCCESS )
	{
		conf->rssi.no_ack++;
	}

	// Wait for answer
	/* Wait for OK, ERR, END or no answer */
	while (!end_rx)
	{
		com_status = XBEE_GetData(&rx_frame, 5000);
		if (com_status == XBEE_RX_ERROR)
		{
			status= PROTOCOL_RX_HW_ERROR;
			end_rx=1;
		}
		else if (com_status == XBEE_RX_TIMEOUT)
		{
			status = PROTOCOL_RX_TIMEOUT;
			end_rx=1;
		}
		else if (com_status == XBEE_INVALID_FRAME)
		{
			status = PROTOCOL_RX_CMD_ERROR;
		}
		else
		{
			if ((rx_frame->type == XBEE_RX_PACKET_TYPE) || (rx_frame->type == XBEE_RX_EXPLICIT_TYPE))
			{
				data = ((XBEE_RX_PACKET_FRAME*)rx_frame)->data;

				ptr = strtok(data, PROTOCOL_SEPARATOR);
				if (strcmp (ptr, PROTOCOL_ANS_ERR)==0)
				{ // answer is "ERR"
					ptr = strtok(NULL, PROTOCOL_SEPARATOR);
					conf->last_error = (uint16_t)strtol(ptr,NULL, 10);
					status = PROTOCOL_RX_CMD_ERROR;
					end_rx=1;

					free(rx_frame);
				}
				else if (strcmp (ptr, PROTOCOL_ANS_END)==0)
				{
					end_rx=1;
					free(rx_frame);
				}
				else
				{ // its an acceptable frame
					if (framelisthead == NULL)
					{
						framelisthead = (PROTOCOL_CalFrameList*)malloc(sizeof(PROTOCOL_CalFrameList));
						framelistindex=framelisthead;
					}
					else
					{
						framelistindex->nextframe = (PROTOCOL_CalFrameList*)malloc(sizeof(PROTOCOL_CalFrameList));
						framelistindex = framelistindex->nextframe;
					}

					framelistindex->frame=(XBEE_RX_PACKET_FRAME*)rx_frame;
					framelistindex->nextframe=NULL;
				}
			}
			else
			{
				free(rx_frame);
			}
		}
	}

	if ((status == PROTOCOL_OK) || (status == PROTOCOL_RX_TIMEOUT))
	{
		int string_length=0;
		framelistindex = framelisthead;

		while (framelistindex!=NULL)
		{
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
		while (framelisthead!=NULL)
		{
			if (strstr(framelisthead->frame->data,"OK"))
			{
				ptr=strncpy(ptr,framelisthead->frame->data+3, framelisthead->frame->data_length-3);// move data 3 chars ahead, to step over "OK" and find start of PDU
				framelistindex = framelisthead;
				ptr= ptr+(framelisthead->frame->data_length-3); // go to end of string 0 ending
			}
			else
			{
				ptr=strncpy(ptr,framelisthead->frame->data, framelisthead->frame->data_length);// move data 3 chars ahead, to step over "OK" and find start of PDU
				framelistindex = framelisthead;
				ptr= ptr+(framelisthead->frame->data_length); // go to end of string 0 ending
			}

			free ((void*)framelisthead->frame);
			framelisthead = framelisthead->nextframe;
			free ((void*)framelistindex);
		}

		    /*-------------------------------------------------------------------------------------------------------------------------------*/
			/*-------------------------------------------------------------------------------------------------------------------------------*/
			/*-------------------------------------------------------------------------------------------------------------------------------*/
			/*-------------------------------------------------------------------------------------------------------------------------------*/
			/*-------------------------------------------------------------------------------------------------------------------------------*/
			/*-------------------------------------------------------------------------------------------------------------------------------*/
			HAL_FLASH_Unlock();


			__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
			FirstPage = GetPage(FLASH_USER_START_ADDR);
			NbOfPages = GetPage(FLASH_USER_END_ADDR) - FirstPage + 1;
			BankNumber = GetBank(FLASH_USER_START_ADDR);

			EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
			EraseInitStruct.Banks       = BankNumber;
			EraseInitStruct.Page        = FirstPage;
			EraseInitStruct.NbPages     = NbOfPages;

			if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
			{
			    while (1)
			    {
			    }
			}


			length = strlen(data);

			if(length > 12000)
			{
				status = PROTOCOL_RX_HW_ERROR;
				return status;
			}

			Address = FLASH_USER_START_ADDR;

			for (int i = 0; i <= length ;)
			{
					copy_u64(&buffer, data + i);
					if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Address, buffer) == HAL_OK)
					{
						Address = Address + 8;
						i = i + 8;
					}
					else
					{
						while (1)
						{

						}
					}
			}

		    HAL_FLASH_Lock();

			/*-------------------------------------------------------------------------------------------------------------------------------*/
			/*-------------------------------------------------------------------------------------------------------------------------------*/
			/*-------------------------------------------------------------------------------------------------------------------------------*/
			/*-------------------------------------------------------------------------------------------------------------------------------*/
			/*-------------------------------------------------------------------------------------------------------------------------------*/
			/*-------------------------------------------------------------------------------------------------------------------------------*/

		status=PROTOCOL_ParseCalendar(data);
	}

	return status;
}


/*
 * *****************************************
 * *****************************************
 */

PROTOCOL_Status PROTOCOL_GetFlashCalendar()
{
	PROTOCOL_Status status=PROTOCOL_OK;

	Address = FLASH_USER_START_ADDR;


	txt = ((char*)Address);
    length = strlen(txt);

	txtCopy = (char *) malloc(length);
	strcpy(txtCopy, txt);

	status = PROTOCOL_ParseCalendar(txtCopy);

	return status;
}


/*
 * *****************************************
 * *****************************************
 */

PROTOCOL_Status PROTOCOL_GetCalendarUpdateStatus(PROTOCOL_ConfigurationTypedef* conf, CALENDAR_Status* update_status)
{
	int com_status;
	PROTOCOL_Status status = PROTOCOL_OK;
	XBEE_GENERIC_FRAME *rx_frame;
	uint8_t transmit_status;
	char *data;
	char *ptr;

	/* Send a "UPDATE" command */
	if (XBEE_SendData_Zigbee(ZIGBEE_SERVER_ADDR, 1, PROTOCOL_CMD_GET_UPDATE, &transmit_status)!= XBEE_OK)
	{
		status= PROTOCOL_RX_HW_ERROR;
	}

	if (transmit_status !=XBEE_TX_STATUS_SUCCESS )
	{
		// here we should increment no ack counter for report
		// not good to have either no ack or CCA failure
		conf->rssi.no_ack++;
	}

	// Wait for answer
	/* Wait for OK, ERR or no answer */
	com_status = XBEE_GetData(&rx_frame, DEBUG_PROTOCOL_TIMEOUT);

	if (com_status == XBEE_RX_ERROR)
	{
		status= PROTOCOL_RX_HW_ERROR;
	}
	else if (com_status == XBEE_RX_TIMEOUT)
	{
		status = PROTOCOL_RX_TIMEOUT;
	}

	if ((rx_frame->type == XBEE_RX_PACKET_TYPE) || (rx_frame->type == XBEE_RX_EXPLICIT_TYPE))
	{
		data = ((XBEE_RX_PACKET_FRAME*)rx_frame)->data;

		ptr = strtok(data, PROTOCOL_SEPARATOR); // ptr is on first word

		//check if it is OK or ERR
		if (strcmp (ptr, PROTOCOL_ANS_OK) == 0)
		{ // answer is "OK"
			ptr = strtok(NULL, PROTOCOL_SEPARATOR);
			if (ptr==NULL)
				status = PROTOCOL_INVALID_FRAME;
			else
				if ((strcmp (ptr, CAL_UPDATE) == 0)) //update_status = 1 UPDATE NEEDED
				*(update_status) = CALENDAR_UPDATE;
				else if ((strcmp (ptr, CAL_NO_UPDATE) == 0))
				*(update_status) = CALENDAR_NO_UPDATE; //update_status = 0 UPDATE NEEDED
				else
				status = PROTOCOL_INVALID_FRAME;
		}
		else if (strcmp (ptr, PROTOCOL_ANS_ERR)==0)
		{ // answer is "ERR"
			ptr = strtok(NULL, PROTOCOL_SEPARATOR);
			conf->last_error = (uint16_t)strtol(ptr,NULL, 10);
			status = PROTOCOL_RX_CMD_ERROR;
		} else
		{ // unknown answer
			status= PROTOCOL_INVALID_FRAME;
		}
	}

	free (rx_frame);
	return status;
}

/*
 * *****************************************
 * *****************************************
 */

PROTOCOL_Status PROTOCOL_GetLastPrintDisplay(PROTOCOL_ConfigurationTypedef* conf, DISPLAY_Status* display_status)
{
	int com_status;
	PROTOCOL_Status status = PROTOCOL_OK;
	XBEE_GENERIC_FRAME *rx_frame;
	uint8_t transmit_status;
	char *data;
	char *ptr;

	/* Send a "UPDATE" command */
	if (XBEE_SendData_Zigbee(ZIGBEE_SERVER_ADDR, 1, PROTOCOL_CMD_GET_UPDATE, &transmit_status)!= XBEE_OK)
	{
		status= PROTOCOL_RX_HW_ERROR;
	}

	if (transmit_status !=XBEE_TX_STATUS_SUCCESS )
	{
		// here we should increment no ack counter for report
		// not good to have either no ack or CCA failure
		conf->rssi.no_ack++;
	}

	// Wait for answer
	/* Wait for OK, ERR or no answer */
	com_status = XBEE_GetData(&rx_frame, DEBUG_PROTOCOL_TIMEOUT);

	if (com_status == XBEE_RX_ERROR)
	{
		status= PROTOCOL_RX_HW_ERROR;
	}
	else if (com_status == XBEE_RX_TIMEOUT)
	{
		status = PROTOCOL_RX_TIMEOUT;
	}

	if ((rx_frame->type == XBEE_RX_PACKET_TYPE) || (rx_frame->type == XBEE_RX_EXPLICIT_TYPE))
	{
		data = ((XBEE_RX_PACKET_FRAME*)rx_frame)->data;

		ptr = strtok(data, PROTOCOL_SEPARATOR); // ptr is on first word

		//check if it is OK or ERR
		if (strcmp (ptr, PROTOCOL_ANS_OK) == 0)
		{ // answer is "OK"
			ptr = strtok(NULL, PROTOCOL_SEPARATOR);
			if (ptr==NULL)
				status = PROTOCOL_INVALID_FRAME;
			else
				if ((strcmp (ptr, CAL_DAY_CONFIG) == 0)) //display_status = 0 CURRENTLY DAY CONFIG
				*(display_status) = CALENDAR_DAY;
				else if ((strcmp (ptr, CAL_WEEK_CONFIG) == 0)) //display_status = 1 CURRENTLY WEEK CONFIG
				*(display_status) = CALENDAR_WEEK;
				else
				status = PROTOCOL_INVALID_FRAME;
		}
		else if (strcmp (ptr, PROTOCOL_ANS_ERR)==0)
		{ // answer is "ERR"
			ptr = strtok(NULL, PROTOCOL_SEPARATOR);
			conf->last_error = (uint16_t)strtol(ptr,NULL, 10);
			status = PROTOCOL_RX_CMD_ERROR;
		} else
		{ // unknown answer
			status= PROTOCOL_INVALID_FRAME;
		}
	}

	free (rx_frame);
	return status;
}

/*
 * *****************************************
 * *****************************************
 */

PROTOCOL_Status PROTOCOL_SendReport(PROTOCOL_ConfigurationTypedef* conf)
{
	PROTOCOL_Status status=PROTOCOL_OK;
	uint8_t transmit_status;
	char report_string[32] = {0};
	uint8_t max_rssi=0;
	uint8_t min_rssi=0xFF;
	uint8_t moy_rssi=0;
	uint8_t index;

	for (index=0; index<=conf->rssi.index; index++)
	{
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
	if (XBEE_SendData(XBEE_BROADCAST_ADDRESS, 1, XBEE_NO_PANID_BROADCAST, report_string, &transmit_status)!= XBEE_OK) {
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

/*
 * *****************************************
 * *****************************************
 */

static uint32_t GetPage(uint32_t Addr)
{
  uint32_t page = 0;

  if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
  {
    /* Bank 1 */
    page = (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
  }
  else
  {
    /* Bank 2 */
    page = (Addr - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
  }

  return page;
}

/*
 * *****************************************
 * *****************************************
 */

static uint32_t GetBank(uint32_t Addr)
{
  return FLASH_BANK_1;
}

/*
 * *****************************************
 * *****************************************
 */

void copy_u64(uint64_t *buf, char* data)
{
    for (int i=0; i<8; i++)
    {
        *buf = (*buf)>>8;
        *buf |= ((uint64_t)*data)<<56; //64-8
        data++;
    }
}


/*
 * *****************************************
 * *****************************************
 */


