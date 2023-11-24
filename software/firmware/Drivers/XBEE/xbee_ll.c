/*
 * *****************************************
 * *****************************************
 */

#include "xbee_ll.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_ll_usart.h"

#include "cmsis_os.h"
#include "config.h"

/*
 * *****************************************
 * *****************************************
 */

static TaskHandle_t xbee_ll_rx_thread_handler;
static TaskHandle_t xbee_ll_tx_thread_handler;

/*
 * *****************************************
 * *****************************************
 */

typedef enum
{
	XBEE_LL_MODE_TRANSPARENT=0,
	XBEE_LL_MODE_API
} XBEE_LL_ModeType;


typedef enum
{
	XBEE_LL_RX_STATE_OK=0,
	XBEE_LL_RX_STATE_WAIT_DATA,
	XBEE_LL_RX_STATE_WAIT_HEADER,
	XBEE_LL_RX_STATE_WAIT_EOF,
	XBEE_LL_RX_STATE_TIMEOUT,
	XBEE_LL_RX_STATE_ERROR,
} XBEE_LL_RxStatus;

typedef struct
{
	uint8_t startChar;
	uint16_t frameLength;
} API_LENGTH_ST;

/*
 * *****************************************
 * *****************************************
 */

UART_HandleTypeDef XBEE_LL_Uart;

XBEE_LL_ModeType XBEE_LL_Mode;
XBEE_LL_RxStatus XBEE_LL_RXState;

FlagStatus XBEE_LL_RxReady;
FlagStatus XBEE_LL_TxReady;

uint16_t XBEE_LL_RxBufferIndex;
uint16_t XBEE_LL_RxBufferLength;

volatile uint16_t tmp;
char* XBEE_LL_RxBuffer;

/*
 * *****************************************
 * *****************************************
 */

void XBEE_LL_StartTimeout(uint32_t timeout);
void XBEE_LL_StopTimeout(void);

/*
 * *****************************************
 * *****************************************
 */

int XBEE_LL_ConfigureUart(USART_TypeDef* usart, uint32_t baudrate)
{
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

	XBEE_LL_TxReady = SET;
	XBEE_LL_RxReady = SET;

	XBEE_LL_RxBufferIndex = 0;
	XBEE_LL_RxBufferLength=0;
	XBEE_LL_Uart.RxState=HAL_UART_STATE_READY;
	XBEE_LL_RxBuffer = (char*)0x1;

	HAL_UART_Receive_IT(&XBEE_LL_Uart, (uint8_t*)&XBEE_LL_RxBuffer[XBEE_LL_RxBufferIndex], 1);

	LL_USART_Disable(XBEE_LL_Uart.Instance);
	LL_USART_DisableOverrunDetect(XBEE_LL_Uart.Instance);
	LL_USART_Enable(XBEE_LL_Uart.Instance);

	XBEE_LL_RxBuffer = (char*)0x0;
	XBEE_LL_RxReady = SET;

	return XBEE_LL_OK;
}


/*
 * *****************************************
 * *****************************************
 */

void XBEE_LL_ConfigureGPIO(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*
	        USART1 GPIO Configuration
		    PA8      ------>      XBEE_RESET
		    PA11     ------>      XBEE_SLEEP_RQ
		    PB3      ------->     XBEE_SLEEP
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


	HAL_GPIO_WritePin(XBEE_SLEEP_RQ_PORT, XBEE_SLEEP_RQ_PIN, GPIO_PIN_RESET);;
	vTaskDelay(msToTicks(300));

	/* RESET XBEE */
	HAL_GPIO_WritePin(XBEE_RST_PORT, XBEE_RST_PIN, GPIO_PIN_RESET);
	vTaskDelay(msToTicks(300));

	HAL_GPIO_WritePin(XBEE_RST_PORT, XBEE_RST_PIN, GPIO_PIN_SET);;
	vTaskDelay(msToTicks(1000));
}

/*
 * *****************************************
 * *****************************************
 */

int XBEE_LL_SendData(char* data, int length)
{
	int data_length;

	xbee_ll_tx_thread_handler = xTaskGetCurrentTaskHandle();

	if (length == -1)
	{
		data_length = ((API_LENGTH_ST*)data)->frameLength + 3 +1; //+3 for header and + 1 for checksum
	}
	else data_length = length;

	ulTaskNotifyTake( pdTRUE, pdMS_TO_TICKS(100));

	XBEE_LL_TxReady = RESET;
	if(HAL_UART_Transmit_DMA(&XBEE_LL_Uart, (uint8_t*)data, data_length)!= HAL_OK)
	{
		XBEE_LL_TxReady = SET;
		return XBEE_LL_ERROR_TX;
	}

	xbee_ll_tx_thread_handler = NULL;
	return XBEE_LL_OK;
}

/*
 * *****************************************
 * *****************************************
 */

