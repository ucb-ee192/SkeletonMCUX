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
#include "fsl_uart.h"

#include "pin_mux.h"
#include "clock_config.h"

#include "telemetry/telemetry_uart.h"
#include "telemetry/telemetry.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/

uint32_t time;
float motor_pwm;
uint32_t camera[128];


void init_board(void){
	BOARD_InitPins();
	BOARD_BootClockRUN();
}


void delay(uint32_t t)
{
	uint32_t i;
	for(i=0; i< t; i++)
	{
		asm("nop");
	}
}

//Dummy function to fill up camera array with data
void take_pic(){
	uint16_t k = 0;
	for(k=0; k < 128; k++){
		camera[k] = 50;
	}
	camera[60] = 30000;
	camera[61] = 30000;
	camera[62] = 30000;
	camera[63] = 30000;
	camera[64] = 30000;
}


/*!
 * @brief Main function
 */
int main(void)
{
	time = 0;
	motor_pwm = .25f;

	init_board();
	init_uart();

	register_telemetry_variable("uint", "time", "Time", "ms", (uint32_t*) &time,  1, 0,  0.0);
	register_telemetry_variable("float", "motor", "Motor PWM", "Percent DC", (uint32_t*) &motor_pwm,  1, 0.0f,  0.5f);
	register_telemetry_variable("uint", "linescan", "Linescan", "ADC", (uint32_t*) &camera,  128, 0.0f,  0.0f);

	transmit_header();

    while (1)
    {
    	time += 500;
    	take_pic();
    	delay(5000000);
    	do_io();
    }
}
