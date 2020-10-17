#include "application.h"

extern int flag;

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
  if((err=send_set(transmiter_fd))==-1){
    printf("Couldn't send set\nAborted program\n");
    llclose(transmiter_fd, TRANSMITTER);
    return -1;
  }

  int tries=0;
  struct timespec start;
  do
  {
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    llwrite(transmiter_fd, "~}hello}~~~", 11);
    alarm(TIMEOUT);
    flag=0;
    int ack;
    while(1) {
      if ((ack = receive_acknowledgement(transmiter_fd)) != -1) {
      printf("Send packet: %d\n", ack);
      break;
      }
    }
    if(flag)printf("Rejected last packet\n");
    print_elapsed_time(start);
  } while (tries<TRIES && flag);
  alarm(RESET_ALARM);
  

  /* resets and closes the receiver fd for the port */
  llclose(transmiter_fd, TRANSMITTER);

  /* TODO - Check for controll messages and then implement read and write*/

  return 0;
}
