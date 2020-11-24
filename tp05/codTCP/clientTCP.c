/*      (C)2000 FEUP  */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <strings.h>
#include <string.h>

#include "getip.h"

#define SERVER_PORT 6000
#define SERVER_ADDR "192.168.28.96"

int main(int argc, char** argv){

	int	sockfd, sockfdCl=-1;
	struct	sockaddr_in server_addr, server_addrCl;
	char	buf[] = "Mensagem de teste na travessia da pilha TCP/IP\n";  
	int	bytes;
	struct hostent *h;
	char *user, *password, *host, *url_path;

	parseArg(argv[1],user,password,host,url_path);

	h = getIP(host);

	/*server address handling*/
	bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);	/*32 bit Internet address network byte ordered*/
	server_addr.sin_port = htons(SERVER_PORT);		/*server TCP port must be network byte ordered */
    
	/*open an TCP socket*/
	if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
		perror("socket()");
		exit(0);
	}
	/*connect to the server*/
	if(connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
		perror("connect()");
		exit(0);
	}
    	/*send a string to the server*/
	bytes = write(sockfd, buf, strlen(buf));
	printf("Bytes escritos %d\n", bytes);

	close(sockfd);
	exit(0);
}

// ./download ftp://user:1234@sftp.up.pt/pub/ficheiro.zip
void parseArg(char *arg, char *user, char *password, char *host, char *url_path) {
	
	
	char *args = strtok(arg,"/");
	args = strtok(NULL,"/:");
	user=args;
	printf("1 - %s\n",user);
	
	args = strtok(NULL,"@");
	password=args;
	printf("2 - %s\n",password);
	
	args = strtok(NULL,"/");
	if(args==NULL){
	    printf("No User\nSetting Default- user\n");
	    host=user;
	    user= "user";
	    printf("3 - %s\n",host);
	}
	else{
	    host=args;
	    printf("3 - %s\n",host);
	}
	
	args = strtok(NULL,"\0");
	if(args==NULL){
	    printf("No Password\nSetting Default- 1234\n");
	    url_path=password;
	    password= "1234";
	    printf("4 - %s\n",url_path);
	}
	else{
	    url_path=args;
	    printf("4 - %s\n",url_path);
	}

}


