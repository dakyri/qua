#ifndef _TIMEKEEPER
#define _TIMEKEEPER

#include "QuaTime.h"

class TimeKeeper
{
public:
							TimeKeeper(Metric *m);
	Time					theTime;
	Metric					*metric;	
};

#endif