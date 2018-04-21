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
//#include "fsl_pit.h"  /* periodic interrupt timer */
#include "fsl_ftm.h"

#include "pin_mux.h"
#include "clock_config.h"


/*******************************************************************************
 * Board Definitions - make sure sure in Pin config to do Update Project Code
 ******************************************************************************/
#define BOARD_LED_GPIO BOARD_LED_RED_GPIO
#define BOARD_LED_GPIO_PIN BOARD_LED_RED_GPIO_PIN

// PTB2
#define BOARD_PTB2_GPIO_PIN BOARD_INITPINS_ADC0_SE12_GPIO_PIN

// PTD1 J2-12 on freescale K64 board
#define BOARD_PTD1_GPIO_PIN BOARD_INITPINS_PTD1_PIN

//SW3
#define BOARD_SW_GPIO BOARD_SW3_GPIO  // = GPIOA
#define BOARD_SW_PORT BOARD_SW3_PORT // = PORTA
#define BOARD_SW_GPIO_PIN BOARD_SW3_GPIO_PIN // = 4 (PTA4)
#define BOARD_SW_IRQ BOARD_SW3_IRQ // = PORTA_IRQn
#define BOARD_SW_IRQ_HANDLER BOARD_SW3_IRQ_HANDLER // PORTA_IRQHandler
#define BOARD_SW_NAME BOARD_SW3_NAME



/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* The Flextimer base address/channel used for board */
/* using J2-4 */
#define BOARD_FTM_BASEADDR FTM0
#define BOARD_FTM_OUT_CHANNEL kFTM_Chnl_3

/* Get source clock for FTM driver */
/* bus clock is 60 MHz */
#define FTM_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_BusClk)

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
/* Logger queue handle */
extern QueueHandle_t log_queue;

/* timer variables */
double lap_start = 0.0;
double lap_time;
double lockout_time;
volatile bool timer_triggered = false;
volatile bool timer_lockout_period = false;


