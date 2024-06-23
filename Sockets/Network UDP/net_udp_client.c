#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUF_SIZE 128

int main(void)
{
	struct sockaddr_in server;
	int serv_fd;
	socklen_t serv_len;
	char send_buf[MAX_BUF_SIZE] = "Hi!";
	char recv_buf[MAX_BUF_SIZE] = { 0 };
	
	if ((serv_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){						//create the same socket as the server	
		perror("socket");
		exit(1);
	}
	
	memset(&server, 0, sizeof(server));								//clear the server struct	
	server.sin_family = AF_INET;									//in server struct indicate family...
	server.sin_port = htons(7777);									//...and port...
	server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);						//...and address
	serv_len = sizeof(server);							
	
	sendto(serv_fd, send_buf, MAX_BUF_SIZE, 0, (struct sockaddr *) &server, serv_len);		//send message "Hi!" to server						
	recvfrom(serv_fd, recv_buf, MAX_BUF_SIZE, 0, (struct sockaddr *) &server, &serv_len);		//receive a response message "Hello!" from server						
	if ((strcmp(recv_buf, "Hello!")) == 0){
		printf("Connected to server!\nHi!\n");
	}
	close(serv_fd);											//close fd		
	
	return 0;
}
