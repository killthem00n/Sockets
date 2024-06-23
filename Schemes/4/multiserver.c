#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define SERV_PORT 7777
#define MAX_BUF_SIZE 128

char time_buf[3] = { 0 }; 									//buf for sending time

void tell_time(void)										//function for determine time
{					
	time_t mytime = time(NULL);								
	struct tm *now = localtime(&mytime);
	time_buf[0] = now->tm_hour;
	time_buf[1] = now->tm_min;
	time_buf[2] = now->tm_sec;																							
}

int max(int x, int y)										//function for find max number between fd's
{
	if (x>y){
		return x;
	} else {
		return y;
	}
}

int main(void)
{
	struct sockaddr_in server, client;							
	int tcp_fd, udp_fd, ready_fd, connected_tcp_fd, maxfd;
	socklen_t len;
	fd_set rset;
	char recv_buf[MAX_BUF_SIZE] = { 0 };			
	
	if ((tcp_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){					//creating tcp socket			
		perror("socket");
		exit(1);
	}
	
	memset(&server, 0, sizeof(server));							//clear server struct	
	memset(&client, 0, sizeof(client));							//clear client struct						
	server.sin_family = AF_INET;								//indicate server family
	server.sin_port = htons(7777);								//indicate server port
	server.sin_addr.s_addr = htonl(INADDR_ANY);						//indicate local address
	
	if ((bind(tcp_fd, (struct sockaddr *) &server, sizeof(server))) == -1){			//binding the server struct to tcp fd
		perror("bind");
		exit(1);
	}
	
	if ((listen(tcp_fd, 10)) == -1){							//limit the number of clients in queue		
		perror("listen");
		exit(1);
	}
	
	if ((udp_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){					//creating udp socket
		perror("socket");
		exit(1);
	}
	
	if ((bind(udp_fd, (struct sockaddr *) &server, sizeof(server))) == -1){			//binding the server struct to udp fd
		perror("bind");
		exit(1);
	}
	
	FD_ZERO(&rset);										//clear the fd set
	maxfd = max(tcp_fd, udp_fd) + 1;							//find max fd
	
	while (1){										
		FD_SET(tcp_fd, &rset);								//set tcp fd in readset
		FD_SET(udp_fd, &rset);								//set udp fd in readset
		ready_fd = select(maxfd, &rset, NULL, NULL, NULL);				//select the ready fd
		
		if (FD_ISSET(tcp_fd, &rset)){							//if tcp fd ready...
			len = sizeof(client);
			connected_tcp_fd = accept(tcp_fd,  (struct sockaddr *)&client, &len);	//...accept the client...
			tell_time();			
			send(connected_tcp_fd, time_buf, 3, 0);					//...and send time to client
			close(connected_tcp_fd);						//close client fd
		}
		
		if (FD_ISSET(udp_fd, &rset)){							//if udp fd ready...
			len = sizeof(client);
			tell_time();
			recvfrom(udp_fd, recv_buf, MAX_BUF_SIZE, 0, (struct sockaddr *)&client, &len);		//receive msg from client for fill a client struct
			sendto(udp_fd, time_buf, 3, 0, (struct sockaddr *) &client, len);			//send time to client
		}
	}
}
