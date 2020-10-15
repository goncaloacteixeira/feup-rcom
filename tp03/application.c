#include "application.h"

int mesh_send=0, mesh_rec=0;

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
  int size_info = 6+length;
  unsigned char *information_frame= (unsigned char*)malloc(size_info*sizeof(unsigned char)); /* changed so that we can adjust size dinamically*/
  int i = 0;
  information_frame[i++] = FLAG;
  information_frame[i++] = A;
  
  /* C byte - Controls package, alternating between 0 and 1*/
  if(mesh_send=0)
    information_frame[i++] = C_I0; /* change this */
  else
    information_frame[i++] = C_I1;

  information_frame[i++] = information_frame[1] ^ information_frame[2];
  unsigned char bcc = 0xff;
  for (int j = 0; j < length; j++) {
    /* Data stuffing and buffer size adjusting*/
    if(buffer[j]==ESCAPE) {
      information_frame = (unsigned char*)realloc(information_frame, ++size_info);
      information_frame[i++]=ESCAPE;information_frame[i++]=ESCAPE_ESC;
    }
    else if(buffer[j]==FLAG) {
      information_frame = (unsigned char*)realloc(information_frame, ++size_info);
      information_frame[i++]=ESCAPE;information_frame[i++]=ESCAPE_FLAG;
    }
    else
      information_frame[i++] = buffer[j];
    
    bcc = buffer[j] ^ bcc;
  }
  /* BCC stuffing*/
  if(bcc==ESCAPE) {
    information_frame = (unsigned char*)realloc(information_frame, ++size_info);
    information_frame[i++]=ESCAPE;information_frame[i++]=ESCAPE_ESC;
  }
  else if(bcc==FLAG) {
    information_frame = (unsigned char*)realloc(information_frame, ++size_info);
    information_frame[i++]=ESCAPE;information_frame[i++]=ESCAPE_FLAG;
  }
  else
    information_frame[i++] = bcc;

  information_frame[i++] = FLAG;

  return write(fd, information_frame, i);
}

int llread(int fd, char* buffer) {
  information_frame_t information_frame;

  int i = 0;
  int part=0;
  unsigned char rcv_msg,rcv_c;
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
        /* Started implementing ACK, incomplete*/
        if(rcv_msg == C_I0){
          part = 3;
          mesh_rec=0;
          rcv_c=rcv_msg;
          // printf("Control: 0x%x\n",rcv_msg);
          information_frame.control = rcv_msg;
        }
        else if(rcv_msg == C_I1){
          part = 3;
          mesh_rec=1;
          rcv_c=rcv_msg;
          // printf("Control: 0x%x\n",rcv_msg);
          information_frame.control = rcv_msg;
        }
        else
          part=0;
        break;
      case 3:
        if(rcv_msg==(A^rcv_c)){
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
  int data_size=i-1;

  information_frame.data = (unsigned char *) malloc (data_size * sizeof(unsigned char));
  
  int j = 0, p=0;
  for (; j < i && p<i; j++) {
    /* Destuffing data*/
    if(buffer[p]==ESCAPE) {
      information_frame.data = (unsigned char*)realloc(information_frame.data, (--data_size)*sizeof(unsigned char));
      if(buffer[p+1]==ESCAPE_ESC) 
        information_frame.data[j]=ESCAPE;
      else if (buffer[p+1]==ESCAPE_FLAG)
        information_frame.data[j]=FLAG;

      p+=2;
    }
    else {
      information_frame.data[j] = buffer[p];
      p++;
    }
      
  }
  information_frame.bcc2 = buffer[i-1];

  printf("Printing message received...\n");
  printf("A: 0x%x\n", information_frame.address);
  printf("C: 0x%x\n", information_frame.control);
  printf("BCC1: 0x%x\n", information_frame.bcc1);
  for (int k = 0; k < j - 1; k++) {
    printf("Data[%d]: 0x%x - %c\n", k, information_frame.data[k], information_frame.data[k]);
  }
  printf("BCC2: 0x%x\n", information_frame.bcc2);

  return j;
}
