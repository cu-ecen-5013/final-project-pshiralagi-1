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
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define MAX 20
#define PORT 9000
#define SA struct sockaddr

void ipc();

typedef struct {
	int unique_id;
	int tmp;
} data;
char buff[MAX]; 

void func(int sockfd) 
{
	write(sockfd, buff, sizeof(buff)); 
	bzero(buff, sizeof(buff)); 
	read(sockfd, buff, sizeof(buff)); 
	printf("From Server : %s\n\r", buff); 
} 

int main(int argc, char *argv[]) 
{ 
	int sockfd, i; 
	struct sockaddr_in servaddr; 
	for(i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-d") == 0)
		{
			printf("Daemon\n\r");
			daemon(1, 1);
		}	
	}
	// socket create and varification 
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
	servaddr.sin_addr.s_addr = inet_addr("10.0.0.136"); 
	servaddr.sin_port = htons(PORT); 

	// connect the client socket to server socket 
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
		printf("connection with the server failed...\n"); 
		exit(0); 
	} 
	else
		printf("connected to the server..\n"); 

	while(1)
	{
		ipc();
		// function for chat 
		func(sockfd);
		sleep(5);
	}

	// close the socket 
	close(sockfd); 
}

void ipc(void)
{
	data temperature;
	data *temperature_ptr = &temperature;
	data *temp_ptr = NULL;
	int fd_shared;
	fd_shared = shm_open("shareTmp", O_RDONLY, 0666);
	if (fd_shared < 0)
	{
		perror("open");
	}
	temp_ptr = (data *)mmap(NULL, sizeof(data), PROT_READ, MAP_SHARED, fd_shared, 0);
	if (close(fd_shared) < 0)
	{
		perror("close");
	}
	memcpy((void*)temperature_ptr,(void*)(&temp_ptr[0]),sizeof(data));
	printf("Temperature is %d\n\r", temp_ptr->tmp);
	buff = temp_ptr->tmp;
	munmap(temp_ptr,sizeof(data));
}

