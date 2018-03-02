#include "fsl_uart.h"
#include "telemetry.h"

void _do_io(uint8_t *data, size_t length, uint8_t is_header);

/* UART instance and clock */
#define DEMO_UART UART0
#define DEMO_UART_CLKSRC UART0_CLK_SRC
#define DEMO_UART_CLK_FREQ CLOCK_GetFreq(UART0_CLK_SRC)
#define DEMO_UART_IRQn UART0_RX_TX_IRQn
#define DEMO_UART_IRQHandler UART0_RX_TX_IRQHandler

extern struct Packet *header_packet;
extern struct Packet *data_packet;

uint8_t header_sent = 0;

void init_uart(void){
    uart_config_t config;
    /*
     * config.baudRate_Bps = 115200U;
     * config.parityMode = kUART_ParityDisabled;
     * config.stopBitCount = kUART_OneStopBit;
     * config.txFifoWatermark = 0;
     * config.rxFifoWatermark = 1;
     * config.enableTx = false;
     * config.enableRx = false;
     */
    UART_GetDefaultConfig(&config);
    config.baudRate_Bps = 115200U;
    config.enableTx = true;
    config.enableRx = true;

    UART_Init(DEMO_UART, &config, DEMO_UART_CLK_FREQ);
}

//Transmit a header packet (data definition)
void transmit_header(){
	init_header_packet();
	init_data_packet();
	build_header_packet();
	_do_io(header_packet->data, header_packet->len, 1U);
}

//Transmit a data packet (data values)
void do_io(){
	build_data_packet();
	_do_io(data_packet->data, data_packet->len, 0U);
}

//Transmit a byte string (data) of size (length)
//Return an error if the header has not been sent yet
void _do_io(uint8_t *data, size_t length, uint8_t is_header){
	if (is_header || header_sent){
		UART_WriteBlocking(DEMO_UART, data, length);
		header_sent = 1U;
	}
	//TODO: Return an error if the header hasn't been sent yet
}
