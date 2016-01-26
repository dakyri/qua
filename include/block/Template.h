#ifndef _TEMPLATE
#define _TEMPLATE

#ifdef __INTEL__ 
#include <InterfaceKit.h>
#endif

#include <stdio.h>

class Block;
class StabEnt;
class Template;
class Qua;
class Executable;

#include "QuaDisplay.h"
#include "Executable.h"

#if defined(WIN32)
class BBitmap;
#endif

class Template: public Executable
{
public:					
							Template(char *nm, StabEnt *S, long type, char *mim, char *path);
							~Template();
	bool					Init();
	status_t				Instantiate(StabEnt *q);
	
	status_t				Save(FILE *fp, short indent);
	
	long					type;
	char					*mimeType;
	char					*path;
	
	Event					rx,
							tx,
							wake,
							sleep,
							receive,
							cue,
							start,
							stop,
							record,
							init;

	
	Template				*next;
};

#endif