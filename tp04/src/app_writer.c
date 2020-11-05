#include "data_link.h"
#include "files.h"

extern int flag;

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Usage: %s <number for serial port> <filename>\n", argv[0]);
    printf("\nExample: %s 11\t-\tfor '/dev/ttyS11' pinguim.gif\n", argv[0]);
    return -1;
  }

  FILE *fp;
  file_t file;

  /* opens transmiter file descriptor on second layer */
  int transmiter_fd = llopen(atoi(argv[1]), TRANSMITTER);
  /* in case there's an error oppening the port */
  if (transmiter_fd == ERROR) {
    exit(ERROR);
  }

  char* filename = argv[2];

  fp = fopen(filename, "rb");
  file.name = filename;
  file.size = get_file_size(fp);
  file.data = read_file(fp, file.size);

  control_packet_t c_packet_start = generate_control_packet(START, &file);
  control_packet_t c_packet_stop = generate_control_packet(STOP, &file);

  // sending control packet
  struct timespec start;
  clock_gettime(CLOCK_MONOTONIC_RAW, &start);

  print_control_packet(&c_packet_start);

  int size = llwrite(transmiter_fd, c_packet_start.raw_bytes, c_packet_start.raw_bytes_size);
  if (size == ERROR) {
    printf("Error writing START Control Packet, aborting...\n");
    llclose(transmiter_fd, TRANSMITTER);
    free(c_packet_start.file_size);
    free(c_packet_start.raw_bytes);
    free(c_packet_stop.file_size);
    free(c_packet_stop.raw_bytes);
    free(file.data);
    return ERROR;
  }
  free(c_packet_start.file_size);
  free(c_packet_start.raw_bytes);

  unsigned long bytes_left = file.size;
  int index_start;
  int index_end = -1;
  int sequence = 0;
  while (bytes_left != 0 && index_end != file.size - 1) {
    usleep(STOP_AND_WAIT);

    index_start = index_end + 1;
    if (bytes_left >= PACKET_SIZE-1) {
      index_end = index_start + PACKET_SIZE-1;
    } else {
      index_end = index_start + bytes_left - 1;
    }
    bytes_left -= (index_end - index_start) + 1;

    unsigned char* frame = split_file(file.data, index_start, index_end);
    data_packet_t data = generate_data_packet(frame, index_end - index_start + 1, sequence++);
    //print_data_packet(&data, TRUE);

    printProgressBar(file.size-bytes_left,file.size);
    /* caso o write não seja bem sucedido tentar de novo */
    while ((size = llwrite(transmiter_fd, data.raw_bytes, data.raw_bytes_size)) == ERROR) { 
      usleep(STOP_AND_WAIT);
    }
    clearProgressBar();

    free(frame);
    free(data.raw_bytes);
    if (size == ERROR) {
      printf("Error writing Data Packet, aborting...\n");
      llclose(transmiter_fd, TRANSMITTER);
      return ERROR;
    }
  }
  printProgressBar(1,1);

  usleep(STOP_AND_WAIT);
  print_control_packet(&c_packet_stop);
  size = llwrite(transmiter_fd, c_packet_stop.raw_bytes, c_packet_stop.raw_bytes_size);
  if (size == ERROR) {
    printf("Error writing STOP Control Packet, aborting...\n");
    llclose(transmiter_fd, TRANSMITTER);
    return ERROR;
  }
  free(c_packet_stop.file_size);
  free(c_packet_stop.raw_bytes);

  usleep(STOP_AND_WAIT);
  print_elapsed_time(start);
  /* resets and closes the receiver fd for the port */
  llclose(transmiter_fd, TRANSMITTER);

  return OK;
}
