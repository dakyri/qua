#ifndef _CONTROLVAR
#define _CONTROLVAR

#include <stdio.h>

class StabEnt;
class QuasiStack;
class Instance;
class EnvelopePanel;
class Envelope;
class Stacker;
class Block;
class TimeKeeper;

// a bridge between the world of symbols with multiple parallel values in the stack,
// and interface elements which might represent them
class QuaControllerBridge
{
public:
	 QuaControllerBridge(	TimeKeeper *,
							Stacker *,
							QuasiStack *,
							StabEnt *p,
							StabEnt *);
	 ~QuaControllerBridge();

	void SetDisplayValue();
	void SetSymbolValue();

	status_t Save(FILE *fp, short indent);

	bool SetValue(Block *b);

	void DrawNewEnvSegment(Envelope *, long);
	void DrawNewEnvSegPosition(Envelope *, long, bool);
	
	QuasiStack *stackFrame;
	StabEnt *sym;
	Stacker *stacker;
	StabEnt *stackCtxt;
	Envelope *envelope;
};

#endif