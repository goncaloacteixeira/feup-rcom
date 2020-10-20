#include "application.h"

extern int flag;
extern int conta;

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
  // printf("Message: %s\n", buffer);
  printf("Coding message...\n\n");

  information_frame_t frame; // to keep everything organized

  frame.address = A;

  /* C byte - Controls package, alternating between 0 and 1*/
  frame.control = (current_frame == 0) ? C_I0 : C_I1;

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
  /* BCC2 stuffing*/
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

  // printf("Coded message:\n");
  // print_message(frame, TRUE);

  int count = -1;
  if ((count = write(fd, frame.raw_bytes, j)) != -1) {
    printf("Message sent!\n");
  } else {
    printf("Message not sent!\n");
  }


  /* fazer aqui a parte de timeouts com *sigaction*
  ignorar a parte de baixo pq o stop and wait não vai ser
  feito aqui */

  /* STOP AND WAIT */
  // usleep(STOP_AND_WAIT);
  printf("Stopping and waiting for acknowledgement...\n");
  do {
	alarm(TIMEOUT);
	flag=0;
	while(!flag) {
 		int ack = receive_acknowledgement(fd);
          if (ack != -1) {
   		  printf("Received positive ACK\n");
		  alarm(RESET_ALARM);
     	  return count;
 		}
 		else {
    	  printf("Received negative ACK\n");
		  alarm(RESET_ALARM);
    	  return ERROR;
	 	}
	}
	printf("Timed out\nTrying again\n");
  } while(flag && conta<4);
 return ERROR;
}

int llread(int fd, char* buffer) {
  information_frame_t information_frame;
  information_frame.raw_bytes = (unsigned char*) malloc (sizeof(unsigned char));

  int i = 0;
  int part = 0;
  unsigned char rcv_msg;
  printf("Reading...\n");

  /* lógica: processar os dados todos em raw bytes, depois fazer o unstuffing,
      e depois fazer o tratamento dos dados */

/*
  part 0 - before first flag
  part 1 - between flag start and flag stop
  part 2 - after flag stop */
  while (part != 2) {
    read(fd, &rcv_msg, 1);

    if (rcv_msg == FLAG && part == 0) {
      part = 1;
      continue;
    }
    else if (rcv_msg == FLAG && part == 1) {
      part = 2;
      break;
    }
    information_frame.raw_bytes[i++] = rcv_msg;
    information_frame.raw_bytes = (unsigned char*) realloc (information_frame.raw_bytes, (i+1));
  }

  int data_size = i;
  /* UNSTUFFING BYTES */
  int j = 0, p = 0;
  for (; j < i && p<i; j++) {
    if (information_frame.raw_bytes[p] == ESCAPE) {
      information_frame.raw_bytes = (unsigned char*)realloc(information_frame.raw_bytes, --data_size);
      if (information_frame.raw_bytes[p+1] == ESCAPE_ESC)
        information_frame.raw_bytes[j] = ESCAPE;
      else if (information_frame.raw_bytes[p+1] == ESCAPE_FLAG)
        information_frame.raw_bytes[j] = FLAG;
      p+=2;
    }
    else {
      information_frame.raw_bytes[j] = information_frame.raw_bytes[p];
      p++;
    }
  }

  information_frame.data = (unsigned char*) malloc ((data_size - 4) * sizeof(unsigned char));

  information_frame.address = information_frame.raw_bytes[0];
  information_frame.control = information_frame.raw_bytes[1];
  information_frame.bcc1 = information_frame.raw_bytes[2];
  p = 0;
  for (int byte = 3; byte < data_size - 1; byte++) {
    information_frame.data[p++] = information_frame.raw_bytes[byte];
  }
  information_frame.bcc2 = information_frame.raw_bytes[data_size - 1];
  information_frame.data_size = data_size - 4;

  // buffer = (char*) malloc (information_frame.data_size);

  // print_message(information_frame, FALSE);
  /* verificar se existem erros nos BCCs caso existam, return error */
  if (verify_message(information_frame) != OK) {
    // sleep(15);
    send_acknowledgement(fd, current_frame, FALSE);
    memcpy(buffer, information_frame.data, information_frame.data_size);
    free(information_frame.raw_bytes);
    free(information_frame.data);
    return ERROR;
  } else {
    // sleep(15);
    send_acknowledgement(fd, current_frame, TRUE);
    current_frame = (current_frame == 0) ? 1 : 0;
    memcpy(buffer, information_frame.data, information_frame.data_size);
    free(information_frame.raw_bytes);
    free(information_frame.data);
    return information_frame.data_size;
  }
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

int verify_message(information_frame_t frame) {
  if (frame.bcc1 != (frame.control ^ frame.address)) {
    return ERROR;
  }
  unsigned char bcc2 = 0xff;
  for (int i = 0; i < frame.data_size; i++) {
    bcc2 = frame.data[i] ^ bcc2;
  }

  if (bcc2 != frame.bcc2) {
    return ERROR;
  }

  return OK;
}
