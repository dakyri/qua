#ifndef _TIMEKEEPER
#define _TIMEKEEPER

#include "include/Time.h"

class TimeKeeper
{
public:
							TimeKeeper(Metric *m);
	Time					theTime;
	Metric					*metric;	
};

#endif