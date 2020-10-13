#include "macros.h"

volatile int STOP=FALSE;

/**
 * Reads Control message from Writer 
 * @param fd File descriptor
 * @param msg Message to be deconstructed and analyzed
 * 
 * @return TRUE, when message is OK
 */

int readMsg(int fd, unsigned char msg);


/**
 * Reads Control message from Writer 
 * @param fd File descriptor
 * @param msg Cntrol message to be constructed
 *
 */

void resendMsg(int fd, unsigned char msg);

/**
 * Reading Fucntion 
 * @param port Serial Port to be read
 *
 */
int reader_main(char* port);