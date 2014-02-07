#include <iostream>
using namespace std;

struct process_phase{
	int iterations;
	int cpu_time;
	int io_time;
};

enum state { READY , BLOCKED };

class Process{
	private:
		int current_phase;
		int current_iteration;
		int p_id;
		int start_priority;
		int admission;
		state cur_state;
		vector<process_phase> phases;
		int complete_time;

	public:
		Process(int _pid,int _start,int _admission, vector<process_phase> _phase);
		void setcurphase(int _phase);
		void setcuritr(int _itr);
		int getcurphase();
		int getcuritr();
		int getpriority();
		int getadmission();
		state getstate();
		void setstate(state _state);
		vector<process_phase> getphases();
		void setcompletetime(int _time){
			complete_time=_time;
		}

		int getcompletetime(){
			return complete_time;
		}

		int lefttime(){
			return phases[current_phase].cpu_time - complete_time;
		}
		
};