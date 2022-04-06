/*
 * xbee.h
 *
 *  Created on: 4 avr. 2022
 *      Author: dimercur
 */

#ifndef BSP_XBEE_XBEE_H_
#define BSP_XBEE_XBEE_H_

#include "stm32l4xx_hal.h"

#define XBEE_OK   					0
#define XBEE_CONFIG_ERROR   		2
#define XBEE_AT_CMD_ERROR			3
#define XBEE_RX_TIMEOUT				4
#define XBEE_RX_ERROR				5
#define XBEE_TX_ERROR				6
#define XBEE_INVALID_FRAME			7

int XBEE_ConfigureDevice();
int XBEE_Init (UART_HandleTypeDef* uart);

#endif /* BSP_XBEE_XBEE_H_ */
