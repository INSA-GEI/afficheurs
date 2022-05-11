/*
 * display.h
 *
 *  Created on: 26 avr. 2022
 *      Author: dimercur
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "stm32l4xx_hal.h"
#include "config.h"

#include "calendar.h"
#include "protocol.h"

typedef enum {
	DISPLAY_PromptIconNext=0,
	DISPLAY_PromptIconInfo,
	DISPLAY_PromptIconAlert
} DISPLAY_PromptIcon;

typedef enum {
	DISPLAY_OK=0,
	DISPLAY_HW_ERR,
	DISPLAY_FRAMEBUFFER_ERR
} DISPLAY_StatusTypedef;

DISPLAY_StatusTypedef DISPLAY_Init(void);
void DISPLAY_Update(void);
void DISPLAY_EnterPowerOff(void);

void DISPLAY_ShowReservation(PROTOCOL_ConfigurationTypedef* conf, CAL_Reservation* reservation, const char* prompt, DISPLAY_PromptIcon icon);
void DISPLAY_ShowDayReservation(PROTOCOL_ConfigurationTypedef* conf, uint8_t day);
void DISPLAY_ShowWeekReservation(PROTOCOL_ConfigurationTypedef* conf);

void DISPLAY_ShowWaitToConnect(uint64_t uid);
void DISPLAY_ShowConfiguration(PROTOCOL_ConfigurationTypedef* conf);
void DISPLAY_ShowPanic(uint32_t error_code);

#endif /* DISPLAY_H_ */
