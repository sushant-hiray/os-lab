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


void child_handler_kill(int sig, siginfo_t *siginfo, void *context){
    printf("Killing myself : domain %s .. printing emails\n", p2cshared->email);
    int i;
    for(i=0;i<noEmails;i++){
            printf("\tEmail : %s\n", emails[i].email);
    }
    raise(SIGKILL);
}

int searchDomain(char* domain){ // -1 implies not found
	bool check=false;
	int i;
	for(i=0;i<noDomains;i++){
		if(strcmp(data[i].domain,domain)==0){
			check = true;
			return i;
		}
	}
	if(!check){
		return -1; 
	}
}

void addtoemaillist(char* email,char* domainname){
	int i;
	for(i=0;i<noEmails;i++){
		if(strcmp(emails[i].email,email) == 0){
			sprintf(c2pshared->msg, "Child process %s - Email address already exists\n", domainname);
			strcpy(c2pshared->op,"add_email");
			sleep(1);
			kill(getppid(),SIGUSR2);
			sleep(1);
			return;
		}
	}

	mailid addedmail;
	strcpy(addedmail.email, email);
	emails[noEmails]=addedmail;
	noEmails++;
	sprintf(c2pshared->msg, "Child process %s - Email address %s added successfully\n", domainname,email);
	strcpy(c2pshared->op,"add_email");
	kill(getppid(),SIGUSR2);
	fflush ( stdout );
}

char* addemail4child(){
    char username[100];
    char domainname[100];
    sscanf(p2cshared->email, "%[a-zA-Z0-9]@%s",username, domainname);
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
    sleep(2);
    kill(childpid,SIGUSR1);
    sleep(2);
}
void deletemailc(){
	int i=0;
	for(i=0;i<noEmails;i++){
		if(strcmp(emails[i].email,p2cshared->email)==0){
			strcpy(c2pshared->msg,p2cshared->email);
			strcpy(c2pshared->op,"delete_email");
			c2pshared->index=i;
			emails[i].email[0]='\0';
			kill(getppid(),SIGUSR2);
			return;
		}

	}
	strcpy(c2pshared->msg,p2cshared->email);
	strcpy(c2pshared->op,"delete_email");
	c2pshared->index=-1;
	kill(getppid(),SIGUSR2);
	return;
}

void child_handler(int signum)
{
    if (signum == SIGUSR1)
    {	
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
	        deletemailc();
			
		}
		else if(strcmp(p2cshared->op, "search_email")==0){
	        searchemailc();
			
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
			kill(getppid(),SIGUSR2);
			return;
		}

	}
	strcpy(c2pshared->msg,p2cshared->email);
	strcpy(c2pshared->op,"search_email");
	c2pshared->index=-1;
	kill(getppid(),SIGUSR2);
	return;

}

void operationp(char* mailid,char* op){
	int i;
	char username[100];
	char domainname[100];
	//printf ("child handler : : My PID :%d, Sending PID: %ld, UID: %ld\n",getpid(),
	        //(long)siginfo->si_pid, (long)siginfo->si_uid);
	sscanf(mailid, "%[a-zA-Z0-9]@%s",username, domainname);
	for(i=0;i<noDomains;i++){
		if(strcmp(data[i].domain,domainname)==0){
			strcpy(p2cshared->email,mailid);
			strcpy(p2cshared->op,op);
			sleep(2);
			kill(data[i].pid,SIGUSR1);
			sleep(2);
			return;
		}
	}
	printf("Parent Process: Domain doesnt exist\n");
}
void parent_handler(int signum){
if (signum == SIGUSR2)
    {	
    	if(strcmp(c2pshared->op, "add_email")==0){
	        printf("%s\n", c2pshared->msg);
		}
		else if(strcmp(c2pshared->op, "delete_email")==0){
			char username[100];
			char domainname[100];
			sscanf(c2pshared->msg, "%[a-zA-Z0-9]@%s",username, domainname);
	        if(c2pshared->index==-1){
	        	printf("Parent Process: child %s could not find email address %s\n",domainname,c2pshared->msg );
	        }
	        else{
	        	
	        	printf("Child Process: child %s deleted %s at index %d\n",domainname,c2pshared->msg,c2pshared->index );
	        }
			
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
						
						struct sigaction act2;
						
						memset (&act2, '\0', sizeof(act2));
						
						/* Use the sa_sigaction field because the handles has two additional parameters */
						act2.sa_sigaction = &child_handler_kill;
						
						/* The SA_SIGINFO flag tells sigaction() to use the sa_sigaction field, not sa_handler. */
						act2.sa_flags = SA_SIGINFO;
						
						if (sigaction(SIGTERM, &act2, NULL) < 0) {
						    printf ("sigaction");
						}
						while(1){;}
				    	
				    }
					// connect to (and possibly create) the segment: //
			           if(childpid > 0){
			           		sigfillset (&block_mask);
			           		usr_action.sa_handler = parent_handler;
			           		usr_action.sa_mask = block_mask;
			           		usr_action.sa_flags = 0;
			           		sigaction (SIGUSR2, &usr_action, NULL);	
				         
			           		strcpy(p2cshared->email, mailid);
			           		strcpy(p2cshared->op, op);
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
	           	strcpy(p2cshared->email, mailid);
	           	strcpy(p2cshared->op, op);
	    		sleep(2);
	    		kill(data[searchVal].pid,SIGUSR1);
	    		sleep(2);      

			}
		}
		else if(comparestr(op,"delete_email")){
			printf("Entered deleting zone. Enter mailid\n");
			scanf("%s",mailid);
			operationp(mailid,op);
		}
		else if(comparestr(op,"search_email")){
			printf("Entered searching zone. Enter mailid\n");
			scanf("%s",mailid);
			operationp(mailid,op);
		}
		else if(comparestr(op,"delete_domain")){
			printf("Entered deleting zone. Enter mailid\n");
			scanf("%s",mailid);
			int id=searchDomain(mailid);
			if(id==-1){
				printf("Parent: Domain does not exist\n");
			}
			else{
				kill(data[id].pid,SIGTERM);
				waitpid(data[id].pid, 0, 0); //important for wait so that child doesnt become zombie
				                                    //A child that terminates, but has not been waited for becomes a "zombie". 
				printf("Parent process - Domain %s with PID - %d deleted\n", mailid, data[id].pid);
				data[id].pid = -1;
				data[id].domain[0]='\0';
			}
		}
		else if(comparestr(op,"Quit")){
			printf("Quit\n");
			int i=0;
			for(i=0;i<noDomains;i++){
			    if(data[i].pid != 0){
			         
			         kill(data[i].pid, SIGTERM); 
			         waitpid(data[i].pid, 0, 0); //important for wait so that child doesnt become zombie
			     }
			}
			printf("Parent : Killed all children, now suicide awaits\n");
			//raise(SIGKILL);
			
		}
		else{
			printf("Incorrect input: try again");
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



