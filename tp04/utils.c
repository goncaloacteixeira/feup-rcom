#include "utils.h"

control_packet_t parse_control_packet(unsigned char *raw_bytes, int size) {
  control_packet_t packet;
  memset(&packet, 0, sizeof(control_packet_t));
  packet.control = raw_bytes[0];

  char *name;
  int namesize = 0;

  for (int i = 1; i < size; i++) {
    if (raw_bytes[i] == FILE_SIZE) {
      int length = raw_bytes[++i];
      int offset = i + length;
      for (int j = 1; i < offset;) {
        packet.file_size += j * raw_bytes[++i];
        j *= 256;
      }
    }
    if (raw_bytes[i] == FILE_NAME) {
      int length = raw_bytes[++i];
      name = (unsigned char *) malloc (length);
      int offset = i + length;
      for (int j = 0; i < offset;) {
        name[j++] = raw_bytes[++i];
        namesize++;
      }
    }
  }

  packet.file_name = (unsigned char*) malloc (namesize + 1);
  memcpy(packet.file_name, name, namesize);
  packet.file_name[namesize] = '\0';
  free(name);
  return packet;
}

data_packet_t parse_data_packet(unsigned char *raw_bytes, int size) {
  data_packet_t packet;
  memset(&packet, 0, sizeof(data_packet_t));
  packet.raw_bytes_size = size;
  packet.control = raw_bytes[0];
  packet.sequence = raw_bytes[1];

  packet.data_field_size = (raw_bytes[2] << 8) | raw_bytes[3];

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

void print_data_packet(data_packet_t* packet, int full_info) {
  printf("---- DATA PACKET ----\n");
  printf("Control: - (0x%x)\n", packet->control);
  printf("Data size: %d (0x%x)\n", packet->data_field_size,
         packet->data_field_size);
  printf("Sequence: %d (0x%x)\n", packet->sequence, packet->sequence);

  if (full_info) {
    for (int i = 0; i < packet->data_field_size; i++) {
      printf("DATA[%d]: %c (0x%x)\n", i, packet->data[i], packet->data[i]);
    }
  }

  printf("---------------------\n");
}

void print_message(information_frame_t frame, int stuffed) {
  printf("Address: 0x%x\n", frame.address);
  printf("Control: 0x%x\n", frame.control);
  printf("BCC1: 0x%x\n", frame.bcc1);
  int j = 0;
  for (int i = 0; i < frame.data_size; i++) {
    if (frame.data[i] == ESCAPE && stuffed) {
      printf("DATA[%d]: 0x%x - ESCAPE\n", j++, frame.data[i++]);
      if (frame.data[i] == ESCAPE_ESC) {
        printf("DATA[%d]: 0x%x - ESCAPED ESCAPE\n", j++, frame.data[i]);
      } else if (frame.data[i] == ESCAPE_FLAG) {
        printf("DATA[%d]: 0x%x - ESCAPED FLAG\n", j++, frame.data[i]);
      }
    } else {
      printf("DATA[%d]: 0x%x - %c (char)\n", j++, frame.data[i], frame.data[i]);
    }
  }
  printf("BCC2: 0x%x\n", frame.bcc2);
  printf("Message: %s - size: %d - strlen: %ld\n", frame.data, frame.data_size,
         strlen(frame.data));
}

int verify_message(information_frame_t frame) {
  if (frame.bcc1 != (frame.control ^ frame.address)) {
    return ERROR;
  }
  unsigned char bcc2 = 0xff;
  for (int i = 0; i < frame.data_size; i++) {
    bcc2 = frame.data[i] ^ bcc2;
  }

  if (bcc2 != frame.bcc2) {
    return ERROR;
  }

  return OK;
}

void print_elapsed_time(struct timespec start) {
  struct timespec end;
  clock_gettime(CLOCK_MONOTONIC_RAW, &end);
  double delta = (end.tv_sec - start.tv_sec) * 1000.0 +
                 (end.tv_nsec - start.tv_nsec) / 1000000.0;
  printf("Elapsed time: %f s\n\n", delta);
}

int check_connection(int fd) {
  if (fcntl(fd, F_GETFD) == -1) {
    printf("Connection closed\n");
    return -1;
  }
  return 0;
}
