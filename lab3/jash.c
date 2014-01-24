#include <unistd.h>     /* Symbolic Constants */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <signal.h>
#include <sys/types.h>
#include <errno.h>


#define MAXLINE 1000
#define DEBUG 0
#define MAXPARALLEL 80

typedef enum { false, true } bool;
//declarations
char ** tokenize(char*);
bool analyze(char**);
void run(char*);
bool fexists(char*);
void parallel(char**);

pid_t childpid=-1;

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
		 if(i!=0)
			analyze(tokens);
	}
  
  
	printf("Print and deallocate %s\n", tokens[0]);
	// Freeing the allocated memory	
	for(i=0;tokens[i]!=NULL;i++){
		free(tokens[i]);
	}
	free(tokens);
	return 0;
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
			while(strcmp(input[i],":::")!=0 && input[i]!=NULL){
				tokenlist[tokenNo] = (char*)malloc(sizeof(token));
				strcpy(tokenlist[tokenNo],input[i]);
				tokenNo++;
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
				analyze(tokenlist);
				return;
			}
			else{
				childpid=pid;
				waitpid(pid,&status,0);
			}

			free(tokenlist);
		}
	}
	free(token);
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

/*
Analyzes and runs appropriate command
*/
bool analyze(char **tokens){
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
			return true;
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



