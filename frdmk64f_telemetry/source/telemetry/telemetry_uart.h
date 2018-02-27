void init_uart();
void transmit_header();
void do_io();
void _do_io(uint8_t *data, size_t length, uint8_t is_header);
