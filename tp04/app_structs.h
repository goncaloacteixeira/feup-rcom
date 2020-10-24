typedef struct {
  unsigned char address;
  unsigned char control;
  unsigned char bcc1;
  unsigned char *data;
  int data_size; /* size of the data array */
  unsigned char bcc2;

  unsigned char *raw_bytes; /* full set of bytes for the message */
} information_frame_t;

typedef struct {
  unsigned char control;
  unsigned char sequence;
  int data_field_size;
  unsigned char data[1024];

  unsigned char *raw_bytes;
  int raw_bytes_size;
} data_packet_t;

typedef struct {
  unsigned char control;
  unsigned char *file_size;
  unsigned char *file_name;
  unsigned int filesize_size;

  unsigned char *raw_bytes;
  int raw_bytes_size;
} control_packet_t;

typedef struct {
  unsigned char* data;
  unsigned char* name;
  unsigned long size;
} file_t;
