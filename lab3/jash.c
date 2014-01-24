#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <signal.h>
#include <sys/types.h>
#include <errno.h>


#define MAXLINE 1000
#define DEBUG 0

//declarations
char ** tokenize(char*);
void analyze(char** token);
int main(int argc, char** argv){

	//Setting the signal interrupt to its default function. 
	signal(SIGINT, SIG_DFL);

	//Allocating space to store the previous commands.
	int numCmds = 0;
	char **cmds = (char **)malloc(1000 * sizeof(char *));

	int printDollar = 1;

	char input[MAXLINE];
	char** tokens;

	int notEOF = 1;
	int i;

	FILE* stream = stdin;

	while(notEOF) { 
		if (printDollar == 1){ 
			printf("$ "); // the prompt
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
	 
		// for(i=0;tokens[i]!=NULL;i++){
		// 	printf("%s\n", tokens[i]);
		// }

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
void analyze(char **tokens){
	char* command = (char *)malloc(1000*sizeof(char));
	strcpy(command,tokens[0]);
	pid_t pid;
	int status;
	if(strcmp(command,"cd")==0){
		if(chdir(tokens[1])!=0){
			printf("ERROR: %s: No such directory\n", tokens[1]);
		}
	}
	else if(strcmp(command,"run")==0){
		printf("Calling run\n");
	}
	else if(strcmp(command,"exit")==0){
		exit(0);
	}
	else{
		pid = fork();
		if( pid < 0)
		{
			printf("Error occured");
			exit(-1);
		}
		else if(pid == 0)
		{
			if(execvp(*tokens,tokens)==-1){
				char *error_str = strerror(errno);
				printf("ERROR: %s\n",error_str);
			}
		}
		else{
			waitpid(pid,&status,0);
		}
	}
	free(command);
}
