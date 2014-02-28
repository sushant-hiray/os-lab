#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <list>
#include <iostream>


#define QUEUESIZE 20
#define MSGLEN 10
#define NUM_THREADS 4
#define FILESIZE 20

using namespace std;


typedef struct{
	int tid;
	char msg[MSGLEN];
} q_inp;

list<q_inp> Msg_List;

typedef struct {
	list<q_inp>* msg_list;
	int cur_size;
	//long head, tail;
	int full, empty;
	pthread_mutex_t *mut;
	pthread_cond_t *notFull, *notEmpty;
} queue;

typedef struct {
	int tid;
	char filename[FILESIZE];
	queue* buffer;
} p_input;

queue *queueInit (){
	queue *q;

	q = (queue *)malloc (sizeof (queue));
	q->msg_list = &Msg_List;
	if (q == NULL) return (NULL);

	q->empty = 1;
	q->full = 0;
	q->cur_size = 0;
	// q->head = 0;
	// q->tail = 0;
	q->mut = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
	pthread_mutex_init (q->mut, NULL);
	q->notFull = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
	pthread_cond_init (q->notFull, NULL);
	q->notEmpty = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
	pthread_cond_init (q->notEmpty, NULL);
	
	return (q);
}

void queueAdd (queue *q, q_inp inp){
	cout<<"fdsfcsd\n";
	//list<q_inp> l ;
	if(q->cur_size < QUEUESIZE){
		//cout<<inp.msg<<endl;
		//l.push_back(inp);
		//cout<<inp.msg<<endl;
		(q->msg_list)->push_back(inp);

		q->cur_size++;
		if(q->cur_size == QUEUESIZE){
			q->full = 1;
		}
		q->empty = 0;
	}
	return;
}

char* queueRead(queue* q, int id){
	char msg[MSGLEN];
	list<q_inp>::iterator it = (q->msg_list)->begin();
	for(;it!=(q->msg_list)->end();it++){
		if((*it).tid == id){
			//copy the msg
			//msg = new char(MSGLEN);;
			
			strncpy(msg, (*it).msg, MSGLEN);
			//delete the msg
			(q->msg_list)->erase(it);
			q->cur_size--;
			q->full = 0;
			break;
		}
	}
	//
	if(q->cur_size == 0){
		q->empty = 1;
	}
	return msg;
}

// void queueDel (queue *q, list<q_inp>::iterator it){
// 	if(it!=q->msg_list.end()){
// 		q->msg_list.erase(it);
// 	}
// 	return;
// }


void* USERS(void* inp){

	p_input * input = (p_input*)inp;
	// pthread_mutex_lock ((input->buffer)->mut);
	// cout<<"hellow " << input->tid<<endl;
	// pthread_mutex_unlock ((input->buffer)->mut);
	while(0){
		//read froom file


		if(/*send*/0){
			pthread_mutex_lock ((input->buffer)->mut);
			while((input->buffer)->full){
				printf ("producer: queue FULL.\n");
				pthread_cond_wait ((input->buffer)->notFull, (input->buffer)->mut);
			}
			q_inp q;
			queueAdd (input->buffer, q);
			cout<<"hellow " << input->filename<<endl;
			pthread_mutex_unlock ((input->buffer)->mut);
			pthread_cond_signal ((input->buffer)->notEmpty);
			usleep (100000);
		}
		else if(/*recieve*/0){

		}
	}

	pthread_mutex_lock ((input->buffer)->mut);
	cout<<"hellow " << input->filename<<endl;
	pthread_mutex_unlock ((input->buffer)->mut);
}


int main(){
	queue* FCFS;
	char a[MSGLEN] = "012345678";
	FCFS = queueInit();
	if (FCFS ==  NULL) {
		fprintf (stderr, "main: Queue Init failed.\n");
		exit (1);
	}

	// q_inp inp;// = {1,"0123456789"};
	// inp.tid =1;
	//inp.msg = "012345678";
	


	// strncpy(inp.msg,"012345678",MSGLEN);
	//l.push_back(inp);

	// queueAdd(FCFS, inp);
	// cout<<FCFS->cur_size<<endl;
	// strncpy(a,queueRead(FCFS,1),MSGLEN);
	//cout<<a<<" "<< FCFS->full <<endl;
	//char* msg = queueRead(FCFS,1);
	//cout<<msg<<endl;
	//msg = queueRead(FCFS,1);
	//cout<<msg<<endl;
	pthread_t users[NUM_THREADS];
	char files[NUM_THREADS][FILESIZE];
	strncpy(files[0], "1.txt", FILESIZE);
	strncpy(files[1], "2.txt", FILESIZE);
	strncpy(files[2], "3.txt", FILESIZE);
	strncpy(files[3], "4.txt", FILESIZE);

	for(int i =0 ;i<NUM_THREADS ;++i){
		p_input* p;
		p = (p_input*)malloc(sizeof(p_input));
		p->buffer = FCFS;
		p->tid = i;
		strncpy(p->filename, files[i], MSGLEN);
		pthread_create(&users[i],NULL,USERS,p);
		//cout<<p.filename<<endl;
	}

	for(int i =0 ;i<NUM_THREADS ;++i){
		pthread_join (users[i], NULL);
	}

	return 0;
}