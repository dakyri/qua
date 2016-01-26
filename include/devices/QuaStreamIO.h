#ifndef _QUASTREAMIO
#define _QUASTREAMIO

class Qua;

#define QUA_INSERT_NAME_LENGTH	256

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