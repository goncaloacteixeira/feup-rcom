#include "macros.h"

/**
 * \addtogroup DATALINKLAYER
 * @{
 */

/**
 * @brief Opens the reader
 */
int open_reader(char *port);

/**
 * @brief Reset and close the reader
 */
int close_reader(int fd);

/** @} */