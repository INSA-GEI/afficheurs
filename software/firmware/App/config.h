/*
 * config.h
 *
 *  Created on: 27 avr. 2022
 *      Author: dimercur
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "stm32l4xx_hal.h"
#include "cmsis_os.h"

TickType_t msToTicks(TickType_t ms);

#define STACK_SIZE 					200
#define PriorityDrawDisplay			16
#define PriorityProtocol			15
#define PriorityPeriodicWakeup		14
#define PriorityXBEERxThread        13

#define DEBUG_PROTOCOL				1
#define DEBUG_CHANNEL				0x12
#define DEBUG_PANID					0x1337
#define DEBUG_PROTOCOL_TIMEOUT	    2000 // Timeout of around 2s

#define DEBUG_PROTOCOL_FAKE_CONFIG  1


#endif /* CONFIG_H_ */
