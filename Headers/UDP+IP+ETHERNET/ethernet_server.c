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
	char msg_buf[MAX_BUF_SIZE] = { 0 };
	
	if ((serv_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){											
		perror("socket");
		exit(1);
	}
												
	server.sin_family = AF_INET;																											
	server.sin_port = htons(7777);												
	server.sin_addr.s_addr = htonl(INADDR_ANY);										
	
	if ((bind(serv_fd, (struct sockaddr *)&server, sizeof(server))) == -1){								
		perror("bind");
		exit(1);
	}
	
	client_size = sizeof(client);
							
	if (recvfrom(serv_fd, msg_buf, MAX_BUF_SIZE, 0, (struct sockaddr *) &client, &client_size) > 0){			
		strcat(msg_buf, " accepted");											
		if (sendto(serv_fd, msg_buf, MAX_BUF_SIZE, 0, (struct sockaddr *) &client, client_size) < 0){			
			perror("sendto");
			exit(1);
		}		
	}		
																			
	close(serv_fd);																						
	
	return 0;
}
