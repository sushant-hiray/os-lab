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

typedef enum { false, true } bool;

bool isvalid(char**);
int position(char**, char*);
int two_occurance(char**, char*);
char ** tokenize(char*);

int two_occurance(char** tokens, char* str){
  bool first = false;
  bool second =false;
  int i=0;
  for(i=0;tokens[i]!=NULL;i++){
    if(strcmp(tokens[i],str)==0){
      if(first!=false) {second = true;break;}
      if(second!=true) first = true;
    }
  }
  if (second == true) return true;
  return false;
}

int position(char** tokens, char* str){
  int i;

  for(i=0;tokens[i]!=NULL;i++){
    if(strcmp(tokens[i],str)==0){
      //printf("referferferafeergfverar  %d \n",i);
      return i;
    }
  }
  return -1;
}

bool isvalid(char** tokens){
  if((position(tokens,"<")!=-1) && (position(tokens,"<<")!=-1)) return false;
  if((position(tokens,">")!=-1) && (position(tokens,">>")!=-1)) return false;
  if((two_occurance(tokens,"<")==true) || (two_occurance(tokens,">")==true)) return false;
  return true;
}