#ifndef _SCHEDULER
#define _SCHEDULER 
#include <queue>

class Scheduler{
	private:
		int level;
		EventHandler eh;
		priority_queue<Process*> process_list;

	public:
		Scheduler(EventHandler _eh, priority_queue<Process*> _list);
		EventHandler geteh();
		int getlevel();
		priority_queue<Process> getlist();
		void setlevel(int l);
		Process* gettopprocess();	//--
		void addprocess(Process*);	//--
		void removetop();			//--
		void schedule();			//--


};

#endif