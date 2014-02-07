#include "Event.h"

class CompareTime {
public:
    bool operator()(Event* e1, Event* e2)
    {
       if (e1->get_etime() < e2->get_etime()) return true;
       return false;
    }
};

