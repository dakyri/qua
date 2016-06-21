#ifndef _CLOCK
#define _CLOCK

#include <thread>
#include <mutex>
#include <condition_variable>

class Clock
{
public:
					Clock(bool ext);
					~Clock();
	void			Start();
	void			Stop();
	void			Zero();
	void			Set(bigtime_t to);
	void			Set(float to);
	void			SetTimeSource(bool);
	bigtime_t		Counter();
	static int32	SpringWrapper(void *);
	int32			SpringBody();
	void			Tick();
	bigtime_t		CounterTime();
	bigtime_t		USecTime();
	double			MSecTime();
	bigtime_t		WakeMeAfter(bigtime_t usec, bigtime_t timeout);
	void			UnsetAlarm();

	bigtime_t		MSec2Count(double);
	double			Count2MSec(bigtime_t);
	bigtime_t		USec2Count(bigtime_t);
	bigtime_t		Count2USec(bigtime_t);

private:
#ifdef WIN32
	double			usecsPerCount;
	int64			performanceFrequency;
#endif

	bigtime_t		startTime;
	bigtime_t		theTime;
	bigtime_t		alarmTime;
	bigtime_t		timingError;
	
	bool			theEndOfTime;
	bool			alarmOn;
	bool			externalTimeSource;

	std::thread		theWorks;
	std::condition_variable spring;
	std::unique_lock<std::mutex> underTheSheets;
};

#endif