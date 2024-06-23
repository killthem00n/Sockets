#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define SERV_PORT 7777
#define MAX_BUF_SIZE 128					

int main(void)
{
	struct sockaddr_in server;								//one struct for server
	int serv_fd;										//var for fd					
	char recv_buf[MAX_BUF_SIZE] = { 0 };							//buf for receive
	char send_buf[MAX_BUF_SIZE] = "connected!";						//buf for send
	socklen_t server_size;
	
	if ((serv_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){					//creating socket		
		perror("socket");
		exit(1);
	}
	
	memset(&server, 0, sizeof(server));							//clear server struct				
	server.sin_family = AF_INET;								//indicate server family
	server.sin_port = htons(SERV_PORT);							//indicate server port
	server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);					//indicate server IP
	server_size = sizeof(server);
							
	sendto(serv_fd, send_buf, MAX_BUF_SIZE, 0, (struct sockaddr *)&server, server_size); 	//send for npoint to server
	recvfrom(serv_fd, recv_buf, 3, 0, (struct sockaddr *)&server, &server_size);		//receive time from server				
	printf("Time: %d:%d:%d\n", recv_buf[0], recv_buf[1], recv_buf[2]);			//printing time
	close(serv_fd);										//close fd	
	
	return 0;
}
