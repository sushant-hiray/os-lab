#include <iostream>
#include <cstdio>
#include <fstream>
#include <string>
#include <string.h>
#include <stdlib.h> 
#define MSGLEN 10
using namespace std;


struct message{
	int type; //send = 0 receive = 1
	int receiver;
	char msg[MSGLEN];
};


int main(){
  string line;
  ifstream myfile ("example.txt");
  if (myfile.is_open())
  {
    while ( getline (myfile,line) )
    {
    		string delimiter = " ";
    		string token = line.substr(0, line.find(delimiter));
	      	message m;
	      	char* send = "send";
	      	char* receive = "receive" ;
	      	if(strcmp(token.c_str(),send)==0){
	      		m.type = 0;
	      		token = line.substr(line.find(delimiter)+1,strlen(line.c_str()));
	      		m.receiver = atoi(token.substr(0,token.find(delimiter)).c_str());
	      		token = token.substr(token.find(delimiter)+ 1, strlen(token.c_str()));
	      		strcpy(m.msg, token.c_str());
	      		
	      	}
	      	else if(strcmp(token.c_str(),receive)==0){
	      		m.type = 1;
	      		m.receiver = -1;
	      	}

    } 
    myfile.close();
  }
}