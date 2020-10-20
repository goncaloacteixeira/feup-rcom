#include "application.h"

extern int flag;

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

	/* tirei a parte do timeout pq estava a dar um problema com 
	a alteração que estava a fazer sry :) mas agora já podemos tratar disso
	e começar a tratar da parte dos slides a partir do 22 */
	struct timespec start;
	clock_gettime(CLOCK_MONOTONIC_RAW, &start);
	llwrite(transmiter_fd, "~}hello}~~~", 11);
	print_elapsed_time(start);
  

	/* resets and closes the receiver fd for the port */
	llclose(transmiter_fd, TRANSMITTER);

	/* TODO - Check for controll messages and then implement read and write*/

	return 0;
}
