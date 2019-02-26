#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>


int main(){
	char *data="12345";
	char *packet=(char*)malloc(3);
	memcpy(packet,data,6);
	printf("%s\n", packet);

}