/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void start_timer(void);
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
{   /* Clear external interrupt flag. */
    GPIO_PortClearInterruptFlags(BOARD_SW_GPIO, 1U << BOARD_SW_GPIO_PIN);
    NVIC_ClearPendingIRQ(BOARD_SW_IRQ);
    DisableIRQ(BOARD_SW_IRQ); // only one interrupt per car start, wait for back wheels, etc
    start_timer();
    /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
      exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

void PORTB_IRQHandler(void)
{   /* Clear external interrupt flag. */
    GPIO_PortClearInterruptFlags(GPIOB, 1U << BOARD_PTB2_GPIO_PIN);
    NVIC_ClearPendingIRQ(PORTB_IRQn);
    DisableIRQ(PORTB_IRQn); // only one interrupt per car start, wait for back wheels, etc
    start_timer();
    /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
      exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

void PORTD_IRQHandler(void)
{   /* Clear external interrupt flag. */
    GPIO_PortClearInterruptFlags(GPIOD, 1U << BOARD_PTD1_GPIO_PIN);
    NVIC_ClearPendingIRQ(PORTD_IRQn);
    DisableIRQ(PORTD_IRQn); // only one interrupt per car start, wait for back wheels, etc
    start_timer();
    /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
      exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}




void start_timer(void)
{ char log[MAX_LOG_LENGTH + 1];
    timer_triggered = true;
    timer_lockout_period = true;

/// *** don't use any FreeRTOS inside ISR...
       // get precise current time
       lap_time = (double)(xTaskGetTickCountFromISR()/10000.0); // with ticks at 100 us, convert to sec
       sprintf(log, "\n\rTimer Triggered! Previous lap = %10.3f\n\r", lap_time - lap_start);
       log_add(log);
       lap_start = lap_time; // next lap start time
       lockout_time = lap_start + 5.0; // to avoid false double triggers
       //sprintf(log, "\n\rTimer Triggered! \n\r");
       //log_add(log);
       LED_RED_ON(); // triggered, in lockout period
}

/*!
 * @brief Main function
 */
int main(void)
{   ftm_config_t ftmInfo;
	uint32_t compareValue = 0x40; // using toggle, so get 50% duty cycle
   /* Define the init structure for the input switch pin */
       gpio_pin_config_t sw_config = {
           kGPIO_DigitalInput, 0,
       };
   /* define init structure for input line from IR sensor for timer */
 /*      gpio_pin_config_t ptb2_config = {
                  kGPIO_DigitalInput, 0,
              }; */
       gpio_pin_config_t ptd1_config = {
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

    /* Initialize logger for 32 entries with maximum lenght of one log 20 B */
    log_init(32, MAX_LOG_LENGTH); // buffer up to 32 lines of text
    /* welcome message */
    PRINTF("\n\r using slow slew on FTM0 Ch3 to avoid glitches on trigger");
    PRINTF("\n\r EE192 Spring 2018 Race Timer v0.0\n\r");
    PRINTF("Using SW3 PTA4 or PTD1 (J2-12)for trigger, and FTM0 Ch3 (J2-4) for LED drive\n\r");
	// LED_GREEN_ON();
//	PRINTF("Floating point PRINTF %8.4f  %8.4f\n\r", pif, pid);
//	printf("Floating point printf %8.4f  %8.4lf\n\r", pif, pid); // only for semihost console, not release!

	 /* Init input switch GPIO. */
	    PORT_SetPinInterruptConfig(BOARD_SW_PORT, BOARD_SW_GPIO_PIN, kPORT_InterruptFallingEdge);
	    NVIC_SetPriority(BOARD_SW_IRQ,24); // make sure priority is lower than FreeRTOS queue
	    EnableIRQ(BOARD_SW_IRQ);
	    GPIO_PinInit(BOARD_SW_GPIO, BOARD_SW_GPIO_PIN, &sw_config);

	  /* Init PTB2 GPIO */
/*	   PORT_SetPinInterruptConfig(PORTB, BOARD_PTB2_GPIO_PIN, kPORT_InterruptRisingEdge);
	   NVIC_SetPriority(PORTB_IRQn,24); // make sure priority is lower than FreeRTOS queue
	   EnableIRQ(PORTB_IRQn);
	   GPIO_PinInit(GPIOB, BOARD_PTB2_GPIO_PIN, &ptb2_config);
	*/

	   /* Init PTD1 GPIO */
	    /* if using digital filter: PORT_SetDigitalFilterConfig */
	   PORT_SetPinInterruptConfig(PORTD, BOARD_PTD1_GPIO_PIN, kPORT_InterruptRisingEdge);
	   NVIC_SetPriority(PORTD_IRQn,24); // make sure priority is lower than FreeRTOS queue
	   EnableIRQ(PORTD_IRQn);
	   GPIO_PinInit(GPIOD, BOARD_PTD1_GPIO_PIN, &ptd1_config);

    if (xTaskCreate(write_task_1, "WRITE_TASK_1", configMINIMAL_STACK_SIZE + 300, NULL, tskIDLE_PRIORITY + 2, NULL) !=
        pdPASS)
    {   PRINTF("Task creation failed!.\r\n");
        while (1); // hang indefinitely
    }


    FTM_GetDefaultConfig(&ftmInfo);
    #if defined(FTM_PRESCALER_VALUE)
        /* Set divider to FTM_PRESCALER_VALUE instead of default 1 to be the led toggling visible */
        ftmInfo.prescale = FTM_PRESCALER_VALUE;
    #endif

        /* Initialize FTM module */
        FTM_Init(BOARD_FTM_BASEADDR, &ftmInfo);

        /* Setup the output compare mode to toggle output on a match, so divide by 2 */
        FTM_SetupOutputCompare(BOARD_FTM_BASEADDR, BOARD_FTM_OUT_CHANNEL, kFTM_ToggleOnMatch, compareValue);

        /* Set the timer to be in free-running mode */
        BOARD_FTM_BASEADDR->MOD = 750; // BusClk 60 MHz  ?, 33.3 ns ==> 25 us or 40 kHz

        /* Update the buffered registers */
        FTM_SetSoftwareTrigger(BOARD_FTM_BASEADDR, true);
        FTM_StartTimer(BOARD_FTM_BASEADDR, kFTM_SystemClock);

        PRINTF("Starting Scheduler\n\r");

       LED_GREEN_OFF();
    vTaskStartScheduler();
    /* should not get here */

    for (;;);
}

/*******************************************************************************
 * Interrupt functions
 ******************************************************************************/



/*******************************************************************************
 * Application functions- should be in separate file for modularity
 ******************************************************************************/


