#include "data_link.h"


int send_set(int fd); /* main function for writer */

/**
* function to open and set port
*/
int open_writer(char* port);

/**
* function to reset and close port
*/
int close_writer(int fd);
