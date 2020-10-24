#include "data_link.h"

int reetransmit = 1;
extern int flag;
extern int conta;

int send_supervision_frame(int fd, unsigned char msg, unsigned char address) {
  unsigned char mesh[5];
  mesh[0] = FLAG;
  mesh[1] = address;
  mesh[2] = msg;
  mesh[3] = mesh[1] ^ mesh[2];
  mesh[4] = FLAG;
  int err = write(fd, mesh, 5);
  if (!(err == 5))
    return ERROR;
  return 0;
}

unsigned char receive_acknowledgement(int fd) {
  // ! Remove comments if you want to debug the data being read
  int part = 0;
  unsigned char rcv_msg;
  unsigned char ctrl;
  unsigned char address;
  printf("Reading ACK supervision frame...\n");
  while (part != 5) {
    int rd = read(fd, &rcv_msg, 1);
    if (rd == -1 && errno == EINTR) {
      printf("READ failed\n");
      return 2;
    }
    switch (part) {
    case 0:
      if (rcv_msg == FLAG) {
        part = 1;
        // printf("FLAG: 0x%x\n",rcv_msg);
      }
      break;
    case 1:
      if (rcv_msg == A_1 || rcv_msg == A_3) {
        address = rcv_msg;
        part = 2;
        // printf("A: 0x%x\n",rcv_msg);
      } else {
        if (rcv_msg == FLAG)
          part = 1;
        else
          part = 0;
      }
      break;
    case 2:
      if ((rcv_msg == C_RR0) || (rcv_msg == C_RR1) || (rcv_msg == C_REJ0) ||
          (rcv_msg == C_REJ1)) {
        part = 3;
        // printf("Control: 0x%x\n",rcv_msg);
        ctrl = rcv_msg;
      } else
        part = 0;
      break;
    case 3:
      if (rcv_msg == (address ^ ctrl)) {
        part = 4;
        // printf("Control BCC: 0x%x\n",rcv_msg);
      } else
        part = 0;
      break;
    case 4:
      if (rcv_msg == FLAG) {
        part = 5;
        // printf("FINAL FLAG: 0x%x\nReceived Control\n",rcv_msg);
      } else
        part = 0;
      break;
    default:
      break;
    }
  }
  return ctrl;
}

int receive_supervision_frame(int fd, unsigned char msg) {
  // ! Remove comments if you want to debug the data being read
  int part = 0;
  unsigned char rcv_msg;
  unsigned char address;

  printf("Reading supervision frame...\n");
  while (part != 5) {
    int rd = read(fd, &rcv_msg,1);
    if (rd == -1 && errno == EINTR) {
      printf("READ failed\n");
      return 2;
    }
    switch (part) {
    case 0:
      if (rcv_msg == FLAG) {
        part = 1;
        // printf("FLAG: 0x%x\n",rcv_msg);
      }
      break;
    case 1:
      if (rcv_msg == A_1 || rcv_msg == A_3) {
        address = rcv_msg;
        part = 2;
        // printf("A: 0x%x\n",rcv_msg);
      } else {
        if (rcv_msg == FLAG)
          part = 1;
        else
          part = 0;
      }
      break;
    case 2:
      if (rcv_msg == msg) {
        part = 3;
        // printf("Control: 0x%x\n",rcv_msg);
      } else
        part = 0;
      break;
    case 3:
      if (rcv_msg == (address ^ msg)) {
        part = 4;
        // printf("Control BCC: 0x%x\n",rcv_msg);
      } else
        part = 0;
      break;
    case 4:
      if (rcv_msg == FLAG) {
        part = 5;
        // printf("FINAL FLAG: 0x%x\nReceived Control\n",rcv_msg);
      } else
        part = 0;
      break;
    default:
      break;
    }
  }
  return (part == 5) ? 0 : -1;
}

int receive_set(int fd) {
  if (receive_supervision_frame(fd, SET) == 0) {
    printf("Sending UA reply...\n");
    send_supervision_frame(fd, UA, A_3);
  }
  return 0;
}

int send_set(int fd) {
  struct timespec start;
  do {
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    if (send_supervision_frame(fd, SET, A_3) == -1) {
      printf("Error writing SET\n");
      continue;
    }
    alarm(TIMEOUT); // activa alarme de 3s
    printf("Sent SET frame\n");
    flag = 0;
    printf("Receiving UA response...\n");
    while (!flag) {
      if (receive_supervision_frame(fd, UA) == 0) {
        reetransmit = 0;
        break;
      }
    }

    if (flag)
      printf("Timed Out - Retrying\n");
    print_elapsed_time(start);

  } while (conta < 4 && flag);

  alarm(RESET_ALARM);

  if (conta == 4) {
    reetransmit = 2;
    printf("Gave up\n");
    return -1;
  }

  return 0;
}

