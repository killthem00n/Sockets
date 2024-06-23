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
	char msg_buf[MAX_BUF_SIZE] = { 0 };										
	struct sockaddr_in server;
	int serv_fd;
	int flag = 1;
																		
	server.sin_family = AF_INET;											//in server struct indicate family...							
	server.sin_port = htons(7777);											//... port...
	server.sin_addr.s_addr = htonl(INADDR_ANY);									//... and address
	
	if ((serv_fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) == -1){							//create the socket						
		perror("socket");
		exit(1);
	}
	
	if ((setsockopt(serv_fd, IPPROTO_IP, IP_HDRINCL, &flag, sizeof(flag)) == -1)){
		perror("setsockopt");
		exit(1);
	}
	
	//TCP
	msg_buf[0] = 69;												//add ver/ihl (0100/0101)
	
	msg_buf[1] = 0;													//add ds (0)
										
	msg_buf[2] = 0;													//add length (128)
	msg_buf[3] = 128;
	
	msg_buf[4] = 0;													//add identification (0)
	msg_buf[5] = 0;
	
	msg_buf[6] = 0;													//add flags/offset (0)
	msg_buf[7] = 0;
	
	msg_buf[8] = 255;												//add ttl (255)
	
	msg_buf[9] = 17;												//add transport protocol (17)
	
	msg_buf[10] = 0;												//add checksum (0)
	msg_buf[11] = 0;
	
	msg_buf[12] = 127;												//add source ip											
	msg_buf[13] = 0;
	msg_buf[14] = 0;
	msg_buf[15] = 1;
												
	msg_buf[16] = 127;												//add destination ip											
	msg_buf[17] = 0;
	msg_buf[18] = 0;
	msg_buf[19] = 1;
	
	//UDP
	msg_buf[20] = 28;												//add source_port (7272)
	msg_buf[21] = 104;
							
	msg_buf[22] = 30;												//add destination_port (7777)
	msg_buf[23] = 97;
															
	msg_buf[24] = 0;												//add length (108)
	msg_buf[25] = 108;
	
	msg_buf[26] = 0;												//add check_sum (0)
	msg_buf[27] = 0;

	char payload[4] = "GGWP";											//add payload
	msg_buf[28] = payload[0];											
	msg_buf[29] = payload[1];
	msg_buf[30] = payload[2];
	msg_buf[31] = payload[3];
	
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
