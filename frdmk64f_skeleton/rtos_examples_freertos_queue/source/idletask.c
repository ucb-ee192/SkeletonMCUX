/*
 * idletask.c
 *
 *  Created on: Dec 31, 2017
 *      Author: ronf
 *
 *      idle task: foreground prcesses that do not need to be real time, e.g. user IO
 *
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


/* Logger API */
#define MAX_LOG_LENGTH 64 // max line length for print
extern void log_add(char *log);

/* Declare a variable that will be incremented by the hook function. */
volatile uint32_t ulIdleCycleCount = 0UL;
static double ZSum = 0.0; //accumulate with every idle cycle


/* Idle hook functions MUST be called vApplicationIdleHook(), take no parameters, and return void. */
void vApplicationIdleHook( void )
{
	 char log[MAX_LOG_LENGTH + 1]= "Idle. No sprintf\n\r"; // this uses stack...
	 TickType_t tick_start, tick_now;
	 uint32_t PITCount_start, PITCount_end;

	 /* This hook function does nothing but increment a counter. */
	ulIdleCycleCount++;
	tick_start = xTaskGetTickCount();
	PITCount_start = PIT_GetCurrentTimerCount(PIT, kPIT_Chnl_0);
	//ZSum = ZSum + cos((double)ulIdleCycleCount/1e6);  // *** CAUTION uses alot of stack ****
	 ZSum = ZSum+1.0;
	// every 1 million idle cycles
	if ((ulIdleCycleCount % 1000000) == 0)
	{	PITCount_end = PIT_GetCurrentTimerCount(PIT, kPIT_Chnl_0);
		sprintf(log, "Idle. sum of cos = %d \n\r", (long) ZSum);
		log_add(log);
/*		log[0] = (char) ((PITCount_start & 0x0000000f)+0x30); //ascii lower digit
		log[1] = '\n';
		log[2] = 0; // make string
		*/
		sprintf(log, "Idle. PIT: start=%d  end=%d \n\r",
						(long) PITCount_start, (long) PITCount_end);
		log_add(log);
		tick_now = xTaskGetTickCount();
	    sprintf(log, "Idle. tick_start %d tick_now %d\n\r",
	        		(int)tick_start, (int)tick_now);
	    log_add(log);
	}

}
