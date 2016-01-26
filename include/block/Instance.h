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

#include "Time.h"
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
	status_t SaveLogInfo(FILE *fp, short indent);
	status_t SetLogValue(Block *b);

	Stream log;
	std::mutex logSem;
};
   
class Instance: public Notified,
				public Loggable
{
public:
	Instance(Schedulable *s, std::string nm, Time t, Time d, Channel *c);
	virtual ~Instance();

	virtual Time		RelativeTime(Time &t);
	
	virtual bool		Init();
	virtual void		Reset();
	virtual status_t	Save(FILE *fp, short indent);
	status_t			SaveStackInfo(FILE *fp, short indent);
	status_t			SaveSnapshot(FILE *fp);

	status_t			LoadSnapshotElement(tinyxml2::XMLElement *);
	status_t			LoadSnapshotChildren(tinyxml2::XMLElement *element);

	virtual void		Cue(Time &t);
	virtual void		Recv(Stream &stream);

	virtual void		StartRecording();
	virtual void		StopRecording();

	virtual status_t	SetChannel(Channel *, bool);
	virtual status_t	SetDuration(Time &t, bool);
	virtual status_t	SetStartTime(Time &t, bool);
	virtual bool		Move(short ch_idx, Time *startt, Time *dur, bool disp);

	virtual size_t		Generate(float **bufs, long nf, short nc);	// add to out buf
	virtual status_t	Run();

	bool				SetStacks();
	bool				SetValue(Block *b);
	bool				ResetStacks();
	bool				DestroyStacks();

	Qua					*uberQua;

	Time				startTime;
	Time				duration;

	Channel				*channel;
	Schedulable			*schedulable;
	
	QuasiStack			*mainStack;
	QuasiStack			*wakeStack;
	QuasiStack			*sleepStack;
	QuasiStack			*cueStack;
	QuasiStack			*rxStack;
	
    Time				wokenAt;
    Time				wakeDuration;
    
    StabEnt				*sym;
    
    bool				thruEnabled;
    
    Block				*initBlock;
};

#endif
