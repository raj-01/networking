// socket programming    tcpserver.c
/* int select(int nfds, fd_set *readfds , fd_set *writefds , fd_set *exceptfds, struct timeval *timeout);
	synchonous I/O multiplexing over asynchronous inputs

	readfds --> will be watched to see if charters become available for reading
	writefds --> will be watched to see if space is available for write
	exceptfds --> will be watched for exceptional conditions
	
	Timeout : the inteval that select() should waiting for a file descriptor to become ready
	select cal will block until either
	> a file descriptor becomes ready
	> the call is interrupted by a signal handler
	> the timeout expires

	struct timeval{
		long tv_sec;
		long tv_usec;
	}

	retun value if select() , -1 is for error , 0 is for time out 
	, >0 is number of sockets that have events pending ( read , write , exception)
*/
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

	int fds[16];
	int fdCapa = 16;
	int fdlen = 0;

	fd_set rfds;
	int maxfd = socketfd;
	while(1){
		FD_ZERO(&rfds);
		FD_SET(socketfd,&rfds);
		int x = 0;
		maxfd = socketfd;
		for(x = 0; x< fdlen;x++){
			FD_SET(fds[x],&rfds);
			if(maxfd < fds[x]) maxfd = fds[x];
		}
		// As there is no time out val in select so whenever some 
		// event happens then only it will come out from select
		select(maxfd +1 , &rfds,NULL, NULL ,NULL);
		// checking is the socket is ready to read the data
		if(FD_ISSET(socketfd,&rfds)){
			cliLen = sizeof(clientAddr);
			int childfd = accept(socketfd,( struct sockaddr *)&clientAddr,&cliLen);
			if( childfd < 0){
				perror( " error in accept");
				continue;
				}
			
			//recvBuf[recvLen] = 0;
			printf("new connection from %s: %d :\n", inet_ntoa(clientAddr.sin_addr),ntohs(clientAddr.sin_port));
			if( fdlen == fdCapa){
				close(childfd);
			}
			else{
				fds[fdlen] = childfd;
				fdlen++;
			}
		}
		else{

			int childfd = 0;
			for(x = 0 ; x < fdlen;x++){
				if(!FD_ISSET(fds[x],&rfds)) continue;
				childfd = fds[x];
				if((recvLen = recv(childfd , recvBuf,bufLen,0))<0){
					perror("error in recving");
					fds[x] = fds[fdlen-1];
					fdlen-- ;
					break;
					}
				if( recvLen == 0){
					close(childfd);
					FD_CLR(childfd,&rfds);
					fds[x] = fds[fdlen-1];
					fdlen--;
					printf("closing\n");
					break;				
				}	
				recvBuf[recvLen] = 0;
				printf("recv %s\n", recvBuf);
				if(send(childfd, recvBuf , recvLen,0) < 0){
					perror("error in sending");
					continue;
					}
			}

			
		}
	}

	return 0;
}