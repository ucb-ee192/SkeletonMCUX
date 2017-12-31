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

/*******************************************************************************
 * Definitions
 ******************************************************************************/

//#define MAX_LOG_LENGTH 20
#define MAX_LOG_LENGTH sizeof("Task1 Message 1, ticks 655535, z=3.14159  ")



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
    uint32_t i = 0, j = 0, systime_start;
    const TickType_t xDelay1000ms = pdMS_TO_TICKS( 1000 );
    // double z;

    tick_start = xTaskGetTickCount();
    for (i = 0; i < 9; i++)
    {
    	systime_start = systime;
    	for (j=0; j < 1000; j++)
    		sqrt_array[j]= sqrtf((float)j);  // sqrt ~ 100 us, sqrtf ~ 60 us
        sprintf(log, "Task1 # %d, 1000 sqrt time (us) %ld",
        		(int)i, 100*(long)(systime-systime_start));
        log_add(log);
        vTaskDelay(xDelay1000ms); // relative delay in ticks
 //       vTaskDelayUntil( &tick_start, xDelay1000ms );  // unblocks at absolute time- needed for periodic functions
        LED_RED_TOGGLE();
        taskYIELD();
    }
    tick_end = xTaskGetTickCount();
    sprintf(log, "Task1 finished. tick_start %d tick_end %d", (int) tick_start, (int) tick_end);
    log_add(log);
    vTaskSuspend(NULL);
}


