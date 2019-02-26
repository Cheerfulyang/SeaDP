#include "seadp.h"
#include "thread_safe_quene.h"

char data[]="Hello seadp.";
TS_QUENE *quene;
char *hostIp="192.168.46.117";
int hostSeaPort=1000;
char *hostEid="eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee1";
int flag=1;

struct para_sender{
	unsigned char *packet;
	int packet_len;
	char *data;
	int data_len;
	int port;
};


void *reciever(int port){
	int sockfd;
	IP_HDR *ip;
	SEANET_HDR *seanet;
	SEADP_HDR *seadp;
	unsigned char packet[PACKET_LEN];
	memset(packet,0,PACKET_LEN);
	sockfd=socket(PF_PACKET,SOCK_DGRAM,htons(ETH_P_IP));
	if (sockfd<0){
		printf("sockfd: %d\n", sockfd);
		return -1;
	}else{
		printf("Create sockfd success.\n");
	}

	while(1){
		int n=recv(sockfd,packet,PACKET_LEN,0);
		if (n==-1){
			printf("recv error.\n");
			break;
		}else if (n==0){
			continue;
		}
		ip=(IP_HDR*)packet;
		if (ip->protocol==176){
			seanet=(SEANET_HDR*)(packet+sizeof(IP_HDR));
			if (seanet->next==1){
				seadp=(SEADP_HDR*)(packet+sizeof(IP_HDR)+sizeof(SEANET_HDR));
				if ((ntohs(seadp->dstPort)==port)&&(((seadp->pflag)>>5)&0x01==1)){
					printf("A ret for chunk: ");
					for (int i = 0; i < 20; i++){
						printf("%02x", seanet->dstEid[i]);
					}
					printf("\n");
					int id=ntohs(seadp->id);
					enquene(quene,id);
				}
			}
		}
	}

	return (void *)0;
}

void *sender(struct para_sender *parameter){

	IP_HDR *rcv_ip=(IP_HDR*)parameter->packet;
	SEANET_HDR *rcv_seanet=(SEANET_HDR*)(parameter->packet+sizeof(IP_HDR));
	SEADP_HDR *rcv_seadp=(SEADP_HDR*)(parameter->packet+sizeof(IP_HDR)+sizeof(SEANET_HDR));
    
    int packet_num=(int)((parameter->data_len+PAYLOAD_LEN-1)/PAYLOAD_LEN);
    quene=create_quene();
    for(int i=1;i<=packet_num;i++){
    	enquene(quene,i);
    }
    //traverse_quene(quene);

    pthread_t t_reciever;
    pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);

	int ret_create=pthread_create(&t_reciever,&attr,reciever,parameter->port);
	if (ret_create!=0){
		printf("Create thread failed.\n");
	}else{
		printf("Create thread success.\n");
	}



    int sockfd;
	struct sockaddr_in dst;
	unsigned char packet[PACKET_LEN];
	memset(packet,0,PACKET_LEN);

	sockfd=socket(AF_INET,SOCK_RAW,IPPROTO_RAW);
	if (sockfd<0){
		printf("sockfd: %d\n", sockfd);
		return -1;
	}
	dst.sin_family =AF_INET;
	dst.sin_addr.s_addr = rcv_ip->srcAddr;
	int one=1;
	if (setsockopt(sockfd,IPPROTO_IP,IP_HDRINCL,&one,sizeof(one))<0){
		perror("setsockopt failed.\n");
		return -1;
	}

	IP_HDR *ip=(IP_HDR*)packet;
	ip->version = 4; 
    ip->ihl = 5;
    ip->tos = 0;
    ip->len = htons(PACKET_LEN);
    ip->id = htons(1);
    ip->off = htons(0);
    ip->ttl = 10;
    ip->protocol = 176;
    ip->srcAddr = inet_addr(hostIp);
    ip->dstAddr = rcv_ip->srcAddr;
    ip->checksum=checksum(packet,20);

    SEANET_HDR *seanet=(SEANET_HDR*)(packet+sizeof(IP_HDR));
    seanet->next=1;
    seanet->len=44;
    seanet->attr=htons(1);

    for (int i = 0; i < 19; ++i)
    {
    	seanet->srcEid[i]=0xee;
    }
    seanet->srcEid[19]=0x01;
    memcpy(seanet->dstEid,rcv_seanet->srcEid,20);

	SEADP_HDR *seadp=(SEADP_HDR*)(packet+sizeof(IP_HDR)+sizeof(SEANET_HDR));
	seadp->srcPort=htons(parameter->port);
	//printf("rcv_seadp->srcPort: %d\n", ntohs(rcv_seadp->srcPort));
	seadp->dstPort=rcv_seadp->srcPort;
	seadp->pflag=0;
	seadp->cflag=0;
	seadp->tflag=0;
	seadp->reserve=htons(0);
	seadp->total=htonl(41);
	//seadp->off=htonl(0);
	//seadp->id=htons(1);
	//seadp->checksum=checksum((packet+20+44),20);
	while(1){
		if (isEmpty(quene)){
			usleep(10000);
			continue;
		}
		int pn=dequene(quene);
		printf("pn: %d\n", pn);
		seadp->id=htons(pn);
		seadp->off=htonl(pn-1);
		seadp->checksum=checksum((packet+20+44),21);
		memcpy(packet+84,data+pn-1,1);
		
		// printIp(ip);
		// printSeanet(seanet);
		printSeadp(seadp);
		printf("Message: %c\n", (packet+20+44+20)[0]);
		if (seadp->id==5&&flag==1){
			printf("skip id=5\n");
			flag=0;
			continue;
		}
		if (sendto(sockfd,packet,PACKET_LEN,0,(struct sockaddr*)&dst,sizeof(struct sockaddr))<0){
			perror("send failed.");
			return -1;
		}
	}
	free(parameter->packet);
	parameter->packet=NULL;
	free(parameter);
	parameter=NULL;
	return (void *)0;
}


