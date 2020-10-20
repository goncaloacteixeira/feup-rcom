#include "app_structs.h"
#include "macros.h"

control_packet_t parse_control_packet(unsigned char* raw_bytes, int size);

data_packet_t parse_data_packet(unsigned char* raw_bytes, int size);

void print_control_packet(control_packet_t packet);

void print_data_packet(data_packet_t packet, int full_info);
