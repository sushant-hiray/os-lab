#include <iostream>
using namespace std;


Scheduler::Scheduler(EventHandler _eh, priority_queue<Process*> _list){
	eh = _eh;
	process_list = _list;
}


EventHandler Scheduler::geteh(){
	return eh;
}

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

void Scheduler::schedule();{
	Process* p = this->gettopprocess();
	Event* e = new Event(myclock->getcurtime() + p->getiostart(), p->getpid, IOStart, p);
	event_table.push(e);
}