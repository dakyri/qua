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

#include <string>
using namespace std;

class Template: public Executable
{
public:					
	Template(string nm, StabEnt *S, long type, string mim, string path);
	~Template();

	bool initialize();
	status_t instantiate(StabEnt *q);
	
	status_t save(FILE *fp, short indent);
	
	long type;
	string mimeType;
	string path;
	
	Event	rx,
			tx,
			wake,
			sleep,
			receive,
			cue,
			start,
			stop,
			record,
			init;

	
	Template *next;
};

#endif