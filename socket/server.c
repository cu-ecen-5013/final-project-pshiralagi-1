/*
/ @description: Client socket program
/ @date: 12th November 2020
/ @author: Pavan Shiralagi
/ Reference: https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>
#define MAX 80 
#define PORT 8080 
#define SA struct sockaddr 

// Function designed for chat between client and server. 
int func(int sockfd) 
{ 
	char buff[MAX]; 
	int n; 
	// infinite loop for chat 
	for (;;) 
	{ 
		bzero(buff, MAX); 

		// read the message from client and copy it in buffer 
		n = read(sockfd, buff, sizeof(buff));
		if (n < 0)
		{
			perror("read");
			return -1;
		}
		// print buffer which contains the client contents 
		printf("From client: %s\n\r ", buff); 
		n = write(sockfd, buff, sizeof(buff));
		if (n < 0)
		{
			perror("write");
			return -1;
		}

	} 
} 

// Driver function 
int main(int argc, char *argv[]) 
{
	int sockfd, connfd, len, i; 
	struct sockaddr_in servaddr, cli;
	for(i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-d") == 0)
		{
			printf("Daemon\n\r");
			daemon(1, 1);
		}	
	} 

	// socket create and verification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully created..\n"); 
	bzero(&servaddr, sizeof(servaddr)); 

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servaddr.sin_port = htons(PORT); 

	// Binding newly created socket to given IP and verification 
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
		printf("socket bind failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully binded..\n"); 

	// Now server is ready to listen and verification 
	if ((listen(sockfd, 5)) != 0) { 
		printf("Listen failed...\n"); 
		exit(0); 
	} 
	else
		printf("Server listening..\n"); 
	len = sizeof(cli); 

	// Accept the data packet from client and verification 
	connfd = accept(sockfd, (SA*)&cli, (socklen_t*)&len); 
	if (connfd < 0) { 
		printf("server acccept failed...\n"); 
		exit(0); 
	} 
	else
		printf("server acccept the client...\n"); 

	// Function for chatting between client and server 
	func(connfd); 

	// After chatting close the socket 
	close(sockfd); 
} 

