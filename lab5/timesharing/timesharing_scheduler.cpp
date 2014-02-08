#include <iostream>
#include "utils.h"
#include "timesharing_scheduler.hh"
#include "eventhandler.hh"
extern Clock* myclock;
using namespace std;

extern EventHandler* eh;

TimeSharingScheduler::TimeSharingScheduler(queue<Process*> _list, int _time){
	process_list = _list;
	current_process = NULL;
	timeslice = _time;
}


int TimeSharingScheduler::getlevel(){
	return level;
}

queue<Process*>TimeSharingScheduler::getlist(){
	return process_list;
}


void TimeSharingScheduler::setlevel(int l){
	level = l;
}

Process* TimeSharingScheduler::gettopprocess(){
	if(!process_list.empty()){
		//cout<<"fgeer\n";
		return process_list.front();
	}
	return NULL;
}

void TimeSharingScheduler::addprocess(Process* p){
	process_list.push(p);
}

void TimeSharingScheduler::removetop(){
	if(!process_list.empty()){
		process_list.pop();
	}
}

void TimeSharingScheduler::schedule(){
	Process* p = this->gettopprocess();
	//IFBUG cout<<"Inside scheduler:scdule\n"; ENDBUG
	if(p!=NULL){
		state pstate = p->getstate();
		IFBUG cout<<"Inside scheduler:scdule\n"; ENDBUG
		switch(pstate){
			case READY:
			{	
				p->setadmission(myclock->getcurtime());
				cout<<"admission time of p is "<<p->getadmission()<<" "<<p->getcompletetime()<<endl;
				p->setstate(RUNNING);
				if(p->lefttime() <= timeslice){
					//schelude io
					Event* iostart = new Event(p->lefttime() + myclock->getcurtime(), p->getpid(),IOStart,p);
					eh->addevent(iostart);
				}
				else{
					//schedule timeslice
					Event* etimeslice = new Event(timeslice + myclock->getcurtime(), p->getpid(),Timeslice,p);
					eh->addevent(etimeslice);
				}
			}
		}
	}
	return;
	
}

void TimeSharingScheduler::settopprocess(Process* _p){
	current_process = _p;

}


