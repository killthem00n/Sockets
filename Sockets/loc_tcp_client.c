#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SERV_PATH "/tmp/server"
#define MAX_BUF_SIZE 128

int main(void)
{
	struct sockaddr_un server;
	int serv_fd;
	socklen_t server_size;
	char send_buf[MAX_BUF_SIZE] = "Hi!";
	char recv_buf[MAX_BUF_SIZE] = { 0 };
	
	if ((serv_fd = socket(AF_LOCAL, SOCK_STREAM, 0)) == -1){					//creat the same socket as the server
		perror("socket");
		exit(1);
	}
	
	memset(&server, 0, sizeof(server));								//clear the server struct
	server.sun_family = AF_LOCAL;									//in server struct indicate family...
	strncpy(server.sun_path, SERV_PATH, sizeof(server.sun_path) - 1);				//...and path to server
	server_size = sizeof(server);
	
	if ((connect(serv_fd, (struct sockaddr *) &server, server_size)) == -1){			//connect to the server
		perror("connect");
		exit(1);
	}
	
	send(serv_fd, send_buf, MAX_BUF_SIZE, 0);							//send message "Hi!" to server
	recv(serv_fd, recv_buf, MAX_BUF_SIZE, 0);							//receive a response message "Hello!" from server
	if ((strcmp(recv_buf, "Hello!")) == 0){
		printf("Connected to server!\nHi!\n");
	}
	close(serv_fd);											//close the exchange fd
	
	return 0;
}
