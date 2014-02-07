#include <iostream>
using namespace std;


Scheduler::Scheduler(EventHandler _eh, priority_queue<Process> _list){
	eh = _eh;
	process_list = _list;
}


EventHandler Scheduler::geteh(){
	return eh;
}

int Scheduler::getlevel(){
	return level;
}

priority_queue<Process> Scheduler::getlist(){
	return process_list;
}


void Scheduler::setlevel(int l){
	level = l;
}

