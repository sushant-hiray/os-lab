#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */ 
#include <errno.h>      /* Errors */
#include <stdio.h>      /* Input/Output */
#include <sys/wait.h>   /* Wait for Process Termination */
#include <stdlib.h>     /* General Utilities */


#define INPUT_STRING_SIZE   80
#define NORMAL 				00
#define OUTPUT_REDIRECTION 	11
#define INPUT_REDIRECTION 	22
#define PIPELINE 			33
#define BACKGROUND			44
#define OUTPUT_APP			55

int parse(char *inputString, char *cmdArgv[], char **supplementPtr, int *modePtr){
	int cmdArgc = 0, terminate = 0;
	char *srcPtr = inputString;
	//printf("parse fun%sends", inputString);

	while(*srcPtr != '\0' && terminate == 0)
	{
		*cmdArgv = srcPtr;
		cmdArgc++;
		//printf("parse fun2%sends", *cmdArgv);
		while(*srcPtr != ' ' && *srcPtr != '\t' && *srcPtr != '\0' && *srcPtr != '\n' && terminate == 0)
		{
			switch(*srcPtr)
			{
				case '&':
					*modePtr = BACKGROUND;
					break;
				// case '>':
				// 	*modePtr = OUTPUT_REDIRECTION;
				// 	*cmdArgv = '\0';
				// 	srcPtr++;
				// 	if(*srcPtr == '>')
				// 	{
				// 		*modePtr = OUTPUT_APP;
				// 		srcPtr++;
				// 	}
				// 	while(*srcPtr == ' ' || *srcPtr == '\t')
				// 		srcPtr++;
				// 	*supplementPtr = srcPtr;
				// 	//chop(*supplementPtr);
				// 	terminate = 1;
				// 	break;
				// case '<':
				// 	*modePtr = INPUT_REDIRECTION;
				// 	*cmdArgv = '\0';
				// 	srcPtr++;
				// 	while(*srcPtr == ' ' || *srcPtr == '\t')
				// 		srcPtr++;
				// 	*supplementPtr = srcPtr;
				// 	chop(*supplementPtr);
				// 	terminate = 1;
				// 	break;
				case '|':
					*modePtr = PIPELINE;
					*cmdArgv = '\0';
					srcPtr++;
					while(*srcPtr == ' ' || *srcPtr == '\t')
						srcPtr++;
					*supplementPtr = srcPtr;
					//chop(*supplementPtr);
					terminate = 1;
					break;
			}
			srcPtr++;
		}
		while((*srcPtr == ' ' || *srcPtr == '\t' || *srcPtr == '\n') && terminate == 0)
		{
			*srcPtr = '\0';
			srcPtr++;
		}
		cmdArgv++;
	}
	*cmdArgv = '\0';
	return cmdArgc;
}

void execute(char **cmdArgv, int mode, char **supplementPtr)
{
	pid_t pid, pid2;
	FILE *fp;
	int mode2 = NORMAL, cmdArgc, status1, status2;
	char *cmdArgv2[INPUT_STRING_SIZE], *supplement2 = NULL;
	int myPipe[2];
	if(mode == PIPELINE)
	{
		if(pipe(myPipe))					//create pipe
		{
			fprintf(stderr, "Pipe failed!");
			exit(-1);
		}
		parse(*supplementPtr, cmdArgv2, &supplement2, &mode2);
	}
	pid = fork();
	if( pid < 0)
	{
		printf("Error occured");
		exit(-1);
	}
	else if(pid == 0)
	{
		switch(mode)
		{
			case OUTPUT_REDIRECTION:
				fp = fopen(*supplementPtr, "w+");
				dup2(fileno(fp), 1);
				break;
			case OUTPUT_APP:
				fp = fopen(*supplementPtr, "a");
				dup2(fileno(fp), 1);
				break;
			case INPUT_REDIRECTION:
				fp = fopen(*supplementPtr, "r");
				dup2(fileno(fp), 0);
				break;
			case PIPELINE:
				close(myPipe[0]);		//close input of pipe
				dup2(myPipe[1], fileno(stdout));
				close(myPipe[1]);
				break;
		}
		execvp(*cmdArgv, cmdArgv);
	}
	else
	{
		if(mode == BACKGROUND)
			;
		else if(mode == PIPELINE)
		{
			waitpid(pid, &status1, 0);		//wait for process 1 to finish
			pid2 = fork();
			if(pid2 < 0)
			{
				printf("error in forking");
				exit(-1);
			}
			else if(pid2 == 0)
			{
				close(myPipe[1]);		//close output to pipe
				dup2(myPipe[0], fileno(stdin));
				close(myPipe[0]);
				execvp(*cmdArgv2, cmdArgv2);
			}
			else
			{
				;//wait(NULL);
				//waitpid(pid, &status1, 0);
				//waitpid(pid2, &status2, 0);
				close(myPipe[0]);
				close(myPipe[1]);
			}
		}
		else
			waitpid(pid, &status1, 0);
			//wait(NULL);
	}
}


int main(){
	int i, mode = NORMAL, cmdArgc;
	size_t len = INPUT_STRING_SIZE;
	char *cpt, *inputString, *cmdArgv[INPUT_STRING_SIZE], *supplement = NULL;
	char curDir[100];
	inputString = (char*)malloc(sizeof(char)*INPUT_STRING_SIZE);

	while(1)
		{
			mode = NORMAL;
			getcwd(curDir, 100);
			printf("%s@%s->", getlogin(),curDir);
			getline( &inputString, &len, stdin);
			if(strcmp(inputString, "exit\n") == 0)
				exit(0);
			cmdArgc = parse(inputString, cmdArgv, &supplement, &mode);
			if(strcmp(*cmdArgv, "cd") == 0)
			{
				chdir(cmdArgv[1]);
			}
			else{
				execute(cmdArgv, mode, &supplement);
			}
				
				
		}
		return 0;
}