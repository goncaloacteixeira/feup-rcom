#include "data_link.h"
#include "files.h"

file_t file;

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s <number for serial port>\n", argv[0]);
    printf("\nExample: %s 11\t-\tfor '/dev/ttyS11'\n", argv[0]);
    exit(ERROR);
  }

  /* opens transmiter file descriptor on second layer */
  int receiver_fd = llopen(atoi(argv[1]), RECEIVER);
  /* in case there's an error oppening the port */
  if (receiver_fd == ERROR) {
    exit(ERROR);
  }

  char buffer[1024];
  int size;

  int state = 0;

  // * START Control Packet
  while (state == 0) {
    memset(buffer, 0, sizeof(buffer));
    while ((size = llread(receiver_fd, buffer)) == ERROR) {
      printf("Error reading\n");
      llclose(receiver_fd, RECEIVER);
      return ERROR;
    }
    control_packet_t packet = parse_control_packet(buffer, size);

    file.size = array_to_number(packet.file_size, packet.filesize_size);
    file.name = packet.file_name;

    print_control_packet(&packet);
    if (packet.control == START) {
      state = 1;
    }
  }

  // * DATA Packets
  unsigned char *full_message = (unsigned char*) malloc (file.size);
  int index = 0;
  int current_sequence = -1;

  while (state == 1) {
    memset(buffer, 0, sizeof(buffer));
    while ((size = llread(receiver_fd, buffer)) == ERROR) {
      printf("Error reading\n");
    }
    if (buffer[0] == STOP) {
      state = 2;
      break;
    }
    data_packet_t data = parse_data_packet(buffer, size);
    
    if (data.control != DATA) continue;
    
    print_data_packet(&data, FALSE);
    join_file(full_message, data.data, data.data_field_size, index);

    // * caso o numero de sequencia seja diferente do anterior deve atualizar o index
    if (current_sequence != data.sequence) {
      current_sequence = data.sequence;
      index += data.data_field_size;
    }
  }

  // * STOP Control Packet
  if (state == 2) {
    control_packet_t packet = parse_control_packet(buffer, size);
    print_control_packet(&packet);

    char* name = (char*) malloc ((strlen(file.name) + 7) * sizeof(char));
    sprintf(name, "cloned_%s", file.name);
    write_file(name, full_message, file.size);
    printf("Received file\n");
  }

  /* resets and closes the receiver fd for the port */
  llclose(receiver_fd, RECEIVER);

  return 0;
}
