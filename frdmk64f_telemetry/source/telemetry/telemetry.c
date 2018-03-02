/*System includes.*/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "telemetry.h"

struct Packet *header_packet;
struct Packet *data_packet;
volatile Telemetry_Obj telemetry_data[MAX_ARRAY_SIZE];
volatile uint8_t data_len = 0;
volatile uint8_t sequence_num = 0x00;

void get_kvs(uint8_t *data);
void get_data(uint8_t *data);
uint32_t get_kvs_length(void);
uint16_t get_data_length(void);

//Initialize Header Packet and Write Constant Values
void init_header_packet(){
	uint16_t kvs_length = get_kvs_length();
		header_packet = new_packet(7+kvs_length);

		//Start of packet
		header_packet->data[0] = 0x05;
		header_packet->data[1] = 0x39;

		//Packet length
		kvs_length += 3;
		header_packet->data[2] = ((uint8_t *)(&kvs_length))[1];
		header_packet->data[3] = ((uint8_t *)(&kvs_length))[0];

		//Data Definition Packet
		header_packet->data[4] = 0x81;

		//End Header Packet
		header_packet->data[kvs_length+3] = 0x00;
}

//Initialize Data Packet and Write Constant Values
void init_data_packet(){


	uint16_t data_length = get_data_length();
	data_packet = new_packet(7+data_length);

	//Start of packet
	data_packet->data[0] = 0x05;
	data_packet->data[1] = 0x39;

	//Packet length
	data_length += 3;
	data_packet->data[2] = ((uint8_t *)(&data_length))[1];
	data_packet->data[3] = ((uint8_t *)(&data_length))[0];

	//Packet Op Code
	data_packet->data[4] = 0x01;

	//Terminate Data Packet
	data_packet->data[data_length+3] = 0x00;
}

//Fill Up Header Packet
void build_header_packet(){
	header_packet->data[5] = sequence_num++;
	get_kvs(&(header_packet->data[6]));
}

//Fill Up Data Packet
void build_data_packet() {
	data_packet->data[5] = sequence_num++;
	get_data(&(data_packet->data[6]));
}

void get_data(uint8_t *data){
	uint32_t j = 0;
	uint8_t i;
	for(i=0; i< data_len; i++){
			data[j++] = telemetry_data[i].data_id;
			if (telemetry_data[i].numeric_data_type == 0x01){
				data[j++] = ((uint8_t*)( telemetry_data[i].value_pointer))[3];
				data[j++] = ((uint8_t*)( telemetry_data[i].value_pointer))[2];
				data[j++] = ((uint8_t*)( telemetry_data[i].value_pointer))[1];
				data[j++] = ((uint8_t*)( telemetry_data[i].value_pointer))[0];
			} else {
				int32_t k;
				for(k= telemetry_data[i].num_elements*telemetry_data[i].elem_size-1; k >=0; k--){
					data[j++] = ((uint8_t*)( telemetry_data[i].value_pointer))[k];
				}
			}
		}
}

uint16_t get_data_length(){
	uint32_t size = 0;
	uint8_t i;
	for(i=0; i< data_len; i++){
		size += 1 + telemetry_data[i].elem_size*telemetry_data[i].num_elements;
	}
	return size;
}

uint32_t get_kvs_length(){
	uint8_t i;
	uint32_t size = 0;
	for(i=0; i < data_len; i++){
		size += 8 + strlen(telemetry_data[i].internal_name) + strlen(telemetry_data[i].display_name) + strlen(telemetry_data[i].units);//Size of first 3 KV's
		if (telemetry_data[i].numeric_data_type == 0x01){
			size += 14;//2 (Data Type) + 2 (Data Length) + 10 (Limits)
		} else {
			size += 19;//2 (Data Type) + 2 (Data Length) + 5 (Array Count) + 10 (Limits)
		}

	}
	return size;
}

