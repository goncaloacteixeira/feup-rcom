/*Non-Canonical Input Processing*/ #include "writenoncanonical.h"

int flag = 1, conta = 1;

static int STOP = FALSE;

void atende() { // atende alarme
    printf("alarme # %d\n", conta);
    flag = 1;
    conta++;
}

void send_supervision_frame(int fd, unsigned char msg) {
    unsigned char mesh[5];
    mesh[0] = FLAG;
    mesh[1] = A;
    mesh[2] = msg;
    mesh[3] = mesh[1] ^ mesh[2];
    mesh[4] = FLAG;
    write(fd, mesh, 5);
}

void receiveUA(int* part, unsigned char* msg) {
    switch (*part) {
    case 0:
        if (*msg == FLAG) {
            *part = 1;
            printf("FLAG: 0x%x\n", * msg);
        }

        break;
    case 1:
        if (*msg == A) {
            *part = 2;
            printf("A: 0x%x\n", * msg);
        } else {
            if (*msg == FLAG)
                *part = 1;
            else
                *part = 0;
        }
        break;
    case 2:
        if (*msg == UA) {
            *part = 3;
            printf("UA: 0x%x\n", * msg);
        } else
            *part = 0;
        break;
    case 3:
        if (*msg == (UA_BCC)) {
            *part = 4;
            printf("UA_BCC: 0x%x\n", * msg);
        } else
            *part = 0;
        break;
    case 4:
        if (*msg == FLAG) {
            STOP = TRUE;
            printf("FINAL FLAG: 0x%x\nReceived UA\n", * msg);
        } else
            *part = 0;
        break;
    default:
        break;
    }
}

static struct termios oldtio;
static struct termios newtio;

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
    unsigned char rcv_msg;

    do {
        send_supervision_frame(fd, SET);
        alarm(3); // activa alarme de 3s
        printf("Sent mesh\n");
        flag = 0;
        int part = 0;
        printf("Receiving response...\n");
        while (!STOP && !flag) {
            read(fd, &rcv_msg, 1);
            receiveUA(&part, &rcv_msg);
        }
        if (flag) printf("Timed Out - Retrying\n");
    } while (conta < 4 && flag);

		if (conta == 4) {
			printf("gave up\n");
			return -1;
		}

    return 0;
}
