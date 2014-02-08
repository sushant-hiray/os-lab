#include <iostream>       // std::cout
#include <queue>   

using namespace std;

priority_queue<int> event_table;

void removeio(int n){
	priority_queue<int> tempqueue;
	while(!event_table.empty()){
		if(event_table.top() == n){
			event_table.pop();
			while(!tempqueue.empty()){
				event_table.push(tempqueue.top());
				tempqueue.pop();
			}
			break;
		}
		else{
			tempqueue.push(event_table.top());
			event_table.pop();
		}
	}
}

void myprint(){
	while(!event_table.empty()){
		cout<<event_table.top()<<" ";
		event_table.pop();
	}
}

int main(){
	for(int i=0; i<10;i++){
		event_table.push(i);
	}
	//myprint();
	removeio(7);
	myprint();
	return 0;
}