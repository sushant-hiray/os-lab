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

typedef struct {
	int min;
	int hr;
	int day;
	int mon;
	int week;
	char** argv;
} cron_data;

extern cron_data* c_data;
extern char* cfile;
extern int row;
extern volatile sig_atomic_t flag;

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

	// printf("%d %d %d %d %d\n",tm_s->tm_min,tm_s->tm_hour,tm_s->tm_mday,(tm_s->tm_mon)+1,tm_s->tm_wday);
	// printf("%s %s %s %s %s\n",schedule[i][0],schedule[i][1],schedule[i][2],schedule[i][3],schedule[i][4]);
	
	if(c_data[i].min==-1 || c_data[i].min==tm_s->tm_min){
		min_match=1;
	}
	if(c_data[i].hr==-1 || c_data[i].hr==tm_s->tm_hour){
		hr_match=1;
	}
	if(c_data[i].day==-1 || c_data[i].day==tm_s->tm_mday) {
		day_match=1;
	}
	if(c_data[i].mon==-1 || c_data[i].mon==tm_s->tm_mon){
		mon_match=1;
	}
	if(c_data[i].week==-1 || c_data[i].week==tm_s->tm_wday){
		week_match=1;
	}
	if((min_match==1) && (hr_match==1)&&(day_match==1)&&(mon_match==1)&&(week_match==1)){
		return 1;
	}
	else{
		printf("%d%d%d%d%d\n",min_match,hr_match,day_match,mon_match,week_match);
		return 0;
	}
}


int cron_run(){
	while(1){
		if(flag==1){
			int i;
			for(i=0;i<row;i++){
				if(can_exec(i)){
					printf("can execute %s\n",c_data[i].argv[0]);
					fflush(stdout);
					analyze(c_data[i].argv);
				}
			}
		}
		flag=0;
	}
	return 1;
}

void file_read(char* file){

	char* file_name=malloc(100);
	strcpy(file_name,file);
	char cline[80]="";
	int i=0;

	c_data=malloc(10*sizeof(cron_data));
	FILE* fp;
	fp=fopen(file_name, "r");
	
	if(fp!=NULL){
		while((fgets(cline, 80, fp) != NULL) && i<10){
			char** ctoken = tokenize(cline);
			cron_data cd;
			if(strcmp(ctoken[0],"*")==0){cd.min=-1;}else{cd.min=atoi(ctoken[0]);}
			if(strcmp(ctoken[1],"*")==0){cd.hr=-1;}else{cd.hr=atoi(ctoken[1]);}
			if(strcmp(ctoken[2],"*")==0){cd.day=-1;}else{cd.day=atoi(ctoken[2]);}
			if(strcmp(ctoken[3],"*")==0){cd.mon=-1;}else{cd.mon=atoi(ctoken[3]);}
			if(strcmp(ctoken[4],"*")==0){cd.week=-1;}else{cd.week=atoi(ctoken[4]);}
			cd.argv=&(ctoken[5]);
			c_data[i]=cd;
			i++;
		}
		row=i;
		//return 0;
		int j;
		fclose(fp);
		free(file_name);
	}

	else{
		printf("ERROR: file can't be opened");
	}
}



