#include "qua_version.h"
 
#include "StdDefs.h"

#if defined(WIN32)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#endif

#include "Clock.h"

// Clock
// runs from 0 till late o clock
// the time is measured in integer ticks
// ultimately the time is returned in micro secs

flag	debug_clock = 0;

bigtime_t
Clock::Counter()
{
#if defined(WIN32)
	LARGE_INTEGER	pcv;
	QueryPerformanceCounter(&pcv);
	return pcv.QuadPart;
#endif
}

bigtime_t
Clock::CounterTime()
{
    return Counter() - startTime;
}

bigtime_t
Clock::USecTime()
{
	return Count2USec(CounterTime());
}

double
Clock::MSecTime()
{
	return Count2MSec(CounterTime());
}

bigtime_t
Clock::MSec2Count(double ms)
{
	return bigtime_t((ms/1000)*performanceFrequency);
}

double	
Clock::Count2MSec(bigtime_t t)
{
	return double(1000*(t/((double)performanceFrequency)));
}

bigtime_t
Clock::USec2Count(bigtime_t us)
{
	return bigtime_t((us/1000000.0)*performanceFrequency);
}

bigtime_t
Clock::Count2USec(bigtime_t t)
{
	return bigtime_t(1000000.0*(t/((double)performanceFrequency)));
}

void
Clock::Zero()
{
	startTime = Counter();
	alarmTime = 0;
	theTime = 0;
}

void
Clock::Start()
{
	startTime = Counter();
	theTime = 0;
	alarmTime = 0;
	if (!externalTimeSource) {
		SetTimeSource(false);
		SetTimeSource(true);
	}
}

void
Clock::Set(bigtime_t time_to)
{
	startTime = Counter() - time_to;
}

void
Clock::Set(float time_to)
{
	startTime = Counter() - bigtime_t(time_to*performanceFrequency);
}

//  called by someone who owns the clock.
// blocks on the clock's bell semaphore, which is
// triggered at alarmTime
bigtime_t
Clock::WakeMeAfter(bigtime_t alm, bigtime_t timeout)
{
	alarmTime = (alm/usecsPerCount);
	if (theTime < alarmTime) {
		alarmOn = true;
		if (debug_clock)
			fprintf(stderr, "Waiting on alarm clock till %gms @ %gms\n", alm/1000.0, Count2MSec(theTime));
		status_t	err;

		spring.wait(underTheSheets);
	} else {
		if (debug_clock)
			fprintf(stderr, "The wait time is now: %gms @ %gms\nn", alm/1000.0, Count2MSec(theTime));
	}
	return theTime*usecsPerCount;
}

void
Clock::UnsetAlarm()
{

	alarmOn = false;
}

void
Clock::Tick()
{
	theTime = CounterTime();
	if (debug_clock)
		fprintf(stderr, "tick %gms\n", Count2MSec(theTime));
	if (alarmOn && theTime >= (alarmTime-timingError)) {
		alarmOn = false;
	}
}

int32
Clock::SpringBody()
{
	while (!theEndOfTime) {
		Tick();
#if defined(WIN32)
		Sleep(1);
#else
		usleep(0);
#endif
	}
	return B_OK;
}

Clock::Clock(bool e)
{
#if defined(WIN32)
	LARGE_INTEGER	pcf;
	QueryPerformanceFrequency(&pcf);
	printf("performance freq %Ld, err %d\n", pcf.QuadPart, GetLastError());
	performanceFrequency = pcf.QuadPart;
	usecsPerCount = 1000000.0/((double)performanceFrequency);
	timingError = MSec2Count(1);
#endif
	theEndOfTime = false;
	alarmOn = false;
	alarmTime = 0;
	theTime = 0;
	startTime = Counter();
	SetTimeSource(e);
}

void
Clock::SetTimeSource(bool e)
{
	externalTimeSource = e;
	if (!e) {
		theWorks = std::thread(SpringWrapper, this);
	} else {
		theEndOfTime = true;
		theWorks.join();
	}
}

int32
Clock::SpringWrapper(void *x)
{
	return ((Clock*)x)->SpringBody();
}

Clock::~Clock()
{
	theEndOfTime = true;
	UnsetAlarm();
	theWorks.join();
}
