#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <list>
#include <iostream>
#include <fstream>


#define QUEUESIZE 20
#define MSGLEN 10
#define NUM_THREADS 4
#define FILESIZE 20

using namespace std;

typedef struct{
	int tsize[NUM_THREADS];
	pthread_mutex_t *mut;
	pthread_cond_t* notEmpty[NUM_THREADS];
} size_info;

typedef struct{
	int tid;
	int sid;
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
	size_info* sinfo;
} p_input;

struct message{
	int type; //send = 0 receive = 1
	int receiver;
	char msg[MSGLEN];
};

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

void queueDelete (queue *q)
{
	pthread_mutex_destroy (q->mut);
	free (q->mut);	
	pthread_cond_destroy (q->notFull);
	free (q->notFull);
	pthread_cond_destroy (q->notEmpty);
	free (q->notEmpty);
	free (q);
}

size_info *sizeinfoInit (){
	size_info *si;
	si = (size_info *)malloc (sizeof (size_info));
	for(int i=0;i<NUM_THREADS;i++){
		si->tsize[i] = 0;
	}
	si->mut = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
	pthread_mutex_init (si->mut, NULL);
	for(int i=0;i<NUM_THREADS;i++){
		si->notEmpty[i] = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
		pthread_cond_init (si->notEmpty[i], NULL);
	}
	
	return (si);
}

void sizeinfoDelete (size_info *si)
{
	pthread_mutex_destroy (si->mut);
	free (si->mut);
	for(int i=0;i<NUM_THREADS;i++){
		pthread_cond_destroy((si->notEmpty)[i]);
		free ((si->notEmpty)[i]);	
	}
	free(si);
}

void queueAdd (queue *q, q_inp inp){

	if(q->cur_size < QUEUESIZE){
		(q->msg_list)->push_back(inp);

		q->cur_size++;
		if(q->cur_size == QUEUESIZE){
			q->full = 1;
		}
		q->empty = 0;
	}
	return;
}

q_inp queueRead(queue* q, int id){
	char msg[MSGLEN];
	list<q_inp>::iterator it = (q->msg_list)->begin();
	for(;it!=(q->msg_list)->end();it++){
		if((*it).tid == id){
			//copy the msg
			//msg = new char(MSGLEN);;
			q->cur_size--;
			q->full = 0;
			(q->msg_list)->erase(it);
			if(q->cur_size == 0){
				q->empty = 1;
			}
			return *it;
		}
	}
	//
	if(q->cur_size == 0){
		q->empty = 1;
	}
	q_inp pq;
	strncpy(pq.msg,"",MSGLEN);
	return pq;
}


void* USERS(void* inp){

	p_input * input = (p_input*)inp;

	  string line;
	  ifstream myfile (input->filename);
	  if (myfile.is_open())
	  {
	    while ( getline (myfile,line) )
	    {
	    		string delimiter = " ";
	    		string token = line.substr(0, line.find(delimiter));
		      	message m;
		      	char* send = "send";
		      	char* receive = "recieve" ;
		      	if(strcmp(token.c_str(),send)==0){
		      		m.type = 0;
		      		token = line.substr(line.find(delimiter)+1,strlen(line.c_str()));
		      		m.receiver = atoi(token.substr(0,token.find(delimiter)).c_str());
		      		token = token.substr(token.find(delimiter)+ 1, strlen(token.c_str()));
		      		strcpy(m.msg, token.c_str());

		      		while((input->buffer)->full){
		      			printf ("sender: queue FULL.\n");
		      			pthread_cond_wait ((input->buffer)->notFull, (input->buffer)->mut);
		      		}
		      		pthread_mutex_lock ((input->buffer)->mut);
		      		pthread_mutex_lock ((input->sinfo)->mut);
		      		q_inp add;
		      		add.tid = m.receiver;
		      		add.sid = input->tid;
		      		strncpy(add.msg, m.msg, MSGLEN);
		      		queueAdd (input->buffer, add);
		      		(input->sinfo->tsize)[m.receiver]+=1; 
		      		cout<<"Message sent: " << input->tid << " " << m.receiver << " "<< m.msg<<endl;
		      		pthread_mutex_unlock ((input->sinfo)->mut);
		      		pthread_mutex_unlock ((input->buffer)->mut);
		      		//pthread_cond_signal ((input->buffer)->notEmpty);
		      		pthread_cond_signal ((input->sinfo->notEmpty)[m.receiver]);
		      		usleep (100000);	
		      	}

		      	else if(strcmp(token.c_str(),receive)==0){
		      		m.type = 1;
		      		m.receiver = -1;
		      		//cout<<"receive\n";
		      		while((input->buffer)->empty){
		      			printf ("[Thread %d]reciever: queue Empty.\n", input->tid);
		      			pthread_cond_wait ((input->sinfo->notEmpty)[input->tid], (input->buffer)->mut);
		      		}
		      		pthread_mutex_lock ((input->buffer)->mut);
		      		pthread_mutex_lock ((input->sinfo)->mut);
		      		q_inp rec = queueRead (input->buffer, input->tid);
		      		(input->sinfo->tsize)[input->tid] -= 1;
		      		cout<<"Message received: " << rec.sid << " " << input->tid << " "<< rec.msg<<endl;
		      		pthread_mutex_unlock ((input->sinfo)->mut);
		      		pthread_mutex_unlock (input->buffer->mut);
		      		pthread_cond_signal (input->buffer->notFull);
		      		//printf ("consumer: recieved %s.\n", rec);
		      		usleep (50000);
		      	}

	    }
	    myfile.close();
	  }
	  else{
	  	pthread_mutex_lock ((input->buffer)->mut);
	  	cout<<"file doesn't exist "<<input->tid<<endl;
	  	pthread_mutex_unlock ((input->buffer)->mut);
	  }

	  return (NULL);
}



int main(){
	queue* FCFS;
	char a[MSGLEN] = "012345678";
	FCFS = queueInit();
	if (FCFS ==  NULL) {
		fprintf (stderr, "main: Queue Init failed.\n");
		exit (1);
	}
	size_info *si = sizeinfoInit();
	if (si ==  NULL) {
		fprintf (stderr, "main: Size Info Init failed.\n");
		exit (1);
	}

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
		p->sinfo = si;
		p->tid = i;
		strncpy(p->filename, files[i], MSGLEN);
		pthread_create(&users[i],NULL,USERS,p);
		//cout<<p.filename<<endl;
	}

	for(int i =0 ;i<NUM_THREADS ;++i){
		pthread_join (users[i], NULL);
	}
	queueDelete(FCFS);
	sizeinfoDelete(si);
	return 0;
}