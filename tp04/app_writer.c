#include "application.h"
#include "utils.h"

extern int flag;

control_packet_t
generate_control_packet(/* int file fize, int file name */ int control) {
  control_packet_t c_packet;
  c_packet.control = control;
  c_packet.file_size = 0x25;
  c_packet.file_name = "hello_there.zip";

  int i = 0;
  // control packet
  c_packet.raw_bytes = (unsigned char *)malloc(i + 1);
  c_packet.raw_bytes[i++] = c_packet.control;
  c_packet.raw_bytes = (unsigned char *)realloc(c_packet.raw_bytes, (i + 1));
  // file size
  c_packet.raw_bytes[i++] = FILE_SIZE;
  c_packet.raw_bytes = (unsigned char *)realloc(c_packet.raw_bytes, (i + 1));
  c_packet.raw_bytes[i++] = sizeof(c_packet.file_size);
  c_packet.raw_bytes = (unsigned char *)realloc(c_packet.raw_bytes, (i + 1));
  c_packet.raw_bytes[i++] = c_packet.file_size;
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

data_packet_t generate_data_packet(unsigned char *buffer, int size,
                                   int sequence) {
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
  d_packet.data = (unsigned char *)malloc(size);
  for (int j = 0; j < size; j++) {
    d_packet.data[j] = buffer[j];
    d_packet.raw_bytes[i++] = buffer[j];
    d_packet.raw_bytes = (unsigned char *)realloc(d_packet.raw_bytes, (i + 1));
  }

  d_packet.raw_bytes_size = i;

  return d_packet;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s <number for serial port>\n", argv[0]);
    printf("\nExample: %s 11\t-\tfor '/dev/ttyS11'\n", argv[0]);
    return -1;
  }

  /* opens transmiter file descriptor on second layer */
  int transmiter_fd = llopen(atoi(argv[1]), TRANSMITTER);
  /* in case there's an error oppening the port */
  if (transmiter_fd == -1) {
    exit(-1);
  }
  /* sends a set mesh to the receiver */
  int err;
  if ((err = send_set(transmiter_fd)) == -1) {
    printf("Couldn't send set\nAborted program\n");
    llclose(transmiter_fd, TRANSMITTER);
    return -1;
  }

  control_packet_t c_packet_start = generate_control_packet(START);
  control_packet_t c_packet_stop = generate_control_packet(STOP);

  // sending control packet
  struct timespec start;
  clock_gettime(CLOCK_MONOTONIC_RAW, &start);

  print_control_packet(c_packet_start);

  int tries = 0;
  int size = -1;
  while (tries != TRIES && size == ERROR) {
    size = llwrite(transmiter_fd, c_packet_start.raw_bytes,
                   c_packet_start.raw_bytes_size);
    if (size == ERROR) {
      usleep(1000000); // waiting and then retry
      tries++;
      printf("Retrying...\n");
      continue;
    }
  }
  if (tries == TRIES) {
    printf("Limit Tries Exceeded - ABORT\n");
    llclose(transmiter_fd, TRANSMITTER);
    return -1;
  }
  tries = 0; // reset tries counter
  print_elapsed_time(start);

  unsigned char *messages[4] = {"hello world", "hi there", "rcom is nice",
                                "when it works"};

  for (int i = 0; i < 4; i++) {
    usleep(STOP_AND_WAIT);
    data_packet_t data =
        generate_data_packet(messages[i], strlen(messages[i]), i);
    print_data_packet(data, FALSE);
    while (tries != TRIES) {
      int count = llwrite(transmiter_fd, data.raw_bytes, data.raw_bytes_size);
      if (count == ERROR) {
        printf("Retrying...\n");
        tries++;
      } else {
        tries = 0; // reset tries counter
        break;
      }
      usleep(1000000);
    }
    if (tries == TRIES) {
      printf("Limit Tries Exceeded - ABORT\n");
      llclose(transmiter_fd, TRANSMITTER);
      return -1;
    }
    print_elapsed_time(start);
  }
  tries = 0; // reset

  usleep(STOP_AND_WAIT);

  print_control_packet(c_packet_stop);

  while (tries != TRIES) {
    int size = llwrite(transmiter_fd, c_packet_stop.raw_bytes,
                       c_packet_stop.raw_bytes_size);
    if (size == ERROR) {
      printf("Retrying...\n");
      tries++;
    } else {
      tries = 0;
      break;
    }
  }
  if (tries == TRIES) {
    printf("Limit Tries Exceeded - ABORT\n");
    llclose(transmiter_fd, TRANSMITTER);
    return -1;
  }

  print_elapsed_time(start);

  /* resets and closes the receiver fd for the port */
  llclose(transmiter_fd, TRANSMITTER);

  return 0;
}
