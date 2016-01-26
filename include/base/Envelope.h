#ifndef _ENVELOPE
#define _ENVELOPE

#if defined(WIN32)
#endif

#include <stdio.h>

#include "Time.h"

enum {
	ENV_SEG_TYPE_FIXED = 0,
	ENV_SEG_TYPE_LINEAR = 1
};

enum {
	ENV_RST_NOT = 0,
	ENV_RST_IMMEDIATE = 1,
	ENV_RST_LOOP_START = 2
};

#include <mutex>

struct EnvelopeSegment
{
	float		val;
	Time		time;
	uchar		type;
};

class Qua;
class StabEnt;
class Block;
class Stacker;
class QuasiStack;

class Envelope
{
public:
						Envelope(StabEnt *s, 
								Metric *m, bool add_seg, float v,
								Stacker *envper, StabEnt *stackCtxt, QuasiStack *qs);
						~Envelope();

	virtual status_t	Save(FILE *fp);
	bool				SetValue(Block *b);

	EnvelopeSegment		*Segment(int i);
	int					AddSegment(Time &t, float v, uchar type=ENV_SEG_TYPE_LINEAR, bool draw=true);
	bool				DelSegment(int i, bool draw=true);
	bool				MoveSegment(int i, Time &t, float v, bool draw=true);
	bool				MoveableSegment(int i, Time &t, float v);

	int					Segment(Time &t);
	bool				Cue(Time &t);
	void				Start(bool);
	bool				CueAndSet(Time &t, bool force=false);
	void				Print(FILE *);
	void				SetName(const char *nm);
	
	int					nSeg;
	EnvelopeSegment		*segment;
	int					segSize;

	StabEnt				*sym;
	QuasiStack			*stack;
		
	float				cueValue;
	Time				cueTime;
	int					cueSegment;
	Metric				*valueMetric;
	
// reset: at end, at loop start, at bar, at beat, ...
	ushort				rstFlags;
	std::mutex			segLock;
	Time				startTime;
	Stacker				*stacker;
	StabEnt				*stackCtxt;
};

#define DEFAULT_SEG 10

#endif