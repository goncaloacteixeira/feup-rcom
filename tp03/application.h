#include "writenoncanonical.h"
#include "noncanonical.h"

#define TRANSMITTER 0
#define RECEIVER    1
#define STOP_AND_WAIT 50


typedef struct {
  unsigned char address;
  unsigned char control;
  unsigned char bcc1;
  unsigned char* data;
  int data_size; /* size of the data array */
  unsigned char bcc2;

  unsigned char* raw_bytes; /* full set of bytes for the message */
} information_frame_t;


static int current_frame = 0;

int llopen(int port, int type);

int llclose(int fd, int type);

int llwrite(int fd, char* buffer, int length);

int llread(int fd, char* buffer);

int verify_message(information_frame_t frame);

/**
*   Method to pretty-print an information frame details
*/
void print_message(information_frame_t frame, int coded);

