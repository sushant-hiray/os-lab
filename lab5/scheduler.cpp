#include <iostream>
#include "utils.h"
#include "scheduler.hh"
#include "eventhandler.hh"
extern Clock* myclock;
using namespace std;

extern EventHandler* eh;

Scheduler::Scheduler(priority_queue<Process*> _list){
	//eh = _eh;
	process_list = _list;
}


// EventHandler Scheduler::geteh(){
// 	return eh;
// }

int Scheduler::getlevel(){
	return level;
}

priority_queue<Process*> Scheduler::getlist(){
	return process_list;
}


void Scheduler::setlevel(int l){
	level = l;
}

Process* Scheduler::gettopprocess(){
	return process_list.top();
}

void Scheduler::addprocess(Process* p){
	process_list.push(p);
}

void Scheduler::removetop(){
	process_list.pop();
}

void Scheduler::schedule(){
	Process* p = this->gettopprocess();
	Event* e = new Event(myclock->getcurtime() + p->getiostart(), p->getpid(), IOStart, p);
	eh->addevent(e);
}