#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <queue>
//#include "process.hh"
#include "timesharing_scheduler.hh"
#include "Event.h"
#include "eventhandler.hh"
//#include "utils.h"

extern TimeSharingScheduler* scheduler;

using namespace std;

EventHandler::EventHandler(priority_queue<Event*, vector<Event*>, CompareTime> _table){
	event_table = _table ;
}

void EventHandler::addevent(Event* e){
	event_table.push(e);
	//cout<<"event added"<<endl;
}

void EventHandler::popevent(){
	if(!event_table.empty()){
		event_table.pop();
	}
}

Event* EventHandler::gettopevent(){
	if(!event_table.empty()){
		return event_table.top();
	}
	return NULL;
}



void EventHandler::removeio(Process* process){
	priority_queue<Event*> tempqueue;
	while(gettopevent()!=NULL){
		if(event_table.top()->get_epid() == process->getpid() && event_table.top()->get_etype()==IOStart){
			//cout<<"event poped is: "<<process->getpid()<<endl;
			popevent();
			while(!tempqueue.empty()){
				addevent(tempqueue.top());
				tempqueue.pop();
			}
			//cout<<"wfr'jewrf"<<endl;
			break;
		}
		else{
			tempqueue.push(gettopevent());
			popevent();
		}
	}
}

void EventHandler::eventaction(){
	//cout<<"hey"<<endl;
	Event* top_event = gettopevent();
	popevent();
	Process* p = top_event->getprocess();
	cout<<"Clock: "<<myclock->getcurtime()<<"	Event: "<<state_type[top_event->get_etype()]<<"   Process: "<<p->getpid()<<"   State: "<<p->getstate()<< "   Phase No.: "<< p->getcurphase()<< "    Iteration No.: " << p->getcuritr()<<endl;
	//cout<<"masala"<<endl;
	//cout<<scheduler->getlevel();
	Process* cpuprocess = scheduler->gettopprocess();
	//cout<<"masalak"<<endl;
	switch(top_event->get_etype()){
		case Admission:
		{	
			cout<<"In side Admission"<<endl;
			p->setstate(READY);
			scheduler->addprocess(p);
			scheduler->schedule();

			break;
		}

		case IOStart:
		{	
			cout<<"In side iostart"<<endl;
			if(p->getstate()==RUNNING){
				scheduler->removetop();
			}
			p->setstate(BLOCKED);
			Event* iostop = new Event(p->getiostop() + myclock->getcurtime(), p->getpid(),IOStop,p);
			addevent(iostop);
			scheduler->schedule();
			break;
		}
				
		case IOStop:
		{	
			cout<<"In side iostop"<<endl;
			int flag=p->completeiteration(); //updated state
			cout<<"flag is "<<flag<<" phase is "<<p->getcurphase()<<endl;
			if(!flag){
				p->setstate(READY);
				scheduler->addprocess(p);
				scheduler->schedule();
				//Event* admission = new Event(myclock->getcurtime(),)
			}
			break;
		}

		case Timeslice:
		{
			cout<<"In side Timeslice"<<endl;
			p->savestate(); //save state of current process
			p->setstate(READY);
			scheduler->removetop();
			scheduler->addprocess(p);
			scheduler->schedule();
			break;
		}
	}
}