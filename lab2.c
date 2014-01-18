#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */ 
#include <errno.h>      /* Errors */
#include <stdio.h>      /* Input/Output */
#include <sys/wait.h>   /* Wait for Process Termination */
#include <stdlib.h>     /* General Utilities */
#include <signal.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define SHM_SIZE 1024  /* make it a 1K shared memory segment */

typedef enum { false, true } bool;
typedef struct{
	char* domain;
	int pid;
} domainData;

typedef struct{
	char* email;
} mailid;

mailid emails[100]; //array of emails

domainData data[100]; //array of domain and child pids
int noDomains;
int shmid;
int noEmails;
volatile sig_atomic_t usr_interrupt = 0;

void getdomain(char email[], char domain[]){
    char *p;
    p = strtok(email, "@");

    if(p){
        p = strtok(NULL, "@");       
        if(p){
            strcpy(domain,p);
        }
    }
}

int comparestr(char a[], char b[]){
    int sa =strlen(a);
    int sb =strlen(b);
    if (sa!=sb){
        return 0; // They must be different
    }
    int i=0;
    for (i; i < sa; i++)    {
        if (a[i] != b[i])
            return 0;  // They are different
    }
    return 1;  // They must be the same
}



int searchDomain(char domain[]){ // -1 implies not found
	bool check=false;
	int i;
	printf("In search Domain, no of Domains=%d\n",noDomains );
	for(i=0;i<noDomains;i++){
		if(comparestr(data[i].domain,domain)){
			check = true;
			return data[i].pid;
		}
	}
	if(!check){
		return -1; 
	}
}

void addtoemaillist(char* email){
	int i;
	printf("email to be added %s\n",email );
	for(i=0;i<noEmails;i++){
		if(strcmp(emails[i].email,email)){
			printf("Email is already present\n");
			return;
		}
	}

	mailid addedmail;
	addedmail.email=email;
	emails[noEmails]=addedmail;
	noEmails++;
	printf("Added email! Sending signal\n");
	kill(getppid(),SIGUSR1);
}

char* addemail4child(){
	while (!usr_interrupt){;} 
    printf("CHILD: I am the child process with pid:%d !\n",getpid());
   	char* masala = malloc(1000*sizeof(char));
   	int id;
   	if ((id = shmget(ftok(".",'s'), SHM_SIZE, IPC_CREAT|0644)) == -1) {
       perror("shmget");
       exit(1);
   }
    // attach to the segment to get a pointer to it: //
    
    masala = shmat(id, (void *)0, 0);
    if (masala == (char *)(-1)) {
        perror("shmat");
        exit(1);
    }
    printf("%s : shared data, in process with pid: %d \n",masala,getpid());
    return masala;
    fflush ( stdout );
}


void addemail4parent(char* mailid,pid_t childpid){
	char domainname[80];
	getdomain(mailid,domainname);
	domainData newdomain;
	newdomain.domain=domainname;
	newdomain.pid=childpid;
	data[noDomains]=newdomain;
	noDomains++;
    printf("PARENT: I am the parent process with pid: %d!\n",getpid());  
    printf("No of domains added are: %d\n",noDomains);       
    printf("Recent Domain Added: %s by pid: %d\n",data[noDomains-1].domain,data[noDomains-1].pid );
    kill(childpid,SIGUSR1);
}

void add(int signum)
{
    if (signum == SIGUSR1)
    {	
    	usr_interrupt = 1;
        printf("Inside Add!\n");
        char* mail=addemail4child();
		addtoemaillist(mail);
        
    }
    fflush ( stdout );
}

void recvsignal(int signum){
if (signum == SIGUSR1)
    {	
    	usr_interrupt = 1;
        printf("Inside RecvSignal!\n");
        run();
        
    }
    fflush ( stdout );
}

