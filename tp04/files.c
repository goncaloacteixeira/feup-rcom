#include "files.h"

unsigned long get_file_size(FILE* f) {
  fseek(f, 0, SEEK_END); // seek to end of file
  unsigned long size = ftell(f); // get current file pointer
  fseek(f, 0, SEEK_SET); // seek back to beginning of file
  // proceed with allocating memory and reading the file
  return size;
}

unsigned char* read_file(FILE* f, unsigned long filesize) {
  unsigned char* data = (unsigned char*) malloc (filesize);
  fread(data, sizeof(unsigned char), filesize, f);
  return data;
}

unsigned char* split_file(unsigned char* data, unsigned long index_start, unsigned long index_end) {
  int range = index_end - index_start + 1;
  unsigned char* frame = (unsigned char*) malloc (range);

  for (int k = 0; k < range; k++) {
    frame[k] = data[index_start + k];
  }
  
  return frame;
}

void join_file(unsigned char* data, unsigned char* frame, unsigned long size, int index) {
  for (int j = 0; j < size; j++) {
    data[index + j] = frame[j];
  }
}

void write_file(char* name, unsigned char* bytes, unsigned long size) {
  FILE *fh = fopen (name, "wb");
  if (fh != NULL) {
      fwrite (bytes, sizeof (unsigned char), size, fh);
      fclose (fh);
  }
}