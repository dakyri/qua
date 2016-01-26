#ifndef _METHOD
#define _METHOD

#include <stdio.h>

class Qua;
class Block;
class StabEnt;
class QuaControllerBridge;
class QuaObject;
class QuaMethodBridge;

#include "BaseVal.h"
#include "Executable.h"

class Method: public Executable
{
public:					
	status_t Save(FILE *fp, short indent);

	Method(std::string nm, StabEnt *S, Qua* q, bool f1=false,
		bool f2 = false,
		bool f3 = false,
		bool f4 = false,
		bool f6 = false,
		bool f7 = false);
	Method(Method *S, StabEnt *Sy);
	virtual ~Method();
	bool Init();
	
	bool isLocus;
	bool isModal;
	bool isOncer;
	bool isFixed;
	bool isHeld;
	bool isInit;
	
	TypedValue resetVal;
	
    Method *next;
};

Method *LoadMethod(FILE *fp, char *nm, StabEnt *S, Qua *q);

#endif
