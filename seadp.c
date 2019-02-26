#include "seadp.h"

// TODO: padding 0x00 when buffer's 'length is odd
unsigned short checksum(unsigned char *buffer, int size){
	unsigned long sum = 0;
	unsigned short answer;
	unsigned short *temp;
	temp = (short *)buffer;
	for( ; temp<buffer+size; temp++){
		sum += *temp;
	}
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	answer = ~sum;
	return answer;
}

void ip2str(char *ipaddr,int size, uint32_t ip){
	snprintf(ipaddr,size,"%d.%d.%d.%d",(ip>>24)&0xff,(ip>>16)&0xff,(ip>>8)&0xff,ip&0xff);
}



void printIp(IP_HDR *ip){
	printf("##### IP HEADER #####\n");
	printf("# version: %x\n", ip->version);
	printf("# ihl: %x\n",ip->ihl);
	printf("# tos: %02x\n",ip->tos);
	printf("# len: %04x\n",ip->len);
	printf("# id: %04x\n",ip->id);
	printf("# off: %04x\n",ip->off);
	printf("# ttl: %02x\n",ip->ttl);
	printf("# protocol: %02x\n",ip->protocol);
	printf("# checksum: %04x\n",ip->checksum);
	printf("# srcAddr: %08x\n",ip->srcAddr);
	printf("# dstAddr: %08x\n",ip->dstAddr);
	printf("#####################\n\n");
}

void printSeanet(SEANET_HDR *seanet){
	printf("##### SEANET HEADER #####\n");
	printf("# next: %02x\n", seanet->next);
	printf("# len: %02x\n",seanet->len);
	printf("# attr: %04x\n",seanet->attr);
	printf("# srcEid: ");
	for (int i = 0; i < 20; ++i){
		printf("%02x",seanet->srcEid[i]);
	}
	printf("\n# dstEid: ");
	for (int i = 0; i < 20; ++i){
		printf("%02x",seanet->dstEid[i]);
	}
	printf("\n#########################\n\n");
}

void printSeadp(SEADP_HDR *seadp){
	printf("##### SEADP HEADER #####\n");
	printf("# srcPort: %04x\n", seadp->srcPort);
	printf("# dstPort: %04x\n",seadp->dstPort);
	printf("# pflag: %02x\n",seadp->pflag);
	printf("# cflag: %02x\n",seadp->cflag);
	printf("# tflag: %x\n",seadp->tflag);
	printf("# reserve: %03x\n",seadp->reserve);
	printf("# total: %08x\n",seadp->total);
	printf("# off: %08x\n",seadp->off);
	printf("# id: %04x\n",seadp->id);
	printf("# checksum: %04x\n",seadp->checksum);
	printf("########################\n\n");


}