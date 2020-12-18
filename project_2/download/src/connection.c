#include "connection.h"

static char cmd_send[1024];
static int cmd_socket;
static int data_socket;

int socket_connection(char * ip, int port){
    int	sockfd;
    struct	sockaddr_in server_addr;
    bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip);	/*32 bit Internet address network byte ordered*/
	server_addr.sin_port = htons(port);		/*server TCP port must be network byte ordered */
    
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
    return sockfd;
}

int socket_exit(){
    int ret;
    sprintf(cmd_send,"QUIT \r\n");
	ret = ftp_write_socket(cmd_socket,cmd_send);
	if(ret==-1){
		printf("Couldn\'t send retrieve command");
		close(cmd_socket);
		return -1;
	}
    close(cmd_socket);
    return 0;
}

int ftp_read_socket(int sockfd){
    FILE* f = fdopen(sockfd,"r");

    char *buf;
    size_t bRead = 0;
    int response;

    while(getline(&buf,&bRead,f)>0){
        printf("%s",buf);
        if(buf[3]==' '){
            sscanf(buf,"%d",&response);
            break;
        }
    }
    return response;
}

int ftp_write_socket(int sockfd, char *msg) {
    int b, len=strlen(msg);

    if((b=write(sockfd,msg,len))!=len){
        printf("Couldn't write to socket");
        return -1;
    }
    return 0;
}

int ftp_enter_pasv(int sockfd, char *ip, int *port)
{
	int ret;
	char *find, *buf;
	int a,b,c,d,pa,pb;
    size_t bRead = 0;
    FILE* f = fdopen(sockfd,"r");

    while(getline(&buf,&bRead,f)>0){
        printf("%s",buf);
        if(buf[3]==' '){
            sscanf(buf,"%d",&ret);
            break;
        }
    }

    if(ret!=PASV_READY) return -1;
	find = strrchr(buf, '(');
	sscanf(find, "(%d,%d,%d,%d,%d,%d)", &a, &b, &c, &d, &pa, &pb);
	sprintf(ip, "%d.%d.%d.%d", a, b, c, d);
	*port = pa * 256 + pb;
	return 0;
}

int ftp_init_connection(char * ip){
    int ret;
    if((cmd_socket=socket_connection(ip,PORT_FTP))<0){
        printf("Error establishing connection\n");
        return -1;
    }
    ret = ftp_read_socket(cmd_socket);
    if(ret!=SERV_READY){
        printf("Received Bad Response\n");
        close(cmd_socket);
        return -1;
    }
    return 0;
}

int ftp_login(char * username, char* password){
    int ret;
    sprintf(cmd_send,"USER %s\r\n",username);
	ret = ftp_write_socket(cmd_socket,cmd_send);
	if(ret==-1){
		printf("Couldn\'t send user command");
		close(cmd_socket);
		return -1;
	}
	/* Receive Login message*/
	ret = ftp_read_socket(cmd_socket);
	if(ret!=USER_LOGIN){
		printf("Couldn't login user\n");
		close(cmd_socket);
		return -1;
	}

    sprintf(cmd_send,"PASS %s\r\n",password);
	ret = ftp_write_socket(cmd_socket,cmd_send);
	if(ret==-1){
		printf("Couldn\'t send password command");
		close(cmd_socket);
		return -1;
	}
	/* Receive Login message*/
	ret = ftp_read_socket(cmd_socket);
	if(ret!=PASS_LOGIN){
		printf("Couldn't login\n");
		close(cmd_socket);
		return -1;
	}

    sprintf(cmd_send,"TYPE I\r\n");
	ret = ftp_write_socket(cmd_socket,cmd_send);
	if(ret==-1){
		printf("Couldn\'t send binary command");
		close(cmd_socket);
		return -1;
	}
	/* Receive Login message*/
	ret = ftp_read_socket(cmd_socket);
	if(ret!=BIN_READY){
		printf("Couldn't login\n");
		close(cmd_socket);
		return -1;
	}

    return 0;
}

int ftp_passive(char * ip, int* port){
    int ret;

    sprintf(cmd_send,"PASV\r\n");
	ret = ftp_write_socket(cmd_socket,cmd_send);
	if(ret==-1){
		printf("Couldn\'t send passive command");
		close(cmd_socket);
		return -1;
	}
	
	ret = ftp_enter_pasv(cmd_socket,ip,port);
	if(ret!=0){
		printf("Couldn't set passive\n");
		close(cmd_socket);
		return -1;
	}
    return 0;
}

int ftp_retrieve(char * url_path){
    int ret;

    sprintf(cmd_send,"RETR %s\r\n",url_path);
	ret = ftp_write_socket(cmd_socket,cmd_send);
	if(ret==-1){
		printf("Couldn\'t send retrieve command");
		close(cmd_socket);
		return -1;
	}
	ret = ftp_read_socket(cmd_socket);
	if(ret!=RETRV_READY){
		printf("Couldn't retrieve file\n");
		close(cmd_socket);
		return -1;
	}
    return 0;
}

int ftp_get_file(char * url_path){
    int fd;

    char pathcpy[1024],*filename;
    strcpy(pathcpy,url_path);

    char * token = strtok(pathcpy,"/");
    while(token!=NULL){
        filename=token;
        token=strtok(NULL,"/");
    }
    if ((fd = open(filename, O_WRONLY | O_CREAT, 0777)) < 0) {
        printf("Error opening data file!\n");
        return -1;
    }

    char buf[1024];
    int bRead;

    while((bRead = read(data_socket, buf, 1024)) > 0) {
        if (write(fd, buf, bRead) < 0) {
            printf("Error writing data to file!\n");
            return -1;
        }
    }

    if (close(fd) < 0) {
        printf("Error closing file!\n");
        return -1;
    }

    return 0;
}

int ftp_download(char * url_path){
    char ip[64];
    int port,ret;

    if(ftp_passive(ip,&port)==-1)return -1;

    if((data_socket=socket_connection(ip,port))<0){
        close(cmd_socket);
        return -1;
    }
    if(ftp_retrieve(url_path)==-1){
        close(cmd_socket);
        return -1;
    }

    if(ftp_get_file(url_path)==-1){
        close(cmd_socket);
        return -1;
    }

    close(data_socket);
	ret = ftp_read_socket(cmd_socket);
	if(ret!=FILE_READY){
		printf("Couldn't retrieve file\n");
		close(cmd_socket);
		return -1;
	}
    return socket_exit();
}