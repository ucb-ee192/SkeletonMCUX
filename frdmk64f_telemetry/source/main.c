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

#include "board.h"
#include <stdlib.h>
#include "pin_mux.h"
#include "clock_config.h"
#include "telemetry/telemetry_uart.h"
#include "telemetry/telemetry.h"

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
volatile float motor_pwm = 0;
volatile float servo_pwm = 0;
volatile uint32_t time = 0;

/*******************************************************************************
 * Code
 ******************************************************************************/
void delay(uint32_t t)
{
	uint32_t i;
	for(i=0; i< t; i++)
	{
		asm("nop");
	}
}

float r2()
{
  float result = (float)rand() / (float)RAND_MAX;
  if (result > .5) {
    return .5;
  } else if (result < 0) {
    return 0;
  }
    return result;
}

void init_board(){
    BOARD_InitPins();
    BOARD_BootClockRUN();
}
/*!
 * @brief Main function
 */
int main(void)
{
    time = 0;
    motor_pwm = .5;
    servo_pwm = .5;

    init_telemetry_uart();
    register_telemetry_variable("uint", "numeric", "time", "Time", "ms", (uint32_t*) &time, 0, 0);
    register_telemetry_variable("float", "numeric", "motor", "Motor PWM", "Percent DC", (uint32_t*) &motor_pwm, (uint32_t) 0.0f, (uint32_t) 0.0f);
//    register_telemetry_variable("float", "numeric", "servo", "Servo PWM", "Percent DC", (uint32_t*) &servo_pwm, (uint32_t) 0.0f, (uint32_t) 0.0f);
    transmit_header();

    while (1)
    {
    	time += 50;
    	motor_pwm = r2();
//    	servo_pwm = r2();
    	do_io();
    	delay(50000000);
    }
}
