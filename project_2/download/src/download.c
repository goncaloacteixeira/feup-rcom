/*      (C)2000 FEUP  */

#include "connection.h"
#include "getip.h"

#define SERVER_PORT 6000
#define SERVER_ADDR "192.168.28.96"



int main(int argc, char** argv){

	if(argc!=2){
		printf("usage: download ftp://[<user>:<password>@]<host>/<url-path>\n");
        return 1;
	}
	
	char user[1024], password[1024], host[1024], url_path[1024], *ip;

	parseArg(argv[1],user,password,host,url_path);
	printArg(user,password,host,url_path);
	ip = getIP(host);
	printf("IP - %s\n",ip);
	if(ftp_init_connection(ip)==-1)return -1;
	if(ftp_login(user,password)==-1)return -1;
	if(ftp_download(url_path)==-1) return -1;
	
	return 0;
}
void printArg(char *user, char *password, char *host, char *url_path){
	printf("User - %s\n",user);
	printf("Password - %s\n",password);
	printf("Host - %s\n",host);
	printf("URL - %s\n",url_path);
}
// ./download ftp://user:1234@sftp.up.pt/pub/ficheiro.zip
void parseArg(char *arg, char *user, char *password, char *host, char *url_path) {
	
	
	char *args = strtok(arg,"/");
	args = strtok(NULL,"/:");
	strcpy(user,args);
	
	args = strtok(NULL,"@");
	strcpy(password,args);
	
	args = strtok(NULL,"/");
	if(args==NULL){
	    printf("No User\nSetting Default- anonymous\n");
	    strcpy(host,user);
	    strcpy(user,"anonymous");
	}
	else{
	    strcpy(host,args);
	}
	
	args = strtok(NULL,"\0");
	if(args==NULL){
	    printf("No Password\nSetting Default- 1234\n");
	    strcpy(url_path,password);
	    strcpy(password,"1234");
	}
	else{
	    strcpy(url_path,args);
	}

}

