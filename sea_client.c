#include "seadp.h"



void *sender(){

}



int main(){


	char *srcIp="192.168.46.117";
	char *dstIp="192.168.46.117";
	int hostSeaPort=2000;
	//char *srcEid="eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee1";
	//char *dstEid="eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee2";
	int sockfd;
	struct sockaddr_in dst;
	IP_HDR *ip;
	SEANET_HDR *seanet;
	SEADP_HDR *seadp;
	//char data[]="Hello seadp.";
	unsigned char packet[84];
	memset(packet,0,84);

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
    ip->len = htons(84);
    ip->id = htons(1);
    ip->off = htons(0);
    ip->ttl = 10;
    ip->protocol = 176;
    ip->srcAddr = inet_addr(srcIp);
    ip->dstAddr = inet_addr(dstIp);
    ip->checksum=checksum(packet,20);

    seanet=(SEANET_HDR*)(packet+sizeof(IP_HDR));
    seanet->next=1;
    seanet->len=44;
    seanet->attr=htons(1);
    for (int i = 0; i < 19; ++i)
    {
    	seanet->srcEid[i]=0xee;
    	seanet->dstEid[i]=0xee;
    }
    seanet->srcEid[19]=0x02;
    seanet->dstEid[19]=0x03;

	seadp=(SEADP_HDR*)(packet+sizeof(IP_HDR)+sizeof(SEANET_HDR));
	seadp->srcPort=htons(hostSeaPort);
	seadp->dstPort=htons(1000);
	seadp->pflag=64;
	seadp->cflag=2;
	seadp->tflag=3;
	seadp->reserve=htons(300);
	seadp->total=htonl(40);
	seadp->off=htonl(0);
	seadp->id=htons(0);
	seadp->checksum=checksum((packet+20+44),20);
	
	// printIp(ip);
	// printSeanet(seanet);
	// printSeadp(seadp);




	if (sendto(sockfd,packet,84,0,(struct sockaddr*)&dst,sizeof(struct sockaddr))<0){
		perror("send failed.");
		return -1;
	}
	printf("Send over.\n");
	//close(sockfd);

	//rcv chunk
	struct sockaddr_in rcv_dst;
	IP_HDR *rcv_ip;
	SEANET_HDR *rcv_seanet;
	SEADP_HDR *rcv_seadp;
	char chunk[100];
	unsigned char rcv_packet[PACKET_LEN];
	memset(rcv_packet,0,PACKET_LEN);

	int rcv_sockfd=socket(PF_PACKET,SOCK_DGRAM,htons(ETH_P_IP));
	if (rcv_sockfd<0){
		printf("rcv_sockfd: %d\n", rcv_sockfd);
		return -1;
	}
	
	int data_len=13;
	int data_confirm[data_len];
	memset(data_confirm,0,sizeof(data_confirm));
	int count=0;
	while(count<data_len-1){
		int n=recv(rcv_sockfd,rcv_packet,PACKET_LEN,0);
		if (n==-1){
			printf("recv error.\n");
			break;
		}else if (n==0){
			continue;
		}
		rcv_ip=(IP_HDR*)rcv_packet;
		if (rcv_ip->protocol==176){
			rcv_seanet=(SEANET_HDR*)(rcv_packet+sizeof(IP_HDR));
			if (rcv_seanet->next==1){
				rcv_seadp=(SEADP_HDR*)(rcv_packet+sizeof(IP_HDR)+sizeof(SEANET_HDR));
				if (ntohs(rcv_seadp->dstPort)==hostSeaPort){
					int offset=ntohl(rcv_seadp->off);
					//printf("offset: %d\n", offset);
					//printf("data_confirm: %d\n", data_confirm[offset]);
					if (data_confirm[offset]==0){
						memcpy(chunk+offset,rcv_packet+84,1);
						data_confirm[offset]=1;
						count++;
						if (count==11){
							printf("still need packet id=5\n");
							seadp->pflag=32;
							seadp->dstPort=rcv_seadp->srcPort;
							seadp->off=htonl(4);
							seadp->id=htons(5);
							seadp->checksum=checksum((packet+20+44),20);
							if (sendto(sockfd,packet,84,0,(struct sockaddr*)&dst,sizeof(struct sockaddr))<0){
								perror("send failed.");
								return -1;
							}
							printf("send ret request over\n");
						}

					}
				}
			}
		}
	}
	printf("%s\n", chunk);
	close(rcv_sockfd);
	return 0;
}