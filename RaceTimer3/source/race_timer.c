/*
 * The Clear BSD License
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 * that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
#include "fsl_port.h"
#include "fsl_gpio.h"
#include "fsl_common.h"
#include "board.h"
#include "fsl_pit.h"  /* periodic interrupt timer */

#include "pin_mux.h"
#include "clock_config.h"


/*******************************************************************************
 * Board Definitions
 ******************************************************************************/
#define BOARD_LED_GPIO BOARD_LED_RED_GPIO
#define BOARD_LED_GPIO_PIN BOARD_LED_RED_GPIO_PIN

#define BOARD_PTB2_GPIO_PIN BOARD_INITPINS_ADC0_SE12_GPIO_PIN

#define BOARD_SW_GPIO BOARD_SW3_GPIO
#define BOARD_SW_PORT BOARD_SW3_PORT
#define BOARD_SW_GPIO_PIN BOARD_SW3_GPIO_PIN
#define BOARD_SW_IRQ BOARD_SW3_IRQ
#define BOARD_SW_IRQ_HANDLER BOARD_SW3_IRQ_HANDLER
#define BOARD_SW_NAME BOARD_SW3_NAME


/*******************************************************************************
 * Periodic Interrupt Timer (PIT) Definitions
 ******************************************************************************/
#define PIT_IRQ_ID PIT0_IRQn
/* Get source clock for PIT driver */
#define PIT_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_BusClk)
volatile bool pitIsrFlag = false;
volatile uint32_t systime = 0; //systime updated very 100 us = 4 days ==> NEED OVERFLOW protection


/*******************************************************************************
 * Definitions
 ******************************************************************************/
/****************
 *
  * change tick timing in FreeRTOSConfig.h to 100 us
 * configTICK_RATE_HZ = 10000
 *
 */

//#define MAX_LOG_LENGTH 20
#define MAX_LOG_LENGTH 64
// sizeof("Task1 Message 1, ticks 655535, z=3.14159  ")
/*******************************************************************************
* Globals
******************************************************************************/
float sqrt_array[1000]; // to hold results
/* Logger queue handle */
extern QueueHandle_t log_queue;
/* Whether the SW button is pressed */
volatile bool g_ButtonPress = false;
double lap_start, lap_time;


/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* Application API */
extern void write_task_1(void *pvParameters);
extern void write_task_2(void *pvParameters);
/* configUSE_IDLE_HOOK must be set to 1 in FreeRTOSConfig.h for the idle hook function to get called. */
extern void vApplicationIdleHook( void );

/* Logger API */
extern void log_add(char *log);
extern void log_init(uint32_t queue_length, uint32_t max_log_length);
extern void log_task(void *pvParameters);
/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Interrupt service function of switch.
 *
 * This function detects timer event
 * Numerically low interrupt priority numbers represent logically high
			interrupt priorities, therefore the priority of the interrupt must
			be set to a value equal to or numerically *higher* than
			configMAX_SYSCALL_INTERRUPT_PRIORITY.

			Interrupts that	use the FreeRTOS API must not be left at their
			default priority of	zero as that is the highest possible priority,
			which is guaranteed to be above configMAX_SYSCALL_INTERRUPT_PRIORITY,
			and	therefore also guaranteed to be invalid.
 */
void BOARD_SW_IRQ_HANDLER(void)
{	double lockout_time;
	char log[MAX_LOG_LENGTH + 1];
    /* Clear external interrupt flag. */
    GPIO_PortClearInterruptFlags(BOARD_SW_GPIO, 1U << BOARD_SW_GPIO_PIN);
    /* Change state of button. */
    g_ButtonPress = true;
    /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
      exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif

       lap_start = (double)(xTaskGetTickCount()/10000.0); // with ticks at 100 us, convert to sec
       lockout_time = lap_start + 10.0; // to avoid false double triggers
       sprintf(log, "\n\rTimer Triggered! \n\r");
       log_add(log);
       while((double)(xTaskGetTickCount()/10000.0) < lockout_time)
       {   lap_time =  (double)(xTaskGetTickCount()/10000.);
           	   if (((int)(100.0*lap_time) % 10) == 0)
           	   {	sprintf(log, "Elapsed time = %8.3f sec \r",
           			   	   lap_time - lap_start);
               		log_add(log);
               		LED_RED_TOGGLE();
           	   }
         }
}

