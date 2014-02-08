#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <queue>
//#include "process.hh"
#include "scheduler.hh"
#include "Event.h"
#include "eventhandler.hh"
//#include "utils.h"

extern Scheduler* scheduler;

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
	Process* p = top_event->getprocess();
	cout<<"Clock: "<<myclock->getcurtime()<<"	Event: "<<state_type[top_event->get_etype()]<<"   Process: "<<p->getpid()<< "   Phase No.: "<< p->getcurphase()<< "    Iteration No.: " << p->getcuritr()<<endl;

	switch(top_event->get_etype()){
		case Admission:
		{	//admission of event to scheduler
			//IFBUG cout<<"\tIn Admission"<<endl; ENDBUG
			Process* cpuprocess = scheduler->gettopprocess();
			if(cpuprocess==NULL){
				IFBUG cout<<"INside Admission\n"; ENDBUG
				Event* iostart = new Event(p->getiostart() + myclock->getcurtime(),p->getpid(),IOStart,p);
				p->setadmission(myclock->getcurtime());
				p->setstate(RUNNING);
				scheduler->addprocess(p);
				addevent(iostart);
			}
			
			else{	
					IFBUG cout<<"\tINside Admission ELSE\n"; ENDBUG
					//cout<<" cpu process id "<<cpuprocess->getpriority()<<" event top "<<p->getpriority()<<endl;
					if(cpuprocess->getstate()==RUNNING){
						if(cpuprocess->getpriority() < p->getpriority()){
						//preemption
							cout<<" Preemption "<<endl;
							//cout<< " admission time "<<cpuprocess->getadmission()<<endl;
							cpuprocess->savestate();
							//cout<<" left time "<<cpuprocess->lefttime()<<endl;

							cpuprocess->setstate(READY);
							//cout<<"masala"<<endl;
							removeio(cpuprocess);
							
							scheduler->addprocess(p);

							Event* iostart = new Event(p->getiostart() + myclock->getcurtime(),p->getpid(),IOStart,p);
							p->setadmission(myclock->getcurtime());
							//cout<< " admission time second time "<<p->getadmission()<<endl;
							p->setstate(RUNNING);
							addevent(iostart);
						}
						else{
							//no preempt
							//cout<<" no preemption "<<endl;
							p->setstate(READY);
							scheduler->addprocess(p); //todo add event
							
						}
					}

					else{
							//cout<<"here"<<endl;
							
							Event* iostart = new Event(p->getiostart() + myclock->getcurtime(),p->getpid(),IOStart,p);
							p->setadmission(myclock->getcurtime());
							//cout<< " admission time second time "<<p->getadmission()<<endl;
							p->setstate(RUNNING);
							addevent(iostart);
					}
					

				}
			break;
		}

		case IOStart:
		{	//iostart change state of current process, schedule a process, add a i/o stop state
			
			p->setstate(BLOCKED);
			int phase_no = p->getcurphase();
			int itr_no = p->getcuritr();
			Event* iostop= new Event(p->getiostop() + myclock->getcurtime(),p->getpid(),IOStop,p);
			IFBUG cout<<"INside IOStart\n"; ENDBUG
			addevent(iostop);
			//IFBUG cout<< "schedular.top: "<<scheduler->gettopprocess()->getadmission()<<endl; ENDBUG
			scheduler->removetop(); //todo
			scheduler->schedule(); //todo
			break;
		}
				
		case IOStop:
		{	//state transition from blocked to ready and call the schedule
			IFBUG cout<<"INside IOStop\n"; ENDBUG
			int flag=p->completeiteration();
			IFBUG cout<<"flag is "<<flag<<endl; ENDBUG
			if(!flag){
				Event* admission= new Event(myclock->getcurtime(),p->getpid(),Admission,p);
				addevent(admission);
			}
			else{
				IFBUG cout<<"scheduler\n"; ENDBUG
				scheduler->schedule();
			}
			break;
		}
	}
	popevent();
}