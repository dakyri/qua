#ifndef _NOTE
#define _NOTE

#include "StdDefs.h"
#include "Attributes.h"


typedef unsigned char pitch_t;
typedef unsigned char cmd_t;
typedef unsigned char vel_t;
typedef unsigned char ctrl_t;
typedef unsigned char amt_t;
typedef unsigned char prg_t;
typedef char interval_t;
typedef uint32 dur_t;
typedef int16 bend_t;
typedef int8 sysc_data_t;
typedef	uchar bank_t;

/*
 * we're using these inside a union. this precludes initializers and nontrivial assignment.
 * (according to VS, but not to the spec, but we gotta go with VS usability for the moment)
 */
class Note {
public:
 	inline Note& set(pitch_t p=0, vel_t v=0, dur_t d=0, cmd_t c=0) {
		pitch = p;
		dynamic = v;
		duration = d;
		cmd = c;
		return *this;
	}
	dur_t duration;
	cmd_t cmd;		/* raw cmd and channel */
    pitch_t pitch;		/* midi note # */
    vel_t dynamic;
};

class Ctrl {
public:
	inline Ctrl& set(ctrl_t ct=0, amt_t a=0, cmd_t c=0) {
		cmd = c;
		amount = a;
		controller = ct;
		return *this;
	}
	cmd_t cmd;		/* ... midi channel */
    ctrl_t controller;
    amt_t amount;
};

class SysC {
public:
	inline SysC& set(cmd_t c=0, sysc_data_t d1=0, sysc_data_t d2=0) {
		cmd = c;
		data1 = d1;
		data2 = d2;
		return *this;
	}
	cmd_t cmd;
	sysc_data_t data1;
	sysc_data_t data2;
};

#define NON_PROG	0xff
class Prog {
public:
	inline Prog& set(prg_t p=0, bank_t b=0, bank_t sb=0, cmd_t c=0) {
		cmd = c;
		program = p;
		bank = b;
		subbank = sb;
		return *this;
	}
	cmd_t cmd;		/* ... midi channel */
    prg_t program;
    bank_t bank;
    bank_t subbank;
};

class Bend {
public:
	inline Bend& set(bend_t b=0, cmd_t c=0) {
		cmd = c;
		bend = b;
		return *this;
	}
	cmd_t cmd;		/* ... midi channel */
	bend_t bend;
};

/*
 * a container for sysx data. we are part of a union ... we need to be super careful
 * the sysx data is unique. single ownership of the data is transferred by assignment.
 * deletion is explicit!!!!
 * explicit initialization is vital for sysX
 * xxxx todo maybe there is a better way, but not with revising the 
 */
class SysX {
public:
	inline SysX& set(char *dta=nullptr, ulong len=0) {
		data = dta;
		length = len;
		return *this;
	}
	/* problematic in VS because of the union */
	/*
	inline SysX& operator=(SysX & x) {
		if (this != &x) {
			data = x.data;
			length = x.length;
			x.data = nullptr;
			x.length = 0;
		}
		return *this;
	}
	inline SysX& operator=(SysX && x) {
		if (this != &x) {
			data = x.data;
			length = x.length;
			x.data = nullptr;
			x.length = 0;
		}
		return *this;
	}*/
	inline SysX& clear() {
		if (data != nullptr) delete data;
		data = nullptr;
		length = 0;
		return *this;
	}
	/* we probably don't need this literally the streamitem call can be cloned, though so on the off chance, we wrap
	   what's needed here
	   */
	inline SysX& clone() {
		if (data != nullptr) {
			char *d = new char[length];
			memcpy(d, data, length);
			return SysX().set(d, length);
		}
		return SysX().set();
	}
	char *data;
	int	length;
};

#endif
