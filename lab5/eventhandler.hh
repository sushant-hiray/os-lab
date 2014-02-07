#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <queue>
using namespace std;


class CompareTime {
public:
    bool operator()(Event* e1, Event* e2)
    {
       if (e1->getnexttime() < e2->getnexttime()) return true;
       return false;
    }
};


class EventHandler
{
	private:
		priority_queue<Event*, vector<Event*>, CompareTime> event_table;
		
   	public:
   		EventHandler();
   		void eventaction();
   		void addevent(Event* e);
   		void popevent();
      Event* gettopevent();
      void removeio(Event* process);
};