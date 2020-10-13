/*Non-Canonical Input Processing*/
#include "writenoncanonical.h"

int flag = 1, conta = 1;

static struct termios oldtio;
static struct termios newtio;

void atende() { // atende alarme
    printf("alarme # %d\n", conta);
    flag = 1;
    conta++;
}

int open_writer(char* port) {
		/* top level layer should verifiy ports name */

		/*
			Open serial port device for reading and writing and not as controlling tty
			because we don't want to get killed if linenoise sends CTRL-C.
		*/

		int fd = open(port, O_RDWR | O_NOCTTY);
		if (fd < 0) {
				perror(port);
				return -1;
		}

		if (tcgetattr(fd, &oldtio) == -1) {
				/* save current port settings */
				perror("tcgetattr");
				return -1;
		}

		bzero(&newtio, sizeof(newtio));
		newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
		newtio.c_iflag = IGNPAR;
		newtio.c_oflag = 0;

		/* set input mode (non-canonical, no echo,...) */
		newtio.c_lflag = 0;

		newtio.c_cc[VTIME] = 1; /* inter-character timer unused */
		newtio.c_cc[VMIN] = 0; /* blocking read 1 char received */

		/*
			VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
			leitura do(s) pr�ximo(s) caracter(es)
		*/

		tcflush(fd, TCIOFLUSH);

		if (tcsetattr(fd, TCSANOW, &newtio) == -1) {
				perror("tcsetattr");
				return -1;
		}
		(void) signal(SIGALRM, atende); // instala  rotina que atende interrupcao

		printf("New termios structure set\n");

		return fd;
}

int close_writer(int fd) {
		if (tcsetattr(fd, TCSANOW, &oldtio) == -1) {
				perror("tcsetattr");
				return -1;
		}

		close(fd);
		return 0;
}

int send_set(int fd) {
    do {
        send_supervision_frame(fd, SET);
        alarm(3); // activa alarme de 3s
        printf("Sent mesh\n");
        flag = 0;
        printf("Receiving response...\n");
        while (1) {
            if (receive_supervision_frame(fd, UA) == 0) break;
        }
        if (flag) printf("Timed Out - Retrying\n");
    } while (conta < 4 && flag);

		if (conta == 4) {
			printf("gave up\n");
			return -1;
		}

    return 0;
}
