#include "macros.h"
#include "writenoncanonical.h"
#include "noncanonical.h"
#include "app_structs.h"

/**
 * @brief This function parses a buffer of raw bytes to a control_packet_t
 * structure
 * This method takes the raw_bytes and parses them into a control packet
 */
control_packet_t parse_control_packet(unsigned char *raw_bytes, int size);

/**
 * @brief This function parses a buffer of raw bytes to a data_packet_t
 * structure
 * This method takes the raw_bytes and parses them into a data packet
 */
data_packet_t parse_data_packet(unsigned char *raw_bytes, int size);

/**
 * @brief This function pretty-prints a control packet
 */
void print_control_packet(control_packet_t packet);

/**
 * @brief This function pretty-prints a data packet
 */
void print_data_packet(data_packet_t* packet, int full_info);

/**
 * @brief Method to pretty-print the elapsed time between two frames
 */
void print_elapsed_time(struct timespec start);

/**
 * @brief Method to verify an I-Frame
 * Checks if there are errors on the BCC bytes
 * @returns 0 if no error or -1 for error
 */
int verify_message(information_frame_t frame);

/**
 * @brief Method to pretty-print an information frame details
 */
void print_message(information_frame_t frame, int coded);

/**
 * @brief Takes fd and checks if it hasn't closed
 */
int check_connection(int fd);

/**
 * @brief takes an array with length size, and converts it to an 8byte number
 * array[0] = MSB ; array[size - 1] = LSB
 */
unsigned long array_to_number(unsigned char* buffer, unsigned int size);

/**
 * @brief Method to convert a 8byte number into an 8 byte char array
 * array[0] = MSB ; array[return - 1] = LSB
 * @return array's size
 */
unsigned int number_to_array(unsigned long num, unsigned char* buffer);