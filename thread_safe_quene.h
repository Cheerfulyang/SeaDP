#ifndef THREAD_SAFE_QUNEN
#define THREAD_SAFE_QUNEN 1

#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>

typedef int DATA;//PN

typedef struct Node{
	DATA data;
	struct Node *next;
}NODE;

typedef struct{
	int size;
	sem_t sem_mutex;
	NODE *head;
	NODE *rear;
}TS_QUENE;



TS_QUENE *create_quene();

int destroy_quene(TS_QUENE *quene);

void clear_quene(TS_QUENE *quene);

int isEmpty(TS_QUENE *quene);

void enquene(TS_QUENE *quene,DATA data);

DATA dequene(TS_QUENE *quene);

void traverse_quene(TS_QUENE *quene);

#endif