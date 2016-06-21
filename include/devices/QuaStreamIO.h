#ifndef _QUASTREAMIO
#define _QUASTREAMIO

class Qua;
class QuaPort;

#include "StdDefs.h"

class QuaStreamIO
{
public:
					QuaStreamIO(Qua *q, QuaPort *p);

	bool			connected;
	bool			enabled;
	status_t		status;

	QuaPort			*quaport;

	Qua				*uberQua;
};


#endif