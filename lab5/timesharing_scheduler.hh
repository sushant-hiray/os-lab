#ifndef _TIMESCHEDULER
#define _TIMESCHEDULER 
#include <queue>
#include "Event.h"

class TimeSharingScheduler{
	private:
		int level;
		//EventHandler eh;
		queue<Process*> process_list;
		Process* current_process;
		int timeslice;

	public:
		TimeSharingScheduler(queue<Process*> _list, int);
		//EventHandler geteh();
		int getlevel();
		queue<Process*> getlist();
		void setlevel(int l);
		Process* gettopprocess();	//--
		void addprocess(Process*);	//--
		void removetop();			//--
		void schedule();			//--
		void handleadmission(Process*);
		void settopprocess(Process*);

};

#endif