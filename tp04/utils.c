#include "utils.h"

control_packet_t parse_control_packet(unsigned char *raw_bytes, int size) {
  control_packet_t packet;
  memset(&packet, 0, sizeof(control_packet_t));
  packet.control = raw_bytes[0];

  char *name;
  int namesize = 0;

  unsigned char* filesize;
  int filesize_size = 0;

  for (int i = 1; i < size; i++) {
    if (raw_bytes[i] == FILE_SIZE) {
      int length = raw_bytes[++i];
      int offset = i + length;
      filesize = (unsigned char*) malloc (length);
      for (int k = 0; i < offset; k++) {
        filesize[k] = raw_bytes[++i];
        filesize_size++;
      }
      continue;
    }
    if (raw_bytes[i] == FILE_NAME) {
      int length = raw_bytes[++i];
      name = (unsigned char *) malloc (length);
      int offset = i + length;
      for (int j = 0; i < offset;) {
        name[j++] = raw_bytes[++i];
        namesize++;
      }
      continue;
    }
  }

  packet.file_name = (unsigned char*) malloc (namesize + 1);
  memcpy(packet.file_name, name, namesize);
  packet.file_name[namesize] = '\0';
  free(name);

  packet.filesize_size = filesize_size;
  packet.file_size = (unsigned char*) malloc (filesize_size);
  memcpy(packet.file_size, filesize, filesize_size);
  free(filesize);
   
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

void print_control_packet(control_packet_t* packet) {
  printf("\n---- CONTROL PACKET ----\n");
  switch (packet->control) {
  case START:
    printf("Control: START (0x%x)\n", packet->control);
    break;
  case STOP:
    printf("Control: STOP (0x%x)\n", packet->control);
    break;
  default:
    break;
  }

  printf("Size: %ld %#lx\n", array_to_number(packet->file_size, packet->filesize_size), array_to_number(packet->file_size, packet->filesize_size));
  printf("Name: %s\n", packet->file_name);
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

void print_message(information_frame_t* frame, int stuffed) {
  printf("Address: 0x%x\n", frame->address);
  printf("Control: 0x%x\n", frame->control);
  printf("BCC1: 0x%x\n", frame->bcc1);
  int j = 0;
  for (int i = 0; i < frame->data_size; i++) {
    if (frame->data[i] == ESCAPE && stuffed) {
      printf("DATA[%d]: 0x%x - ESCAPE\n", j++, frame->data[i++]);
      if (frame->data[i] == ESCAPE_ESC) {
        printf("DATA[%d]: 0x%x - ESCAPED ESCAPE\n", j++, frame->data[i]);
      } else if (frame->data[i] == ESCAPE_FLAG) {
        printf("DATA[%d]: 0x%x - ESCAPED FLAG\n", j++, frame->data[i]);
      }
    } else {
      printf("DATA[%d]: 0x%x - %c (char)\n", j++, frame->data[i], frame->data[i]);
    }
  }
  printf("BCC2: 0x%x\n", frame->bcc2);
  printf("Message: %s - size: %d - strlen: %ld\n", frame->data, frame->data_size,
         strlen(frame->data));
}

int verify_message(information_frame_t* frame) {
  if (frame->bcc1 != (frame->control ^ frame->address)) {
    printf("Error in BCC1\n");
    return ERROR;
  }
  unsigned char bcc2 = 0x00;
  //printf("BCC2: 0x%x\tFRAME: 0x%x\tFRAME BCC2: 0x%x\n",bcc2,frame->data[0],frame->bcc2);
  for (int i = 0; i < frame->data_size; i++) {
    //printf("BCC2: 0x%x\tFRAME: 0x%x\n",bcc2,frame->data[i]);
    bcc2 ^= frame->data[i];
  }

  if (bcc2 != frame->bcc2) {
    //printf("BCC2: 0x%x\tFRAME BCC2: 0x%x\n",bcc2,frame->bcc2);
    printf("Error in BCC2\n");
    return ERROR;
  }

  return OK;
}

void clearProgressBar() {
    int i;
    for (i = 0; i < NUM_BACKSPACES; ++i) {
        fprintf(stdout, "\b");
    }
    fflush(stdout);
}

void printProgressBar(int progress, int total) {
    int i, percentage = (int)((((double)progress) / total) * 100);
    int num_separators = (int)((((double)progress) / total) * PROGRESS_BAR_SIZE);;
    fprintf(stdout, "[");
    for (i = 0; i < num_separators; ++i) {
        fprintf(stdout, "%c", SEPARATOR_CHAR);
    }
    for (; i < PROGRESS_BAR_SIZE; ++i) {
        fprintf(stdout, "%c", EMPTY_CHAR);
    }
    fprintf(stdout, "]  %2d%%  ", percentage);
    fflush(stdout);
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

unsigned long array_to_number(unsigned char* buffer, unsigned int size) {
  unsigned long value = 0;
  int offset = 0;
  for (int i = 0; i < size; i++) {
    value |= buffer[i] << (8 * offset++);
  }

  return value;
}

unsigned int number_to_array(unsigned long num, unsigned char* buffer) {
  unsigned int size = 0;

  for (int i = 0; i < sizeof(unsigned long); i++) {
    buffer[i] = (num >> (8 * i)) & 0xff;
    size += 1;
  }
  for (int i = sizeof(unsigned long) - 1; i != 0; i--) {
    if (buffer[i] != 0) break;
    size--;
  }

  return size;
}

control_packet_t generate_control_packet(int control, file_t* file) {
  control_packet_t c_packet;
  c_packet.control = control;
  c_packet.file_name = file->name;  

  unsigned char buf[sizeof(unsigned long)];
  int num = number_to_array(file->size, buf);

  c_packet.file_size = (unsigned char *)malloc(num);
  memcpy(c_packet.file_size, buf, num);
  c_packet.filesize_size = num;

  int i = 0;
  // control packet
  c_packet.raw_bytes = (unsigned char *)malloc(i + 1);
  c_packet.raw_bytes[i++] = c_packet.control;
  c_packet.raw_bytes = (unsigned char *)realloc(c_packet.raw_bytes, (i + 1));
  // file size
  c_packet.raw_bytes[i++] = FILE_SIZE;
  c_packet.raw_bytes = (unsigned char *)realloc(c_packet.raw_bytes, (i + 1));
  c_packet.raw_bytes[i++] = c_packet.filesize_size;

  for (int j = 0; j < c_packet.filesize_size; j++) {
    c_packet.raw_bytes = (unsigned char *)realloc(c_packet.raw_bytes, (i + 1));
    c_packet.raw_bytes[i++] = c_packet.file_size[j];
  }
  c_packet.raw_bytes = (unsigned char *)realloc(c_packet.raw_bytes, (i + 1));
  // file name
  c_packet.raw_bytes[i++] = FILE_NAME;
  c_packet.raw_bytes = (unsigned char *)realloc(c_packet.raw_bytes, (i + 1));
  c_packet.raw_bytes[i++] = strlen(c_packet.file_name);
  c_packet.raw_bytes = (unsigned char *)realloc(c_packet.raw_bytes, (i + 1));
  for (int j = 0; j < strlen(c_packet.file_name); j++) {
    c_packet.raw_bytes[i++] = c_packet.file_name[j];
    c_packet.raw_bytes = (unsigned char *)realloc(c_packet.raw_bytes, (i + 1));
  }

  c_packet.raw_bytes_size = i;
  return c_packet;
}

data_packet_t generate_data_packet(unsigned char *buffer, int size, int sequence) {
  data_packet_t d_packet;
  d_packet.control = DATA;
  d_packet.data_field_size = size;
  d_packet.sequence = sequence;

  int i = 0;
  // control
  d_packet.raw_bytes = (unsigned char *)malloc(i + 1);
  d_packet.raw_bytes[i++] = d_packet.control;
  d_packet.raw_bytes = (unsigned char *)realloc(d_packet.raw_bytes, (i + 1));
  // sequence
  d_packet.raw_bytes[i++] = d_packet.sequence;
  d_packet.raw_bytes = (unsigned char *)realloc(d_packet.raw_bytes, (i + 1));
  // size
  unsigned int x = (unsigned int)size;
  unsigned char high = (unsigned char)(x >> 8);
  unsigned char low = x & 0xff;
  d_packet.raw_bytes[i++] = high;
  d_packet.raw_bytes = (unsigned char *)realloc(d_packet.raw_bytes, (i + 1));
  d_packet.raw_bytes[i++] = low;
  d_packet.raw_bytes = (unsigned char *)realloc(d_packet.raw_bytes, (i + 1));
  // data
  for (int j = 0; j < size; j++) {
    d_packet.data[j] = buffer[j];
    d_packet.raw_bytes[i++] = buffer[j];
    d_packet.raw_bytes = (unsigned char *)realloc(d_packet.raw_bytes, (i + 1));
  }

  d_packet.raw_bytes_size = i;
  return d_packet;
}
