#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>

#include "seadp.h"

int main(){
	int sockfd;
	struct sockaddr_in conn;
	IP_HDR *ip;
	SEANET_HDR *seanet;
	SEADP_HDR *seadp;
	char data[100];
	unsigned char packet[PACKET_LEN];
	memset(packet,0,PACKET_LEN);
	int len_per_packet=1;
	ssize_t n;

	sockfd=socket(PF_PACKET,SOCK_DGRAM,htons(ETH_P_IP));
	if (sockfd<0){
		printf("sockfd: %d\n", sockfd);
		return -1;
	}
	printf("Start to recieve seadp packet.\n");
	int data_len=13;
	int count=0;
	while(1){
		n=recv(sockfd,packet,PACKET_LEN,0);
		if (n==-1){
			printf("recv error.\n");
			break;
		}else if (n==0){
			continue;
		}
		ip=(IP_HDR*)packet;
		//printf("%d\n", ip->protocol);
		if (ip->protocol==176){
			printf("Recieve a seanet packet\n");
			seanet=(SEANET_HDR*)(packet+sizeof(IP_HDR));
			seadp=(SEADP_HDR*)(packet+sizeof(IP_HDR)+sizeof(SEANET_HDR));
			//memcpy(data,(packet+20+44+20),1);
			//printIp(ip);
			//printSeanet(seanet);
			//printSeadp(seadp);
			//printf("Message: %s\n", data);
			unsigned int off=ntohl(seadp->off);
			printf("%u\n", off);
			memcpy(data+off,(packet+20+44+20),1);
			count++;
			if (count==13){
				break;
			}
		}
	}
	printf("%s\n", data);
	close(sockfd);
	return 0;
}