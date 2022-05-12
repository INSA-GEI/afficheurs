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

void Error_Handler(void);

TickType_t msToTicks(TickType_t ms);

#define VCP_TX_Pin 					GPIO_PIN_2
#define VCP_TX_GPIO_Port 			GPIOA

#define VCP_RX_Pin 					GPIO_PIN_15
#define VCP_RX_GPIO_Port 			GPIOA

#define RST_Pin 					GPIO_PIN_3
#define RST_GPIO_Port 				GPIOA

#define DC_Pin 						GPIO_PIN_4
#define DC_GPIO_Port 				GPIOA

#define SPI_CS_Pin 					GPIO_PIN_5
#define SPI_CS_GPIO_Port 			GPIOA

#define SPI_MISO_Pin 				GPIO_PIN_1
#define SPI_MISO_GPIO_Port 			GPIOA

#define SPI_MOSI_Pin 				GPIO_PIN_7
#define SPI_MOSI_GPIO_Port 			GPIOA

#define BUSY_Pin 					GPIO_PIN_1
#define BUSY_GPIO_Port 				GPIOB
#define BUSY_EXTI_IRQn 				EXTI1_IRQn

#define BUTTON_Pin 					GPIO_PIN_0
#define BUTTON_GPIO_Port 			GPIOB
#define BUTTON_EXTI_IRQn 			EXTI0_IRQn

#define XBEE_RESET_Pin 				GPIO_PIN_8
#define XBEE_RESET_GPIO_Port 		GPIOA

#define XBEE_SLEEP_RQ_Pin 			GPIO_PIN_11
#define XBEE_SLEEP_RQ_GPIO_Port	 	GPIOA

#define XBEE_SLEEP_Pin 				GPIO_PIN_3
#define XBEE_SLEEP_GPIO_Port 		GPIOB
#define XBEE_SLEEP_EXTI_IRQn 		EXTI3_IRQn

#define SWDIO_Pin 					GPIO_PIN_13
#define SWDIO_GPIO_Port 			GPIOA

#define SWCLK_Pin 					GPIO_PIN_14
#define SWCLK_GPIO_Port 			GPIOA

/**
 * e-Paper GPIO
 **/
#define EPD_RST_PIN     			RST_GPIO_Port, RST_Pin
#define EPD_DC_PIN      			DC_GPIO_Port, DC_Pin
#define EPD_CS_PIN      			SPI_CS_GPIO_Port, SPI_CS_Pin
#define EPD_BUSY_PIN    			BUSY_GPIO_Port, BUSY_Pin

#define STACK_SIZE 					200

#define PriorityInit    			(configMAX_PRIORITIES -1)
#define PriorityEvents        		(configMAX_PRIORITIES -2)
#define PriorityPeriodicWakeup		(configMAX_PRIORITIES -3)
#define PriorityDrawDisplay			(configMAX_PRIORITIES -4)

#define EPD_CONFIG_LUT				1

#define DEBUG_PROTOCOL				1
#define DEBUG_CHANNEL				0x12
#define DEBUG_PANID					0x1337
#define DEBUG_PROTOCOL_TIMEOUT	    2000 // Timeout of around 2s

#define DEBUG_PROTOCOL_FAKE_CONFIG  1


#endif /* CONFIG_H_ */
