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
  printf("\nSending data...\n");
  printf("Message: %s\n", buffer);
  printf("Coding message...\n\n");

  information_frame_t frame; // to keep everything organized

  frame.address = A;

  /* C byte - Controls package, alternating between 0 and 1*/
  if (mesh_send == 0) {
    frame.control = C_I0;
  }
  else {
    frame.control = C_I1;
  }

  frame.bcc1 = frame.address ^ frame.control;

  int size_info = length;
  unsigned char *information_frame = (unsigned char*) malloc (size_info*sizeof(unsigned char)); /* changed so that we can adjust size dinamically*/
  unsigned char bcc = 0xff;
  int i = 0;
  for (int j = 0; j < length; j++) {
    /* Data stuffing and buffer size adjusting*/
    if(buffer[j]==ESCAPE) {
      information_frame = (unsigned char*)realloc(information_frame, ++size_info);
      information_frame[i++] = ESCAPE;
      information_frame[i++] = ESCAPE_ESC;
    }
    else if(buffer[j]==FLAG) {
      information_frame = (unsigned char*)realloc(information_frame, ++size_info);
      information_frame[i++] = ESCAPE;
      information_frame[i++] = ESCAPE_FLAG;
    }
    else
      information_frame[i++] = buffer[j];

    bcc = buffer[j] ^ bcc;
  }
  frame.data = information_frame; /* saves the stuffed data-buffer on the struct */
  frame.data_size = i;            /* size of the suffed data-buffer */
  frame.bcc2 = bcc;               /* this BCC2 is not stuffed yet and it will be displayed *unstuffed* */

  /* Saving all data to be transmitted to .raw_bytes */
  frame.raw_bytes = (unsigned char*) malloc ((frame.data_size + 10) * sizeof(unsigned char*));
  int j = 0;
  frame.raw_bytes[j++] = FLAG;
  frame.raw_bytes[j++] = frame.address;
  frame.raw_bytes[j++] = frame.control;
  frame.raw_bytes[j++] = frame.bcc1;
  for (int k = 0; k < frame.data_size; k++) {
    frame.raw_bytes[j++] = frame.data[k];
  }
  /* BCC stuffing*/
  if (bcc == ESCAPE) {
    frame.raw_bytes[j++] = ESCAPE;
    frame.raw_bytes[j++] = ESCAPE_ESC;
  }
  else if (bcc == FLAG) {
    frame.raw_bytes[j++] = ESCAPE;
    frame.raw_bytes[j++] = ESCAPE_FLAG;
  }
  else
    frame.raw_bytes[j++] = bcc;

  frame.raw_bytes[j++] = FLAG;

  printf("Coded message:\n");
  print_message(frame, TRUE);

  int count = -1;
  if ((count = write(fd, frame.raw_bytes, j)) != -1) {
    printf("Message sent!\n");
  } else {
    printf("Message not sent!\n");
  }

  return count;
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
  information_frame.data_size = j - 1;

  print_message(information_frame, FALSE);

  return j;
}

void print_message(information_frame_t frame, int stuffed) {
  printf("Address: 0x%x\n", frame.address);
  printf("Control: 0x%x\n", frame.control);
  printf("BCC1: 0x%x\n", frame.bcc1);
  int j = 0;
  for (int i = 0; i < frame.data_size; i++) {
    if (frame.data[i] == ESCAPE && stuffed) {
      printf("DATA[%d]: 0x%x - ESCAPE\n", j++, frame.data[i++]);
      if (frame.data[i] == ESCAPE_ESC) {
        printf("DATA[%d]: 0x%x - ESCAPED ESCAPE\n", j++, frame.data[i]);
      }
      else if (frame.data[i] == ESCAPE_FLAG) {
        printf("DATA[%d]: 0x%x - ESCAPED FLAG\n", j++, frame.data[i]);
      }
    }
    else {
      printf("DATA[%d]: 0x%x - %c (char)\n", j++, frame.data[i], frame.data[i]);
    }
  }
  printf("BCC2: 0x%x\n", frame.bcc2);
  printf("Message: %s - size: %d - strlen: %ld\n", frame.data, frame.data_size, strlen(frame.data));
}
