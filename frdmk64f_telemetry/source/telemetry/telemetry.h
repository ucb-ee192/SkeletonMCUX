#define MAX_ARRAY_SIZE 10 //This dictates how many telemetry objects you can "register". Change this if you want to use more, but too many may slow down CPU performance
#define MAX_STRING_LENGTH 50

typedef struct {
	uint8_t data_id;
	uint8_t data_type;//0x01 unsigned int, //0x02 signed int, //0x03 float
	uint8_t numeric_data_type; //0x01 numeric (1 number), 0x02 array (list of values)
	char* internal_name;
	char* display_name;
	char* units;
	uint32_t* value_pointer;
	float lower_bound;
	float upper_bound;
	uint8_t elem_size;
	uint32_t num_elements;
} Telemetry_Obj;

struct Packet{
	uint8_t* data;
	uint32_t len;
};

void register_telemetry_variable(char* data_type, char* internal_name, char* display_name, char* units, uint32_t* value_pointer, uint32_t num_elements, float lower_bound, float upper_bound);
struct Packet *new_packet(uint32_t len);
void init_header_packet(void);
void init_data_packet(void);
void build_header_packet(void);
void build_data_packet(void);
