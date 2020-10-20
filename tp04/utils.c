#include "utils.h"

control_packet_t parse_control_packet(unsigned char* raw_bytes, int size) {
  control_packet_t packet;
  packet.control = raw_bytes[0];

 /* this is giving segfault but raw bytes is fine, gonna work on this later */

/*
  for (int i = 1; i < size; i++) {
    if (raw_bytes[i] == FILE_SIZE) {
      int length = raw_bytes[++i];
      int file_size = 0;
      for (int j = 256; i < i + length;) {
        file_size += j*raw_bytes[++i];
        j *= 256;
      }

    }
    if (raw_bytes[i] == FILE_NAME) {
      int length = raw_bytes[++i];
      char name[length];
      for (int j = 0; i < i + length;) {
        name[j++] = raw_bytes[++i];
      }
    }
  }
  */

  return packet;
}
