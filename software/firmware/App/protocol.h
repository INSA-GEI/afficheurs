/*
 * protocol.h
 *
 *  Created on: 12 avr. 2022
 *      Author: dimercur
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#define PROTOCOL_OK					0
#define PROTOCOL_XBEE_INIT_ERROR	1
#define PROTOCOL_INVALID_FRAME		2
#define PROTOCOL_RX_TIMEOUT			3

int PROTOCOL_Init(void);

#endif /* PROTOCOL_H_ */
