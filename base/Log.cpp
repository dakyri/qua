#include "qua_version.h"

#if defined(WIN32)


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "Log.h"

LogEntry::LogEntry(long t)
{
	type = t;
}
