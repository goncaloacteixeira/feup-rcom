#include "application.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s <number for serial port>\n", argv[0]);
    printf("\nExample: %s 11\t-\tfor '/dev/ttyS11'\n", argv[0]);
    return -1;
  }

  /* opens transmiter file descriptor on second layer */
  int receiver_fd = llopen(atoi(argv[1]), RECEIVER);
  /* in case there's an error oppening the port */
  if (receiver_fd == -1) {
    exit(-1);
  }
  /* sends a set mesh to the receiver */
  receive_set(receiver_fd);
  /* resets and closes the receiver fd for the port */
  llclose(receiver_fd, RECEIVER);

  /* TODO - Check for controll messages and then implement read and write*/

  return 0;
}
