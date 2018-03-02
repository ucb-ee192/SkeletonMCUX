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

//Dummy function for delaying
void delay(uint32_t t)
{
	uint32_t i;
	for(i=0; i< t; i++)
	{
		asm("nop");
	}
}

//Dummy function for reading the current time
uint32_t get_curr_time_ms(){
	return time+500;
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

	/*Track the variables time, motor_pwm, and camera. Function signature is:
	* void register_telemetry_variable(char* data_type, char* internal_name, char* display_name, char* units, uint32_t* value_pointer, uint32_t num_elements, float lower_bound, float upper_bound)
	*
	* data_type = "uint", "int", or "float"
	* internal_name = internal reference name used for the python plotter (must have one variable with internal_name ='time')
	* display_name = string used to label the axis on the plot
	* units = string used to denote the units of the dependent variable
	* value_pointer = pointer to the variable you want to track. Make sure the variable is global or that you malloc space (not recommended) for it
	* num_elements = number of elements to track here (i.e. 1=just 1 number, 128=array of 128 elements)
	* lower_bound = float representing a lower bound on the data (used for setting plot bounds)
	* upper_bound = float representing an upper bound on the data (used for setting plot bounds)
	*/
	register_telemetry_variable("uint", "time", "Time", "ms", (uint32_t*) &time,  1, 0,  0.0);
	register_telemetry_variable("float", "motor", "Motor PWM", "Percent DC", (uint32_t*) &motor_pwm,  1, 0.0f,  0.5f);
	register_telemetry_variable("uint", "linescan", "Linescan", "ADC", (uint32_t*) &camera,  128, 0.0f,  0.0f);

	//Tell the plotter what variables to plot. Send this once before the main loop
	transmit_header();

    while (1)
    {
    	time = get_curr_time_ms();
    	take_pic();
    	delay(5000000);
    	//Send a telemetry packet with the values of all the variables.
    	//Be careful as this uses a blocking write- could mess with timing of other software.
    	do_io();
    }
}
