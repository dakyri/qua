#ifndef _LOGENTRY
#define _LOGENTRY

#include "BaseVal.h"
#include "QuaTime.h"

enum {
	ACTION_NONE		= 0,
	ACTION_CALL		= 1,
	ACTION_FILE		= 2
};


class LogEntry
{
public:
				LogEntry(long t=ACTION_NONE);
				
	long		type;
};

#endif