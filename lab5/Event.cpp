#include <iostream>
#include "Event.h"

using namespace std;

Event::Event(int e, int p, E_Type t, Process* _p){
	etime = e;
	epid = p;
	etype = t;
	process = _p;
}

Event::~Event(){	
}

int Event::get_etime(){
	return etime;
}

int Event::get_pid(){
	return epid;
}

int Event::get_etype(){
	return etime;
}

Process* Event::getProcess(){
	return process;
}

void Event::Action(){
	
}