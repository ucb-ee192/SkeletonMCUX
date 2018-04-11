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

#define BOARD_SW_IRQ BOARD_SW3_IRQ

/* Logger API */
#define MAX_LOG_LENGTH 64 // max line length for print
extern void log_add(char *log);

/* Declare a variable that will be incremented by the hook function. */
volatile uint32_t ulIdleCycleCount = 0UL;

extern double lap_start, lap_time;
extern double lockout_time;
extern volatile bool timer_triggered;
extern volatile bool timer_lockout_period;

/* Idle hook functions MUST be called vApplicationIdleHook(), take no parameters, and return void. */
void vApplicationIdleHook( void )
{
	 char log[MAX_LOG_LENGTH + 1]= "Idle. No sprintf\n\r"; // this uses stack...
	 TickType_t tick_start, tick_now;
//	 uint32_t PITCount_start, PITCount_end;

	 /* This hook function does nothing but increment a counter. */
	ulIdleCycleCount++;
	tick_start = xTaskGetTickCount();
//	PITCount_start = PIT_GetCurrentTimerCount(PIT, kPIT_Chnl_0);

	// every 10 million idle cycles
	if ((ulIdleCycleCount % 10000000) == 0)
	{	LED_BLUE_TOGGLE();

		// PITCount_end = PIT_GetCurrentTimerCount(PIT, kPIT_Chnl_0);
/*		sprintf(log, "Idle. PIT: start=%d  end=%d \n\r",
						(long) PITCount_start, (long) PITCount_end);
		log_add(log);
		tick_now = xTaskGetTickCount();
	    sprintf(log, "Idle. tick_start %d tick_now %d\n\r",
	        		(int)tick_start, (int)tick_now);
	    log_add(log);
	    */
	}

}
