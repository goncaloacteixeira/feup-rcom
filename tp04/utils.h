#include "app_structs.h"
#include "macros.h"

control_packet_t parse_control_packet(unsigned char* raw_bytes, int size);

void print_control_packet(control_packet_t packet);
