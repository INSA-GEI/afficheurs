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

void DISPLAY_ShowDayReservation(PROTOCOL_ConfigurationTypedef* conf, uint8_t day) {
	char *text;
	char title[37];
	char people[((CAL_TRAINER_MAX_LENGTH+2)*4)+1]; // 1 for 0 ending, 2 for ',' and space between name
	char local_prompt[51];
	lv_font_box_t stringBox;
	uint8_t hour =8;

	memset (people, 0,((CAL_TRAINER_MAX_LENGTH+2)*4)+1);

	//1.Select Image
	//printf("Clear framebuffer\r\n");
	Paint_Clear(WHITE);

	// 2.Drawing on the image

	// Draw title
	memset (title, 0, 30);
	snprintf(title, 36, "%s - %s %s", conf->room_name, CAL_DayName[day-1], CAL_GetDayArray()[day-1]);
	stringBox = Paint_GetStringBox(title, &montserrat_m_26);
	Paint_DrawString((EPD_7IN5_V2_WIDTH-stringBox.width)/2,5,title, &montserrat_m_26, BLACK, WHITE);

	for (hour=8; hour<20; hour++) {
		text = CAL_HourToStr(hour*60);
		stringBox = Paint_GetStringBox(text, &montserrat_m_26);
		if (hour<14)
			Paint_DrawString(5, 80+65*(hour-8), text, &montserrat_m_26, BLACK, WHITE);
		else
			Paint_DrawString(EPD_7IN5_V2_WIDTH-stringBox.width-5, 80+65*(hour-14), text, &montserrat_m_26, BLACK, WHITE);
	}

	Paint_DrawLine(EPD_7IN5_V2_WIDTH/2, 80, EPD_7IN5_V2_WIDTH/2, 455, BLACK, DOT_PIXEL_2X2, LINE_STYLE_SOLID);

	text = "Matin";
	stringBox = Paint_GetStringBox(text, &montserrat_m_26);
	Paint_DrawString((EPD_7IN5_V2_WIDTH/2+50)/2 - (stringBox.width/2),40 ,text, &montserrat_m_26, BLACK, WHITE);

	text = "Apres-Midi";
	stringBox = Paint_GetStringBox(text, &montserrat_m_26);
	Paint_DrawString((EPD_7IN5_V2_WIDTH/2+EPD_7IN5_V2_WIDTH-50)/2 - (stringBox.width/2),40 ,text, &montserrat_m_26, BLACK, WHITE);

//	lv_font_box_t stringBox = Paint_GetStringBox(text, &montserrat_li_150);
//	Paint_DrawString(EPD_7IN5_V2_WIDTH-stringBox.width-20, 10, text, &montserrat_li_150, BLACK, WHITE);
//
//	// Draw room type
//	text = conf->room_type;
//	stringBox = Paint_GetStringBox(text, &play_r_60);
//	Paint_DrawString(EPD_7IN5_V2_WIDTH-stringBox.width-20,150,
//			text, &play_r_60, BLACK, WHITE);
//
//	// Draw current reservation with two lines
//	Paint_DrawLine(420, 150+80, EPD_7IN5_V2_WIDTH-20, 150+80, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
//	//Paint_DrawLine(320, 150+81, EPD_7IN5_V2_WIDTH-20, 150+81, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
//	Paint_DrawLine(220, 150+82, EPD_7IN5_V2_WIDTH-20, 150+82, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
//	//Paint_DrawLine(120, 150+83, EPD_7IN5_V2_WIDTH-20, 150+83, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
//	Paint_DrawLine(20, 150+84, EPD_7IN5_V2_WIDTH-20, 150+84, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
//
//	text = reservation->title;
//	stringBox = Paint_GetStringBox(text, &montserrat_m_40);
//	Paint_DrawString((EPD_7IN5_V2_WIDTH-stringBox.width)/2,150+85+15,
//			text, &montserrat_m_40, BLACK, WHITE);
//
//	Paint_DrawLine(20, 150+155, EPD_7IN5_V2_WIDTH-20, 150+155, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
//	//Paint_DrawLine(20, 150+156, EPD_7IN5_V2_WIDTH-120, 150+156, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
//	Paint_DrawLine(20, 150+157, EPD_7IN5_V2_WIDTH-220, 150+157, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
//	//Paint_DrawLine(20, 150+158, EPD_7IN5_V2_WIDTH-320, 150+158, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
//	Paint_DrawLine(20, 150+159, EPD_7IN5_V2_WIDTH-420, 150+159, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
//
//	// Draw INSA Logo
//	Paint_DrawImage(20,(150+80-logoINSA_110.header.h)/2,(lv_img_dsc_t *) &logoINSA_110, DRAW_IMAGE_NORMAL);
//
//	// Draw information about reservation
//	uint16_t ypos_top = (150+160) + (((EPD_7IN5_V2_HEIGHT-30)-(150+160)) - 110)/2;
//	uint16_t ypos_bottom = (EPD_7IN5_V2_HEIGHT-30) - 48 - (((EPD_7IN5_V2_HEIGHT-30)-(150+160)) - 110)/2;
//
//	Paint_DrawImage(40, ypos_top, (lv_img_dsc_t *) &clock_start_48, DRAW_IMAGE_NORMAL);
//	Paint_DrawImage(40, ypos_bottom, (lv_img_dsc_t *) &clock_end_48, DRAW_IMAGE_NORMAL);
//
//	text = CAL_HourToStr(reservation->start_time);
//	stringBox = Paint_GetStringBox(text, &montserrat_m_26);
//	Paint_DrawString(40 + clock_start_48.header.w +20, ypos_top+11, text, &montserrat_m_26, BLACK, WHITE);
//
//	text = CAL_HourToStr(reservation->end_time);
//	stringBox = Paint_GetStringBox(text, &montserrat_m_26);
//	Paint_DrawString(40 + clock_end_48.header.w +20, ypos_bottom+11, text, &montserrat_m_26, BLACK, WHITE);
//
//	Paint_DrawImage(300, ypos_top, (lv_img_dsc_t *) &attendees_48, DRAW_IMAGE_NORMAL);
//	Paint_DrawImage(300, ypos_bottom, (lv_img_dsc_t *) &teacher_48, DRAW_IMAGE_NORMAL);
//
//	//text = "4AE-SE";
//	int i=0;
//
//	while ((i<4) && (reservation->trainees[i]!=0)) {
//		strcat (people, reservation->trainees[i]);
//		if ((i!=3) && (reservation->trainees[i+1]!=0)) {
//			strcat (people, ", ");
//		}
//		i++;
//	}
//
//	if (strlen(people)>32)
//		people[32]= 0; // trunc string to only 32 chars
//
//	stringBox = Paint_GetStringBox(people, &montserrat_m_26);
//	Paint_DrawString(300 + attendees_48.header.w +20, ypos_top+11, people, &montserrat_m_26, BLACK, WHITE);
//
//	//text = "S. DI MERCURIO";
//	memset (people, 0,((CAL_TRAINER_MAX_LENGTH+2)*4)+1); //  flush buffer
//	i=0;
//
//	while ((i<4) && (reservation->trainers[i]!=0)) {
//		strcat (people, reservation->trainers[i]);
//		if ((i!=3) && (reservation->trainers[i+1]!=0)) {
//			strcat (people, ", ");
//		}
//		i++;
//	}
//
//	if (strlen(people)>32)
//		people[32]= 0; // trunc string to only 32 chars
//
//	stringBox = Paint_GetStringBox(people, &montserrat_m_26);
//	Paint_DrawString(300 + attendees_48.header.w +20, ypos_bottom+11, people, &montserrat_m_26, BLACK, WHITE);
//
//	// Draw footer with prompt, connection status and battery state
//	Paint_DrawRectangle(0, EPD_7IN5_V2_HEIGHT-30, EPD_7IN5_V2_WIDTH, EPD_7IN5_V2_HEIGHT, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
//
//	strncpy(local_prompt, prompt, 50);
//	local_prompt[50]=0;
//
//	stringBox = Paint_GetStringBox(local_prompt, &montserrat_m_26);
//	Paint_DrawString((EPD_7IN5_V2_WIDTH-stringBox.width)/2,EPD_7IN5_V2_HEIGHT-30+(30-26)/2,
//			local_prompt, &montserrat_m_26, WHITE, BLACK);
//
//	Paint_DrawImage((EPD_7IN5_V2_WIDTH-stringBox.width)/2 - clock_next_24.header.w - 8,
//			(EPD_7IN5_V2_HEIGHT-30 + (30-clock_next_24.header.h)/2),
//			(lv_img_dsc_t *) &clock_next_24, DRAW_IMAGE_INVERTED);
//
//	Paint_DrawImage(10,(EPD_7IN5_V2_HEIGHT-30 + (30-connection_mid_24.header.h)/2),
//			(lv_img_dsc_t *) &connection_mid_24, DRAW_IMAGE_INVERTED);
//
//	Paint_DrawImage(10+connection_mid_24.header.w+4,(EPD_7IN5_V2_HEIGHT-30 + (30-on_line_24.header.h)/2),
//			(lv_img_dsc_t *) &on_line_24, DRAW_IMAGE_INVERTED);
//
//	Paint_DrawImage(EPD_7IN5_V2_WIDTH-10-24,(EPD_7IN5_V2_HEIGHT-30 + (30-battery_high_24.header.h)/2),
//			(lv_img_dsc_t *) &battery_high_24, DRAW_IMAGE_INVERTED);

	Paint_SendToDisplay();
}

void DISPLAY_ShowWeekReservation(PROTOCOL_ConfigurationTypedef* conf) {

}

void DISPLAY_ShowWaitToConnect(uint64_t uid) {

}

void DISPLAY_ShowConfiguration(void) {

}
