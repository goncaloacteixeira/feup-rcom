#include "utils.h"

/** \addtogroup MACROS
 * @{
 */
#define TRANSMITTER 0
#define RECEIVER 1
#define STOP_AND_WAIT 10000
/** @} */

/**
 * \defgroup DATALINKLAYER
 * @{
 */

/**
 * current I-Frame
 */
static int current_frame = 0;

/**
* @brief takes file descriptor (port) and sends a code msg in a supervision frame
*/
int send_supervision_frame(int fd, unsigned char msg, unsigned char address);

/**
* receives a supervision frame with controll as msg
*/
int receive_supervision_frame(int fd, unsigned char msg);

/**
* @brief Receives a ACK frame and returns it's control byte
*/
unsigned char receive_acknowledgement(int fd);

/**
 * @brief Method to send an ACK message
 * Takes a frame number (0, 1) and a flag (0, 1) and sends the ACK value accordingly 
 */
int send_acknowledgement(int fd, int frame, int accept);

/**
 * Reading Fucntion
 * @param fd Serial Port to be read
 */
int receive_set(int fd);

/**
 * @brief This function sends a SET Control frame and expects an UA
 */
int send_set(int fd);

/**
 * @brief Disconnect method for writer
 * Sends DISC, expects DISC, sends UA to receiver
 */
int disconnect_writer(int fd);

/**
 * @brief Disconnect method for reader
 * Expects DISC, sends DISC, expects UA from emissor
 */
int disconnect_reader(int fd);

/**
 * @brief This function opens a port and starts the connection
 * @param port port number to be open
 * @param type RECEIVER or TRANSMITTOR
 * @return file descriptor for the port
 */
int llopen(int port, int type);

/**
 * @brief Same as llopen but this one disconnects writer or reader and closes the descriptor
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

/** @} */