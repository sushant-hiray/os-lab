#include <unistd.h>     /* Symbolic Constants */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <signal.h>
#include <sys/types.h>
#include <errno.h>
#include <time.h>

#define MAXLINE 1000
#define LINE 100
#define DEBUG 0
#define C_MIN 0
#define C_HR 1
#define C_DAY 2
#define C_MONTH 3
#define C_WDAY 4
#define C_CMD 5

/*
 * struct tm {
  int tm_sec;   // seconds of minutes from 0 to 61
  int tm_min;   // minutes of hour from 0 to 59
  int tm_hour;  // hours of day from 0 to 24
  int tm_mday;  // day of month from 1 to 31
  int tm_mon;   // month of year from 0 to 11
  int tm_year;  // year since 1900
  int tm_wday;  // days since sunday
  int tm_yday;  // days since January 1st
  int tm_isdst; // hours of daylight savings time
}
 * */



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

char schedule[10][100][1000];

int can_exec(int i){
	time_t rawtime;
	time( &rawtime );
	struct tm *tm_s = localtime(&rawtime);
	
	int min_match=0;
	int hr_match=0;
	int day_match=0;
	int mon_match=0;
	int week_match=0;
	int j;
	for(j=0;strcmp(schedule[i][j],"")!=0;j++){
		printf("%s ",schedule[i][j]);
	}
	//printf("%s\n",tm_s->tm_min);
	if((strcmp(schedule[i][0],"*")==0) ||(atoi(schedule[i][0])==tm_s->tm_min)) {
		min_match=1;
	}
	if((strcmp(schedule[i],"*")==0) ||(atoi(schedule[i][1])==tm_s->tm_hour)) {
		hr_match=1;
	}
	if((strcmp(schedule[i][2],"*")==0) ||(atoi(schedule[i][2])==tm_s->tm_mday)) {
		day_match=1;
	}
	if((strcmp(schedule[i][3],"*")==0) ||(atoi(schedule[i][3])==(tm_s->tm_mon)+1)) {
		mon_match=1;
	}
	if((strcmp(schedule[i][4],"*")==0) ||(atoi(schedule[i][4])==tm_s->tm_wday)) {
		week_match=1;
	}
	if((min_match==1) && (hr_match==1)&&(day_match==1)&&(mon_match==1)&&(week_match==1)){
		return 1;
	}
	else{
		printf("svdsfv%d%d%d%d%d",min_match,hr_match,day_match,mon_match,week_match);
		return 0;
	}
}


char* cfile="cront.txt";



int main(){
	int rows=10;
	char* file_name=malloc(100);
	strcpy(file_name,"cront.txt");
	
	//printf("%d\n",sizeof(char**));
//	return 0;
	//printf("%d\n",tm_struct->tm_min);
	char cline[80]="";
	//system("cat cront.txt");
	FILE* fp;
	fp=fopen(file_name, "r");
	int i=0;
	
	printf("%s",cfile);
	//return 0;
	//printf("%d\n",tm_struct->tm_min);
	if(fp!=NULL){
		while((fgets(cline, 80, fp) != NULL) && i<10){
			//setting the timer
			//tokenize the cron command
			printf("%s\n",cline);
			char** ctoken = tokenize(cline);
			int j;
			for(j=0;j<100&&ctoken[j]!=NULL;j++){
				strcpy(schedule[i][j],ctoken[j]);
			}
			printf("%d",j);
			strcpy(schedule[i][j],"");
			
			//strcpy(schedule[i][j],NULL);
			printf("%d fnvlfnlv",i);
			i++;
		}
		int row=i,j;
		printf("%d  fnvlfnlv\n",i);
		fclose(fp);
		for(i=0;i<row;i++){
			for(j=0;strcmp(schedule[0][j],"")!=0;j++){
				printf("%s \n",schedule[0][j]);
			}
		}
		
		printf("%s\n",schedule[0][5]);
		int kk=can_exec(0);
		printf("%d",kk);
	}
	
	else{
		printf("ERROR: file can't be opened");
	}
	
	return 0;
}


