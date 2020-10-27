#include "macros.h"

/**
 * \addtogroup APPLICATIONLAYER
 * @{
 */

/**
 * @brief Method to calculate file's size in bytes
 */
unsigned long get_file_size(FILE* f);

/**
 * @brief Method to read all the bytes from a file
 */ 
unsigned char* read_file(FILE* f, unsigned long filesize);

/**
 * @brief Method to split a file's data
 * This method takes the data array, containing the file's data and returns the data from
 * index_start to index_end
 */
unsigned char* split_file(unsigned char* data, unsigned long index_start, unsigned long index_end);

/**
 * @brief Method to join a frame to the data buffer
 * This method joins the frame to the data, starting on the position given by index
 */
void join_file(unsigned char* data, unsigned char* frame, unsigned long size, int index);

/**
 * @brief Method to write bytes to a file with a given name
 */
void write_file(char* name, unsigned char* bytes, unsigned long size);

/** @} */