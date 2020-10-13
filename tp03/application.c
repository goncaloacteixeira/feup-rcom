#include "application.h"

int llopen(int port, int type) {
  char file[48];
  sprintf(file, "/dev/ttyS%d", port);

  if (type == TRANSMITTER)
    return open_writer(file);
  else if (type == RECEIVER)
    return open_reader(file);

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
