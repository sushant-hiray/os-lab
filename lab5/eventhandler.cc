#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <queue>
#include "process.hh"
#include "scheduler.hh"
#include "Event.h"

using namespace std;

Eventhandler::Eventhandler(priority_queue<Event*, vector<Event*>, CompareTime> _table){
	event_table = _table ;
}

void Eventhandler::addevent(Event* e){
	event_table.push(e);
}

void EventHandler::popevent(){
	event_table.pop();
}

Event* EventHandler::gettopevent(){
	return event_table.top();
}



void EventHandler::removeio(Event* process){
	priority_queue<Event*> tempqueue;
	while(event_table.top()!=NULL){
		if(event_table.top()->get_pid() == process->get_pid() && event_table.top()->get_etype()==IOStart){
			event_table.pop();
			while(tempqueue.top()!=NULL){
				addevent(tempqueue.top());
				tempqueue.pop();
			}
			break;
		}
		else{
			tempqueue.push(gettopevent());
			popevent();
		}
	}
}

void Eventhandler::eventaction(){
	Event* top_event = gettopevent();
	Process* p = top_event->getprocess();
	switch(top_event->get_etype()){
		case Admission:
			//admission of event to scheduler
			Process* cpuprocess = scheduler->gettopprocess(); //todo
			if(cpuprocess==NULL){
				Event* iostart = new Event(p->getiostart() + myclock->getcurtime(),p->get_pid(),IOStart,p);
				p->setadmissiontime(myclock->getcurtime());
				p->setstate(RUNNING);
				scheduler->addprocess(p);
				addevent(iostart);
			}
			
			else{	
					if(cpuprocess->getpriority() < p->getpriority()){
						//preemption
						cpuprocess->savestate();


						cpuprocess->setstate(READY);
						removeio(cpuprocess);
						scheduler->addprocess(p);

						Event* iostart = new Event(p->getiostart() + myclock->getcurtime(),p->get_pid(),IOStart,p);
						p->setadmissiontime(myclock->getcurtime());
						p->setstate(RUNNING);
						addevent(iostart);


					}
					else{
						//no preemption
						p->setstate(READY);
						scheduler->addprocess(p); //todo add event
					
					}

				}
			break;
		

		case IOStart:
			//iostart change state of current process, schedule a process, add a i/o stop state
			p->setstate(BLOCKED);
			int phase_no = p->getcurphase();
			int itr_no = p->getcuritr();
			Event* iostop= new Event(p->getiostop() + myclock->getcurtime(),p->get_pid(),IOStop,p);
			addevent(iostop);
			
			scheduler->removetop(); //todo
			scheduler->schedule(); //todo
			break;

		
		case IOStop:
			//state transition from blocked to ready and call the schedule
			int flag=p->completeiteration();
			
			if(!flag){
				Event* admission= new Event(myclock->getcurtime(),p->get_pid(),Admission,p);
				addevent(admission);
			}
			else{
				scheduler->schedule();
			}
			break;

	}
	popevent();
	myclock->updateClock(gettopevent()->get_etime());
}