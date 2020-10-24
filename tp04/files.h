#include "macros.h"

unsigned long get_file_size(FILE* f);

unsigned char* read_file(FILE* f, unsigned long filesize);

unsigned char* split_file(unsigned char* data, unsigned long index_start, unsigned long index_end);

void join_file(unsigned char* data, unsigned char* frame, unsigned long size, int index);

void write_file(char* name, unsigned char* bytes, unsigned long size);