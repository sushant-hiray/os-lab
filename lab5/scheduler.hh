#ifndef _SCHEDULER
#define _SCHEDULER 
#include <queue>
#include "Event.h"

class ComparePriority {
public:
    bool operator()(Process* p1, Process* p2)
    {
       if (p1->getpriority() < p2->getpriority()) return true;
       return false;
    }
};

class Scheduler{
	private:
		int level;
		//EventHandler eh;
		priority_queue<Process*, vector<Process*>, ComparePriority> process_list;

	public:
		Scheduler(priority_queue<Process*, vector<Process*>, ComparePriority> _list);
		//EventHandler geteh();
		int getlevel();
		priority_queue<Process*, vector<Process*>, ComparePriority> getlist();
		void setlevel(int l);
		Process* gettopprocess();	//--
		void addprocess(Process*);	//--
		void removetop();			//--
		void schedule();			//--


};

#endif