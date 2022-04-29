/*
 * button_led.c
 *
 *  Created on: Apr 29, 2022
 *      Author: dimercur
 */

#include "stm32l4xx_hal.h"
#include "button_led.h"

#define BUTTON_Pin GPIO_PIN_0;

void BUTTON_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = BUTTON_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI0_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

/**
 * @brief This function handles EXTI line0 interrupt.
 */
void EXTI0_IRQHandler(void) {
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);

	// stuff to do here
}
