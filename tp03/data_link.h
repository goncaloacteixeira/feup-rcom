#include "macros.h"

/**
* takes file descriptor (port) and sends a code msg in a
* supervision frame
*/
int send_supervision_frame(int fd, unsigned char msg);

/**
* receives a supervision frame with controll as msg
*/
int receive_supervision_frame(int fd, unsigned char msg);

/**
* receives a supervision frame and returns it's control byte
*/
unsigned char _receive_supervision_frame(int fd);

/**
*   Method to pretty-print the elapsed time between two meshes
*/
void print_elapsed_time(struct timespec start);

/**
* takes fd and checks if it hasn't closed
*/
int check_connection(int fd);