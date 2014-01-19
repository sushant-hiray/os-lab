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
#include <sys/mman.h>
#define SHM_SIZE 1024  /* make it a 1K shared memory segment */

typedef enum { false, true } bool;
typedef struct{
	char domain[50];
	int pid;
} domainData;

typedef struct{
	char email[100];
} mailid;

typedef struct {
    char email[100];
    char op[20];
}p2c;
typedef struct{
    char msg[100];
    char op[20];
    int index; //if index -1 means email not found
} c2p;

mailid emails[100]; //array of emails

domainData data[100]; //array of domain and child pids

int noDomains;
int shmid;
int noEmails;

p2c * p2cshared;//used to pass info parent->child
c2p * c2pshared;//used to pass info parent->child
void getdomain(char *email, char *domain){
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
		printf("Domain %d %s\n",i,data[i].domain);
		if(comparestr(data[i].domain,domain)){
			check = true;
			return data[i].pid;
		}
	}
	if(!check){
		return -1; 
	}
}

void addtoemaillist(char* email,char* domainname){
	int i;
	printf("email to be added %s\n",email );
	for(i=0;i<noEmails;i++){
		if(strcmp(emails[i].email,email) == 0){
			sprintf(c2pshared->msg, "Child process %s - Email address already exists\n", domainname);
			kill(getppid(),SIGUSR1);
			return;
		}
	}

	mailid addedmail;
	strcpy(addedmail.email, email);
	emails[noEmails]=addedmail;
	noEmails++;
	printf("Added email! Sending signal\n");
	sprintf(c2pshared->msg, "Child process %s - Email address %s added successfully\n", domainname,email);
	strcpy(c2pshared->op,"add_email");
	kill(getppid(),SIGUSR1);
	fflush ( stdout );
}

char* addemail4child(){
    printf("CHILD: I am the child process with pid:%d !\n",getpid());
    char username[100];
    char domainname[100];
    sscanf(p2cshared->email, "%[a-zA-Z0-9]@%s",username, domainname);
    printf("%s : shared data, in process with pid: %d \n",p2cshared->email,getpid());
    return p2cshared->email;
    fflush ( stdout );
}


void addemail4parent(char* mailid,pid_t childpid){
	char domainname[80];
	getdomain(mailid,domainname);
	domainData newdomain;
	strcpy(newdomain.domain,domainname);
	newdomain.pid=childpid;
	data[noDomains]=newdomain;
	noDomains++;
    printf("PARENT: I am the parent process with pid: %d!\n",getpid());  
    printf("No of domains added are: %d\n",noDomains);       
    printf("Recent Domain Added: %s by pid: %d\n",data[noDomains-1].domain,data[noDomains-1].pid );
    sleep(2);
    kill(childpid,SIGUSR1);
    sleep(2);
}

