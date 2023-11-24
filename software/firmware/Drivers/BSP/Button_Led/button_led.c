#include "stm32l4xx_hal.h"
#include "button_led.h"

/*
 * *****************************************
 * *****************************************
 */

#define BUTTON_Pin GPIO_PIN_0

/*
 * *****************************************
 * *****************************************
 */

static TaskHandle_t button_thread_handler;

/*
 * *****************************************
 * *****************************************
 */

void BUTTON_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitStruct.Pin = BUTTON_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	HAL_NVIC_SetPriority(EXTI0_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

/*
 * *****************************************
 * *****************************************
 */

BUTTON_Event BUTTON_WaitForEvent(void)
{
	uint32_t ticks_nbr=0;
	BUTTON_Event status = BUTTON_ShortPress;
	button_thread_handler= xTaskGetCurrentTaskHandle();

	ulTaskNotifyTake( pdTRUE, portMAX_DELAY );

	button_thread_handler = NULL;

	vTaskDelay(1);
	ticks_nbr++;

	while ((HAL_GPIO_ReadPin(GPIOB, BUTTON_Pin)) && (ticks_nbr<15))
	{
		vTaskDelay(1);
		ticks_nbr++;
	}

	if (ticks_nbr>=10)
		status = BUTTON_LongPress;

	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);

	return status;
}

/*
 * *****************************************
 * *****************************************
 */

void EXTI0_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);

	if (button_thread_handler != NULL)
	{
		vTaskNotifyGiveFromISR(button_thread_handler, &xHigherPriorityTaskWoken);

		button_thread_handler = NULL;
		HAL_NVIC_DisableIRQ(EXTI0_IRQn);

		portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	}
}
