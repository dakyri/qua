#ifndef _POOL
#define _POOL


#include <stdio.h>
#include <mutex>
#include <vector>

#include "StdDefs.h"

#include "MidiDefs.h"

// includes from qua source tree
#include "Stream.h"
#include "Schedulable.h"
#include "Instance.h"

class TypedValue;

class Block;
class Qua;
class StabEnt;
class QuasiStack;
struct IXMLDOMElement;

#include "Clip.h"

class Pool: public Schedulable
{
public:
						Pool(string nm, Qua *uq, StabEnt *c, bool);
						~Pool();
						
						virtual bool		Init() override;
	virtual Instance	*addInstance(std::string, Time startt, Time dur, Channel * chan) override;
	virtual void		removeInstance(Instance *i, bool display) override;

	virtual void		Cue(Time &t) override;
	virtual status_t	Wake(Instance *i) override;
	virtual	status_t	Sleep(Instance *i) override;
	virtual status_t	Recv(Stream &stream) override;

	StreamItem			*UpForGrabs(Stream *s, StreamItem *pos,
									Time &now);
	Time				UpForGrabs(Stream *s, Time &last,
									Time &now);
	StreamItem			*IndexTo(Time &t);

	status_t			Save(FILE *fp, short indent);
	status_t			SaveSnapshot(FILE *fp);

	status_t			LoadSnapshotElement(tinyxml2::XMLElement *);
	status_t			LoadSnapshotChildren(tinyxml2::XMLElement *element);
	
	Stream				*mainStream;
	Time				duration;
	std::mutex			flock;

	bool				SetRecordTake();
	StreamTake			*recordTake;

	StreamTake			*AddTake(char *, Time &t);
	status_t			DeleteTake(StreamTake *);
	status_t			SelectTake(StreamTake *);
	StreamTake			*selectedTake;
	std::vector<StreamTake*> outTakes;
	
	Block				*initBlock;
};

class PoolInstance: public Instance
{
public:
						PoolInstance(Pool *s, std::string nm, Time t, Time d, Channel * c);
						~PoolInstance();

	virtual void		Reset();
	virtual void		Cue(Time &t);
	virtual status_t	Run();
	
	StreamItem			*nextItem;
	bool				mute;
	
	Time				loopStart;
	Time				loopDuration;
	long				loopCount;
	Block				*loopCondition;
	
	Time				lastLoopStartTime;
};

#endif