#include "macros.h"

int send_set(int fd); /* main function for writer */

/**
* takes file descriptor (port) and sends a code msg in a
* supervision frame
*/
void send_supervision_frame(int fd, unsigned char msg);

/**
* function to receive UA message
*/
void receiveUA(int* part, unsigned char* msg);

/**
* function to open and set port
*/
int open_writer(char* port);

/**
* function to reset and close port
*/
int close_writer(int fd);