void child_handler(int signum)
{
    if (signum == SIGUSR1)
    {	
        printf("Inside Child Handler!\n");
        char username[100];
        char domainname[100];
        //printf ("child handler : : My PID :%d, Sending PID: %ld, UID: %ld\n",getpid(),
                //(long)siginfo->si_pid, (long)siginfo->si_uid);
        sscanf(p2cshared->email, "%[a-zA-Z0-9]@%s",username, domainname);
        if(strcmp(p2cshared->op, "add_email")==0){
	        char* mail=addemail4child();
			addtoemaillist(mail,domainname);
		}
		else if(strcmp(p2cshared->op, "delete_email")==0){
	        char* mail=addemail4child();
			
		}
		else if(strcmp(p2cshared->op, "search_email")==0){
	        searchemailc();
			
		}
		else if(strcmp(p2cshared->op, "delete_domain")==0){
	        char* mail=addemail4child();
			
		}

        
    }
    fflush ( stdout );
}
void searchemailc(){
	int i=0;
	for(i=0;i<noEmails;i++){
		if(strcmp(emails[i].email,p2cshared->email)==0){
			strcpy(c2pshared->msg,p2cshared->email);
			strcpy(c2pshared->op,"search_email");
			c2pshared->index=i;
			kill(getppid(),SIGUSR1);
			return;
		}

	}
	strcpy(c2pshared->msg,p2cshared->email);
	strcpy(c2pshared->op,"search_email");
	c2pshared->index=-1;
	kill(getppid(),SIGUSR1);
	return;

}
void seachemailp(char* mailid){
	int i;
	char username[100];
	char domainname[100];
	//printf ("child handler : : My PID :%d, Sending PID: %ld, UID: %ld\n",getpid(),
	        //(long)siginfo->si_pid, (long)siginfo->si_uid);
	sscanf(mailid, "%[a-zA-Z0-9]@%s",username, domainname);
	for(i=0;i<noDomains;i++){
		if(strcmp(data[i].domain,domainname)==0){
			strcpy(p2cshared->email,mailid);
			strcpy(p2cshared->op,"search_email");
			sleep(2);
			kill(data[i].pid,SIGUSR1);
			sleep(2);
			return;
		}
	}
	printf("Parent Process: Domain doesnt exist\n");
}
void parent_handler(int signum){
if (signum == SIGUSR1)
    {	
    	if(strcmp(c2pshared->op, "add_email")==0){
	        printf("%s\n", c2pshared->msg);
		}
		else if(strcmp(c2pshared->op, "delete_email")==0){
	        
			
		}
		else if(strcmp(c2pshared->op, "search_email")==0){
			if(c2pshared->index==-1){
				printf("Parent Process: could not find email address %s\n",c2pshared->msg );
			}
			else{
				printf("Parent Process: Found email address %s at index %d\n",c2pshared->msg,c2pshared->index );
			}
	        
			
		}
		else if(strcmp(c2pshared->op, "delete_domain")==0){
	        
			
		}
        printf("Done with the request! Starting Over! \n\nxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n\n");
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
				printf("\n------------------------------------\nAdding a new process\n");


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
						usr_action.sa_handler = child_handler;
						usr_action.sa_mask = block_mask;
						usr_action.sa_flags = 0;
						sigaction (SIGUSR1, &usr_action, NULL);
						while(1){;}
				    	
				    }
					// connect to (and possibly create) the segment: //
			           if(childpid > 0){
			           		sigfillset (&block_mask);
			           		usr_action.sa_handler = parent_handler;
			           		usr_action.sa_mask = block_mask;
			           		usr_action.sa_flags = 0;
			           		sigaction (SIGUSR1, &usr_action, NULL);	
				         
			           		strcpy(p2cshared->email, mailid);
			           		strcpy(p2cshared->op, op);
				            printf("writing to segment: \"%s\"\n", mailid);
				            printf("shared email: %s, shared op: %s\n",p2cshared->email,p2cshared->op);
				    		addemail4parent(mailid,childpid);
				       	}
				   
				}
				else /* fork returns -1 on failure */
				{
				    perror("fork"); /* display error message */
				    exit(0); 
				}
			}



			else{
				printf("\n------------------------------------\nDomain already exists\n");
           		sigfillset (&block_mask);
           		usr_action.sa_handler = parent_handler;
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
	    		printf("PARENT: I am the parent process with pid: %d!\n",getpid());  
	    		printf("No of domains added are: %d\n",noDomains); 
	    		sleep(2);
	    		kill(searchVal,SIGUSR1);
	    		sleep(2);      

			}
		}
		else if(comparestr(op,"delete_email")){
			scanf("%s",mailid);
			printf("Delete email\n");
		}
		else if(comparestr(op,"search_email")){
			printf("Entered searching zone. Enter mailid\n");
			scanf("%s",mailid);
			printf("Searching email....\n");
			seachemailp(mailid);
		}
		else if(comparestr(op,"delete_domain")){
			scanf("%s",mailid);
			printf("delete_domain\n");
		}
		else if(comparestr(op,"Quit")){
			printf("Quit\n");
		}
		else{
			printf("incorrect input: try again");
		}
	    
}

int main()
{
						     
   	// Establish the signal handler.
   	p2cshared = mmap(NULL, sizeof(p2c), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    c2pshared = mmap(NULL, sizeof(c2p), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);

   	noDomains = 0;
   	while(1){run();}
   	return 0;
}



