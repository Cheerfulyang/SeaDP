#ifndef SEADP_H
#define SEADP_H 1

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <memory.h> 
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <pthread.h>
#include <semaphore.h>

#define PACKET_LEN 85
#define PAYLOAD_LEN 1

typedef struct{
	unsigned char ihl:4,version:4;
	unsigned char tos;
	unsigned short len;
	unsigned short id;
	unsigned short off;
	unsigned char ttl;
	unsigned char protocol;
	unsigned short checksum;
	unsigned int srcAddr;
	unsigned int dstAddr;

}IP_HDR;

typedef struct{
	unsigned char next;
	unsigned char len;
	unsigned short attr;
	unsigned char srcEid[20];
	unsigned char dstEid[20];
}SEANET_HDR;

typedef struct{
	unsigned short srcPort;
	unsigned short dstPort;
	unsigned char pflag;
	unsigned char cflag;
	unsigned short tflag:4,reserve:12;
	unsigned int total;
	unsigned int off;

	unsigned short id;
	unsigned short checksum;
	
}SEADP_HDR;


unsigned short checksum(unsigned char *buffer, int size);

void printIp(IP_HDR *ip);
void printSeanet(SEANET_HDR *seanet);
void printSeadp(SEADP_HDR *seadp);


#endif