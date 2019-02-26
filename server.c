#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>

#include "seadp.h"

int main(){
	char *srcIp="192.168.233.134";
	char *dstIp="192.168.233.134";
	char *srcEid="eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee1";
	char *dstEid="eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee2";
	int sockfd;
	struct sockaddr_in dst;
	IP_HDR *ip;
	SEANET_HDR *seanet;
	SEADP_HDR *seadp;
	char data[]="Hello seadp.";
	unsigned char packet[PACKET_LEN];
	memset(packet,0,PACKET_LEN);
	int len_per_packet=1;

	sockfd=socket(AF_INET,SOCK_RAW,IPPROTO_RAW);
	if (sockfd<0){
		printf("sockfd: %d\n", sockfd);
		return -1;
	}
	dst.sin_family =AF_INET;
	dst.sin_addr.s_addr = inet_addr(srcIp);
	int one=1;
	if (setsockopt(sockfd,IPPROTO_IP,IP_HDRINCL,&one,sizeof(one))<0){
		perror("setsockopt failed.\n");
		return -1;
	}

	ip=(IP_HDR*)packet;
	ip->version = 4; 
    ip->ihl = 5;
    ip->tos = 0;
    ip->len = htons(PACKET_LEN);
    //ip->id = htons(1);
    ip->off = htons(0);
    ip->ttl = 10;
    ip->protocol = 176;
    ip->srcAddr = inet_addr(srcIp);
    ip->dstAddr = inet_addr(dstIp);
    //ip->checksum=checksum(packet,20);

    seanet=(SEANET_HDR*)(packet+sizeof(IP_HDR));
    seanet->next=1;
    seanet->len=44;
    seanet->attr=htons(1);

    for (int i = 0; i < 19; ++i)
    {
    	seanet->srcEid[i]=0xee;
    	seanet->dstEid[i]=0xee;
    }
    seanet->srcEid[19]=0x01;
    seanet->dstEid[19]=0x02;

	seadp=(SEADP_HDR*)(packet+sizeof(IP_HDR)+sizeof(SEANET_HDR));
	seadp->srcPort=htons(10000);
	seadp->dstPort=htons(10001);
	seadp->pflag=1;
	seadp->cflag=2;
	seadp->tflag=3;
	seadp->reserve=htons(300);
	seadp->total=htonl(41);
	//seadp->off=htonl(0);
	//seadp->id=htons(1);
	//seadp->checksum=checksum((packet+20+44),20);

	int id=1;
	int pdata=0;
	int data_len=sizeof(data);
	while(pdata<data_len){
		ip->id = htons(id);
		ip->checksum=checksum(packet,20);
		memcpy((packet+20+44+20),data+pdata,1);
		seadp->off=htonl(pdata);
		seadp->id=htons(id);
		seadp->checksum=checksum((packet+20+44),21);
		printIp(ip);
		printSeanet(seanet);
		printSeadp(seadp);
		printf("Message: %02x\n", (packet+20+44+20));
		if (sendto(sockfd,packet,PACKET_LEN,0,(struct sockaddr*)&dst,sizeof(struct sockaddr))<0){
			perror("send failed.");
			return -1;
		}
		pdata++;
		id++;
	}
	printf("Send over.\n");

	return 0;
}