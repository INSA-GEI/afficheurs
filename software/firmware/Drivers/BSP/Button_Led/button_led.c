/*
 * button_led.c
 *
 *  Created on: Apr 29, 2022
 *      Author: dimercur
 */

#include "stm32l4xx_hal.h"
#include "button_led.h"

#define BUTTON_Pin GPIO_PIN_0
static TaskHandle_t button_thread_handler;

void BUTTON_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* Enable GPIOB clock */
	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitStruct.Pin = BUTTON_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI0_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

BUTTON_Event BUTTON_WaitForEvent(void) {
	uint32_t ticks_nbr=0;
	BUTTON_Event status = BUTTON_ShortPress;
	button_thread_handler= xTaskGetCurrentTaskHandle();

	// wait for event, without timeout
	ulTaskNotifyTake( pdFALSE, 0 ); // unlimited wait

	button_thread_handler = NULL;

	// check if it is a long press or short press
	vTaskDelay(1);
	ticks_nbr++;

	while ((HAL_GPIO_ReadPin(GPIOB, BUTTON_Pin)) && (ticks_nbr<30)) {
		vTaskDelay(1);
		ticks_nbr++;
	}

	if (ticks_nbr>=30)
		status = BUTTON_LongPress;

	// Reactivation of exti
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);

	return status;
}

/**
 * @brief This function handles EXTI line0 interrupt.
 */
void EXTI0_IRQHandler(void) {
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);

	if (button_thread_handler != NULL) {
		/* Notify the task that an event has been emitted. */
		vTaskNotifyGiveFromISR(button_thread_handler, &xHigherPriorityTaskWoken );

		/* There are no more eventin progress, so no tasks to notify. */
		button_thread_handler = NULL;
		HAL_NVIC_DisableIRQ(EXTI0_IRQn); // On arrete les IT tant que le bouton n'est pas relaché

		/* If xHigherPriorityTaskWoken is now set to pdTRUE then a context switch
			    should be performed to ensure the interrupt returns directly to the highest
			    priority task.  The macro used for this purpose is dependent on the port in
			    use and may be called portEND_SWITCHING_ISR(). */
		portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	}
}
