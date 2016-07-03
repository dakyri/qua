#ifndef _EXECUTABLE
#define _EXECUTABLE

class Block;
class Lambda;

#include "Stackable.h"
#include "QuasiStack.h"

class Executable: public Stackable
{
public:
	Executable(StabEnt *S);
	virtual ~Executable();
    Block 				*mainBlock;
};

class Event: public Stackable
{
public:
						Event(char *nm, StabEnt *ctxt);
						~Event();
	void 				operator=(Event&s);
	void 				operator=(Block *);
	Block				*block;
};

#endif
