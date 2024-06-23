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
	int serv_fd, cl_fd;
	char send_buf[MAX_BUF_SIZE] = "Hello!";
	char recv_buf[MAX_BUF_SIZE] = { 0 };
	
	if ((serv_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){						//create the socket	
		perror("socket");
		exit(1);
	}
	
	memset(&server, 0, sizeof(server));								//clear the server struct							
	server.sin_family = AF_INET;									//in server struct indicate family...
	server.sin_port = htons(7777);									//...and port...
	server.sin_addr.s_addr = htonl (INADDR_ANY);							//...and address
	
	if ((bind(serv_fd, (struct sockaddr *) &server, sizeof(server))) == -1){			//binding the struct		
		perror("bind");
		exit(1);
	}
	
	if ((listen(serv_fd, 1)) == -1){								//limit the number of clients in queue					
		perror("listen");
		exit(1);
	}
	
	if ((cl_fd = accept(serv_fd, NULL, NULL)) == -1){						//accept a request to join from a client
		perror("accept");
		exit(1);
	} 
	
	recv(cl_fd, recv_buf, MAX_BUF_SIZE, 0);								//receive a message from client and...					
	if ((strcmp(recv_buf, "Hi!")) == 0){								//...if message equals "Hi!"...				
		send(cl_fd, send_buf, MAX_BUF_SIZE, 0);							//...then send a response message "Hello!" to client			
		printf("Client connected!\nHello!\n");
	}
	sleep(2);											//timeout to ensure the message is sent				
	close(cl_fd);											//close the exchange fd
	close(serv_fd);											//close the queue fd					
	
	return 0;
}
