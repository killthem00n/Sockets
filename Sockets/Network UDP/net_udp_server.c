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
	struct sockaddr_in server, client;
	int serv_fd;
	socklen_t client_size;
	char send_buf[MAX_BUF_SIZE] = "Hello!";
	char recv_buf[MAX_BUF_SIZE] = { 0 };
	
	if ((serv_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){						//create the socket	
		perror("socket");
		exit(1);
	}
	
	memset(&server, 0, sizeof(server));								//clear the server struct
	memset(&client, 0, sizeof(client));								//clear the client struct				
	server.sin_family = AF_INET;									//in server struct indicate family...
	server.sin_port = htons(7777);									//...and port...	
	server.sin_addr.s_addr = htonl (INADDR_ANY);							//...and address
	
	if ((bind(serv_fd, (struct sockaddr *) &server, sizeof(server))) == -1){			//binding the struct
		perror("bind");
		exit(1);
	}
	
	client_size = sizeof(client);
	
	recvfrom(serv_fd, recv_buf, MAX_BUF_SIZE, 0, (struct sockaddr*) &client, &client_size);		//receive a message from client and...						
	if ((strcmp(recv_buf, "Hi!")) == 0){								//...if message equals "Hi!"...	
		sendto(serv_fd, send_buf, MAX_BUF_SIZE, 0, (struct sockaddr *) &client, client_size);	//...then send a response message "Hello!" to client				
		printf("Client connected!\nHello!\n");
	}
	sleep(2);																						
	close(serv_fd);											//close fd
	
	return 0;
}
