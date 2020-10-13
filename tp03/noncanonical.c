/*Non-Canonical Input Processing*/
#include "noncanonical.h"

int reader_main(char* port) {
  int fd;
  struct termios oldtio,newtio;

  /* top layer does the verification of the port name */

  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */

  fd = open(port, O_RDWR | O_NOCTTY );
  if (fd <0) {
    perror(port); exit(-1);
  }

  if (tcgetattr(fd, &oldtio) == -1) { /* save current port settings */
    perror("tcgetattr");
    exit(-1);
  }

  bzero(&newtio, sizeof(newtio));
  newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;

  /* set input mode (non-canonical, no echo,...) */
  newtio.c_lflag = 0;

  newtio.c_cc[VTIME]    = 1;   /* inter-character timer unused */
  newtio.c_cc[VMIN]     = 0;   /* blocking read until 1 chars received */

  /*
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
    leitura do(s) proximo(s) caracter(es)
  */

  tcflush(fd, TCIOFLUSH);

  if (tcsetattr(fd, TCSANOW, &newtio) == -1) {
    perror("tcsetattr");
    exit(-1);
  }

  printf("New termios structure set\n");

  if (receive_supervision_frame(fd, SET) == 0) {
    printf("Sending UA reply...\n");
    send_supervision_frame(fd, UA);
  }


  if(tcsetattr(fd,TCSANOW,&oldtio)==-1){
    perror("tsetattr");
    exit(-1);
  }
  close(fd);
  return 0;
}
