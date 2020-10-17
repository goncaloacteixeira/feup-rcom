#include "data_link.h"

int send_supervision_frame(int fd, unsigned char msg) {
  unsigned char mesh[5];
  mesh[0] = FLAG;
  mesh[1] = A;
  mesh[2] = msg;
  mesh[3] = mesh[1] ^ mesh[2];
  mesh[4] = FLAG;
  int err = write(fd, mesh, 5);
  if(!(err==5))
    return ERROR;
}

unsigned char _receive_supervision_frame(int fd) {
  int part=0;
  unsigned char rcv_msg, ctrl;
  printf("Reading response...\n");
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
        if((rcv_msg==C_RR0) ||
          (rcv_msg==C_RR1) ||
          (rcv_msg==C_REJ0) ||
          (rcv_msg==C_REJ1)){
          part=3;
          printf("Control: 0x%x\n",rcv_msg);
          ctrl = rcv_msg;
        }
        else
          part=0;
        break;
      case 3:
        if(rcv_msg==(A^ctrl)){
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
  return ctrl;
}

int receive_supervision_frame(int fd, unsigned char msg) {
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
  return (part == 5) ?  0 : -1;
}

void print_elapsed_time(struct timespec start) {
  struct timespec end;
  clock_gettime(CLOCK_MONOTONIC_RAW, &end);
  double delta = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_nsec - start.tv_nsec) / 1000000.0;
  printf("Elapsed time: %f s\n",delta);
}
