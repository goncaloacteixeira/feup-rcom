
typedef struct {
  unsigned char control;
  unsigned char sequence;
  int data_field_size;
  unsigned char* data;

  unsigned char* raw_bytes;
  int raw_bytes_size;
} data_packet_t;

typedef struct {
  unsigned char control;
  unsigned char file_size;
  unsigned char* file_name;

  unsigned char* raw_bytes;
  int raw_bytes_size;
} control_packet_t;
