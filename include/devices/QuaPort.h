#ifndef _QUAPORT
#define _QUAPORT

#include <stdio.h>

#include "QuaTime.h"

class StabEnt;
class RosterView;
class Stream;
class ArrangerObject;

enum {
	QUA_DEV_NOT = 0,
	QUA_DEV_JOYSTICK = 1, // standard joystick data
	QUA_DEV_MIDI = 2, // regular midi port
	QUA_DEV_AUDIO = 3, // regular external audio device
	QUA_DEV_PARALLEL = 4, // historical name, but now to represent hardware ports in general, usb in particular for wierd devices
	QUA_DEV_VST = 5, // a vst or similar plugin, especially one with multiple pinouts or pinins
	QUA_DEV_CHANNEL = 6, // standard qua channel output, internal routing
	QUA_DEV_OSC = 7, // OSC message stream, port is a port on an internat connection
	QUA_DEV_SENSOR = 8 // stream of sensor data eg orientation on a tablet
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

template <class P> class QuaPortManager
{
public:
	QuaPortManager() {

	}

	qua_status status;

	std::vector<P*> ports;
	
	virtual status_t connect(Input *)=0;
	virtual status_t connect(Output *)=0;
	virtual status_t disconnect(Input *)=0;
	virtual status_t disconnect(Output *)=0;

	virtual QuaPort * findPortByName(const string name, int dirn, int nports=-1) = 0;

	bool destinationIndex(std::unordered_map<std::string, P *> &index) {
		for (short i = 0; i < countPorts(); i++) {
			if (Port(i)->mode & QUA_PORT_OUT) {
				index[Port(i)->sym->name] = Port(i);
			}
		}
		return true;
	}

	bool sourceIndex(std::unordered_map<std::string, P *> &index) {
		index.clear();
		for (short i = 0; i < countPorts(); i++) {
			if (Port(i)->mode & QUA_PORT_IN) {
				index[Port(i)->sym->name] = Port(i);
			}
		}
		return true;
	}

	inline int countPorts() {
		return ports.size();
	}
	inline P *port(int i) { 
		return i >= 0 && ((size_t)i) < ports.size() ? ports[i]: nullptr; 
	}
	inline void removePort(int i) { 
		ports.erase(ports.begin() + i);
	}
};

class QuaPort
	: public Stackable, public Insertable
{
public:
	QuaPort(std::string, short typ, short subt, short mode);

	status_t save(FILE *fp, short indent);

	virtual const char * name(uchar);
	virtual bool hasMode(const int mode);

	bool isControllable();
	bool isMultiSchedulable();
	
	short deviceType;
	short deviceSubType;
	short mode;
};

inline const string
portDirectionName(const int direction) {
	return string(direction == QUA_PORT_IN ? "in" : direction == QUA_PORT_OUT ? "out" : direction == QUA_PORT_IO ? "i/o" : "?");
}

#endif