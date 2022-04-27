/*
 * display.c
 *
 *  Created on: 26 avr. 2022
 *      Author: dimercur
 */
#include "stm32l4xx_hal.h"
#include "display.h"

#include "EPD_7in5_V2.h"

#include "DEV_Config.h"
#include "GUI_Paint.h"

#include "string.h"
#include "stdlib.h"
#include "inttypes.h"

DISPLAY_StatusTypedef DISPLAY_Init(void) {
	//printf("EPD_7IN5_V2_test Demo\r\n");
	if(DEV_Module_Init()!=0){
		return DISPLAY_HW_ERR;
	}

	//printf("e-Paper Init and Clear...\r\n");
	EPD_7IN5_V2_Init();

	//printf("Paint_Init\r\n");
	Paint_Init(EPD_7IN5_V2_GetFramebuffer(),EPD_7IN5_V2_WIDTH, EPD_7IN5_V2_HEIGHT, ROTATE_0);
	Paint_SetSendToDisplayFunction(EPD_7IN5_V2_WritePicture);

	return DISPLAY_OK;
}

void DISPLAY_EnterPowerOff(void) {
	DEV_Module_Exit();
}

void DISPLAY_ShowReservation(PROTOCOL_ConfigurationTypedef* conf, CAL_Reservation* reservation, const char* prompt, DISPLAY_PromptIcon icon) {
	char *text;
	char people[((CAL_TRAINER_MAX_LENGTH+2)*4)+1]; // 1 for 0 ending, 2 for ',' and space between name
	char local_prompt[51];
	memset (people, 0,((CAL_TRAINER_MAX_LENGTH+2)*4)+1);

	//1.Select Image
	//printf("Clear framebuffer\r\n");
	Paint_Clear(WHITE);

	// 2.Drawing on the image

	// Draw room number
	text =conf->room_name;
	lv_font_box_t stringBox = Paint_GetStringBox(text, &montserrat_li_150);
	Paint_DrawString(EPD_7IN5_V2_WIDTH-stringBox.width-20, 10, text, &montserrat_li_150, BLACK, WHITE);

	// Draw room type
	text = conf->room_type;
	stringBox = Paint_GetStringBox(text, &play_r_60);
	Paint_DrawString(EPD_7IN5_V2_WIDTH-stringBox.width-20,150,
			text, &play_r_60, BLACK, WHITE);

	// Draw current reservation with two lines
	Paint_DrawLine(420, 150+80, EPD_7IN5_V2_WIDTH-20, 150+80, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
	//Paint_DrawLine(320, 150+81, EPD_7IN5_V2_WIDTH-20, 150+81, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
	Paint_DrawLine(220, 150+82, EPD_7IN5_V2_WIDTH-20, 150+82, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
	//Paint_DrawLine(120, 150+83, EPD_7IN5_V2_WIDTH-20, 150+83, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
	Paint_DrawLine(20, 150+84, EPD_7IN5_V2_WIDTH-20, 150+84, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);

	text = reservation->title;
	stringBox = Paint_GetStringBox(text, &montserrat_m_40);
	Paint_DrawString((EPD_7IN5_V2_WIDTH-stringBox.width)/2,150+85+15,
			text, &montserrat_m_40, BLACK, WHITE);

	Paint_DrawLine(20, 150+155, EPD_7IN5_V2_WIDTH-20, 150+155, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
	//Paint_DrawLine(20, 150+156, EPD_7IN5_V2_WIDTH-120, 150+156, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
	Paint_DrawLine(20, 150+157, EPD_7IN5_V2_WIDTH-220, 150+157, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
	//Paint_DrawLine(20, 150+158, EPD_7IN5_V2_WIDTH-320, 150+158, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
	Paint_DrawLine(20, 150+159, EPD_7IN5_V2_WIDTH-420, 150+159, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);

	// Draw INSA Logo
	Paint_DrawImage(20,(150+80-logoINSA_110.header.h)/2,(lv_img_dsc_t *) &logoINSA_110, DRAW_IMAGE_NORMAL);

	// Draw information about reservation
	uint16_t ypos_top = (150+160) + (((EPD_7IN5_V2_HEIGHT-30)-(150+160)) - 110)/2;
	uint16_t ypos_bottom = (EPD_7IN5_V2_HEIGHT-30) - 48 - (((EPD_7IN5_V2_HEIGHT-30)-(150+160)) - 110)/2;

	Paint_DrawImage(40, ypos_top, (lv_img_dsc_t *) &clock_start_48, DRAW_IMAGE_NORMAL);
	Paint_DrawImage(40, ypos_bottom, (lv_img_dsc_t *) &clock_end_48, DRAW_IMAGE_NORMAL);

	text = CAL_HourToStr(reservation->start_time);
	stringBox = Paint_GetStringBox(text, &montserrat_m_26);
	Paint_DrawString(40 + clock_start_48.header.w +20, ypos_top+11, text, &montserrat_m_26, BLACK, WHITE);

	text = CAL_HourToStr(reservation->end_time);
	stringBox = Paint_GetStringBox(text, &montserrat_m_26);
	Paint_DrawString(40 + clock_end_48.header.w +20, ypos_bottom+11, text, &montserrat_m_26, BLACK, WHITE);

	Paint_DrawImage(300, ypos_top, (lv_img_dsc_t *) &attendees_48, DRAW_IMAGE_NORMAL);
	Paint_DrawImage(300, ypos_bottom, (lv_img_dsc_t *) &teacher_48, DRAW_IMAGE_NORMAL);

	//text = "4AE-SE";
	int i=0;

	while ((i<4) && (reservation->trainees[i]!=0)) {
		strcat (people, reservation->trainees[i]);
		if ((i!=3) && (reservation->trainees[i+1]!=0)) {
			strcat (people, ", ");
		}
		i++;
	}

	if (strlen(people)>32)
		people[32]= 0; // trunc string to only 32 chars

	stringBox = Paint_GetStringBox(people, &montserrat_m_26);
	Paint_DrawString(300 + attendees_48.header.w +20, ypos_top+11, people, &montserrat_m_26, BLACK, WHITE);

	//text = "S. DI MERCURIO";
	memset (people, 0,((CAL_TRAINER_MAX_LENGTH+2)*4)+1); //  flush buffer
	i=0;

	while ((i<4) && (reservation->trainers[i]!=0)) {
		strcat (people, reservation->trainers[i]);
		if ((i!=3) && (reservation->trainers[i+1]!=0)) {
			strcat (people, ", ");
		}
		i++;
	}

	if (strlen(people)>32)
		people[32]= 0; // trunc string to only 32 chars

	stringBox = Paint_GetStringBox(people, &montserrat_m_26);
	Paint_DrawString(300 + attendees_48.header.w +20, ypos_bottom+11, people, &montserrat_m_26, BLACK, WHITE);

	// Draw footer with prompt, connection status and battery state
	Paint_DrawRectangle(0, EPD_7IN5_V2_HEIGHT-30, EPD_7IN5_V2_WIDTH, EPD_7IN5_V2_HEIGHT, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);

	strncpy(local_prompt, prompt, 50);
	local_prompt[50]=0;

	stringBox = Paint_GetStringBox(local_prompt, &montserrat_m_26);
	Paint_DrawString((EPD_7IN5_V2_WIDTH-stringBox.width)/2,EPD_7IN5_V2_HEIGHT-30+(30-26)/2,
			local_prompt, &montserrat_m_26, WHITE, BLACK);

	Paint_DrawImage((EPD_7IN5_V2_WIDTH-stringBox.width)/2 - clock_next_24.header.w - 8,
			(EPD_7IN5_V2_HEIGHT-30 + (30-clock_next_24.header.h)/2),
			(lv_img_dsc_t *) &clock_next_24, DRAW_IMAGE_INVERTED);

	Paint_DrawImage(10,(EPD_7IN5_V2_HEIGHT-30 + (30-connection_mid_24.header.h)/2),
			(lv_img_dsc_t *) &connection_mid_24, DRAW_IMAGE_INVERTED);

	Paint_DrawImage(10+connection_mid_24.header.w+4,(EPD_7IN5_V2_HEIGHT-30 + (30-on_line_24.header.h)/2),
			(lv_img_dsc_t *) &on_line_24, DRAW_IMAGE_INVERTED);

	Paint_DrawImage(EPD_7IN5_V2_WIDTH-10-24,(EPD_7IN5_V2_HEIGHT-30 + (30-battery_high_24.header.h)/2),
			(lv_img_dsc_t *) &battery_high_24, DRAW_IMAGE_INVERTED);

	Paint_SendToDisplay();
}

void DISPLAY_DrawDayReservation(CAL_Reservation* res) {
	uint16_t start_time= res->start_time;
	uint16_t end_time= res->end_time;
	uint16_t time_8_00 = CAL_HourToInt(8, 00);
	uint16_t time_14_00 = CAL_HourToInt(14, 00);
	char hourstr[15];
	char starttime_str[6];

	lv_font_box_t stringBox;

	memset (hourstr, 0, 15);

	if ((start_time<time_14_00)&&(end_time>=time_14_00)) { // reservation is split between morning and afternoon

	} else {
		if (start_time<CAL_HourToInt(14, 00)) {
			Paint_DrawFillRectangleOptim(80, 93+((start_time-time_8_00)*65)/60,
					EPD_7IN5_V2_WIDTH/2-10, 93+((end_time-time_8_00)*65)/60, BLACK);

			stringBox = Paint_GetStringBox(res->title, &montserrat_m_16);
			Paint_DrawString((80+EPD_7IN5_V2_WIDTH/2-10)/2 - stringBox.width/2,
					93+(((start_time+end_time)/2-time_8_00)*65)/60 - stringBox.height-20,
					res->title, &montserrat_m_16, WHITE, BLACK);

			memcpy (starttime_str, CAL_HourToStr(start_time),6);
			snprintf(hourstr, 14, "%s - %s", starttime_str, CAL_HourToStr(end_time));
			stringBox = Paint_GetStringBox(hourstr, &montserrat_m_16);
			Paint_DrawString((80+EPD_7IN5_V2_WIDTH/2-10)/2 - stringBox.width/2,
					93+(((start_time+end_time)/2-time_8_00)*65)/60 + stringBox.height+5,
					hourstr, &montserrat_m_16, WHITE, BLACK);
		} else {
			Paint_DrawFillRectangleOptim(EPD_7IN5_V2_WIDTH/2+10, 93+((start_time-time_14_00)*65)/60,
					EPD_7IN5_V2_WIDTH-80, 93+((end_time-time_14_00)*65)/60, BLACK);
			stringBox = Paint_GetStringBox(res->title, &montserrat_m_16);

			Paint_DrawString((EPD_7IN5_V2_WIDTH/2+16+EPD_7IN5_V2_WIDTH-80)/2 - stringBox.width/2,
					93+(((start_time+end_time)/2-time_14_00)*65)/60 - stringBox.height-20,
					res->title, &montserrat_m_16, WHITE, BLACK);

			memcpy (starttime_str, CAL_HourToStr(start_time),6);
			snprintf(hourstr, 14, "%s - %s", starttime_str, CAL_HourToStr(end_time));
			stringBox = Paint_GetStringBox(hourstr, &montserrat_m_16);
			Paint_DrawString((EPD_7IN5_V2_WIDTH/2+16+EPD_7IN5_V2_WIDTH-80)/2 - stringBox.width/2,
					93+(((start_time+end_time)/2-time_14_00)*65)/60 + stringBox.height+5,
					hourstr, &montserrat_m_16, WHITE, BLACK);
		}
	}
}

void DISPLAY_ShowDayReservation(PROTOCOL_ConfigurationTypedef* conf, uint8_t day) {
	char *text;
	char title[37];
	char people[((CAL_TRAINER_MAX_LENGTH+2)*4)+1]; // 1 for 0 ending, 2 for ',' and space between name
	lv_font_box_t stringBox;
	uint8_t hour =8;
	CAL_Reservation* res;

	memset (people, 0,((CAL_TRAINER_MAX_LENGTH+2)*4)+1);

	//1.Select Image
	//printf("Clear framebuffer\r\n");
	Paint_Clear(WHITE);

	// 2.Drawing on the image
	// Draw header with prompt, connection status and battery state
	Paint_DrawFillRectangleOptim(0, 0, EPD_7IN5_V2_WIDTH, 36, BLACK);

	memset (title, 0, 30);
	snprintf(title, 36, "%s - %s %s", conf->room_name, CAL_DayName[day-1], CAL_GetDayArray()[day-1]);
	stringBox = Paint_GetStringBox(title, &montserrat_m_26);
	Paint_DrawString((EPD_7IN5_V2_WIDTH-stringBox.width)/2,5,title, &montserrat_m_26, WHITE, BLACK);

	for (hour=8; hour<20; hour++) {
		text = CAL_HourToStr(hour*60);
		stringBox = Paint_GetStringBox(text, &montserrat_m_26);
		if (hour<14)
			Paint_DrawString(5, 80+65*(hour-8), text, &montserrat_m_26, BLACK, WHITE);
		else
			Paint_DrawString(EPD_7IN5_V2_WIDTH-stringBox.width-5, 80+65*(hour-14), text, &montserrat_m_26, BLACK, WHITE);
	}

	Paint_DrawLine(EPD_7IN5_V2_WIDTH/2, 60, EPD_7IN5_V2_WIDTH/2, 455, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);

	Paint_DrawImage((EPD_7IN5_V2_WIDTH/2+50)/2-morning_32.header.w/2,
			(80+36)/2-morning_32.header.h/2, (lv_img_dsc_t *) &morning_32, DRAW_IMAGE_NORMAL);
	Paint_DrawImage((EPD_7IN5_V2_WIDTH/2+EPD_7IN5_V2_WIDTH-50)/2-afternoon_32.header.w/2,
			(80+36)/2-afternoon_32.header.h/2, (lv_img_dsc_t *) &afternoon_32, DRAW_IMAGE_NORMAL);

	// 3. Draw today reservations
	res= CAL_GetFirst();
	if ((res!=NULL) && (res->day_nbr == day))
		DISPLAY_DrawDayReservation(res);

	while (res!=NULL) {
		res=CAL_GetNext();
		if ((res!=NULL) && (res->day_nbr == day))
			DISPLAY_DrawDayReservation(res);
	}

	Paint_SendToDisplay();
}

void DISPLAY_ShowWeekReservation(PROTOCOL_ConfigurationTypedef* conf) {
	char *text;
	char title[45];
	lv_font_box_t stringBox;
	uint8_t hour =8;
	CAL_Reservation* res;
	uint8_t column;
	uint16_t time_8_00 = CAL_HourToInt(8, 00);
	char* line_1;
	char* line_2;

	//1.Select Image
	//printf("Clear framebuffer\r\n");
	Paint_Clear(WHITE);

	// 2.Drawing on the image
	// Draw header with prompt, connection status and battery state
	Paint_DrawFillRectangleOptim(0, 0, EPD_7IN5_V2_WIDTH, 36, BLACK);

	memset (title, 0, 30);
	snprintf(title, 44, "%s - Semaine du %s au %s", conf->room_name, CAL_GetDayArray()[0], CAL_GetDayArray()[4]);
	stringBox = Paint_GetStringBox(title, &montserrat_m_26);
	Paint_DrawString((EPD_7IN5_V2_WIDTH-stringBox.width)/2,5,title, &montserrat_m_26, WHITE, BLACK);

	for (hour=8; hour<19; hour+=2) {
		text = CAL_HourToStr(hour*60);
		stringBox = Paint_GetStringBox(text, &montserrat_m_26);

		Paint_DrawString(80-stringBox.width, 60+70*((hour-8)/2), text, &montserrat_m_26, BLACK, WHITE);
	}

	Paint_DrawLine(100, 60, 100, EPD_7IN5_V2_HEIGHT-20, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
	Paint_DrawLine(240, 60, 240, EPD_7IN5_V2_HEIGHT-20, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
	Paint_DrawLine(380, 60, 380, EPD_7IN5_V2_HEIGHT-20, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
	Paint_DrawLine(520, 60, 520, EPD_7IN5_V2_HEIGHT-20, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
	Paint_DrawLine(660, 60, 660, EPD_7IN5_V2_HEIGHT-20, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);

	for (int day=0; day<5; day++) {
		stringBox = Paint_GetStringBox(CAL_DayName[day], &montserrat_r_26);
		Paint_DrawString(170+(140*day)-(stringBox.width)/2,40,CAL_DayName[day], &montserrat_r_26, BLACK, WHITE);
	}

	// 3. Draw today reservations
	res= CAL_GetFirst();

	while (res!=NULL) {
		if (res->day_nbr<6) {
			column = res->day_nbr-1;

			// draw a filled rectangle
			Paint_DrawFillRectangleOptim(110+(column*140), 65+((res->start_time-time_8_00)*70)/120,
					                230+(column*140), 65+((res->end_time-time_8_00)*70)/120, BLACK);

			stringBox = Paint_GetStringBox(res->title, &montserrat_m_16);

			if (stringBox.width>120) { //140-20
				// split in two lines
				memcpy (title, res->title, strlen(res->title)+1);
				line_1 = strtok(title," ");

				stringBox = Paint_GetStringBox(line_1, &montserrat_m_16);

				if (stringBox.height*2+4> ((res->end_time-res->start_time)*70)/120) { // unable to put two lines in the box
					if (stringBox.width<120)
						Paint_DrawString(170+(column*140)-(stringBox.width)/2,
										65+(((res->end_time +res->start_time)/2-time_8_00)*70)/120 - (stringBox.height)/2,
										line_1, &montserrat_m_16, WHITE, BLACK);
				} else {
					if (stringBox.width<120)
						Paint_DrawString(170+(column*140)-(stringBox.width)/2,
										65+(((res->end_time +res->start_time)/2-time_8_00)*70)/120 - (stringBox.height)-2,
										line_1, &montserrat_m_16, WHITE, BLACK);

					line_2 = &line_1[strlen(line_1)+1]; // set line_2 after line_1 string
					stringBox = Paint_GetStringBox(line_2, &montserrat_m_16);

					if (stringBox.width<120)
						Paint_DrawString(170+(column*140)-(stringBox.width)/2,
										65+(((res->end_time +res->start_time)/2-time_8_00)*70)/120 + 2,
										line_2, &montserrat_m_16, WHITE, BLACK);
					else {
						line_2 = strtok(NULL," "); // reduce size of remaining string
						stringBox = Paint_GetStringBox(line_2, &montserrat_m_16);

						if (stringBox.width<120)
							Paint_DrawString(170+(column*140)-(stringBox.width)/2,
											65+(((res->end_time +res->start_time)/2-time_8_00)*70)/120 + 2,
											line_2, &montserrat_m_16, WHITE, BLACK);
					}
				}
			} else {
				Paint_DrawString(170+(column*140)-(stringBox.width)/2,
						65+(((res->end_time +res->start_time)/2-time_8_00)*70)/120 - (stringBox.height)/2,
						res->title, &montserrat_m_16, WHITE, BLACK);
			}

		}

		res=CAL_GetNext();
	}

	Paint_SendToDisplay();
}

void DISPLAY_ShowWaitToConnect(uint64_t uid) {
	char *text;
	char uidtostr[20];

	lv_font_box_t stringBox;

	//1.Select Image
	//printf("Clear framebuffer\r\n");
	Paint_Clear(WHITE);

	//2. draw image
	Paint_DrawImage(120, 70, (lv_img_dsc_t *) &cloud_sync_100, DRAW_IMAGE_NORMAL);

	text="Wait to connect";
	stringBox = Paint_GetStringBox(text, &montserrat_m_40);
	Paint_DrawString((EPD_7IN5_V2_WIDTH/2 - stringBox.width/2)+100,
			70,text, &montserrat_m_40, BLACK, WHITE);

	text="to network";
	stringBox = Paint_GetStringBox(text, &montserrat_m_40);
	Paint_DrawString((EPD_7IN5_V2_WIDTH/2 - stringBox.width/2)+100,
			130,text, &montserrat_m_40, BLACK, WHITE);

	Paint_DrawLine(30, 250, EPD_7IN5_V2_WIDTH-30, 250, BLACK, DOT_PIXEL_2X2, LINE_STYLE_SOLID);

	text="Device UID";
	stringBox = Paint_GetStringBox(text, &montserrat_m_40);
	Paint_DrawString(EPD_7IN5_V2_WIDTH/2 - stringBox.width/2,
			300,text, &montserrat_m_40, BLACK, WHITE);

	memset (uidtostr, 0, 20);
	snprintf(uidtostr, 19, "0x%08lX%08lX",(uint32_t)(uid>>32), (uint32_t)uid);

	stringBox = Paint_GetStringBox(uidtostr, &play_r_60);
	Paint_DrawString(EPD_7IN5_V2_WIDTH/2 - stringBox.width/2,
			360,uidtostr, &play_r_60, BLACK, WHITE);

	Paint_SendToDisplay();
}

void DISPLAY_ShowConfiguration(PROTOCOL_ConfigurationTypedef* conf) {
	char *text;
	char uidtostr[20];
	const uint8_t line_space=30;
	uint8_t line_nbr=0;
	lv_font_box_t stringBox;

	//1.Select Image
	Paint_Clear(WHITE);

	//2. draw image
	Paint_DrawImage(100, 20, (lv_img_dsc_t *) &services_96, DRAW_IMAGE_NORMAL);

	text="CONFIGURATION";
	stringBox = Paint_GetStringBox(text, &montserrat_m_40);
	Paint_DrawString((EPD_7IN5_V2_WIDTH/2 - stringBox.width/2)+40,
			48,text, &montserrat_m_40, BLACK, WHITE);

	Paint_DrawLine(30, 136, EPD_7IN5_V2_WIDTH-30, 136, BLACK, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
	Paint_DrawLine(320, 170, 320, EPD_7IN5_V2_HEIGHT-30, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);

	/* device id */
	text="Device UID";
	stringBox = Paint_GetStringBox(text, &montserrat_m_26);
	Paint_DrawString(300 - stringBox.width,180+(line_nbr*line_space),text, &montserrat_m_26, BLACK, WHITE);

	memset (uidtostr, 0, 20);
	snprintf(uidtostr, 19, "%08lX%08lX",(uint32_t)(conf->device_uid>>32), (uint32_t)conf->device_uid);
	stringBox = Paint_GetStringBox(uidtostr, &montserrat_li_26);
	Paint_DrawString(340, 180+(line_nbr*line_space) ,uidtostr, &montserrat_li_26, BLACK, WHITE);

	/* channel id */
	line_nbr++;
	text="Channel ID";
	stringBox = Paint_GetStringBox(text, &montserrat_m_26);
	Paint_DrawString(300 - stringBox.width,180+(line_nbr*line_space),text, &montserrat_m_26, BLACK, WHITE);

	memset (uidtostr, 0, 20);
	snprintf(uidtostr, 19, "%02X",(uint8_t)(conf->rf_channel));
	stringBox = Paint_GetStringBox(uidtostr, &montserrat_li_26);
	Paint_DrawString(340, 180+(line_nbr*line_space) ,uidtostr, &montserrat_li_26, BLACK, WHITE);

	/* pan id */
	line_nbr++;
	text="Pan ID";
	stringBox = Paint_GetStringBox(text, &montserrat_m_26);
	Paint_DrawString(300 - stringBox.width,180+(line_nbr*line_space),text, &montserrat_m_26, BLACK, WHITE);

	memset (uidtostr, 0, 20);
	snprintf(uidtostr, 19, "%04X",(uint16_t)(conf->rf_panid));
	stringBox = Paint_GetStringBox(uidtostr, &montserrat_li_26);
	Paint_DrawString(340, 180+(line_nbr*line_space) ,uidtostr, &montserrat_li_26, BLACK, WHITE);

	/* gateway ID */
	line_nbr++;
	text="Gateway UID";
	stringBox = Paint_GetStringBox(text, &montserrat_m_26);
	Paint_DrawString(300 - stringBox.width,180+(line_nbr*line_space),text, &montserrat_m_26, BLACK, WHITE);

	memset (uidtostr, 0, 20);
	snprintf(uidtostr, 19, "%08lX%08lX",(uint32_t)(conf->gw_address>>32), (uint32_t)conf->gw_address);
	stringBox = Paint_GetStringBox(uidtostr, &montserrat_li_26);
	Paint_DrawString(340, 180+(line_nbr*line_space) ,uidtostr, &montserrat_li_26, BLACK, WHITE);

	/* current hour */
	line_nbr++;
	text="Current hour";
	stringBox = Paint_GetStringBox(text, &montserrat_m_26);
	Paint_DrawString(300 - stringBox.width,180+(line_nbr*line_space),text, &montserrat_m_26, BLACK, WHITE);

	stringBox = Paint_GetStringBox(CAL_HourToStr(conf->init_time/60), &montserrat_li_26);
	Paint_DrawString(340, 180+(line_nbr*line_space),CAL_HourToStr(conf->init_time/60), &montserrat_li_26, BLACK, WHITE);

	/* current day */
	line_nbr++;
	text="Current day";
	stringBox = Paint_GetStringBox(text, &montserrat_m_26);
	Paint_DrawString(300 - stringBox.width,180+(line_nbr*line_space),text, &montserrat_m_26, BLACK, WHITE);

	memset (uidtostr, 0, 20);
	snprintf(uidtostr, 19, "%s (%u)",CAL_DayName[conf->init_day-1], conf->init_day);
	stringBox = Paint_GetStringBox(uidtostr, &montserrat_li_26);
	Paint_DrawString(340, 180+(line_nbr*line_space), uidtostr, &montserrat_li_26, BLACK, WHITE);

	/* rssi */
	line_nbr++;
	text="RSSI (min/max/moy)";
	stringBox = Paint_GetStringBox(text, &montserrat_m_26);
	Paint_DrawString(300 - stringBox.width,180+(line_nbr*line_space),text, &montserrat_m_26, BLACK, WHITE);

	memset (uidtostr, 0, 20);
	snprintf(uidtostr, 19, "%02X/%02X/%02X",(uint8_t)conf->rssi.rssi_val[0], (uint8_t)conf->rssi.rssi_val[1], (uint8_t)conf->rssi.rssi_val[2]);
	stringBox = Paint_GetStringBox(uidtostr, &montserrat_li_26);
	Paint_DrawString(340, 180+(line_nbr*line_space) ,uidtostr, &montserrat_li_26, BLACK, WHITE);

	/* battery level */
	line_nbr++;
	text="Battery level";
	stringBox = Paint_GetStringBox(text, &montserrat_m_26);
	Paint_DrawString(300 - stringBox.width,180+(line_nbr*line_space),text, &montserrat_m_26, BLACK, WHITE);

	memset (uidtostr, 0, 20);
	snprintf(uidtostr, 19, "%02X",(uint8_t)conf->battery_level);
	stringBox = Paint_GetStringBox(uidtostr, &montserrat_li_26);
	Paint_DrawString(340, 180+(line_nbr*line_space) ,uidtostr, &montserrat_li_26, BLACK, WHITE);

	/* rf power */
	line_nbr++;
	text="RF power";
	stringBox = Paint_GetStringBox(text, &montserrat_m_26);
	Paint_DrawString(300 - stringBox.width,180+(line_nbr*line_space),text, &montserrat_m_26, BLACK, WHITE);

	memset (uidtostr, 0, 20);
	snprintf(uidtostr, 19, "%02X",(uint8_t)conf->rf_power);
	stringBox = Paint_GetStringBox(uidtostr, &montserrat_li_26);
	Paint_DrawString(340, 180+(line_nbr*line_space) ,uidtostr, &montserrat_li_26, BLACK, WHITE);

	Paint_SendToDisplay();
}

void DISPLAY_ShowPanic(uint32_t error_code) {
	char *text;
	char codetostr[12];

	lv_font_box_t stringBox;

	//1.Select Image
	//printf("Clear framebuffer\r\n");
	Paint_Clear(WHITE);

	//2. draw image
	Paint_DrawImage(150, 80, (lv_img_dsc_t *) &error_96, DRAW_IMAGE_NORMAL);

	text="System";
	stringBox = Paint_GetStringBox(text, &montserrat_m_40);
	Paint_DrawString((EPD_7IN5_V2_WIDTH/2 - stringBox.width/2)+120,
			70,text, &montserrat_m_40, BLACK, WHITE);

	text="malfunction";
	stringBox = Paint_GetStringBox(text, &montserrat_m_40);
	Paint_DrawString((EPD_7IN5_V2_WIDTH/2 - stringBox.width/2)+120,
			130,text, &montserrat_m_40, BLACK, WHITE);

	Paint_DrawLine(30, 250, EPD_7IN5_V2_WIDTH-30, 250, BLACK, DOT_PIXEL_2X2, LINE_STYLE_SOLID);

	text="Error code";
	stringBox = Paint_GetStringBox(text, &montserrat_m_40);
	Paint_DrawString(EPD_7IN5_V2_WIDTH/2 - stringBox.width/2,
			300,text, &montserrat_m_40, BLACK, WHITE);

	memset (codetostr, 0, 12);
	snprintf(codetostr, 11, "0x%08lX",(uint32_t)error_code);

	stringBox = Paint_GetStringBox(codetostr, &play_r_60);
	Paint_DrawString(EPD_7IN5_V2_WIDTH/2 - stringBox.width/2,
			360,codetostr, &play_r_60, BLACK, WHITE);

	Paint_SendToDisplay();
}

