/*Non-Canonical Input Processing*/
#include "macros.h"

volatile int STOP = FALSE;
int flag=1, conta=1;

void atende(){                // atende alarme
	printf("alarme # %d\n", conta);
	flag=1;
	conta++;
}

void sendMsg(int fd, unsigned char msg) {
  unsigned char mesh[5];
  mesh[0]=FLAG;
  mesh[1]=A;
  mesh[2]=msg;
  mesh[3]=mesh[1]^mesh[2];
  mesh[4]=FLAG;
  write(fd,mesh,5);
}


void receiveUA(int *part, unsigned char *msg) {

  switch (*part)
  {
  case 0:
    if(*msg==FLAG){
      *part=1;
      printf("FLAG: 0x%x\n",*msg);
    }

    break;
  case 1:
    if(*msg==A){
      *part=2;
      printf("A: 0x%x\n",*msg);
    }
    else {
      if(*msg==FLAG)
        *part=1;
      else
        *part=0;
    }
    break;
  case 2:
    if(*msg==UA){
      *part=3;
      printf("UA: 0x%x\n",*msg);
    }
    else
      *part=0;
    break;
  case 3:
    if(*msg==(UA_BCC)){
      *part=4;
      printf("UA_BCC: 0x%x\n",*msg);
    }
    else
      *part=0;
    break;
  case 4:
    if(*msg==FLAG) {
      STOP = TRUE;
      printf("FINAL FLAG: 0x%x\nReceived UA\n",*msg);
    }
    else
      *part=0;
    break;
  default:
    break;
  }
}

int main(int argc, char** argv) {
  int fd;
  struct termios oldtio,newtio;

  if ( (argc < 2) ||
        ((strcmp("/dev/ttyS0", argv[1])!=0) &&
        (strcmp("/dev/ttyS1", argv[1])!=0) &&
        (strcmp("/dev/ttyS10",argv[1])!=0) &&
        (strcmp("/dev/ttyS11",argv[1])!=0) &&
        (strcmp("/dev/ttyS4",argv[1])!=0))) {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
    exit(1);
  }

  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */

  fd = open(argv[1], O_RDWR | O_NOCTTY );
  if (fd <0) {
    perror(argv[1]); exit(-1);
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
  newtio.c_cc[VMIN]     = 0;   /* blocking read 1 char received */

  /*
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
    leitura do(s) pr�ximo(s) caracter(es)
  */

  tcflush(fd, TCIOFLUSH);

  if (tcsetattr(fd,TCSANOW,&newtio) == -1) {
    perror("tcsetattr");
    exit(-1);
  }
  (void) signal(SIGALRM, atende);  // instala  rotina que atende interrupcao


  printf("New termios structure set\n");

  unsigned char rcv_msg;

  do
  {
    sendMsg(fd,SET);
    alarm(3);                 // activa alarme de 3s
    printf("Sent mesh\n");
    flag=0;
    int part=0;
    printf("Receiving response...\n");
    while(!STOP && !flag){
      read(fd,&rcv_msg,1);
      receiveUA(&part, &rcv_msg);
    }
    if(flag) printf("Timed Out - Retrying\n");
  } while (conta<4 && flag);

  if (tcsetattr(fd, TCSANOW, &oldtio) == -1) {
    perror("tcsetattr");
    exit(-1);
  }

  close(fd);
  return 0;
}
