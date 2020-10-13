#include "data_link.h"

/**
 * Reading Fucntion
 * @param port Serial Port to be read
 *
 */
int receive_set(int fd);

int open_reader(char* port);

int close_reader(int fd);
