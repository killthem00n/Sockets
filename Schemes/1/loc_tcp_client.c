#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define SERV_PATH "sock.server"
#define MAX_BUF_SIZE 128

int main(void)
{
	struct sockaddr_un server;								//one struct for server
	int serv_fd;										//var for fd					
	char recv_buf[MAX_BUF_SIZE] = { 0 };							//buf for receive
	
	if ((serv_fd = socket(AF_LOCAL, SOCK_STREAM, 0)) == -1){				//creating socket		
		perror("socket");
		exit(1);
	}
	
	memset(&server, 0, sizeof(server));							//clear server struct	
	server.sun_family = AF_LOCAL;								//indicate server family	
	strncpy(server.sun_path, SERV_PATH, sizeof(server.sun_path) - 1);			//indicate server path	
	
	if ((connect(serv_fd, (struct sockaddr *) &server, sizeof(server))) == -1){		//connecting to server		
		perror("connect");
		exit(1);
	}
								
	recv(serv_fd, recv_buf, MAX_BUF_SIZE, 0);						//receive time from server				
	printf("Time: %d:%d:%d\n", recv_buf[0], recv_buf[1], recv_buf[2]);			//printing time
	close(serv_fd);										//close fd	
	
	return 0;
}
