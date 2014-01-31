#include <unistd.h>     /* Symbolic Constants */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <signal.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>

#define MAXLINE 1000
#define DEBUG 0
#define MAXPARALLEL 80
#define ALARM_TIME 5

typedef enum { false, true } bool;

typedef struct {
	int min;
	int hr;
	int day;
	int mon;
	int week;
	char** argv;
} cron_data;

cron_data* c_data;
char* cfile="cront.txt";
int row;
volatile sig_atomic_t flag=0;


//declarations
char ** tokenize(char*);
bool analyze(char**);
bool analyze2(char**);
void run(char*);
bool fexists(char*);
void parallel(char**);
void runsource(int[],char**);
void rundest(int[],char**);
int checkredirect(char** ,int* );
void redirect_cmd(char** , char** );

pid_t childpid=-1;

/* 
 Handler for alarm
*/
void crontab_alarm_handler(int signum){
	if(signum==SIGALRM){
		signal(SIGALRM, SIG_IGN);          /* ignore this signal       */
		flag=1;
		signal(SIGALRM, crontab_alarm_handler);     /* reinstall the handler    */
		alarm(ALARM_TIME);
	}
}


/* 
 Handler for Ctrl C
*/
void main_han(int signum){
	if(signum==SIGINT){
		fflush(stdout);
		if(childpid==-1){
			fflush(stdout);
			printf("\n$ ");
			fflush(stdout);
			return;
		}
		else{
			kill(childpid,SIGKILL);
			printf("\n");
			childpid=-1;
			fflush(stdout);
		}	
		return;
	}
}

/* main finction */
int main(int argc, char** argv){

	//Setting the signal interrupt to its default function. 
	signal(SIGINT, main_han);
	//Allocating space to store the previous commands.
	int numCmds = 0;
	char **cmds = (char **)malloc(1000 * sizeof(char *));

	int printDollar = 1;

	char input[MAXLINE];
	char** tokens;

	int notEOF = 1;
	int i;

	FILE* stream = stdin;

	//signal(SIGINT,main_han);
	while(notEOF) { 
		if (printDollar == 1){ 
			fflush(stdout);
			printf("$ "); // the prompt
			fflush(stdout);
			fflush(stdin);
		}

		char *in = fgets(input, MAXLINE, stream); //taking input one line at a time

		//Checking for EOF
		if (in == NULL){
			if (DEBUG) printf("EOF found\n");
			exit(0);
		}

		//add the command to the command list.
		cmds[numCmds] = (char *)malloc(sizeof(input));
		strcpy(cmds[numCmds++], input); 

		// Calling the tokenizer function on the input line    
		tokens = tokenize(input);	
		// Uncomment to print tokens
		for(i=0;tokens[i]!=NULL;i++){
		// 	printf("%s\n", tokens[i]);
		 }
		 if(i!=0){
		 	int pos=0;

		 	int j=checkredirect(tokens,&pos);
			if(j==-1){
             analyze(tokens);
            }
            else if(j==0){
                //piping condition
                char **list1;
                char **list2;
                int tokenNo=0;
 				list1 = (char**)malloc(MAXLINE*sizeof(char**));
 				list2 = (char**)malloc(MAXLINE*sizeof(char**));
 				int k=0;
 				char *token = (char *)malloc(1000*sizeof(char));
 				while(tokens[k]!=NULL){
					if(k<pos){
						list1[tokenNo] = (char*)malloc(sizeof(token));
						strcpy(list1[tokenNo],tokens[k]);
						tokenNo++;
						k++;
					}
					else if(k==pos){
						tokenNo=0;
						k++;
					}
					else{
						list2[tokenNo] = (char*)malloc(sizeof(token));
						strcpy(list2[tokenNo],tokens[k]);
						tokenNo++;
						k++;	
					}
				
				}
				int pid, status; 
				int fd[2]; 
				pipe(fd); 
				runsource(fd,list1);
				rundest(fd,list2); 
				close(fd[0]); 
				close(fd[1]);

				while ((pid = wait(&status)) != -1) 
					/* pick up all the dead children */ 
					fprintf(stderr, "process %d exits with %d\n", pid, WEXITSTATUS(status));
				free(token);
				free(list1);
				free(list2);
            }
            else if(j==1){
            	printf("> \n");

            	//> condition
                char **list1;
                char **list2;
                int tokenNo=0;
 				list1 = (char**)malloc(MAXLINE*sizeof(char**));
 				list2 = (char**)malloc(MAXLINE*sizeof(char**));
 				int k=0;
 				char *token = (char *)malloc(1000*sizeof(char));
 				while(tokens[k]!=NULL){
					if(k<pos){
						list1[tokenNo] = (char*)malloc(sizeof(token));
						strcpy(list1[tokenNo],tokens[k]);
						tokenNo++;
						k++;
					}
					else if(k==pos){
						tokenNo=0;
						k++;
					}
					else{
						list2[tokenNo] = (char*)malloc(sizeof(token));
						strcpy(list2[tokenNo],tokens[k]);
						tokenNo++;
						k++;	
					}
				
				}
				redirect_cmd(list1,list2);

            }
            else if(j==2){
            	printf("< \n");	
            }
            else if(j==3){
            	printf(">> \n");
            }
            else if(j==4){
            	printf("<< \n");
            }
            else if(j==5){
            	printf("& \n");
            }
       }
	}
  
  
	printf("Print and deallocate %s\n", tokens[0]);
	// Freeing the allocated memory	
	for(i=0;tokens[i]!=NULL;i++){
		free(tokens[i]);
	}
	free(tokens);
	return 0;
}

