#include "thread_safe_quene.h"



TS_QUENE *create_quene(){
	TS_QUENE *quene=(TS_QUENE *)malloc(sizeof(TS_QUENE));
	if (quene!=NULL){
		quene->head=NULL;
		quene->rear=NULL;
		quene->size=0;
		if (sem_init(&quene->sem_mutex,0,1)!=0){
			perror("init sem_mutex failed");
			return NULL;
		}
	}else{
		perror("ts quene malloc failed.\n");
		return NULL;
	}
	return quene;
}

int destroy_quene(TS_QUENE *quene){
	if (!quene){
		printf("ts quene is already NULL.\n");
		return 0;
	}
	clear_quene(quene);
	sem_destroy(&quene->sem_mutex);
	free(quene);
	quene=NULL;
	return 1;
}

void clear_quene(TS_QUENE *quene){
	while(!isEmpty(quene)){
		dequene(quene);
	}
}

int isEmpty(TS_QUENE *quene){
	if (quene->size==0){
		return 1;
	}else{
		return 0;
	}
}

void enquene(TS_QUENE *quene,DATA data){
	NODE *node=(NODE *)malloc(sizeof(NODE));
	if (node==NULL){
		perror("node malloc failed.\n");
		return;
	}
	sem_wait(&quene->sem_mutex);
	node->data=data;
	node->next=NULL;
	if (isEmpty(quene)){
		quene->head=node;
	}else{
		quene->rear->next=node;
	}
	quene->rear=node;
	quene->size++;
	sem_post(&quene->sem_mutex);
	return;
}

DATA dequene(TS_QUENE *quene){
	if (isEmpty(quene)){
		printf("quene is empty.\n");
		return NULL;
	}
	sem_wait(&quene->sem_mutex);
	NODE *node=quene->head;
	DATA data=node->data;
	quene->head = node->next;
	if (quene->rear==node){
		quene->rear=quene->head;
	}
	quene->size--;
	free(node);
	node=NULL;
	sem_post(&quene->sem_mutex);
	return data;
}

void traverse_quene(TS_QUENE *quene){
	if (isEmpty(quene)){
		printf("the quene is empty\n");
		return;
	}
	NODE *node=quene->head;
	printf("the quene is: ");
	while(node!=NULL){
		printf("%d ", node->data);
		node=node->next;
	}
	printf("\n");
}


// int main(){

// 	TS_QUENE *quene=create_quene();
// 	printf("%d\n", quene->size);
// 	printf("isEmpty: %d\n", isEmpty(quene));
// 	printf("quene size: %d\n", quene->size);
// 	traverse_quene(quene);
	
// 	enquene(quene,1);
// 	traverse_quene(quene);
// 	enquene(quene,2);
// 	traverse_quene(quene);
// 	enquene(quene,3);
// 	traverse_quene(quene);
// 	// dequene(quene);
// 	// traverse_quene(quene);
// 	// dequene(quene);
// 	// traverse_quene(quene);
// 	// dequene(quene);
// 	// traverse_quene(quene);
// 	destroy_quene(quene);
	
// }