void run(){
	char domainname[80];
	char op[20],mailid[80],temp[80];
	char* add_email="add_email";
	struct sigaction usr_action;
	sigset_t block_mask;

	printf("Enter op: ");
	scanf("%s",op);
		if(comparestr(op,"add_email")){
			printf("Entered addition zone. Enter mailid\n");
			scanf("%s",mailid);
			strcpy(temp,mailid);
			printf("Adding a new email %s\n",mailid);
			getdomain(temp,domainname);
			int searchVal=searchDomain(domainname);
			if(searchVal == -1){	//create new domain
				printf("Adding a new process\n");


				//fork a new process here
				pid_t childpid; /* variable to store the child's pid */
				int retval;     /* child process: user-provided return code */
				int status;     /* parent process: child's exit status */
				
				/* only 1 int variable is needed because each process would have its
				   own instance of the variable
				   here, 2 int variables are used for clarity */
				    
				/* now create new process */
				
				childpid = fork();
				
				if (childpid >= 0) /* fork succeeded */
				{
					if (childpid == 0){ /* fork() returns 0 to the child process */
						noEmails=0;
						sigfillset (&block_mask);
						usr_action.sa_handler = add;
						usr_action.sa_mask = block_mask;
						usr_action.sa_flags = 0;
						sigaction (SIGUSR1, &usr_action, NULL);
						printf("1 \n");
						while(1){;}
				    	
				    }
					// connect to (and possibly create) the segment: //
			           if(childpid > 0){
			           		sigfillset (&block_mask);
			           		usr_action.sa_handler = recvsignal;
			           		usr_action.sa_mask = block_mask;
			           		usr_action.sa_flags = 0;
			           		sigaction (SIGUSR1, &usr_action, NULL);	
				           	if ((shmid = shmget(ftok(".",'s'), SHM_SIZE, IPC_CREAT|0644)) == -1) {
				               perror("shmget");
				               exit(1);
				           }
					        // attach to the segment to get a pointer to it: //
				            char* shareddata = malloc(100*sizeof(char));
				            shareddata = shmat(shmid, (void *)0, 0);
				            if (shareddata == (char *)(-1)) {
				                perror("shmat");
				                exit(1);
				            }
				            //writing to the segment shared memory
				            strncpy(shareddata, mailid, 80*sizeof(char));
				            printf("writing to segment: \"%s\"\n", mailid);
				            printf("%s shared data\n",shareddata);
				    		addemail4parent(mailid,childpid);
				    		//while(1){;}
				       	}
				   
				}
				else /* fork returns -1 on failure */
				{
				    perror("fork"); /* display error message */
				    exit(0); 
				}
			}



			else{
				printf("Domain already exists\n");
           		sigfillset (&block_mask);
           		usr_action.sa_handler = recvsignal;
           		usr_action.sa_mask = block_mask;
           		usr_action.sa_flags = 0;
           		sigaction (SIGUSR1, &usr_action, NULL);	
	           	if ((shmid = shmget(ftok(".",'s'), SHM_SIZE, IPC_CREAT|0644)) == -1) {
	               perror("shmget");
	               exit(1);
	           }
		        // attach to the segment to get a pointer to it: //
	            char* shareddata = malloc(100*sizeof(char));
	            shareddata = shmat(shmid, (void *)0, 0);
	            if (shareddata == (char *)(-1)) {
	                perror("shmat");
	                exit(1);
	            }
	            //writing to the segment shared memory
	            strncpy(shareddata, mailid, 80*sizeof(char));
	            printf("writing to segment: \"%s\"\n", mailid);
	            printf("%s shared data\n",shareddata);
	    		addemail4parent(mailid,searchVal);
	    		while(1){;}

			}
		}
		else if(comparestr(op,'delete_email')){
			scanf("%s",mailid);
			printf("Delete email\n");
		}
		else if(comparestr(op,'search_email')){
			scanf("%s",mailid);
			printf("Search email\n");
		}
		else if(comparestr(op,'delete_domain')){
			scanf("%s",mailid);
			printf("delete_domain\n");
		}
		else if(comparestr(op,'Quit')){
			printf("Quit\n");
		}
		else{
			printf("incorrect input: try again");
		}
	    
}

int main()
{
						     
   	// Establish the signal handler.
   	noDomains = 0;
   	while(1){run();}
   	
#if 0
	while(true){
		printf("Enter op\n");
		scanf("%s",op);
			if(comparestr(op,"add_email")){
				printf("Entered addition zone. Enter mailid\n");
				scanf("%s",mailid);
				printf("Adding a new email %s\n",mailid);
				int searchVal=searchDomain(mailid);
				if(searchVal == -1){	//create new domain
					printf("Adding a new process\n");


					//fork a new process here
					pid_t childpid; /* variable to store the child's pid */
					int retval;     /* child process: user-provided return code */
					int status;     /* parent process: child's exit status */
					
					/* only 1 int variable is needed because each process would have its
					   own instance of the variable
					   here, 2 int variables are used for clarity */
					    
					/* now create new process */
					
					childpid = fork();
					
					if (childpid >= 0) /* fork succeeded */
					{
						if (childpid == 0){ /* fork() returns 0 to the child process */
							noEmails=0;
							sigfillset (&block_mask);
							usr_action.sa_handler = add;
							usr_action.sa_mask = block_mask;
							usr_action.sa_flags = 0;
							sigaction (SIGUSR1, &usr_action, NULL);	
					    	while(1){;}
					    	
					    }
						// connect to (and possibly create) the segment: //
				           if(childpid > 0){
				           		sigfillset (&block_mask);
				           		usr_action.sa_handler = recvsignal;
				           		usr_action.sa_mask = block_mask;
				           		usr_action.sa_flags = 0;
				           		sigaction (SIGUSR1, &usr_action, NULL);	
					           	if ((shmid = shmget(ftok(".",'s'), SHM_SIZE, IPC_CREAT|0644)) == -1) {
					               perror("shmget");
					               exit(1);
					           }
						        // attach to the segment to get a pointer to it: //
					            char* shareddata = malloc(100*sizeof(char));
					            shareddata = shmat(shmid, (void *)0, 0);
					            if (shareddata == (char *)(-1)) {
					                perror("shmat");
					                exit(1);
					            }
					            //writing to the segment shared memory
					            strncpy(shareddata, mailid, SHM_SIZE);
					            printf("writing to segment: \"%s\"\n", mailid);
					            printf("%s shared data\n",shareddata);
					    		addemail4parent(mailid,childpid);
					    		while(1){;}
					       	}


					    

					   
					}
					else /* fork returns -1 on failure */
					{
					    perror("fork"); /* display error message */
					    exit(0); 
					}
				}
				break;
			}
			else if(comparestr(op,'delete_email')){
				scanf("%s",mailid);
				printf("Delete email\n");
				break;
			}
			else if(comparestr(op,'search_email')){
				scanf("%s",mailid);
				printf("Search email\n");
				break;
			}
			else if(comparestr(op,'delete_domain')){
				scanf("%s",mailid);
				printf("delete_domain\n");
				break;
			}
			else if(comparestr(op,'Quit')){
				printf("Quit\n");
				break;
			}
		
		return 0;
	    
    }
    #endif
    return 0;
}