int disconnect_writer(int fd) {
  struct timespec start;
  do {
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    if (send_supervision_frame(fd, DISC, A_3) == -1) {
      printf("Error writing DISC\n");
      continue;
    }
    alarm(TIMEOUT); // activa alarme de 3s
    printf("Sent DISC frame\n");
    flag = 0;
    printf("Receiving DISC response...\n");
    while (!flag) {
      if (receive_supervision_frame(fd, DISC) == 0) {
        reetransmit = 0;
        break;
      }
    }

    if (flag)
      printf("Timed Out - Retrying\n");
    print_elapsed_time(start);

  } while (conta < 4 && flag);

  alarm(RESET_ALARM);

  if (conta == 4) {
    reetransmit = 2;
    printf("Gave up\n");
    return -1;
  }

  printf("Sending UA ACK...\n");
  return send_supervision_frame(fd, UA, A_1);
}

int disconnect_reader(int fd) {
  struct timespec start;
  do {
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    alarm(TIMEOUT); // activa alarme de 3s
    flag = 0;
    printf("Receiving DISC from writer...\n");
    while (!flag) {
      if (receive_supervision_frame(fd, DISC) == 0) {
        reetransmit = 0;
        break;
      }
    }
    printf("DISC received, sending DISC..\n");
    if (send_supervision_frame(fd, DISC, A_3) == -1) {
      printf("Error writing DISC\n");
      continue;
    }

    if (flag)
      printf("Timed Out - Retrying\n");
    print_elapsed_time(start);

  } while (conta < 4 && flag);

  printf("Receiving UA...\n");

  return receive_supervision_frame(fd, UA);
}

int send_acknowledgement(int fd, int frame, int accept) {
  printf("Sending acknowledgement...\n");
  if (frame == 0) {
    if (accept == 1) {
      // caso seja o frame 0 e seja aceite então pede o frame 1
      send_supervision_frame(fd, C_RR1, A_3);
    } else {
      send_supervision_frame(fd, C_REJ0, A_3);
    }
  } else {
    if (accept == 1) {
      send_supervision_frame(fd, C_RR0, A_3);
    } else {
      send_supervision_frame(fd, C_REJ1, A_3);
    }
  }
  return 0;
}

int llopen(int port, int type) {
  char file[48];
  sprintf(file, "/dev/ttyS%d", port);

  int fd;
  if (type == TRANSMITTER) {
    if ((fd = open_writer(file)) == ERROR) {
      perror("llopen: error on open_writer");
      return ERROR;
    }
    if (send_set(fd) == ERROR) {
      perror("llopen: error sending SET");
      return ERROR;
    }
    return fd;
  }
  else if (type == RECEIVER) {
    if ((fd = open_reader(file)) == ERROR) {
      perror("llopen: error on open_reader");
      return ERROR;
    }
    if (receive_set(fd) == ERROR) {
      perror("llopen: error receiving SET");
      return ERROR;
    }
    return fd;
  }
  perror("llopen: type not valid");
  return ERROR;
}

int llclose(int fd, int type) {
  printf("\nDisconnecting...\n");
  if (type == TRANSMITTER) {
    if (disconnect_writer(fd) == ERROR) {
      perror("llclose: error disconnecting writer: ");
      return ERROR;
    }
    if (close_writer(fd) != ERROR) {
      printf("Writer Successfully Closed!\n");
      return OK;
    } else {
      perror("llclose: writer not closed successfully: ");
      return ERROR;
    }
  }
    
  else if (type == RECEIVER) {
    if (disconnect_reader(fd) == ERROR) {
      perror("llclose: error disconnecting reader: ");
      return ERROR;
    }
    if (close_reader(fd) != ERROR) {
      printf("Reader Successfully Closed!\n");
      return OK;
    } else {
      perror("llclose: reader not closed successfully: ");
      return ERROR;
    }
  }
  return ERROR;
}

