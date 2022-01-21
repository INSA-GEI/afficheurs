/*
 * tasks.c
 *
 *  Created on: 14 janv. 2022
 *      Author: dimercur
 */

#include "tasks.h"
#include "task.h"

#include "EPD_Test.h"

#define STACK_SIZE 200
#define PriorityDrawDisplay		16

/* Structure that will hold the TCB of the task being created. */
StaticTask_t xTaskBufferDrawDisplay;

/* Buffer that the task being created will use as its stack.  Note this is
    an array of StackType_t variables.  The size of StackType_t is dependent on
    the RTOS port. */
StackType_t xStackDrawDisplay[ STACK_SIZE ];

TaskHandle_t xHandleDrawDisplay = NULL;

/* Function that implements the task being created. */
void vTaskDrawDisplay(void* params)
{
	//for( ;; )
	//{
	//	/* Task code goes here. */
	//}

	EPD_7in5_V2_test();
}

/* Function that creates a task. */
void TASKS_Create(void)
{
	/* Create the task without using any dynamic memory allocation. */
	xHandleDrawDisplay = xTaskCreateStatic(
			vTaskDrawDisplay,       /* Function that implements the task. */
			"DrawDisplay",          /* Text name for the task. */
			STACK_SIZE,      /* Number of indexes in the xStack array. */
			NULL,    /* Parameter passed into the task. */
			PriorityDrawDisplay,/* Priority at which the task is created. */
			xStackDrawDisplay,          /* Array to use as the task's stack. */
			&xTaskBufferDrawDisplay);  /* Variable to hold the task's data structure. */

	/* puxStackBuffer and pxTaskBuffer were not NULL, so the task will have
        been created, and xHandleDrawDisplay will be the task's handle.  Use the handle
        to suspend the task. */
	//vTaskSuspend(xHandleDrawDisplay);
}

/* Function that creates a task. */
void TASKS_Run(void)
{
	//vTaskResume(xHandleDrawDisplay);
}
