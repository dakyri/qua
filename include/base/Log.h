#ifndef _LOGENTRY
#define _LOGENTRY

#include "BaseVal.h"
#include "QuaTime.h"

enum {
	ACTION_NONE		= 0,
	ACTION_CALL		= 1,
	ACTION_FILE		= 2
};

/**
 * LogEntry ... an otherwise unclassifiable action
 * initially this existed as a placeholder to simulate patch loading from files into various external apps
 * that was a very BeOS-ish quirk, and maybe XXXX TODO FIXME this should dissappear as this functionaliity
 * gets subsumed by OSC
 */
class LogEntry
{
public:
				LogEntry(long t=ACTION_NONE);
				
	long		type;
};

#endif