void get_kvs(uint8_t *data){
	uint8_t i, k;
	uint32_t j = 0;
	for(i=0; i < data_len; i++){
		data[j++] = telemetry_data[i].data_id;
		data[j++] = telemetry_data[i].numeric_data_type;
		data[j++] = 0x1;
		for(k=0; k < strlen(telemetry_data[i].internal_name); k++){
			data[k+j] = (uint8_t) telemetry_data[i].internal_name[k];
		}
		j+=k;
		data[j++]= 0x00;//terminate previous kv record

		data[j++] = 0x02;
		for(k=0; k < strlen(telemetry_data[i].display_name); k++){
			data[k+j] = (uint8_t) telemetry_data[i].display_name[k];
		}
		j+=k;
		data[j++]= 0x00; //terminate previous kv record

		data[j++] = 0x03;
		for(k=0; k < strlen(telemetry_data[i].units); k++){
			data[k+j] = (uint8_t) telemetry_data[i].units[k];
		}
		j+=k;
		data[j++]= 0x00;//terminate previous kv record

		data[j++] = 0x40;//Data Type (0x01 unsigned int, 0x02 signed int, 0x03 float)
		data[j++] = telemetry_data[i].data_type;

		data[j++] = 0x41; //Data Length in bytes
		data[j++] = telemetry_data[i].elem_size;//;

		//Number of Elements in Array
		if(telemetry_data[i].numeric_data_type == 0x02){
			data[j++] = 0x50;//length of array packet
			data[j++] = 0x00;//TODO: Dont hardcode this
			data[j++] = 0x00;
			data[j++] = 0x00;
			data[j++] = telemetry_data[i].num_elements;
		}

		data[j++] = 0x42;//Data Plotting Limits
		//Lower Bound
		data[j++] = ((uint8_t*)( &(telemetry_data[i].lower_bound)))[3];
		data[j++] = ((uint8_t*)( &(telemetry_data[i].lower_bound)))[2];
		data[j++] = ((uint8_t*)( &(telemetry_data[i].lower_bound)))[1];
		data[j++] = ((uint8_t*)( &(telemetry_data[i].lower_bound)))[0];

		//Upper Bound
		data[j++] = ((uint8_t*)( &(telemetry_data[i].upper_bound)))[3];
		data[j++] = ((uint8_t*)( &(telemetry_data[i].upper_bound)))[2];
		data[j++] = ((uint8_t*)( &(telemetry_data[i].upper_bound)))[1];
		data[j++] = ((uint8_t*)( &(telemetry_data[i].upper_bound)))[0];


		data[j++] = 0x00;
	}
}

void register_telemetry_variable(char* data_type, char* internal_name, char* display_name, char* units, uint32_t* value_pointer, uint32_t num_elements, float lower_bound, float upper_bound){
	uint8_t int_data_type, numeric_data_type;

	if (strcmp("uint", data_type) == 0){//uint, int, or float
		int_data_type = 0x01;
		telemetry_data[data_len].lower_bound = (float) ((uint32_t) lower_bound);
		telemetry_data[data_len].upper_bound = (float) ((uint32_t) lower_bound);
	} else if (strcmp("int", data_type) == 0){
		int_data_type = 0x02;
		telemetry_data[data_len].lower_bound = (float) ((int32_t) lower_bound);
		telemetry_data[data_len].upper_bound = (float) ((int32_t) lower_bound);
	} else if (strcmp("float", data_type) == 0){
		int_data_type = 0x03;
		telemetry_data[data_len].lower_bound = lower_bound;
		telemetry_data[data_len].upper_bound = upper_bound;
	}//TODO: Deal with another case

	//numeric or array
	if (num_elements == 1){//numeric (single element)
		numeric_data_type = 0x01;
	} else {// array (multiple elements
		numeric_data_type = 0x02;
		telemetry_data[data_len].lower_bound = 0x00; //no plotting bounds on array values
		telemetry_data[data_len].upper_bound = 0x00;

	}//TODO: Deal with another case

	//TODO: Do something if the buffer is full
	telemetry_data[data_len].data_id = data_len+1;
	telemetry_data[data_len].data_type = int_data_type;
	telemetry_data[data_len].numeric_data_type = numeric_data_type;
	telemetry_data[data_len].internal_name = malloc(sizeof(char)*strlen(internal_name));
	telemetry_data[data_len].display_name = malloc(sizeof(char)*strlen(display_name));
	telemetry_data[data_len].units = malloc(sizeof(char)*strlen(units));
	strcpy(telemetry_data[data_len].internal_name, internal_name);
	strcpy(telemetry_data[data_len].display_name, display_name);
	strcpy(telemetry_data[data_len].units, units);
	telemetry_data[data_len].value_pointer = value_pointer;

	telemetry_data[data_len].elem_size = 0x04;//TODO: Make this work for other sizes (2 bytes, 1 byte)
	telemetry_data[data_len].num_elements = num_elements;
	data_len++;
}


//Should only initialize the header and data packet once.
struct Packet *new_packet(uint32_t len){
	struct Packet *packet = malloc(sizeof(struct Packet));
	packet->len = len;
	packet->data = malloc(sizeof(uint8_t)*(packet->len));
	return packet;
}

//Shouldn't need to use this (only malloc header and data packet once)
void destroy_packet(struct Packet *packet){
	free(packet->data);
	free(packet);
}
