#include "writenoncanonical.h"
#include "noncanonical.h"

#define TRANSMITTER 0
#define RECEIVER    1


typedef struct {
  unsigned char address;
  unsigned char control;
  unsigned char bcc1;
  unsigned char* data;
  unsigned char bcc2;

  unsigned char* raw_bytes;
} information_frame_t;


int llopen(int port, int type);

int llclose(int fd, int type);

int llwrite(int fd, char* buffer, int length);

int llread(int fd, char* buffer);