void printlist(char** a){
	int i=0;
	while(a[i]!=NULL){
		printf("%s ",a[i]);
		i++;
	}
	printf("\n");
}

void parallel(char** input){
	int tokenIndex = 0;
	int tokenNo = 0;
	int i,status;
	pid_t pid;
	char *token = (char *)malloc(1000*sizeof(char));
	for(i=0;input[i]!=NULL;i++){
		if(strcmp(input[i],"parallel")==0){
			continue;
		}
		else{
			tokenNo = 0;
			char **tokenlist;
 			tokenlist = (char**)malloc(MAXLINE*sizeof(char**));
			while(input[i]!=NULL){
				if(strcmp(input[i],":::")==0){
					break;
				}
				else{
					tokenlist[tokenNo] = (char*)malloc(sizeof(token));
					strcpy(tokenlist[tokenNo],input[i]);
					tokenNo++;
					i++;
					}
			}
			pid = fork();
			if( pid < 0)
			{
				printf("Error occured: Could not fork\n");
				return;
				exit(-1);
			}
			else if(pid == 0)
			{
				analyze2(tokenlist);
				return;
			}
			else{
				
				continue;
			}

			free(tokenlist);
		}
	}

	free(token);
	kill(getpid(),SIGINT);
	return;
}


/*the tokenizer function takes a string of chars and forms tokens out of it*/
char ** tokenize(char* input){
	int i;
	int doubleQuotes = 0;
	
	char *token = (char *)malloc(1000*sizeof(char));
	int tokenIndex = 0;

	char **tokens;
	tokens = (char **) malloc(MAXLINE*sizeof(char**));
 
	int tokenNo = 0;
	
	for(i =0; i < strlen(input); i++){
		char readChar = input[i];
		
		if (readChar == '"'){
			doubleQuotes = (doubleQuotes + 1) % 2;
			if (doubleQuotes == 0){
				token[tokenIndex] = '\0';
				if (tokenIndex != 0){
					tokens[tokenNo] = (char*)malloc(sizeof(token));
					strcpy(tokens[tokenNo++], token);
					tokenIndex = 0; 
				}
			}
		}
		else if (doubleQuotes == 1){
			token[tokenIndex++] = readChar;
		}
		else if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
			token[tokenIndex] = '\0';
			if (tokenIndex != 0){
				tokens[tokenNo] = (char*)malloc(sizeof(token));
				strcpy(tokens[tokenNo++], token);
				tokenIndex = 0; 
			}
		}
		else{
			token[tokenIndex++] = readChar;
		}
	}
	
	if (doubleQuotes == 1){
		token[tokenIndex] = '\0';
		if (tokenIndex != 0){
			tokens[tokenNo] = (char*)malloc(sizeof(token));
			strcpy(tokens[tokenNo++], token);
		}
	}
	
	return tokens;
}

