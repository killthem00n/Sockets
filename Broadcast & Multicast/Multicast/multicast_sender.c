#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUF_SIZE 128

int main(void)
{
	struct sockaddr_in client;
	struct ip_mreqn mcast;
	socklen_t client_size;
	int sock;
	int broadcastEnable = 1;
	char send_buf[MAX_BUF_SIZE] = "packet delivered!";
	
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1){							
		perror("socket");
		exit(1);
	}
	
	mcast.imr_multiaddr.s_addr = inet_addr("224.0.0.1");
	mcast.imr_address.s_addr = htonl(INADDR_ANY);
	mcast.imr_ifindex = 0;
	
	if ((setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mcast, sizeof(mcast)) == -1)){
		perror("setsockopt");
		exit(1);
	}
																				
	client.sin_family = AF_INET;									
	client.sin_port = htons(7777);										
	client.sin_addr.s_addr = inet_addr("224.0.0.1");							
	
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
