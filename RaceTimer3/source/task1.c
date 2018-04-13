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

/* K64F switch definitions */
#define BOARD_SW_GPIO BOARD_SW3_GPIO
#define BOARD_SW_GPIO_PIN BOARD_SW3_GPIO_PIN
#define BOARD_SW_IRQ BOARD_SW3_IRQ

// PTB2
#define BOARD_PTB2_GPIO_PIN BOARD_INITPINS_ADC0_SE12_GPIO_PIN

// PTD1
#define BOARD_PTD1_GPIO_PIN BOARD_INITPINS_PTD1_PIN

/* globals */
extern uint32_t ulIdleCycleCount;

extern double lap_start, lap_time;
extern double lockout_time;
extern volatile bool timer_triggered;
extern volatile bool timer_lockout_period;

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
  //  uint32_t systime_start;

    const TickType_t xDelay100ms = pdMS_TO_TICKS( 100 );
    // double z;
    double lap_time;

    tick_start = xTaskGetTickCount();
  //  lap_start = (double)(tick_start/10000.); // with ticks at 100 us, convert to sec
    while(1)
    {   lap_time =  (double)(xTaskGetTickCount()/10000.);

   		sprintf(log, "Start at %10.3f. Current lap time = %8.3f sec \r",
   				    	lap_start, lap_time - lap_start);
        log_add(log);

        if( timer_lockout_period  && (lap_time > lockout_time))
        {  timer_lockout_period = false; // allow next interrupt
        		LED_RED_OFF();
        		LED_GREEN_ON(); // running
       		GPIO_PortClearInterruptFlags(BOARD_SW_GPIO, 1U << BOARD_SW_GPIO_PIN); // disable pending interrupts
       		NVIC_ClearPendingIRQ(BOARD_SW_IRQ);
       		EnableIRQ(BOARD_SW_IRQ); // re-enable trigger interrupt
       		//GPIO_PortClearInterruptFlags(GPIOB, 1U << BOARD_PTB2_GPIO_PIN);
       		//NVIC_ClearPendingIRQ(PORTB_IRQn);
       		//EnableIRQ(PORTB_IRQn); // only one interrupt per car start, wait for back wheels, etc

       		GPIO_PortClearInterruptFlags(GPIOD, 1U << BOARD_PTD1_GPIO_PIN);
       		NVIC_ClearPendingIRQ(PORTD_IRQn);
       		EnableIRQ(PORTD_IRQn); // only one interrupt per car start, wait for back wheels, etc
         }

        vTaskDelay(xDelay100ms); // relative delay in ticks
        // LED_BLUE_TOGGLE();
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


