#pragma once
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <strings.h>
#include <string.h>

#define PORT_FTP 21
#define SERV_READY 220
#define USER_LOGIN 331
#define PASS_LOGIN 230
#define BIN_READY 200
#define PASV_READY 227
#define RETRV_READY 150
#define FILE_READY 226



int socket_connection(char * ip, int port);

int socket_exit();

int ftp_init_connection(char * ip);

int ftp_read_socket(int sockfd);

int ftp_write_socket(int sockfd, char *msg);

int ftp_enter_pasv(int sockfd, char *ip, int *port);

int ftp_login(char * username, char* password);

int ftp_passive(char * ip, int* port);

int ftp_retrieve(char * url_path);

int ftp_get_file(char * url_path);

int ftp_download(char * url_path);