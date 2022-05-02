/*
 * app.h
 *
 *  Created on: 12 avr. 2022
 *      Author: dimercur
 */

#ifndef APP_H_
#define APP_H_

#include "stm32l4xx_hal.h"
#include "config.h"

void TASKS_Create(void);
void TASKS_Run(void);
void APP_Init(void);

#endif /* APP_H_ */
