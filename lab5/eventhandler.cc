#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <queue>
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

void Eventhandler::eventaction(){
	Event* top_event = event_table->top();
	switch(top_event->get_etype()){
		case Admission:
			//admission 
			scheduler.addprocess(top_event->getprocess()); //todo add event
			event_table->pop();
			break;
		
		case CStart:
			//admission 
			
			break;
		case CStop:
			//admission 
			break;
		case IOStart:
			//admission 
			break;
		case IOStop:
			//admission 
			break;

		case Complete:
			//completion
			break;
	}
}