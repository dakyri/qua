#ifndef _NOTIFIED
#define _NOTIFIED

#include "Executable.h"
#include "Stacker.h"

#include <stdio.h>

class Stacker;
class Event;
class StabEnt;

class Notifiable: public Executable
{
public:
	Notifiable(StabEnt *S);
	virtual ~Notifiable();

	virtual status_t QuaStart()=0;
	virtual status_t QuaStop()=0;
	virtual status_t QuaRecord()=0;
	virtual void Notification(Block *B, Stacker *i, StabEnt *, QuasiStack *S);

	Event stop;
	Event start;
	Event record;
};


class Notified: public Stacker
{
public:
	Notified(Qua *q);
	virtual ~Notified();
	bool SetValue(Block *b);
	void SaveStackInfo(ostream &out, short indent);
	bool Init(Notifiable *, StabEnt *, QuasiStack *);
	
	QuasiStack *startStack;
	QuasiStack *stopStack;
	QuasiStack *recordStack;
};	
#endif
