#ifndef _EVENT_HANDLER
#define _EVENT_HANDLER
#include <queue>

class CompareTime {
public:
    bool operator()(Event* e1, Event* e2)
    {
       if (e1->get_etime() < e2->get_etime()) return true;
       return false;
    }
};

class EventHandler
{
		priority_queue<Event*, vector<Event*>, CompareTime> event_table;
   	public:
   		EventHandler(priority_queue<Event*, vector<Event*>, CompareTime>);
   		void eventaction();
   		void addevent(Event* e);
   		void popevent();
      Event* gettopevent();
      void removeio(Process* process);
};

#endif