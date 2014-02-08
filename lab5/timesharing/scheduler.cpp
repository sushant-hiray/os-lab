#include <iostream>
#include "utils.h"
#include "scheduler.hh"
#include "eventhandler.hh"
extern Clock* myclock;
using namespace std;

extern EventHandler* eh;

Scheduler::Scheduler(priority_queue<Process*, vector<Process*>, ComparePriority> _list){
	//eh = _eh;
	process_list = _list;
}


// EventHandler Scheduler::geteh(){
// 	return eh;
// }

int Scheduler::getlevel(){
	return level;
}

priority_queue<Process*, vector<Process*>, ComparePriority> Scheduler::getlist(){
	return process_list;
}


void Scheduler::setlevel(int l){
	level = l;
}

Process* Scheduler::gettopprocess(){
	if(!process_list.empty()){
		return process_list.top();
	}
	return NULL;
}

void Scheduler::addprocess(Process* p){
	process_list.push(p);
}

void Scheduler::removetop(){
	if(!process_list.empty()){
		//cout<<"Process removed from scheduler is: "<<process_list.top()->getpid()<<endl;
		process_list.pop();
	}
}

void Scheduler::removeblocked(Process* p){
	list<Process*>::iterator it;
	bool flag=false;
	int id = p->getpid();
	for(it=blocked_processes.begin();it!=blocked_processes.end();it++){
		if((*it)->getpid() == id ){
			flag=true;
			break;
		}
	}
	if(flag){
		(*it)->setstate(READY);
		process_list.push(*it);
		it=blocked_processes.erase(it);
	}
}

void Scheduler::addblocked(Process* p){
	if(p!=NULL){
		p->setstate(BLOCKED);
		blocked_processes.push_back(p);
	}
}

void Scheduler::schedule(){
	Process* p = this->gettopprocess();
	//IFBUG cout<<"Inside scheduler:scdule\n"; ENDBUG
	if(p!=NULL){
		state pstate = p->getstate();
		IFBUG cout<<"Inside scheduler:scdule\n"; ENDBUG
		switch(pstate){
			
			case READY:
			{	
				Event* e = new Event(myclock->getcurtime(), p->getpid(), Admission, p);
				eh->addevent(e);
				break;
			}
		}
		

	}
	return;
	
}