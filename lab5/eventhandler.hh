#ifndef _EVENT_HANDLER
#define _EVENT_HANDLER
#include <queue>

class CompareTime {
public:
    bool operator()(Event* e1, Event* e2)
    {
       if (e1->get_etime() > e2->get_etime()) return true;
       return false;
    }
};

class EventHandler
{
		priority_queue<Event*, vector<Event*>, CompareTime> event_table;
   	public:
   		EventHandler(priority_queue<Event*, vector<Event*>, CompareTime>);
   		void eventaction();
   		void addevent(Event* e);
   		void popevent();
      Event* gettopevent();
      void removeio(Process* process);
      void eprint(){
        while(!event_table.empty()){
          Event* e = event_table.top();
          cout<<e->get_etime()<< "   "<< e->get_epid()<< "   "<< e->get_etype()<<endl;
          vector<process_phase> phases=e->getprocess()->getphases();
          for(int i=0;i<phases.size();i++){
            cout<<"\t"<<phases[i].iterations<<"   "<< phases[i].cpu_time<<"    "<<phases[i].io_time<<endl; 
          }
          event_table.pop();
        }
      }
};

#endif