int checkpipe(char** tokens){
    int i;
    for(i=0;tokens[i]!=NULL;i++){
        if(strcmp(tokens[i],"|")==0){
            return i;
        }
    }
  
    return -1;
}


/* 
0 pipe, 
1 > 
2 <, 
3 >> 
4 <<
5 &
ELSE -1
*/

int checkredirect(char** tokens,int *pos){
	int i;
    for(i=0;tokens[i]!=NULL;i++){
        if(strcmp(tokens[i],"|")==0){
            *pos=i;
            return 0;
        }
        else if(strcmp(tokens[i],">")==0){
        	*pos=i;
            return 1;
        }
        else if(strcmp(tokens[i],"<")==0){
        	*pos=i;
            return 2;
        }
        else if(strcmp(tokens[i],">>")==0){
        	*pos=i;
            return 3;
        }
        else if(strcmp(tokens[i],"<<")==0){
        	*pos=i;
            return 4;
        }
        else if(strcmp(tokens[i],"&")==0){
        	*pos=i;
            return 5;
        }
    }
  
    return -1;
}
/*
Analyzes and runs appropriate command
*/
bool analyze(char **tokens){
	//printf("in analyze\n");
	char* command = (char *)malloc(1000*sizeof(char));
	strcpy(command,tokens[0]);
	pid_t pid;
	int status;
	bool flag=true;
	
	if(strcmp(command,"cd")==0){
		if(chdir(tokens[1])!=0){
			printf("ERROR: %s: No such directory\n", tokens[1]);
			return false;
		}
	}
	else if(strcmp(command,"run")==0){
		
		pid = fork();
		if( pid < 0)
		{
			printf("Error occured");
			return false;
			exit(-1);
		}
		else if(pid == 0)
		{
			run(tokens[1]);
			exit(0);
			//return true;
		}
		else{
			childpid=pid;
			waitpid(pid,&status,0);
		}
		
		
	}
	else if(strcmp(command,"exit")==0){
		exit(0);
		return true;
	}
	else if(strcmp(command,"parallel")==0){
		int i,j;
		// for(i=0;tokens[i]!=NULL;i++){
		//  	printf("%s\n", tokens[i]);
		//  }
		 parallel(tokens);
		 return true;
	}
	else if(strcmp(command,"cron")==0){
		file_read(tokens[1]);
		//printf("%s\n",c_data[0].argv[0]);
		
		pid = fork();
		if( pid < 0)
		{
			printf("Error occured");
			return false;
			exit(-1);
		}
		else if(pid == 0)
		{
			signal(SIGALRM,crontab_alarm_handler);
			alarm(ALARM_TIME);
			cron_run();
		}
		else{
			childpid=pid;
		}
	}
	else{
		
		pid = fork();
		if( pid < 0)
		{
			printf("Error occured");
			return false;
			exit(-1);
		}
		else if(pid == 0)
		{
			if(execvp(*tokens,tokens)==-1){
				char *error_str = strerror(errno);
				printf("ERROR:%s %s\n",tokens[0],error_str);
				return false;
				exit(0);
			}
		}
		else{
			childpid=pid;
			waitpid(pid,&status,0);
		}
	}
	free(command);
	return flag;
}

/* 
 Checks if the file exists
*/

bool fexists(char* file){
	if(access(file, F_OK) != -1){
	//	printf("[in run] %s file exists\n",file);
		return true;
	} 
	else{
		printf("ERROR:%s file doesn't exists\n",file);return false;
	}
}


/*
 Reads the file line by line and calls appropriate function or execvp
*/

void run(char* bat_file){
	if(fexists(bat_file)){
		FILE *fp;
		fp = fopen(bat_file, "r");
		if(fp!=NULL){
			char line[80];
			while(fgets(line, 80, fp) != NULL){
				char *cmd;
				char **argv;
				argv=tokenize(line);
				if(!analyze(argv)){
					kill(getpid(),SIGINT);
				}
			}
		}
		else{
			printf("ERROR:file %s didn't open\n", bat_file);
		}
		fclose(fp);
	}
	return;
}







