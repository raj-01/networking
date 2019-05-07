// socket programming    tcpserver.c

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include<string.h>

int  main(int argc, char const *argv[])
{
	struct sockaddr_in serverAddr;
	int socketfd ;
	if( argc < 2)
	{ 	fprintf(stderr, "use %s< port >\n",argv[0] );
		exit(-1);
	}
	int portno = atoi( argv[1]);

	// creating socket 
	socketfd = socket(AF_INET,SOCK_STREAM,0);
	if(!socketfd){
		perror("error in opening socket");
		exit(-1);
	}
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(portno);

	// binding socket 
	if(bind(socketfd, ( struct sockaddr *)&serverAddr,sizeof(serverAddr)) < 0){
		perror("failed to bind");
		exit(2);
	}
	
	// so 5 connection at max can wait/backloged while one connection is handled by server
	if(listen(socketfd , 5)){
		perror("error in listen :");
		exit(4);
	}

	int optval = 1 ;
	setsockopt( socketfd, SOL_SOCKET, SO_REUSEADDR,(const void *)&optval , sizeof(int));

	// creating new socket because the creating sockets can be many 
	struct sockaddr_in clientAddr;
	char recvBuf[2048];
	int bufLen = sizeof(recvBuf);
	int recvLen = 0;
	int cliLen = sizeof(clientAddr);
	while(1){
		cliLen = sizeof(clientAddr);
		/*if((recvLen = recvfrom(socketfd, recvBuf , bufLen,0,(struct sockaddr *)&clientAddr,&cliLen))<0){
			perror("error in recving");
			exit(4);*/
		int childfd = accept(socketfd,( struct sockaddr *)&clientAddr,&cliLen);
		if( childfd < 0){
			perror( " error in accept");
			continue;
			}
		
		//recvBuf[recvLen] = 0;
		printf("new connection from %s: %d :\n", inet_ntoa(clientAddr.sin_addr),ntohs(clientAddr.sin_port));
		// for parrent process forkRet != 0 , while for the child process its 0
		// so , for parent it will not go into if condition
		// for child process it will exicute the if conditio
		// things are happening in parallel
		int forkRet = fork();
		if(forkRet == 0){
			while(1){
				// recvLen : amount of data which is recived
				if(( recvLen = recv(childfd, recvBuf , bufLen,0)) < 0){
					perror("error in recving");
					break;
				}
				// if recvLen == 0 that means you are not getting any data from cli 
				// so, close the connection
				if(recvLen == 0){
					close(childfd);
					break;
				}
				recvBuf[recvLen] = 0;
				printf("recv %s\n", recvBuf);
				if(send(childfd, recvBuf , recvLen,0) < 0){
					perror("error in sending");
					continue;
				}	
			}
			exit(0);
		}


		/*printf("%s\n",recvBuf );
		if( sendto(socketfd , recvBuf, recvLen,0,(struct sockaddr *)&clientAddr,sizeof(clientAddr))<0){
			perror("error in sending");
			exit(3);
		}*/
	}
	return 0;
}