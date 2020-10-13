/*Non-Canonical Input Processing*/
#include "noncanonical.h"

static int STOP = FALSE; /* unused */

int readMsg(int fd, unsigned char msg) {
  int part=0;
  unsigned char rcv_msg;
  printf("Reading...\n");
  while (part!=5) {

    read(fd,&rcv_msg,1);
    switch (part) {
      case 0:
        if(rcv_msg==FLAG){
          part=1;
          printf("FLAG: 0x%x\n",rcv_msg);
        }
        break;
      case 1:
        if(rcv_msg==A){
          part=2;
          printf("A: 0x%x\n",rcv_msg);
        }
        else {
          if(rcv_msg==FLAG)
            part=1;
          else
            part=0;
        }
        break;
      case 2:
        if(rcv_msg==msg){
          part=3;
          printf("Control: 0x%x\n",rcv_msg);
        }
        else
          part=0;
        break;
      case 3:
        if(rcv_msg==(A^msg)){
          part=4;
          printf("Control BCC: 0x%x\n",rcv_msg);
        }
        else
          part=0;
        break;
      case 4:
        if(rcv_msg==FLAG) {
          part = 5;
          printf("FINAL FLAG: 0x%x\nReceived Control\n",rcv_msg);
        }
        else
          part=0;
        break;
      default:
        break;
    }
  }

  return TRUE;
}

void resendMsg(int fd, unsigned char msg) {
  printf("Resending...\n");
  unsigned char mesh[5];
  mesh[0]=FLAG;
  mesh[1]=A;
  mesh[2]=msg;
  mesh[3]=mesh[1]^mesh[2];
  mesh[4]=FLAG;
  write(fd,mesh,5);
}

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

  if(readMsg(fd,SET))
    resendMsg(fd,UA);

  if(tcsetattr(fd,TCSANOW,&oldtio)==-1){
    perror("tsetattr");
    exit(-1);
  }
  close(fd);
  return 0;
}
