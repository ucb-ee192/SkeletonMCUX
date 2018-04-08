/*
 * task1.c
 *
 *  Created on: Dec 31, 2017
 *      Author: ronf
 */

/*!
 * @brief write_task_1 function
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
extern uint32_t ulIdleCycleCount;

/*******************************************************************************
 * Definitions
 ******************************************************************************/

//#define MAX_LOG_LENGTH 20
#define MAX_LOG_LENGTH sizeof("Task1 Message 1, ticks 655535, z=3.14159 \n\r     ")



/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* Application API */
void write_task_1(void *pvParameters);


/* Logger API */
extern void log_add(char *log);
extern void log_task(void *pvParameters);


/*!
 * @brief write_task_1 function
 */
void write_task_1(void *pvParameters)
{
    char log[MAX_LOG_LENGTH + 1];
   TickType_t tick_start, tick_end;
    uint32_t systime_start;
    int i, j;
    const TickType_t xDelay1000ms = pdMS_TO_TICKS( 1000 );
    // double z;
    double lap_start, lap_time;

    tick_start = xTaskGetTickCount();
    lap_start = (double)(tick_start/10000.); // with ticks at 100 us, convert to sec
    while(1)
    {   lap_time =  (double)(xTaskGetTickCount()/10000.);

   		sprintf(log, "Elapsed time = %8.3f sec \r",
    		lap_time - lap_start);
        log_add(log);
        vTaskDelay(xDelay1000ms); // relative delay in ticks
        LED_RED_TOGGLE();
        taskYIELD();
    }

    tick_end = xTaskGetTickCount();
    sprintf(log, "Task1 done. tick_start %d tick_end %d\n\r", (int) tick_start, (int) tick_end);
    log_add(log);
    sprintf(log, "Task1. ulIdleCycleCount=%d\n\r",
    		(long)ulIdleCycleCount);
        log_add(log);
    //vTaskDelay(3*xDelay1000ms); // give time for all tasks to finish printing before suspending
    vTaskSuspend(NULL);
}


