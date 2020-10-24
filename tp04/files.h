#include "macros.h"

unsigned long get_file_size(FILE* f);

unsigned char* read_file(FILE* f, unsigned long filesize);

unsigned char* split_file(unsigned char* data, unsigned long index_start, unsigned long index_end);