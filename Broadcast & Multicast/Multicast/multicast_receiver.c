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
	socklen_t serv_len;
	int sock;
	char recv_buf[MAX_BUF_SIZE] = { 0 };
	
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1){						
		perror("socket");
		exit(1);
	}
	
	memset(&server, 0, sizeof(server));								
	server.sin_family = AF_INET;									
	server.sin_port = htons(7777);									
	server.sin_addr.s_addr = htonl(INADDR_ANY);						
	serv_len = sizeof(server);
	
	if ((bind(sock, (struct sockaddr *)&server, sizeof(server))) == -1){		
		perror("bind");
		exit(1);
	}							
							
	while (1){
		if ((recvfrom(sock, recv_buf, MAX_BUF_SIZE, 0, (struct sockaddr *)&server, &serv_len)) == -1){
			perror("recvfrom");
			exit(1);
		}
		printf("%s\n", recv_buf);
	}							
	
	close(sock);													
	
	return 0;
}
