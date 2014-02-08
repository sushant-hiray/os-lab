#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include "scheduler.hh"
#include "utils.h"
#include "eventhandler.hh"
#include <queue>
using namespace std;



Scheduler* scheduler;
EventHandler* eh;
priority_queue<Event*, vector<Event*>, CompareTime> event_table;
Clock* myclock;
priority_queue<Process*, vector<Process*>, ComparePriority> schedule_list;

vector<process> process_list;
sscheduler my_scheduler;
//priority_queue<Process*> _list
vector<Process*> p_list;

void process_proc_file(){
	string line, line2;
	int pid, prior;
	int adm;
	int iter;
	int cpu_t, io_t;
	ifstream infile("PROCESS_SPEC");
	while (std::getline(infile, line))
	{
		if(line=="PROCESS"){
			process proc;
			getline(infile, line2);
			std::istringstream iss(line2);
		    if (!(iss >> pid >> prior >> adm)) { break; } // error
		    // cout<<pid<<endl<<prior<<endl;
			
			proc.p_id = pid;
			proc.start_priority = prior;
			proc.admission = adm;

			getline(infile, line2);
			while(line2 != "END"){
				std::istringstream iss(line2);
				process_phase pp;
			    if (!(iss >> iter >> cpu_t >> io_t)) { break; } // error
			    // cout<<cpu_t<<endl<<io_t<<endl;
			    
			    pp.iterations = iter;
			    pp.cpu_time = cpu_t;
			    pp.io_time = io_t;
			    (proc.phases).push_back(pp);
			    getline(infile, line2);
			}
			process_list.push_back(proc);
			//Process(int _pid,int _start,int _admission, vector<process_phase> _phase);
			Process* p = new Process(proc.p_id, proc.start_priority, proc.admission, proc.phases);
			p_list.push_back(p);

		}
	}
	// cout<<(process_list[1].phases[1]).io_time<<endl;
	// return 0;
}

void process_scheduler_file(){
	string line, line2;
	int level_count;
	int prior;
	int s_lvl;
	int t_slice;
	ifstream infile("SCHEDULER_SPEC");
	while (std::getline(infile, line))
	{
		if(line=="SCHEDULER"){
			getline(infile, line2);
			std::istringstream iss(line2);
		    if (!(iss >> level_count)) { break; } // error
		    // cout<<pid<<endl<<prior<<endl;
			
			my_scheduler.no_levels = level_count;
			for(int i=0; i<level_count; i++){
				getline(infile, line2);
				std::istringstream iss(line2);
				if (!(iss >> s_lvl >> prior >> t_slice)) { break; } // error
				sc_level scl;
				scl.level_number = s_lvl;
				scl.priority = prior;
				scl.time_slice = t_slice;
				my_scheduler.levels.push_back(scl);
			}
		}
	}
}

int main(){
	eh = new EventHandler(event_table);
	scheduler= new Scheduler(schedule_list);
	process_proc_file();
	myclock=new Clock();
	//cout<<p_list.size();
	for(int i=0;i<p_list.size();i++){
		Event* e = new Event(p_list[i]->getadmission(), p_list[i]->getpid(), Admission, p_list[i]);
		eh->addevent(e);
	}
	//eh->eprint();
	//cout<<(process_list[1].phases[1]).io_time<<endl;
	


	while(eh->gettopevent()!=NULL){
		myclock->updateClock(eh->gettopevent()->get_etime());
		//cout<<"in while"<<endl;
		eh->eventaction();
		
	}












	// process_scheduler_file();
	// cout<<my_scheduler.levels[1].time_slice<<endl;
}