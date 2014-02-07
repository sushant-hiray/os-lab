#include <iostream>
using namespace std;



Process::Process(int _pid,int _start,int _admission, vector<process_phase> _phase){
			p_id = _pid;
			admission = _admission;
			phases = _phase;
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