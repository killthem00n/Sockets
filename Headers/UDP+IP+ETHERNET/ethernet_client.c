#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ether.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <unistd.h>
#include <net/if.h>
#include <linux/if_packet.h>

#define ETH_HDR_LEN 14											
#define IP_HDR_LEN 20											
#define UDP_HDR_LEN 8											
	
uint16_t checksum(struct iphdr *ip)									
{
	uint16_t *ptr = (uint16_t *)ip;																									
	uint32_t sum = 0;
	
	for (int i = 0; i < IP_HDR_LEN; i += 2){																						
		sum += *(ptr + i / 2);
	}
	
	sum = (sum >> 16) + (sum & 0xffff);												
	sum += (sum >> 16);														
	
	return ~sum;
}

int main(void)
{	
	char msg[ETH_HDR_LEN + IP_HDR_LEN + UDP_HDR_LEN + 20];											
	struct sockaddr_ll server;
	struct ethhdr eth_hdr;
	struct iphdr ip_hdr;
	struct udphdr udp_hdr;
	int serv_fd;
			
	//sockaddr_ll	
	memset(&server, 0, sizeof(server));																									
	server.sll_family = AF_PACKET;																		
	server.sll_ifindex = if_nametoindex("enp0s3");	
	server.sll_halen = ETH_ALEN;	
	server.sll_protocol = htons(ETH_P_IP);
	server.sll_addr[0] = 0x08;	
	server.sll_addr[1] = 0x00;	
	server.sll_addr[2] = 0x27;	
	server.sll_addr[3] = 0xf1;	
	server.sll_addr[4] = 0x5e;	
	server.sll_addr[5] = 0xe2;										
	
	serv_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)); 									
	if (serv_fd < 0){
		perror("socket");
		exit(1);
	}				
	
	//ETHERNET HEADER
	memset(&eth_hdr, 0, sizeof(struct ethhdr));	
	eth_hdr.h_dest[0] = 0x08;									
	eth_hdr.h_dest[1] = 0x00;
	eth_hdr.h_dest[2] = 0x27;
	eth_hdr.h_dest[3] = 0xf1;
	eth_hdr.h_dest[4] = 0x5e;
	eth_hdr.h_dest[5] = 0xe2;
	eth_hdr.h_source[6] = 0x08;									
	eth_hdr.h_source[7] = 0x00;
	eth_hdr.h_source[8] = 0x27;
	eth_hdr.h_source[9] = 0xfe;
	eth_hdr.h_source[10] = 0x28;
	eth_hdr.h_source[11] = 0x74;
	eth_hdr.h_proto = htons(ETH_P_IP);								
	
	//IP HEADER
	memset(&ip_hdr, 0, sizeof(struct iphdr));
	ip_hdr.ihl = 5;													
	ip_hdr.version = 4;												
	ip_hdr.tos = 16;
	ip_hdr.tot_len = htons(IP_HDR_LEN + UDP_HDR_LEN + 20);								
	ip_hdr.id = htons(54321);
	ip_hdr.frag_off = 0;
	ip_hdr.ttl = 64;
	ip_hdr.protocol = 17;
	ip_hdr.check = 0;
	ip_hdr.saddr = inet_addr("10.0.2.15");
	ip_hdr.daddr = inet_addr("10.0.2.6");
	
	ip_hdr.check = checksum(&ip_hdr);
	
	//UDP HEADER
	memset(&udp_hdr, 0, sizeof(struct udphdr));
	udp_hdr.source = htons(7272);
	udp_hdr.dest = htons(7777);
	udp_hdr.len = htons(UDP_HDR_LEN + 20);
	udp_hdr.check = 0;

	memcpy(msg, &eth_hdr, ETH_HDR_LEN);
	memcpy(msg + ETH_HDR_LEN, &ip_hdr, IP_HDR_LEN);
	memcpy(msg + ETH_HDR_LEN + IP_HDR_LEN, &udp_hdr, UDP_HDR_LEN);
	memcpy(msg + ETH_HDR_LEN + IP_HDR_LEN + UDP_HDR_LEN, "GGWP", 20);
	
	if (sendto(serv_fd, msg, ETH_HDR_LEN + IP_HDR_LEN + UDP_HDR_LEN + 20, 0, (struct sockaddr *)&server, sizeof(server)) < 0){			
		perror("sendto");
		exit(1);
	}
	
	while (1){													
		if (recvfrom(serv_fd, msg, ETH_HDR_LEN + IP_HDR_LEN + UDP_HDR_LEN + 20, 0, NULL, NULL) > 0){
			if (msg[36] == 28 & msg[37] == 104){						
				printf("%s\n", (char *)&msg[42]);							
				break;
			}
		}
		
		sleep(0.01);
	}							
	
	close(serv_fd);																						
	
	return 0;
}
