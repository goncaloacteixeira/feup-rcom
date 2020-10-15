#include "application.h"

int llopen(int port, int type) {
  char file[48];
  sprintf(file, "/dev/ttyS%d", port);

  if (type == TRANSMITTER)
    return open_writer(file);
  else if (type == RECEIVER)
    return open_reader(file);

  return -1;
}

int llclose(int fd, int type) {
  if (type == TRANSMITTER)
    return close_writer(fd);
  else if (type == RECEIVER)
    return close_reader(fd);
  return -1;
}

int llwrite(int fd, char* buffer, int length) {
  unsigned char information_frame[6 + length]; /* need to workout this value */
  int i = 0;
  information_frame[i++] = FLAG;
  information_frame[i++] = A;
  information_frame[i++] = 0x1; /* change this */
  information_frame[i++] = information_frame[1] ^ information_frame[2];
  unsigned char bcc = 0xff;
  for (int j = 0; j < length; j++) {
    information_frame[i++] = buffer[j];
    bcc = buffer[j] ^ bcc;
  }
  information_frame[i++] = bcc;
  information_frame[i++] = FLAG;

  return write(fd, information_frame, i);
}

int llread(int fd, char* buffer) {
  information_frame_t information_frame;

  int i = 0;
  int part=0;
  unsigned char rcv_msg;
  printf("Reading...\n");
  while (part!=6) {

    read(fd, &rcv_msg, 1);
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
          // printf("A: 0x%x\n",rcv_msg);
          information_frame.address = rcv_msg;
        }
        else {
          if(rcv_msg==FLAG)
            part=1;
          else
            part=0;
        }
        break;
      case 2:
        if(rcv_msg == 0x1){
          part = 3;
          // printf("Control: 0x%x\n",rcv_msg);
          information_frame.control = rcv_msg;
        }
        else
          part=0;
        break;
      case 3:
        if(rcv_msg==(A^0x1)){
          part=4;
          // printf("Control BCC: 0x%x\n",rcv_msg);
          information_frame.bcc1 = rcv_msg;
        }
        else
          part=0;
        break;
      case 4:
        if (rcv_msg != FLAG) {
          // printf("Data [%d]: 0x%x\n", i, rcv_msg);
          buffer[i++] = rcv_msg;
          part = 4;
        } else {
          part = 5;
        }
        break;

      case 5:
        if(rcv_msg==FLAG) {
          part = 6;
          printf("FINAL FLAG: 0x%x\nReceived Information Frame\n",rcv_msg);
        }
        else
          part=0;
        break;
      default:
        break;
    }
  }

  information_frame.data = (unsigned char *) malloc ((i-1) * sizeof(unsigned char));
  int j = 0;
  for (; j < i; j++) {
    information_frame.data[j] = buffer[j];
  }
  information_frame.bcc2 = buffer[i-1];

  printf("Printing message received...\n");
  printf("A: 0x%x\n", information_frame.address);
  printf("C: 0x%x\n", information_frame.control);
  printf("BCC1: 0x%x\n", information_frame.bcc1);
  for (int k = 0; k < j - 1; k++) {
    printf("Data[%d]: 0x%x\n", k, information_frame.data[k]);
  }
  printf("BCC2: 0x%x\n", information_frame.bcc2);

  return j;
}
