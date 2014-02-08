#include <iostream>
#include "Event.h"
//#include "utils.h"

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

int Event::get_epid(){
	return epid;
}

E_Type Event::get_etype(){
	return etype;
}

Process* Event::getprocess(){
	return process;
}