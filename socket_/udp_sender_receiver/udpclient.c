// udp client udpclient.c 

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include<string.h>
#include<netdb.h>

int main(int argc, char const *argv[])
{
	struct sockaddr_in serverAddr ;
	struct hostent *server;
	char const *hostname;
	int socketfd;
	if(argc < 3){
		fprintf(stderr, "use%s<hostname<port>\n",argv[0] );
		exit(-1);
	}
	hostname = argv[1];
	int portno = atoi(argv[2]);

	socketfd = socket(AF_INET , SOCK_DGRAM,0);
	if(!socketfd){
		perror(" error in opening socket");
		exit(1);
	}
	server = gethostbyname(hostname);
	if(server == NULL){
		fprintf(stderr, "can't resolve hostname\n");
		exit(2);
	}
	bzero((char *)&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	bcopy((char *)server -> h_addr,(char *)&serverAddr.sin_addr.s_addr,server -> h_length);
	serverAddr.sin_port = htons(portno);
	
	char *buf = "Hello there!!";
	int bufLen = strlen(buf);
	if(sendto(socketfd, buf, bufLen+1,0,(struct sockaddr *)&serverAddr,sizeof(serverAddr)) < 0){
		perror("error in sending");
		exit(3);
	}
	char recvBuf[2048];
	bufLen = sizeof(recvBuf);
	int recvLen = 0;
	int serLen = sizeof(serverAddr);
	if(serLen = recvfrom(socketfd,recvBuf, bufLen,0,(struct sockaddr *)&serverAddr,&serLen) <0){
		perror("error in recving");
		exit(4);
	}
	recvBuf[recvLen] = 0;
	printf("Recv fro %s:%d :",inet_ntoa(serverAddr.sin_addr),ntohs(serverAddr.sin_port));
	printf("%s\n", recvBuf );

	return 0;
}
