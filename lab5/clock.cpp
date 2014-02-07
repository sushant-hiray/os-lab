#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <queue>
#include "clock.hh"

using namespace std;


Clock::Clock(){
	curtime=0;
}

int Clock::getcurtime(){
	return curtime;
}

void Clock::incrementtime(int _time){
	curtime+=_time;
}

void Clock::updateClock(int _time){
	curtime = _time;
}