#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <queue>
//#include "process.hh"
#include "timesharingscheduler.hh"
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
					//cout<<"\tINside Admission ELSE\n";
					//cout<<" cpu process id "<<cpuprocess->getpid()<<"  State: "<< cpuprocess->getstate()<<endl;
					if(cpuprocess->getstate()==RUNNING){
						if(cpuprocess->getpriority() < p->getpriority()){
						//preemption
							cout<<" Preemption "<<endl;
							//cout<< " admission time "<<cpuprocess->getadmission()<<endl;
							cpuprocess->savestate();
							//cout<<" left time "<<cpuprocess->lefttime()<<endl;

							cpuprocess->setstate(READY);
							//cout<<"masala"<<endl;
							//cout<<"cpuprocess before remove io is: "<<cpuprocess->getpid()<<endl;
							removeio(cpuprocess);
							p->setadmission(myclock->getcurtime());
							scheduler->addprocess(p);
							Event* iostart = new Event(p->getiostart() + myclock->getcurtime(),p->getpid(),IOStart,p);
							//cout<< " admission time second time "<<p->getadmission()<<endl;
							p->setstate(RUNNING);
							addevent(iostart);
						}
						else{
							//no preempt
							//cout<<" no preemption "<<endl;
							p->setstate(READY);
							p->setadmission(myclock->getcurtime());
							scheduler->addprocess(p); //todo add event
							
						}
					}

					else{
							//cout<<"here"<<endl;
							p->setstate(RUNNING);
							p->setadmission(myclock->getcurtime());
							Event* iostart = new Event(p->getiostart() + myclock->getcurtime(),p->getpid(),IOStart,p);
							//cout<< " admission time second time "<<p->getadmission()<<endl;
							addevent(iostart);
					}
					

				}
			//cout<<"After admission state of process: "<<p->getpid() <<"is "<<p->getstate()<<endl;
			//cout<<"Admission and completion time of p: "<<p->getadmission()<<" and "<<p->getcompletetime()<<endl;
			break;
		}

		case IOStart:
		{	//iostart change state of current process, schedule a process, add a i/o stop state
			//cout<<"In IoStart"<<endl;
			//cout<<"process: "<<p->getpid()<<endl;
			p->savestate();
			p->setstate(BLOCKED);
			int phase_no = p->getcurphase();
			int itr_no = p->getcuritr();
			Event* iostop= new Event(p->getiostop() + myclock->getcurtime(),p->getpid(),IOStop,p);
			//IFBUG cout<<"INside IOStart\n"; ENDBUG
			addevent(iostop);
			//IFBUG cout<< "schedular.top: "<<scheduler->gettopprocess()->getadmission()<<endl; ENDBUG
			scheduler->addblocked(p);
			if(p!=NULL && scheduler->gettopprocess()!=NULL)
				//cout<<p->getpid() << "   "<<scheduler->gettopprocess()->getpid()<<endl;
			scheduler->removetop(); //can it be possible that process starting IO is not on top of list
			scheduler->schedule(); //todo
			break;
		}
				
		case IOStop:
		{	//state transition from blocked to ready and call the schedule
			//cout<<"INside IOStop\n";
			int flag=p->completeiteration();
			IFBUG cout<<"flag is "<<flag<<endl; ENDBUG
			if(!flag){
				//cout<<"cerating admisssion of process: "<< p->getpid()<< "at "<< myclock->getcurtime()<<endl;
				
				Process* cpuprocess = scheduler->gettopprocess();
				
				if(cpuprocess!=NULL){
					if(cpuprocess->getpriority() < p->getpriority()){
						cout<<" Preemption IO Stop"<<endl;
						if(cpuprocess->getstate()==RUNNING){cpuprocess->savestate();}
						//cout<< " "<< cpuprocess->getpid() << "	"<< myclock->getcurtime()<<"	"<<cpuprocess->getcompletetime()<<"	"<<endl;
						cpuprocess->setstate(READY);
						//cout<<"cpuprocess before remove io is: "<<cpuprocess->getpid()<<endl;
						removeio(cpuprocess);

						p->setstate(READY);
						scheduler->addprocess(p);

					}

				}
				Event* admission= new Event(myclock->getcurtime(),p->getpid(),Admission,p);
				addevent(admission);

			}
			else{
				//cout<<"scheduler\n";
				scheduler->schedule();
			}
			break;
		}
	}
}