int main(){
	int port=10000;

	int server_sockfd;
	IP_HDR *ip;
	SEANET_HDR *seanet;
	SEADP_HDR *seadp;
	unsigned char packet[PACKET_LEN];
	memset(packet,0,PACKET_LEN);

	server_sockfd=socket(PF_PACKET,SOCK_DGRAM,htons(ETH_P_IP));
	if (server_sockfd<0){
		printf("server_sockfd: %d\n", server_sockfd);
		return -1;
	}else{
		printf("Create server_sockfd success.\n");
	}
	printf("Start to recieve seadp packet.\n");
	while(1){
		int n=recv(server_sockfd,packet,PACKET_LEN,0);
		if (n==-1){
			printf("recv error.\n");
			break;
		}else if (n==0){
			continue;
		}
		ip=(IP_HDR*)packet;
		if (ip->protocol==176){
			seanet=(SEANET_HDR*)(packet+sizeof(IP_HDR));
			if (seanet->next==1){
				//printf("Recieve a seadp packet\n");
				seadp=(SEADP_HDR*)(packet+sizeof(IP_HDR)+sizeof(SEANET_HDR));
				if ((ntohs(seadp->dstPort)==hostSeaPort)&&(((seadp->pflag)>>6)&0x01==1)){
					printf("A request for chunk: ");
					for (int i = 0; i < 20; i++){
						printf("%02x", seanet->dstEid[i]);
					}
					printf("\n");

					unsigned char *packet_copy=(unsigned char *)malloc(sizeof(packet));
					if (packet_copy==NULL)
					{
						printf("malloc wrong\n");
					}
					memcpy(packet_copy,packet,PACKET_LEN);
					
					pthread_t t_handler;
					struct para_sender *parameter=(struct para_sender*)malloc(sizeof(struct para_sender));
					parameter->packet=packet_copy;
					parameter->packet_len=PACKET_LEN;
					parameter->data=data;
					parameter->data_len=sizeof(data);
					parameter->port=port++;

					pthread_attr_t attr;
					pthread_attr_init(&attr);
					pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);

					int ret_create=pthread_create(&t_handler,&attr,sender,parameter);
					if (ret_create!=0){
						printf("Create thread failed.\n");
					}else{
						printf("Create thread success.\n");
					}


				}
			}
		}
	}
	close(server_sockfd);
	return 0;
}