#include "macros.h"

/**
* takes file descriptor (port) and sends a code msg in a
* supervision frame
*/
void send_supervision_frame(int fd, unsigned char msg);

/**
* receives a supervision frame with controll as msg
*/
int receive_supervision_frame(int fd, unsigned char msg);

int _write(int fd, char* buffer, int length);

int _read(int fd, char* buffer);
