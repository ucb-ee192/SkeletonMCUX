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

#define MAX_LOG_LENGTH 64

static QueueHandle_t log_queue = NULL;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static void log_task(void *pvParameters);

/*******************************************************************************
 * Logger functions
 ******************************************************************************/
/*!
 * @brief log_add function - make sure safe from inside interrupt
 */
void log_add(char *log)
{
    xQueueSendFromISR(log_queue, log, 0);  // send data to back of queue,
    // non-blocking, wait=0 ==> return immediately if the queue is already full.
}

/*!
 * @brief log_init function
 */
void log_init(uint32_t queue_length, uint32_t max_log_length)
{
    log_queue = xQueueCreate(queue_length, max_log_length);
    if (xTaskCreate(log_task, "log_task", configMINIMAL_STACK_SIZE + 166, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
    {   PRINTF("Task creation failed!.\r\n");
        while (1)
            ;
    }
    vQueueAddToRegistry(log_queue, "PrintQueue");
}

/*!
 * @brief log_print_task function
 */
static void log_task(void *pvParameters)
{   uint32_t counter = 0;
    char log[MAX_LOG_LENGTH + 1];
    while (1)
    {   xQueueReceive(log_queue, log, portMAX_DELAY);
//       PRINTF("Log %d: %s\r\n", counter, log);
        PRINTF("Log %d: %s", counter, log);
        counter++;
    }
}
