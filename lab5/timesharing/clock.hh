#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <queue>
using namespace std;

class Clock
{
private:
	int curtime;
public:
	Clock();
	~Clock();
	int getcurtime();
	void incrementtime(int);
	void updateClock(int);

	/* data */
};