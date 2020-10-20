#include "application.h"
#include "utils.h"

extern int flag;

control_packet_t generate_control_packet(/* int file fize, int file name */ int control) {
	control_packet_t c_packet;
	c_packet.control = control;
	c_packet.file_size = 0x25;
	c_packet.file_name = "hello_there.zip";

	int i = 0;
	// control packet
	c_packet.raw_bytes = (unsigned char*) malloc (i+1);
	c_packet.raw_bytes[i++] = c_packet.control; c_packet.raw_bytes = (unsigned char*) realloc (c_packet.raw_bytes, (i+1));
	// file size
	c_packet.raw_bytes[i++] = FILE_SIZE; c_packet.raw_bytes = (unsigned char*) realloc (c_packet.raw_bytes, (i+1));
	c_packet.raw_bytes[i++] = sizeof(c_packet.file_size); c_packet.raw_bytes = (unsigned char*) realloc (c_packet.raw_bytes, (i+1));
	c_packet.raw_bytes[i++] = c_packet.file_size; c_packet.raw_bytes = (unsigned char*) realloc (c_packet.raw_bytes, (i+1));
	// file name
	c_packet.raw_bytes[i++] = FILE_NAME; c_packet.raw_bytes = (unsigned char*) realloc (c_packet.raw_bytes, (i+1));
	c_packet.raw_bytes[i++] = strlen(c_packet.file_name); c_packet.raw_bytes = (unsigned char*) realloc (c_packet.raw_bytes, (i+1));
	for (int j = 0; j < strlen(c_packet.file_name); j++) {
		c_packet.raw_bytes[i++] = c_packet.file_name[j]; c_packet.raw_bytes = (unsigned char*) realloc (c_packet.raw_bytes, (i+1));
	}

	c_packet.raw_bytes_size = i;

	return c_packet;
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Usage: %s <number for serial port>\n", argv[0]);
		printf("\nExample: %s 11\t-\tfor '/dev/ttyS11'\n", argv[0]);
		return -1;
	}

	/* opens transmiter file descriptor on second layer */
	int transmiter_fd = llopen(atoi(argv[1]), TRANSMITTER);
	/* in case there's an error oppening the port */
	if (transmiter_fd == -1) {
		exit(-1);
	}
	/* sends a set mesh to the receiver */
	int err;
	if((err=send_set(transmiter_fd))==-1){
		printf("Couldn't send set\nAborted program\n");
		llclose(transmiter_fd, TRANSMITTER);
		return -1;
	}

	control_packet_t c_packet_start = generate_control_packet(START);
	control_packet_t c_packet_stop = generate_control_packet(STOP);

	// sending control packet
	struct timespec start;
	clock_gettime(CLOCK_MONOTONIC_RAW, &start);

	print_control_packet(c_packet_start);
	llwrite(transmiter_fd, c_packet_start.raw_bytes, c_packet_start.raw_bytes_size);
	print_elapsed_time(start);

	usleep(STOP_AND_WAIT);

	print_control_packet(c_packet_stop);
	llwrite(transmiter_fd, c_packet_stop.raw_bytes, c_packet_stop.raw_bytes_size);
	print_elapsed_time(start);


	/* resets and closes the receiver fd for the port */
	llclose(transmiter_fd, TRANSMITTER);

	return 0;
}
