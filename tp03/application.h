#include "writenoncanonical.h"

#define TRANSMITTER 0
#define RECEIVER    1

int llopen(int port, int type) {
  char file[48];
  sprintf(file, "/dev/ttyS%d", port);

  if (type == TRANSMITTER)
    return open_writer(file);
  else if (type == RECEIVER)
    return 0; /* todo - substituir por open_reader quando o ´noncanonical` estiver pronto */

  return -1;
}

int llclose(int fd) {
  return 0; /* TODO yet */
}

int llwrite(int fd, char* buffer, int length) {
  return 0; /* TODO yet */
}

int llread(int fd, char* buffer) {
  return 0; /* TODO yet */
}