int XBEE_LL_ReceiveData(char* data, int length, int timeout)
{
	uint32_t ulNotificationValue;
	int data_length;

	xbee_ll_rx_thread_handler = xTaskGetCurrentTaskHandle();
	while (XBEE_LL_RxReady != SET);

	XBEE_LL_RxBuffer = data;

	if (length == -1)
	{
		// set API mode
		XBEE_LL_Mode = XBEE_LL_MODE_API;
		data_length = 3;
		XBEE_LL_RXState = XBEE_LL_RX_STATE_WAIT_HEADER;
	}
	else
	{
		// set TRANSPARENT mode
		XBEE_LL_Mode = XBEE_LL_MODE_TRANSPARENT;
		XBEE_LL_RXState = XBEE_LL_RX_STATE_WAIT_DATA;
		data_length = length;
	}

	XBEE_LL_RxReady = RESET;

	XBEE_LL_RxBufferIndex=0;
	XBEE_LL_RxBufferLength=data_length;

	if (timeout == 0)
		ulNotificationValue = ulTaskNotifyTake( pdTRUE, pdMS_TO_TICKS(portMAX_DELAY));
	else
		ulNotificationValue = ulTaskNotifyTake( pdTRUE, pdMS_TO_TICKS(timeout));


	if (ulNotificationValue != 1)
	{
		XBEE_LL_RXState = XBEE_LL_RX_STATE_TIMEOUT;
	}

	xbee_ll_rx_thread_handler = NULL;
	XBEE_LL_RxBufferIndex = 0;
	XBEE_LL_RxBufferLength=0;
	XBEE_LL_RxBuffer = (char*)0x0;
	XBEE_LL_RxReady = SET;

	if (XBEE_LL_RXState == XBEE_LL_RX_STATE_ERROR)
		return XBEE_LL_ERROR_RX;
	else if (XBEE_LL_RXState == XBEE_LL_RX_STATE_TIMEOUT)
	{
		HAL_UART_DMAStop(&XBEE_LL_Uart);
		return XBEE_LL_ERROR_RX_TIMEOUT;
	}
	else
		return XBEE_LL_OK;
}

/*
 * *****************************************
 * *****************************************
 */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	int frame_length;
	volatile uint16_t dummy_read;

	dummy_read=UartHandle->Instance->RDR;

	if (XBEE_LL_RxBuffer != NULL)
	{
		XBEE_LL_RxBuffer[XBEE_LL_RxBufferIndex] = (char)dummy_read;

		if (XBEE_LL_Mode == XBEE_LL_MODE_API)
		{
			if (XBEE_LL_RxBuffer[XBEE_LL_RxBufferIndex] == (uint8_t)'~') {
				XBEE_LL_RxBuffer[0] = (uint8_t)'~';
				XBEE_LL_RxBufferIndex = 0;
			}
		}

		XBEE_LL_RxBufferIndex++;

		if (XBEE_LL_RxBufferIndex>=XBEE_LL_RxBufferLength)
		{
			if (XBEE_LL_Mode == XBEE_LL_MODE_TRANSPARENT)
			{
				XBEE_LL_RXState = XBEE_LL_RX_STATE_OK;
				XBEE_LL_RxReady = SET;

				if (xbee_ll_rx_thread_handler != NULL)
				{
					vTaskNotifyGiveFromISR(xbee_ll_rx_thread_handler, &xHigherPriorityTaskWoken );
					xbee_ll_rx_thread_handler = NULL;
					portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
				}
			}
			else
			{
				if (XBEE_LL_RXState == XBEE_LL_RX_STATE_WAIT_EOF)
				{
					XBEE_LL_RXState = XBEE_LL_RX_STATE_OK;
					XBEE_LL_RxReady = SET;

					if (xbee_ll_rx_thread_handler != NULL)
					{
						vTaskNotifyGiveFromISR(xbee_ll_rx_thread_handler, &xHigherPriorityTaskWoken );
						xbee_ll_rx_thread_handler = NULL;
						portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
					}
				}
				else {
					frame_length = 1+(((int)XBEE_LL_RxBuffer[1]<<8))+(int)XBEE_LL_RxBuffer[2];
					XBEE_LL_RXState = XBEE_LL_RX_STATE_WAIT_EOF;
					XBEE_LL_RxBufferLength+=frame_length;
					}
				}
			}
	}
}//END FUNCTION

/*
 * *****************************************
 * *****************************************
 */

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	XBEE_LL_TxReady = SET;

	if (xbee_ll_tx_thread_handler != NULL)
	{
		vTaskNotifyGiveFromISR(xbee_ll_tx_thread_handler, &xHigherPriorityTaskWoken );
		xbee_ll_tx_thread_handler = NULL;
		portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	}
}

/*
 * *****************************************
 * *****************************************
 */

void USART1_IRQHandler(void)
{

	if (LL_USART_IsActiveFlag_RXNE(XBEE_LL_Uart.Instance))
	{
		HAL_UART_RxCpltCallback(&XBEE_LL_Uart);
	}
	else if (LL_USART_IsActiveFlag_TXE(XBEE_LL_Uart.Instance))
	{
		HAL_UART_IRQHandler(&XBEE_LL_Uart);
	}
	else
	{
		if (LL_USART_IsActiveFlag_TC(XBEE_LL_Uart.Instance))
			HAL_UART_IRQHandler(&XBEE_LL_Uart);
		else if (LL_USART_IsActiveFlag_IDLE(XBEE_LL_Uart.Instance))
			LL_USART_ClearFlag_IDLE(XBEE_LL_Uart.Instance);
		else if (LL_USART_IsActiveFlag_ORE(XBEE_LL_Uart.Instance))
		{
			LL_USART_ClearFlag_ORE(XBEE_LL_Uart.Instance);
		}
	}
}
