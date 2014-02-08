#include <iostream>
#include "process.hh"


extern Clock* myclock;

using namespace std;


Process::Process(int _pid,int _start,int _admission, vector<process_phase> _phase){
			p_id = _pid;
			admission = _admission;
			phases = _phase;
			current_phase = 0;
			current_iteration = 1;
			start_priority = _start;
		}

void Process::setcurphase(int _phase){
	current_phase = _phase;
}

void Process::setcuritr(int _itr){
	current_iteration = _itr;
}

int Process::getcurphase(){
	return current_phase;
}

int Process::getcuritr(){
	return current_iteration;
}

int Process::getpriority(){
	return start_priority;
}

int Process::getadmission(){
	return admission;
}

state Process::getstate(){
	return cur_state;
}

void Process::setstate(state _state){
	cur_state = _state;
}
vector<process_phase> Process::getphases(){
	return phases;
}

int Process::getiostop(){
	return phases[current_phase].io_time;
}

//1 -> complete 0->ongoing
int Process::completeiteration(){
	int expected=phases[current_phase].iterations;
	if(current_iteration < expected){
		current_iteration++;
		return 0;
	}
	else if(current_iteration == expected){
		current_iteration = 1;
		int expected_phase = phases.size() - 1;
		if(current_phase < expected_phase){
			current_phase++;
			return 0;
		}
		else if(current_phase == expected_phase){
			return 1;
		}
	}
}

void Process::setadmission(int _time){
	admission = _time;
}

int Process::getiostart(){
	return lefttime();
}

void Process::savestate(){
	int cur_time = myclock->getcurtime();
	int com_time = cur_time - admission;
	if (com_time < phases[current_phase].cpu_time){
		complete_time = com_time;
	}
	else{
		complete_time=0;
	}
	//cout<<" completed time of process: "<<getpid()<<" is "<<complete_time<<endl;
}