int llwrite(int fd, char *buffer, int length) {
  printf("Sending data...\n");
  // printf("Message: %s\n", buffer);
  printf("Coding message...\n");

  information_frame_t frame; // to keep everything organized

  frame.address = A_3;

  /* C byte - Controls package, alternating between 0 and 1*/
  frame.control = (current_frame == 0) ? C_I0 : C_I1;

  frame.bcc1 = frame.address ^ frame.control;

  int size_info = length;
  unsigned char *information_frame = (unsigned char *) malloc (size_info *sizeof(unsigned char));
  unsigned char bcc = 0xff;
  int i = 0;
  for (int j = 0; j < length; j++) {
    /* Data stuffing and buffer size adjusting*/
    if (buffer[j] == ESCAPE) {
      information_frame =
          (unsigned char *)realloc(information_frame, ++size_info);
      information_frame[i++] = ESCAPE;
      information_frame[i++] = ESCAPE_ESC;
    } else if (buffer[j] == FLAG) {
      information_frame =
          (unsigned char *)realloc(information_frame, ++size_info);
      information_frame[i++] = ESCAPE;
      information_frame[i++] = ESCAPE_FLAG;
    } else
      information_frame[i++] = buffer[j];

    bcc = buffer[j] ^ bcc;
  }
  frame.data = information_frame; /* saves the stuffed data-buffer on the struct */
  frame.data_size = i;            /* size of the suffed data-buffer */
  frame.bcc2 = bcc;               /* this BCC2 is not stuffed yet and it will be displayed *unstuffed* */

  /* Saving all data to be transmitted to .raw_bytes */
  frame.raw_bytes = (unsigned char *)malloc((frame.data_size + 10) * sizeof(unsigned char *));
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
  } else if (bcc == FLAG) {
    frame.raw_bytes[j++] = ESCAPE;
    frame.raw_bytes[j++] = ESCAPE_FLAG;
  } else
    frame.raw_bytes[j++] = bcc;

  frame.raw_bytes[j++] = FLAG;

  // ! remove next comment if you want to see the coded message being written
  // print_message(frame, TRUE);
  conta = 1;
  int count = -1;

  do {
    if ((count = write(fd, frame.raw_bytes, j)) != ERROR) {
      printf("Message sent! Waiting for ACK\n");
    } else {
      printf("Message not sent!\n");
      return ERROR;
      // adicionei esta linha, pq caso não escreva corretamente 
      // deve retornar -1 para escrever de novo
    }
    alarm(TIMEOUT);
    flag = 0;

    unsigned char ack = receive_acknowledgement(fd);
    if (ack == C_REJ0 || ack == C_REJ1) {
      printf("Received negative ACK\n");
      alarm(RESET_ALARM);
      return ERROR;
    }
    // Retransmition
    if ((ack == C_RR0 && current_frame == 0) ||
        (ack == C_RR1 && current_frame == 1)) {
      printf("Received positive ACK (retransmition)\n");
      alarm(RESET_ALARM);
      // returns error but to the application only means it has to
      // send the same frame again
      return ERROR; 
    }

    if ((ack == C_RR0 && current_frame == 1) ||
        (ack == C_RR1 && current_frame == 0)) {
      printf("Received positive ACK\n");
      alarm(RESET_ALARM);
      current_frame = (current_frame == 0) ? 1 : 0; // changes the current frame
      return count;
    } else {
      printf("Timed out\nTrying again\n");
      alarm(RESET_ALARM);
    }
    printf("Couldn't receive ACK in time\n");
  } while (flag && conta < 4);

  return ERROR;
}

int llread(int fd, char *buffer) {
  information_frame_t information_frame;
  information_frame.raw_bytes = (unsigned char *)malloc(sizeof(unsigned char));

  int i = 0;
  int part = 0;
  unsigned char rcv_msg;
  printf("Reading...\n");

  // * lógica: processar os dados todos em raw bytes, depois fazer o unstuffing,
  // * e depois fazer o tratamento dos dados

  /*
    part 0 - before first flag
    part 1 - between flag start and flag stop
    part 2 - after flag stop */
  while (part != 2) {
    read(fd, &rcv_msg, 1);

    if (rcv_msg == FLAG && part == 0) {
      part = 1;
      continue;
    } else if (rcv_msg == FLAG && part == 1) {
      part = 2;
      break;
    }
    information_frame.raw_bytes[i++] = rcv_msg;
    information_frame.raw_bytes = (unsigned char *)realloc(information_frame.raw_bytes, (i + 1));
  }

  int data_size = i;
  /* UNSTUFFING BYTES */
  int j = 0, p = 0;
  for (; j < i && p < i; j++) {
    if (information_frame.raw_bytes[p] == ESCAPE) {
      information_frame.raw_bytes =
          (unsigned char *)realloc(information_frame.raw_bytes, --data_size);
      if (information_frame.raw_bytes[p + 1] == ESCAPE_ESC)
        information_frame.raw_bytes[j] = ESCAPE;
      else if (information_frame.raw_bytes[p + 1] == ESCAPE_FLAG)
        information_frame.raw_bytes[j] = FLAG;
      p += 2;
    } else {
      information_frame.raw_bytes[j] = information_frame.raw_bytes[p];
      p++;
    }
  }

  information_frame.data =
      (unsigned char *)malloc((data_size - 4) * sizeof(unsigned char));

  information_frame.address = information_frame.raw_bytes[0];
  information_frame.control = information_frame.raw_bytes[1];
  information_frame.bcc1 = information_frame.raw_bytes[2];
  p = 0;
  for (int byte = 3; byte < data_size - 1; byte++) {
    information_frame.data[p++] = information_frame.raw_bytes[byte];
  }
  information_frame.bcc2 = information_frame.raw_bytes[data_size - 1];
  information_frame.data_size = data_size - 4;

  // ! remove *sleep* comments if you want to check what happens when ACK is not received in time
  // ! remove print_message comment if you want to see the data byte-by-byte
  int bccError = verify_message(information_frame);
  if (bccError == ERROR) {
    // sleep(15);
    send_acknowledgement(fd, current_frame, FALSE);
  } else {
    // sleep(4);
    send_acknowledgement(fd, current_frame, TRUE);
    current_frame = (current_frame == 0) ? 1 : 0;
    // print_message(information_frame, FALSE);
  }

  for (i = 0; i < information_frame.data_size; i++) {
    buffer[i] = information_frame.data[i];
  }
  
  free(information_frame.raw_bytes);
  free(information_frame.data);
  return (bccError == OK) ? information_frame.data_size : ERROR;
}
