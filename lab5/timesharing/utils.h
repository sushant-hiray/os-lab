#ifndef _UTILS
#define _UTILS
#include <iostream>
#include "clock.hh"
#include <queue>
#include <vector>

#define DEBUG 0
#define IFBUG if(DEBUG) { 
#define ENDBUG }

const static char* state_type[] = {"Admission","IOStart", "IOStop","Timeslice"};
using namespace std;

extern Clock* myclock;

enum state { READY , BLOCKED, RUNNING };
enum E_Type {Admission, IOStart, IOStop, Timeslice};
//static EventHandler* eh;

struct process_phase{
	int iterations;
	int cpu_time;
	int io_time;
};

struct process{
	int p_id;
	int start_priority;
	int admission;
	vector<process_phase> phases;
};

struct sc_level{
	int level_number;
	int priority;
	int time_slice;
};

struct sscheduler{
	int no_levels;
	vector<sc_level> levels;
};



#endif