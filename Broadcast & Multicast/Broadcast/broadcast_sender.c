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
	struct sockaddr_in client;
	socklen_t client_size;
	int sock;
	int broadcastEnable = 1;
	char send_buf[MAX_BUF_SIZE] = "packet delivered!";
	
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1){							
		perror("socket");
		exit(1);
	}
	
	if ((setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) == -1)){
		perror("setsockopt");
		exit(1);
	}
									
	memset(&client, 0, sizeof(client));											
	client.sin_family = AF_INET;									
	client.sin_port = htons(7777);										
	client.sin_addr.s_addr = htonl(INADDR_BROADCAST);							
	
	while (1){
		if ((sendto(sock, send_buf, MAX_BUF_SIZE, 0, (struct sockaddr *)&client, sizeof(struct sockaddr_in))) == -1){
			perror("sendto");
			exit(1);
		}
		sleep(1);
	}								
																										
	close(sock);											
	
	return 0;
}
