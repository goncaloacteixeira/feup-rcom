#include "application.h"
#include "utils.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s <number for serial port>\n", argv[0]);
    printf("\nExample: %s 11\t-\tfor '/dev/ttyS11'\n", argv[0]);
    exit(-1);
  }

  /* opens transmiter file descriptor on second layer */
  int receiver_fd = llopen(atoi(argv[1]), RECEIVER);
  /* in case there's an error oppening the port */
  if (receiver_fd == -1) {
    exit(-1);
  }
  /* sends a set mesh to the receiver */
  if(receive_set(receiver_fd)==-1){
    printf("Couldn't send UA\nAborted program\n");
    llclose(receiver_fd, RECEIVER);
    exit(-1);
  }

  char buffer[1024];
  int size;

  int state = 0;

  if ((size = llread(receiver_fd, buffer)) != -1) {
      control_packet_t packet = parse_control_packet(buffer, size);
      print_control_packet(packet);
      if (packet.control == START) {
          state = 1; /* for later, if needed (state machine) */
      }
  }

  int sequence = 0;
  unsigned char** full_message;
  full_message = (unsigned char**) malloc (1024);

  while (size != -1 && state == 1) {
      size = llread(receiver_fd, buffer);
      if (size == ERROR) {
          state = 3;
          break;
      }
      if (buffer[0] == STOP) {
          state = 2;
          break;
      }
      data_packet_t data = parse_data_packet(buffer, size);
      print_data_packet(data, FALSE);
      full_message[sequence++] = data.data; 
  }

  control_packet_t packet = parse_control_packet(buffer, size);
  print_control_packet(packet);

  printf("Displaying full message\n");
  for (int i = 0; i < sequence; i++) {
      printf("Message[%d]: %s\n", i, full_message[i]);
  }

  /* resets and closes the receiver fd for the port */
  llclose(receiver_fd, RECEIVER);

  return 0;
}
