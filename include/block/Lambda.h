#ifndef _METHOD
#define _METHOD

#include <stdio.h>
#include <ostream>

class Qua;
class Block;
class StabEnt;
class QuaControllerBridge;
class QuaObject;
class QuaMethodBridge;

#include "BaseVal.h"
#include "Executable.h"

class Lambda: public Executable
{
public:					
	status_t Save(ostream &out, short indent);

	Lambda(const std::string & nm, StabEnt * const S, bool f1=false,
		bool f2 = false,
		bool f3 = false,
		bool f4 = false,
		bool f5 = false,
		bool f6 = false);
	Lambda(Lambda *S, StabEnt *Sy);
	virtual ~Lambda();
	bool Init();
	
	bool isLocus;
	bool isModal;
	bool isOncer;
	bool isFixed;
	bool isHeld;
	bool isInit;
	
	TypedValue resetVal;
	
    Lambda *next;
};

Lambda *LoadMethod(FILE *fp, char *nm, StabEnt *S, Qua *q);

#endif
