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

data_packet_t parse_data_packet(unsigned char* raw_bytes, int size) {
  data_packet_t packet;
  packet.raw_bytes_size = size;
  packet.control = raw_bytes[0];
  packet.sequence = raw_bytes[1];

  packet.data_field_size = (raw_bytes[2] << 8) | raw_bytes[3];
  
  packet.data = (unsigned char*)malloc(packet.data_field_size);
  for (int i = 0; i < packet.data_field_size; i++) {
      packet.data[i] = raw_bytes[4 + i];
  }

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

void print_data_packet(data_packet_t packet, int full_info) {
    printf("---- DATA PACKET ----\n");
    printf("Control: - (0x%x)\n", packet.control);
    printf("Data size: %d (0x%x)\n", packet.data_field_size, packet.data_field_size);
    printf("Sequence: %d (0x%x)\n", packet.sequence, packet.sequence);

    if (full_info) {
        for (int i = 0; i < packet.data_field_size; i++) {
          printf("DATA[%d]: %c (0x%x)\n", i, packet.data[i], packet.data[i]);
        }
    }

    printf("---------------------\n");
}