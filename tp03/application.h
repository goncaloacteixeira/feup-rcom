#include "writenoncanonical.h"

#define TRANSMITTER 0
#define RECEIVER    1

int llopen(int port, int type);

int llclose(int fd);

int llwrite(int fd, char* buffer, int length);

int llread(int fd, char* buffer);
