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
	int serv_fd;
																		
	server.sin_family = AF_INET;											//in server struct indicate family...							
	server.sin_port = htons(7777);											//... port...
	server.sin_addr.s_addr = htonl(INADDR_ANY);									//... and address
	
	if ((serv_fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) == -1){							//create the socket						
		perror("socket");
		exit(1);
	}
	
	char msg_buf[MAX_BUF_SIZE] = { 0 };										//buf for header+payload
	char *ptr;													//ptr for filling buf
	
	unsigned short source_port = 7272;										//add source_port
	ptr = (char *)&source_port;
	msg_buf[0] = *(ptr+1);
	msg_buf[1] = *ptr;
	
	unsigned short destination_port = 7777;										//add destination_port
	ptr = (char *)&destination_port;
	msg_buf[2] = *(ptr+1);
	msg_buf[3] = *ptr;
	
	unsigned short length = MAX_BUF_SIZE;										//add length
	ptr = (char *)&length;
	msg_buf[4] = *(ptr+1);
	msg_buf[5] = *ptr;
	
	msg_buf[6] = 0;													//add check_sum
	msg_buf[7] = 0;

	char payload[4] = "GGWP";											//add payload
	msg_buf[8] = payload[0];											
	msg_buf[9] = payload[1];
	msg_buf[10] = payload[2];
	msg_buf[11] = payload[3];
	
	if (sendto(serv_fd, msg_buf, MAX_BUF_SIZE, 0, (struct sockaddr *)&server, sizeof(server)) < 0){			//send buf to server
		perror("sendto");
		exit(1);
	}
	
	while (1){													
		if (recvfrom(serv_fd, msg_buf, MAX_BUF_SIZE, 0, NULL, NULL) > 0){
			if (msg_buf[22] == 28 & msg_buf[23] == 104){							//if destination_port == our port(7272) => its our message
				printf("%s\n", (char *)&msg_buf[28]);							
				break;
			}
		}
		
		sleep(0.01);
	}							
	
	close(serv_fd);													//close fd										
	
	return 0;
}
