#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <list>
#include <iostream>
#include <fstream>

#define BUFFERSIZE 20

#define MSGLEN 10
#define NUM_THREADS 4
#define FILESIZE 20
#define QUEUESIZE (BUFFERSIZE + 1)
using namespace std;

typedef struct{
	int tid;
	int sid;
	char msg[MSGLEN];
} q_inp;

list<q_inp> Msg_List[NUM_THREADS];

typedef struct {
	list<q_inp>* msg_list[NUM_THREADS];
	int cur_size;
	int full, empty;
	int no_blocked, no_running;
	pthread_mutex_t *mut;
	pthread_cond_t *notFull;//, *notEmpty;
	pthread_cond_t* notEmpty[NUM_THREADS];
	char th_blocked_status[NUM_THREADS];		//'r' receive, 's' send, 'n' none
} queue;

typedef struct {
	int tid;
	char filename[FILESIZE];
	queue* buffer;
} p_input;

struct message{
	int type; //send = 0 receive = 1
	int receiver;
	char msg[MSGLEN];
};

queue *queueInit (){
	queue *q;

	q = (queue *)malloc (sizeof (queue));
	for(int i=0;i<NUM_THREADS;i++){
		q->msg_list[i] = &Msg_List[i];
	}
	
	if (q == NULL) return (NULL);

	q->empty = 1;
	q->full = 0;
	q->cur_size = 0;
	q->no_running = 0;
	q->no_blocked = 0;
	
	q->mut = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
	pthread_mutex_init (q->mut, NULL);
	for(int i=0;i<NUM_THREADS;i++){
		
		q->notEmpty[i] = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
		pthread_cond_init (q->notEmpty[i], NULL);
		q->th_blocked_status[i] = 'n';
	}
	
	q->notFull = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
	pthread_cond_init (q->notFull, NULL);
	
	return (q);
}

void queueDelete (queue *q)
{
	pthread_mutex_destroy (q->mut);
	free (q->mut);
	for(int i=0;i<NUM_THREADS;i++){
			
		pthread_cond_destroy((q->notEmpty)[i]);
		free ((q->notEmpty)[i]);
	}
	pthread_cond_destroy (q->notFull);
	free (q->notFull);
	free (q);
}

//true if queue is full
void queueAdd (queue *q, q_inp inp, int i){

	if(q->cur_size < QUEUESIZE){
		(q->msg_list)[i]->push_back(inp);

		q->cur_size++;
		if(q->cur_size == QUEUESIZE){
			q->full = 1;
		}
		q->empty = 0;
	}
}

q_inp queueRead(queue* q, int i){
	char msg[MSGLEN];
	list<q_inp>::iterator it = (q->msg_list)[i]->begin();

	if(it!= (q->msg_list)[i]->end()){
		q->cur_size--;
		q->full = 0;
		(q->msg_list)[i]->erase(it);
		if(q->cur_size == 0){
			q->empty = 1;
		}
		return *it;
	}
	else{
		cout<<"[Thread "<< i << "]List empty for "<<endl;
		q_inp pq;
		strncpy(pq.msg,"",MSGLEN);
		return pq;
	}
}

void check_deadlock(p_input * input){
	if((input->buffer->no_blocked == input->buffer->no_running) && (input->buffer->no_running!=0)){
		for(int i=0;i<NUM_THREADS;i++){
			if(input->buffer->th_blocked_status[i] == 'r'){
				if(input->buffer->msg_list[i]->empty()){
					continue;
				}
				else{
					return;
				}
			}
		}
		printf("[Thread %d] Deadlock Detected\n",input->tid);
		exit(0);
	}

}
void* USERS(void* inp){

	p_input * input = (p_input*)inp;

	  string line;
	  ifstream myfile (input->filename);
	  if (myfile.is_open())
	  {
	  	pthread_mutex_lock ((input->buffer)->mut);
		(input->buffer->no_running)++;
		pthread_mutex_unlock ((input->buffer)->mut);
	    while ( getline (myfile,line) )
	    {
	    		string delimiter = " ";
	    		string token = line.substr(0, line.find(delimiter));
		      	message m;
		      	char* send = "send";
		      	char* receive = "receive" ;
		      	int temp=0;
		      	if(strcmp(token.c_str(),send)==0){
		      		m.type = 0;
		      		token = line.substr(line.find(delimiter)+1,strlen(line.c_str()));
		      		m.receiver = atoi(token.substr(0,token.find(delimiter)).c_str());
		      		token = token.substr(token.find(delimiter)+ 1, strlen(token.c_str()));
		      		strcpy(m.msg, token.c_str());

		      		pthread_mutex_lock ((input->buffer)->mut);
		      		
		      		while((input->buffer)->full){
		      			printf ("[Thread %d] Sender: queue FULL.\n",input->tid);
		      			(input->buffer->no_blocked)++;
		      			temp=1;
		      			input->buffer->th_blocked_status[input->tid] = 's';
		      			check_deadlock(input);
		      			pthread_cond_wait ((input->buffer)->notFull, (input->buffer)->mut);

		      		}
		      		if(temp!=0){
		      			(input->buffer->no_blocked)--;
		      			temp=0;
		      		}
		      		input->buffer->th_blocked_status[input->tid] = 'n';
		      		q_inp add;
		      		add.tid = m.receiver;
		      		add.sid = input->tid;
		      		strncpy(add.msg, m.msg, MSGLEN);

		      		queueAdd (input->buffer, add, m.receiver);
		      		cout<<"[Thread "<< input->tid << "] Message sent: " << input->tid << " " << m.receiver << " "<< m.msg<<endl;
		      		pthread_cond_signal ((input->buffer->notEmpty)[m.receiver]);
		      		pthread_mutex_unlock ((input->buffer)->mut);
		      		
		      		usleep (100000);	
		      	}

		      	else if(strcmp(token.c_str(),receive)==0){
		      		m.type = 1;
		      		m.receiver = -1;
		      		pthread_mutex_lock ((input->buffer)->mut);
		      		while((input->buffer->msg_list[input->tid])->empty()){
		      			printf ("[Thread %d] reciever: queue Empty.\n", input->tid);
		      			(input->buffer->no_blocked)++;
		      			input->buffer->th_blocked_status[input->tid] = 'r';
		      			temp=1;
		      			check_deadlock(input);
		      			// cout<<"No of blocked threads: "<< input->buffer->no_blocked<<endl;
		      			// cout<<"No of no_running threads: "<< input->buffer->no_running<<endl;
		      			pthread_cond_wait ((input->buffer->notEmpty)[input->tid], (input->buffer)->mut);
		      		}
		      		if(temp!=0){
		      			(input->buffer->no_blocked)--;
		      			temp=0;
		      		}
		      		input->buffer->th_blocked_status[input->tid] = 'n';
		      		q_inp rec = queueRead (input->buffer, input->tid);
		      		cout<<"[Thread " << input->tid << "] Message received: " << rec.sid << " " << input->tid << " "<< rec.msg<<endl;
		      		pthread_cond_signal (input->buffer->notFull);
		      		pthread_mutex_unlock (input->buffer->mut);
		      		usleep (50000);
		      	}

	    }
	    myfile.close();
	    pthread_mutex_lock ((input->buffer)->mut);
      	(input->buffer->no_running)--;
      	check_deadlock(input);
      	cout<<"[Thread "<< input->tid <<"] Died"<<endl;
      	pthread_mutex_unlock ((input->buffer)->mut);
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
	}

	for(int i =0 ;i<NUM_THREADS ;++i){
		pthread_join (users[i], NULL);
	}
	queueDelete(FCFS);
	return 0;
}
