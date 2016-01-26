#ifndef _QUAPORT
#define _QUAPORT

#include <stdio.h>

#include "Time.h"

class StabEnt;
class RosterView;
class Stream;
class ArrangerObject;

enum {
	QUA_DEV_NOT			= 0,
	QUA_DEV_JOYSTICK	= 1,
	QUA_DEV_MIDI		= 2,
	QUA_DEV_AUDIO		= 3,
	QUA_DEV_PARALLEL	= 4
};

enum {
	QUA_AUDIO_GENERIC = 0,
	QUA_AUDIO_ASIO = 1,
	QUA_AUDIO_MME = 2,
	QUA_AUDIO_DX = 3
};

enum {
	QUA_JOY_GENERIC = 0,
	QUA_JOY_DX = 1,
	QUA_JOY_MME = 2
};

enum {
	QUA_PORT_UNKNOWN = 0,
	QUA_PORT_IN = 1,
	QUA_PORT_OUT = 2,
	QUA_PORT_IO = 3			// note that this is a bitmap IN|OUT
};

enum {
	QUA_DEV_GENERIC = 0
};

class QuaPort;
class Input;
class Output;
class QuaPortBridge;

#include <vector>

#include "Stackable.h"
#include "QuasiStack.h"
#include "Insertable.h"

class QuaPortManager
{
public:
	qua_status			status;

	std::vector<QuaPort*> ports;
	
	virtual status_t	connect(Input *)=0;
	virtual status_t	connect(Output *)=0;
	virtual status_t	disconnect(Input *)=0;
	virtual status_t	disconnect(Output *)=0;

	inline int countPorts() { return ports.size(); }
	inline QuaPort *port(int i) { return i >= 0 && i < ports.size() ? ports[i]: nullptr; }
	inline void removePort(int i) { ports.erase(ports.begin() + i); }
};

class QuaPort
	: public Stackable, public Insertable
{
public:
						QuaPort(std::string, short typ, short subt, short mode);


	status_t			save(FILE *fp, short indent);

	virtual char *		name(uchar);

	bool				isControllable();
	bool				isMultiSchedulable();
	
//	inline PortObject	*PortObjectView() { return (PortObject *)representation; }
	
	short				deviceType;
	short				deviceSubType;
	short				mode;

};


#endif