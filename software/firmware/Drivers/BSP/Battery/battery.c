/*
 * battery.C
 *
 *  Created on: Apr 29, 2022
 *      Author: dimercur
 */

#include "battery.h"
#include "panic.h"

static ADC_HandleTypeDef hadc1;
static uint8_t conversion_complete;
static uint16_t adc_raw_value;
static TaskHandle_t battery_thread_handler;

/**
 * @brief ADC1 Initialization Function
 * @param None
 * @retval None
 */
BATTERY_Status BATTERY_Init(void) {
	ADC_ChannelConfTypeDef sConfig = {0};

	/** Common config
	 */
	hadc1.Instance = ADC1;
	hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
	hadc1.Init.Resolution = ADC_RESOLUTION_12B;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
	hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	hadc1.Init.LowPowerAutoWait = DISABLE;
	hadc1.Init.ContinuousConvMode = DISABLE;
	hadc1.Init.NbrOfConversion = 1;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc1.Init.DMAContinuousRequests = DISABLE;
	hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	hadc1.Init.OversamplingMode = DISABLE;
	if (HAL_ADC_Init(&hadc1) != HAL_OK)
		return BATTERY_HW_ERROR;

	/** Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_5;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_24CYCLES_5;
	sConfig.SingleDiff = ADC_SINGLE_ENDED;
	sConfig.OffsetNumber = ADC_OFFSET_NONE;
	sConfig.Offset = 0;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
		return BATTERY_HW_ERROR;

	/* Run the ADC calibration in single-ended mode */
	if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK) {
		/* Calibration Error */
		return BATTERY_HW_ERROR;
	}

	return BATTERY_OK;
}

BATTERY_Status BATTERY_GetVoltage(uint16_t *val) {
	uint32_t ulNotificationValue;
	conversion_complete = 0;
	adc_raw_value = 0;

	battery_thread_handler = xTaskGetCurrentTaskHandle();

	if (HAL_ADC_Start_IT(&hadc1) != HAL_OK)
		return BATTERY_HW_ERROR;

	ulNotificationValue = ulTaskNotifyTake( pdTRUE, pdMS_TO_TICKS(100)); // wait max 100 ms

	if (ulNotificationValue == 1) {
		/* The transmission ended as expected. */
		*val = adc_raw_value;
	} else {
		/* The call to ulTaskNotifyTake() timed out. */
		return BATTERY_TIMEOUT;
	}

	battery_thread_handler = NULL;

	return BATTERY_OK;
}

/**
 * @brief ADC MSP Initialization
 * This function configures the hardware resources used in this example
 * @param hadc: ADC handle pointer
 * @retval None
 */
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

	if(hadc->Instance==ADC1) {
		/** Initializes the peripherals clock
		 */
		PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
		PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_SYSCLK;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
			PANIC_Handler(PANIC_EVT_ADC_MSP_CONFIG_ERROR);

		/* Peripheral clock enable */
		__HAL_RCC_ADC_CLK_ENABLE();

		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**ADC1 GPIO Configuration
    PA0     ------> ADC1_IN5
		 */
		GPIO_InitStruct.Pin = GPIO_PIN_0;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* ADC1 interrupt Init */
		HAL_NVIC_SetPriority(ADC1_IRQn, 5, 0);
		HAL_NVIC_EnableIRQ(ADC1_IRQn);
	}
}

/**
 * @brief ADC MSP De-Initialization
 * This function freeze the hardware resources used in this example
 * @param hadc: ADC handle pointer
 * @retval None
 */
void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc) {
	if(hadc->Instance==ADC1) {
		/* Peripheral clock disable */
		__HAL_RCC_ADC_CLK_DISABLE();

		/**ADC1 GPIO Configuration
    PA0     ------> ADC1_IN5
		 */
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0);

		/* ADC1 interrupt DeInit */
		HAL_NVIC_DisableIRQ(ADC1_IRQn);
	}
}

/**
 * @brief This function handles ADC1 global interrupt.
 */
void ADC1_IRQHandler(void) {
	HAL_ADC_IRQHandler(&hadc1);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	adc_raw_value = HAL_ADC_GetValue(hadc);

	if (battery_thread_handler != NULL) {
		/* Notify the task that an event has been emitted. */
		vTaskNotifyGiveFromISR(battery_thread_handler, &xHigherPriorityTaskWoken );

		/* There are no more eventin progress, so no tasks to notify. */
		battery_thread_handler = NULL;

		/* If xHigherPriorityTaskWoken is now set to pdTRUE then a context switch
			    should be performed to ensure the interrupt returns directly to the highest
			    priority task.  The macro used for this purpose is dependent on the port in
			    use and may be called portEND_SWITCHING_ISR(). */
		portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	}
}