/*
Analyzes and runs appropriate command for the case of parallel
*/
bool analyze2(char **tokens){
	char* command = (char *)malloc(1000*sizeof(char));
	strcpy(command,tokens[0]);
	pid_t pid;
	int status;
	bool flag=true;
	
	if(strcmp(command,"cd")==0){
		if(chdir(tokens[1])!=0){
			printf("ERROR: %s: No such directory\n", tokens[1]);
			return false;
		}
	}
	else if(strcmp(command,"run")==0){
		
			run(tokens[1]);
			return true;		

	}
	else if(strcmp(command,"exit")==0){
		exit(0);
		return true;
	}
	else if(strcmp(command,"parallel")==0){
		int i,j;
		// for(i=0;tokens[i]!=NULL;i++){
		//  	printf("%s\n", tokens[i]);
		//  }
		 parallel(tokens);
		 return true;
	}
	else{
		
		if(execvp(*tokens,tokens)==-1){
			char *error_str = strerror(errno);
			printf("ERROR:%s %s\n",tokens[0],error_str);
			return false;
			exit(0);
		}
	}
	free(command);
	return flag;
}




void runsource(int pfd[],char** cmd1) 
 /* run the first part of the pipeline, cmd1 */ 
 { 
	 int pid; /* we don't use the process ID here, but you may wnat to print it for debugging */ 
	 switch (pid = fork()) 
	 { 
	 	case 0:
	 		 /* child */ 
	 		 dup2(pfd[1], 1); 
	 		 /* this end of the pipe becomes the standard output */ 
	 		 close(pfd[0]); 
	 		 /* this process don't need the other end */ 
	 		 execvp(cmd1[0], cmd1); 
	 		 /* run the command */ 
	 		 perror(cmd1[0]); /* it failed! */ 
	 	default: /* parent does nothing */ 
	 		break;
	 	case -1: perror("fork"); exit(1); 
	} 
} 


void rundest(int pfd[], char** cmd2) /* run the second part of the pipeline, cmd2 */ 
{ 	
	int pid; 
	switch (pid = fork()) 
	{ 
		case 0: /* child */ 
			dup2(pfd[0], 0); /* this end of the pipe becomes the standard input */ 
			close(pfd[1]); /* this process doesn't need the other end */ 
			execvp(cmd2[0], cmd2); /* run the command */ 
			perror(cmd2[0]); /* it failed! */ 
			exit(0);
		default: /* parent does nothing */ 
			break;
		case -1: perror("fork"); exit(1); 
	} 
} 





void redirect_cmd(char** cmd, char** file) {
  int fds[2]; // file descriptors
  int count;  // used for reading from stdout
  int fd;     // single file descriptor
  char c;     // used for writing and reading a character at a time
  pid_t pid;  // will hold process ID; used with fork()

  pipe(fds);

  // child process #1
  if (fork() == 0) {
    // Thanks to http://linux.die.net/man/2/open for showing which headers
    // need to be included to use this function and its flags.
    fd = open(file[0], O_RDWR | O_CREAT, 0666);

    // open() returns a -1 if an error occurred
    if (fd < 0) {
      printf("Error: %s\n", strerror(errno));
      return;
    }

    dup2(fds[0], 0);

    // Don't need stdout end of pipe.
    close(fds[1]);

    // Read from stdout...
    while ((count = read(0, &c, 1)) > 0)
      write(fd, &c, 1); // Write to file.

    // Okay, so this is a bit contrived, but when I didn't have any kind of exec
    // function call here, I got my SarahShell prompt repeated over and over
    // again on the Multilab machines, I think because of this crazy child
    // process or something.  When I put this execlp here with the useless call
    // to echo, however, that looping stops and you can actually enter things
    // at the prompt again, hurray!
    execlp("echo", "echo", NULL);

  // child process #2
  } else if ((pid = fork()) == 0) {
    dup2(fds[1], 1);

    // Don't need stdin end of pipe.
    close(fds[0]);

    // Output contents of the given file to stdout.
    execvp(cmd[0], cmd);
    perror("execvp failed");

  // parent process
  } else {
    waitpid(pid, NULL, 0);
    close(fds[0]);
    close(fds[1]);
  }
}
