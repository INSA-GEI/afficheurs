/*
 * config.c
 *
 *  Created on: 13 mai 2022
 *      Author: dimercur
 */

#include "stm32l4xx_hal.h"
#include "config.h"

/*!< TRCENA: Enable trace and debug block DEMCR (Debug Exception and Monitor Control Register */
#define DEBUG_InitCycleCounter()  	CoreDebug->DEMCR|= (1UL<<24)

/*!< Reset cycle counter */
#define DEBUG_ResetCycleCounter() 	DWT->CYCCNT = 0

/*!< Enable cycle counter */
#define DEBUG_EnableCycleCounter() 	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk

/*!< Disable cycle counter */
#define DEBUG_DisableCycleCounter() DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk

/*!< Read cycle counter register */
#define DEBUG_GetCycleCounter()		DWT->CYCCNT

void DEBUG_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, RESET);
}

void DEBUG_StartTimeMeasure(void) {
	DEBUG_InitCycleCounter();
	__disable_irq();

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, SET);
	DEBUG_ResetCycleCounter();
	DEBUG_EnableCycleCounter();
}

uint32_t DEBUG_EndTimeMeasure(void) {
	uint32_t val = DEBUG_GetCycleCounter();
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, RESET);
	__enable_irq();
	DEBUG_DisableCycleCounter();

	return val;
}