/*!
 * @brief Main function
 */
int main(void)
{	float pif = 3.14159;
	double pid = 3.14159;


   /* Structure of initialize PIT */
    pit_config_t pitConfig;

   /* Define the init structure for the input switch pin */
       gpio_pin_config_t sw_config = {
           kGPIO_DigitalInput, 0,
       };

    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    asm (".global _printf_float"); // cause linker to include floating point

    /* initialize LEDs */
    LED_BLUE_INIT(LOGIC_LED_OFF);
 	LED_GREEN_INIT(LOGIC_LED_OFF);
 	LED_RED_INIT(LOGIC_LED_OFF);

/* start periodic interrupt timer- should be in its own file */
 	PIT_GetDefaultConfig(&pitConfig);
 	    /* Init pit module */
 	    PIT_Init(PIT, &pitConfig);
 	    /* Set timer period for channel 0 */


 	    PIT_SetTimerPeriod(PIT, kPIT_Chnl_0, USEC_TO_COUNT(100U, PIT_SOURCE_CLOCK)); // 100 us timing
 	    /* Enable timer interrupts for channel 0 */
 	    PIT_EnableInterrupts(PIT, kPIT_Chnl_0, kPIT_TimerInterruptEnable);
 	    /* Enable at the NVIC */
 	    EnableIRQ(PIT_IRQ_ID);
 	    /* Start channel 0 */
 	    PRINTF("\r\nStarting channel No.0 ...");
 	    PIT_StartTimer(PIT, kPIT_Chnl_0);

    /* Initialize logger for 32 entries with maximum lenght of one log 20 B */
    log_init(32, MAX_LOG_LENGTH); // buffer up to 32 lines of text
    /* welcome message */
    PRINTF("\n\r EE192 Spring 2018 Race Timer v0.0\n\r");
	LED_GREEN_ON();
	PRINTF("Floating point PRINTF %8.4f  %8.4lf\n\r", pif, pid);
//	printf("Floating point printf %8.4f  %8.4lf\n\r", pif, pid); // only for semihost console, not release!

	 /* Init input switch GPIO. */
	    PORT_SetPinInterruptConfig(BOARD_SW_PORT, BOARD_SW_GPIO_PIN, kPORT_InterruptFallingEdge);
	    NVIC_SetPriority(BOARD_SW_IRQ,8); // make sure priority is lower than FreeRTOS queue
	    EnableIRQ(BOARD_SW_IRQ);
	    GPIO_PinInit(BOARD_SW_GPIO, BOARD_SW_GPIO_PIN, &sw_config);

    if (xTaskCreate(write_task_1, "WRITE_TASK_1", configMINIMAL_STACK_SIZE + 166, NULL, tskIDLE_PRIORITY + 2, NULL) !=
        pdPASS)
    {   PRINTF("Task creation failed!.\r\n");
        while (1); // hang indefinitely
    }
  /*  if (xTaskCreate(write_task_2, "WRITE_TASK_2", configMINIMAL_STACK_SIZE + 166, NULL, tskIDLE_PRIORITY + 2, NULL) !=
        pdPASS)
    {   PRINTF("Task creation failed!.\r\n");
        while (1);
    }
    */
    PRINTF("Starting Scheduler\n\r");

    LED_GREEN_OFF();
    vTaskStartScheduler();
    /* should not get here */

    for (;;);
}

/*******************************************************************************
 * Interrupt functions
 ******************************************************************************/

void PIT0_IRQHandler(void)
{
    /* Clear interrupt flag.*/
	systime++; /* hopefully atomic operation */
    PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag);
    pitIsrFlag = true;
    LED_GREEN_TOGGLE();
}

/*******************************************************************************
 * Application functions- should be in separate file for modularity
 ******************************************************************************/


