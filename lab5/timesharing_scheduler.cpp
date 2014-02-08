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
				Event* e = new Event(myclock->getcurtime(), p->getpid(), Admission, p);
				eh->addevent(e);
				break;
			}
		}
		

	}
	return;
	
}

void TimeSharingScheduler::handleadmission(Process* p){
	Process* cpuprocess = gettopprocess();
	if(cpuprocess==NULL){
		cout<<"Inside Admission\n";
		Event* iostart = new Event(p->getiostart() + myclock->getcurtime(),p->getpid(),IOStart,p);
		p->setadmission(myclock->getcurtime());
		p->setstate(RUNNING);
		addprocess(p);
		eh->addevent(iostart);
		cout<<"event iostart added when null "<<p->getpid()<<endl;
		settopprocess(cpuprocess);

	}
	else{
		cout<<" Preemption "<<endl;
		//cout<< " admission time "<<cpuprocess->getadmission()<<endl;
		cpuprocess->savestate();
		//cout<<" left time "<<cpuprocess->lefttime()<<endl;

		cpuprocess->setstate(READY);

		//cout<<"masala"<<endl;
		eh->removeio(cpuprocess);
		while(!process_list.empty()){
			if(process_list.front()->getstate()==READY || process_list.front()->getstate()==RUNNING){
				Event* admission = new Event(myclock->getcurtime() + timeslice,process_list.front()->getpid(),Admission, process_list.front());
				eh->addevent(admission);
				process_list.pop();
				break;
			}
			else{
				process_list.pop();
			}
		}
		cout<<"event iostart added for process "<<p->getpid()<<endl;
		Event* iostart = new Event(p->getiostart() + myclock->getcurtime(),p->getpid(),IOStart,p);
		p->setadmission(myclock->getcurtime());
		p->setstate(RUNNING);
		addprocess(cpuprocess);
		eh->addevent(iostart);
		settopprocess(p);
	}
}


void TimeSharingScheduler::settopprocess(Process* _p){
	current_process = _p;

}


