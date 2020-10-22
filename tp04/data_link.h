#include "utils.h"

#define TRANSMITTER 0
#define RECEIVER 1
#define STOP_AND_WAIT 50

/**
 * current I-Frame
 */
static int current_frame = 0;

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
* receives a ACK frame and returns it's control byte
*/
unsigned char receive_acknowledgement(int fd);

int send_acknowledgement(int fd, int frame, int accept);

/**
 * Reading Fucntion
 * @param fd Serial Port to be read
 */
int receive_set(int fd);

/**
 * This function sends a SET Control frame and expects an UA
 */
int send_set(int fd);

/**
 * This function opens a port and returns the file descriptor
 * @param port port number to be open
 * @param type RECEIVER or TRANSMITTOR
 */
int llopen(int port, int type);

/**
 * Same as llopen but this one closes the fd
 */
int llclose(int fd, int type);

/**
 * @brief Function to write a buffer to a file descriptor
 * This function takes the buffer and sends it trought the port,
 * after the byte-stuffing
 * @returns -1 if error or number of bytes written for success
 */
int llwrite(int fd, char *buffer, int length);

/**
 * @brief Function to read a buffer from a file descriptor
 * This function reads a buffer from the port and returns the number
 * @returns -1 if error or number of bytes read for success
 */
int llread(int fd, char *buffer);