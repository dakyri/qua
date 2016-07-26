#ifndef _INSTANCE
#define _INSTANCE

#include <stdio.h>

class Schedulable;
class StabEnt;
class ControlInfoList;
class QuasiStack;
class ControlVariable;
class Block;
class ControlPanel;
class Qua;

#include <mutex>

#include "QuaTime.h"
#include "Stacker.h"
#include "QuaTypes.h"
#include "QuasiStack.h"
#include "Stream.h"
#include "Notifiable.h"

namespace tinyxml2 {
	class XMLElement;
}

class Loggable
{
public:
	status_t SaveLogInfo(ostream &out, short indent);
	status_t SetLogValue(Block *b);

	Stream log;
	std::mutex logSem;
};
   
class Instance: public Notified,
				public Loggable
{
public:
	Instance(Schedulable &s, const std::string &nm, const Time &t, const Time &d, Channel *const c);
	virtual ~Instance();

	virtual Time RelativeTime(Time &t);
	
	virtual bool Init();
	virtual void Reset();
	virtual status_t Save(ostream &out, short indent);
	status_t SaveStackInfo(ostream &out, short indent);
	status_t SaveSnapshot(ostream &out);

	status_t LoadSnapshotElement(tinyxml2::XMLElement *);
	status_t LoadSnapshotChildren(tinyxml2::XMLElement *element);

	virtual void Cue(Time &t);
	virtual void Recv(Stream &stream);

	virtual void StartRecording();
	virtual void StopRecording();

	virtual status_t SetChannel(Channel *const, const bool);
	virtual status_t SetDuration(const Time &t, const bool);
	virtual status_t SetStartTime(const Time &t, const bool);
	virtual bool Move(const short ch_idx, const Time &startt, const Time &dur, bool const disp);

	virtual size_t Generate(float **bufs, long nf, short nc);	// add to out buf
	virtual status_t Run();

	bool SetStacks();
	bool SetValue(Block *b);
	bool ResetStacks();
	bool DestroyStacks();

	Schedulable &schedulable;
	Time startTime;
	Time duration;

	Qua *uberQua;
	Channel *channel;
	
	QuasiStack *mainStack;
	QuasiStack *wakeStack;
	QuasiStack *sleepStack;
	QuasiStack *cueStack;
	QuasiStack *rxStack;
	
    Time wokenAt;
    Time wakeDuration;
    
    StabEnt *sym;
    
    bool thruEnabled;
    
    Block *initBlock;
};

#endif
