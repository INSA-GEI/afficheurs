/*
 * tasks.h
 *
 *  Created on: 14 janv. 2022
 *      Author: dimercur
 */

#ifndef INC_TASKS_H_
#define INC_TASKS_H_

#include "stm32l4xx.h"
#include "FreeRTOS.h"

void TASKS_Create(void);
void TASKS_Run(void);

#endif /* INC_TASKS_H_ */
