/*
 * xbee_ll.c
 *
 *  Created on: Apr 6, 2022
 *      Author: dimercur
 */

#include "xbee_ll.h"
#include "stm32l4xx_hal.h"

UART_HandleTypeDef XBEE_LL_Uart;
uint8_t XBEE_LL_RxReady;
uint8_t XBEE_LL_TxReady;
int XBEE_LL_Mode;
char* XBEE_LL_RxBuffer;

int XBEE_LL_RXState;

#define XBEE_LL_RX_STATE_WAIT_HEADER	1
#define XBEE_LL_RX_STATE_WAIT_EOF		2

typedef struct {
	uint8_t startChar;
	uint16_t frameLength;
} API_LENGTH_ST;

volatile uint16_t tmp; // to be used by XBEE_LL_ConfigureUart

int XBEE_LL_ConfigureUart(USART_TypeDef* usart, uint32_t baudrate) {
	__HAL_RCC_USART1_FORCE_RESET();
	__HAL_RCC_USART1_RELEASE_RESET();

	XBEE_LL_Uart.Instance = usart;
	XBEE_LL_Uart.Init.BaudRate = baudrate;
	XBEE_LL_Uart.Init.WordLength = UART_WORDLENGTH_8B;
	XBEE_LL_Uart.Init.StopBits = UART_STOPBITS_1;
	XBEE_LL_Uart.Init.Parity = UART_PARITY_NONE;
	XBEE_LL_Uart.Init.Mode = UART_MODE_TX_RX;
	XBEE_LL_Uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	XBEE_LL_Uart.Init.OverSampling = UART_OVERSAMPLING_16;
	XBEE_LL_Uart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	XBEE_LL_Uart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

	if (HAL_UART_Init(&XBEE_LL_Uart) != HAL_OK)
	{
		return XBEE_LL_ERROR_USART_CFG;
	}

	/* Debug usart */
	tmp = usart->CR1;
	tmp = usart->CR2;
	tmp = usart->CR3;
	tmp = usart->BRR;
	tmp = usart->ICR;
	tmp = usart->ISR;
	tmp = usart->GTPR;
	tmp = usart->RDR;
	tmp = usart->TDR;

	XBEE_LL_TxReady = SET;
	XBEE_LL_RxReady = SET;

	return XBEE_LL_OK;
}

void XBEE_LL_ConfigureGPIO(void) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/**USART1 GPIO Configuration
		    PA8     ------> XBEE_RESET
		    PA11     ------> XBEE_SLEEP_RQ
		    PB3  -------> XBEE_SLEEP
	 */
	GPIO_InitStruct.Pin = XBEE_RST_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(XBEE_RST_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = XBEE_SLEEP_RQ_PIN;
	HAL_GPIO_Init(XBEE_SLEEP_RQ_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = XBEE_SLEEP_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(XBEE_SLEEP_PORT, &GPIO_InitStruct);

	/* Set SLEEP_REQ pin to 0 (disable sleep mode)
	 *
	 */
	HAL_GPIO_WritePin(XBEE_SLEEP_RQ_PORT, XBEE_SLEEP_RQ_PIN, GPIO_PIN_RESET);

	/* Reset XBEE module
	 * Pin to 0 -> wait 10 ms -> pin to 1 -> wait 300 ms
	 */
	HAL_GPIO_WritePin(XBEE_RST_PORT, XBEE_RST_PIN, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(XBEE_RST_PORT, XBEE_RST_PIN, GPIO_PIN_SET);
	HAL_Delay(300);
}

int XBEE_LL_SendData(char* data, int length) {
	int data_length;

	if (length == -1) {
		/* Envoi d'un trame API, donc, recherche de la longueur dans la trame */
		data_length = ((API_LENGTH_ST*)data)->frameLength + 3 +1; //+3 for header and + 1 for checksum
	}
	else data_length = length;

	while (XBEE_LL_TxReady != SET); // wait for last transfert to end

	/* Restore huart->gState to ready */
	//XBEE_LL_Uart.gState = HAL_UART_STATE_READY;
	XBEE_LL_TxReady = RESET;
	if(HAL_UART_Transmit_DMA(&XBEE_LL_Uart, (uint8_t*)data, data_length)!= HAL_OK) {
		XBEE_LL_TxReady = SET;
		return XBEE_LL_ERROR_TX;
	}

	return XBEE_LL_OK;
}

int XBEE_LL_ReceiveData(char* data, int length) {
	int data_length;

	while (XBEE_LL_RxReady != SET); // wait for last RX to end

	if (length == -1) {
		// set API mode
		XBEE_LL_Mode = XBEE_LL_MODE_API;
		data_length = 3; // 3 bytes for api header (start char and length)
		XBEE_LL_RxBuffer = data;
		XBEE_LL_RXState = XBEE_LL_RX_STATE_WAIT_HEADER;
	} else {
		// set TRANSPARENT mode
		XBEE_LL_Mode = XBEE_LL_MODE_TRANSPARENT;
		data_length = length;
	}

	XBEE_LL_RxReady = RESET;

	if(HAL_UART_Receive_DMA(&XBEE_LL_Uart, (uint8_t*)data, data_length)!= HAL_OK) {
		XBEE_LL_RxReady = SET;
		return XBEE_LL_ERROR_RX;
	}

	while (XBEE_LL_RxReady != SET); // wait for RX to end
	return XBEE_LL_OK;
}

/**
 * @brief  Rx Transfer completed callback
 * @param  UartHandle: UART handle
 * @note   This example shows a simple way to report end of DMA Rx transfer, and
 *         you can add your own implementation.
 * @retval None
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle) {
	int frame_length;

	if (XBEE_LL_Mode == XBEE_LL_MODE_TRANSPARENT)
		/* Set reception flag: transfer complete*/
		XBEE_LL_RxReady = SET;
	else {
		if (XBEE_LL_RXState == XBEE_LL_RX_STATE_WAIT_EOF)
			/* Set reception flag: transfer complete*/
			XBEE_LL_RxReady = SET;
		else {
			frame_length = 1+(((int)XBEE_LL_RxBuffer[1]<<8))+(int)XBEE_LL_RxBuffer[2];
			XBEE_LL_RXState = XBEE_LL_RX_STATE_WAIT_EOF;

			if(HAL_UART_Receive_DMA(&XBEE_LL_Uart, (uint8_t*)(XBEE_LL_RxBuffer+3), frame_length)!= HAL_OK) {
				// Something went wrong
				XBEE_LL_RxReady = SET;
			}
		}
	}
}

/**
 * @brief  Tx Transfer completed callback
 * @param  UartHandle: UART handle
 * @note   This example shows a simple way to report end of DMA Rx transfer, and
 *         you can add your own implementation.
 * @retval None
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle) {
	/* Set transmission flag: transfer complete*/
	XBEE_LL_TxReady = SET;

	/* Restore huart->gState to ready */
	//UartHandle->gState = HAL_UART_STATE_READY;
}
