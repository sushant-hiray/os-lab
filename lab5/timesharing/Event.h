#ifndef _EVENT
#define _EVENT
#include "process.hh"



class Event{
private:
	int etime;
	int epid;
	E_Type etype;
	Process* process;

public:
	Event(int, int, E_Type,Process*);
	~Event();

	int get_etime();
	int get_epid();
	E_Type get_etype();
	Process* getprocess();
	void action();
};

#endif