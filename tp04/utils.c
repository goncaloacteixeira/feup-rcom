#include "utils.h"

control_packet_t parse_control_packet(unsigned char* raw_bytes, int size) {
  control_packet_t packet;
  packet.control = raw_bytes[0];
  int file_size = 0;
  char* name;

  for (int i = 1; i < size; i++) {
    if (raw_bytes[i] == FILE_SIZE) {
      int length = raw_bytes[++i];
      int offset = i + length;
      for (int j = 1; i < offset;) {
        file_size += j*raw_bytes[++i];
        j *= 256;
      }

    }
    if (raw_bytes[i] == FILE_NAME) {
      int length = raw_bytes[++i];
      name = (unsigned char*)malloc(length);
      int offset = i + length;
      for (int j = 0; i < offset;) {
        name[j++] = raw_bytes[++i];
      }
    }
  }

  packet.file_name = name;
  packet.file_size = file_size;

  return packet;
}

void print_control_packet(control_packet_t packet) {
    printf("---- CONTROL PACKET ----\n");
    switch (packet.control) {
    case START:
        printf("Control: START (0x%x)\n", packet.control);
        break;
    case STOP:
        printf("Control: STOP (0x%x)\n", packet.control);
        break;
    default:
        break;
    }
    printf("Size: %d (0x%x)\n", packet.file_size, packet.file_size);
    printf("Name: %s\n", packet.file_name);
    printf("------------------------\n");
}
