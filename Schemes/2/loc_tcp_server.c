#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define SERV_PATH "sock.server"
#define MAX_BUF_SIZE 128
#define MAX_CLIENTS 10

int clients_fd[MAX_CLIENTS] = { 0 };
pthread_t THREADS[MAX_CLIENTS];

void *tell_time(void *args)
{
	char send_buf[MAX_BUF_SIZE] = { 0 };							//buf for sending			
	int *id = (int *) args;									//converting thread id to int
	
	while (1){										//infinite cycle			
		if (clients_fd[*id] != 0){							//if fd in not zero then time sending begins, otherwise the check continues
			time_t mytime = time(NULL);								
			struct tm *now = localtime(&mytime);
			send_buf[0] = now->tm_hour;
			send_buf[1] = now->tm_min;
			send_buf[2] = now->tm_sec;
			send(clients_fd[*id], send_buf, MAX_BUF_SIZE, 0);			//sending time to client		
			close(clients_fd[*id]);							//close client fd		
			clients_fd[*id] = 0;							//clean element after fd
		} 
		sleep(0.01);									//delay in checking because w/o it the VM starts to freeze a lot	
	}
}

int main(void)
{
	int thread_tmp[MAX_CLIENTS];								//array for excluding repeat of thread id's
	struct sockaddr_un server, client[MAX_CLIENTS];						//one struct for server and MAX_CLIENTS structs for each client
	int serv_fd;										//var for passive fd
	socklen_t client_size[MAX_CLIENTS];							//array for client structure sizes
	void *status;										//ptr for execution pthread_join

	if ((serv_fd = socket(AF_LOCAL, SOCK_STREAM, 0)) == -1){				//creating socket
		perror("socket");
		exit(1);
	}
	
	memset(&server, 0, sizeof(server));							//clear server struct							
	server.sun_family = AF_LOCAL;								//indicate server family
	strncpy(server.sun_path, SERV_PATH, sizeof(server.sun_path) - 1);			//indicate server path
	
	if ((bind(serv_fd, (struct sockaddr *) &server, sizeof(server))) == -1){		//binding the struct
		perror("bind");
		exit(1);
	}
	
	if ((listen(serv_fd, MAX_CLIENTS)) == -1){						//limit the number of clients in queue		
		perror("listen");
		exit(1);
	}
	
	for (int i = 0; i < MAX_CLIENTS; i++){							//cycle for... 	
		memset(&client[i], 0, sizeof(client[i]));					//...cleaning client sctructures and...
		client_size[i] = sizeof(client[i]);						//...sizing them
	}
	
	for (int i = 0; i < MAX_CLIENTS; i++){							//create MAX_CLIENT threads
		thread_tmp[i] = i;
		pthread_create(&THREADS[i], NULL, tell_time, (void *)&thread_tmp[i]);
	}
	
	while (1){										//infinite cycle								
		for (int i = 0; i < MAX_CLIENTS; i++){										//server start finding first free element in fd's array and...
			if (clients_fd[i] == 0){
				if ((clients_fd[i] = accept(serv_fd, (struct sockaddr *) &client[i], &client_size[i])) == -1){	//...start waiting for the client when element found
					perror("accept");
					exit(1);
				}							
			}  
		} 
	}
		
	for (int i = 0; i < MAX_CLIENTS; i++){							//termination of all threads
		pthread_join(THREADS[i], (void **)&status);
	}
		
	close(serv_fd);										//close passive fd
																			
	return 0;
}
