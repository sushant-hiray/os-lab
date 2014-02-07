#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
using namespace std;

class EventHandler
{
	private:
		priority_queue<Event*> event_table;
   	public:
   		EventHandler();
   		void eventaction();
   		void addevent(Event* e);
   		void popevent();
};