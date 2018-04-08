/*
 * task2.c
 *
 *  Created on: Dec 31, 2017
 *      Author: ronf
 */

/*System includes.*/
#include <stdio.h>
#include <math.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_pit.h"  /* periodic interrupt timer */

#include "pin_mux.h"
#include "clock_config.h"

/* globals */
extern volatile uint32_t systime; //systime updated very 100 us = 4 days ==> NEED OVERFLOW protection
extern float sqrt_array[1000]; // to hold results

/*******************************************************************************
 * Definitions
 ******************************************************************************/

//#define MAX_LOG_LENGTH 20
#define MAX_LOG_LENGTH 64
//sizeof("Task1 Message 1, ticks 655535, z=3.14159  \n\r    ")

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* Application API */
void write_task_2(void *pvParameters);


/* Logger API */
extern void log_add(char *log);
extern void log_task(void *pvParameters);


/*!
 * @brief write_task_2 function
 */
void write_task_2(void *pvParameters)
{   TickType_t tick_start, tick_now;
	const TickType_t xDelay700ms = pdMS_TO_TICKS( 700 );
    char log[MAX_LOG_LENGTH + 1];
    uint32_t i = 0, j=0;
    // double z;

    tick_start = xTaskGetTickCount();
    for (i = 0; i < 10; i++)
    {   tick_now = xTaskGetTickCount();
    /* deliberately conflict writing to array between 2 tasks */
    	for (j=0; j < 1000; j++)
        		sqrt_array[j]= sqrt((float)j);
   		sprintf(log, "Task2 Mess %d, tick_now %d, 100*z=%d\n\r",
    		(int)i, (int)(tick_now), (long)(100.0*sqrt_array[i]));

        log_add(log);
        //vTaskDelay(xDelay700ms); // relative delay in ticks
        vTaskDelayUntil( &tick_start, xDelay700ms );  // unblocks at absolute time- needed for periodic functions
        LED_BLUE_TOGGLE();
        taskYIELD();
    }
    tick_now = xTaskGetTickCount();
    sprintf(log, "Task2 done. tick_start %d tick_now %d\n\r",
        		(int)i, (int)tick_now);
    log_add(log); // add message to print queue
    vTaskSuspend(NULL);
}

