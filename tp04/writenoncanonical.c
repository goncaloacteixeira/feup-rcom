/*Non-Canonical Input Processing*/
#include "writenoncanonical.h"

int flag = 1, conta = 1;
extern int reetransmit;
static struct termios oldtio;
static struct termios newtio;

void atende() { // atende alarme
  printf("alarme # %d\n", conta);
  flag = 1;
  conta++;
}

int open_writer(char *port) {
  /* top level layer should verifiy ports name */

  /*
          Open serial port device for reading and writing and not as controlling
     tty
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

  newtio.c_cc[VTIME] = TIMEOUT; /* inter-character timer unused */
  newtio.c_cc[VMIN] = 0;        /* blocking read 1 char received */

  /*
          VTIME e VMIN devem ser alterados de forma a proteger com um
     temporizador a
          leitura do(s) pr�ximo(s) caracter(es)
  */

  tcflush(fd, TCIOFLUSH);

  if (tcsetattr(fd, TCSANOW, &newtio) == -1) {
    perror("tcsetattr");
    return -1;
  }
  /*struct sigaction action;
	sigemptyset(&action.sa_mask);
	action.sa_handler = atende;
	action.sa_flags = 0;
	sigaction(SIGALRM,&action,NULL);*/ // instala  rotina que atende interrupcao
  signal(SIGALRM, atende);
  siginterrupt(SIGALRM, 1);
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