#ifndef _NOTE
#define _NOTE

#if defined(WIN32)

#include "StdDefs.h"

#else

#ifdef __INTEL__
#include <SupportDefs.h>
#endif

#endif

#include "Properties.h"


typedef unsigned char		pitch_t;
typedef unsigned char		cmd_t;
typedef unsigned char		vel_t;
typedef unsigned char		ctrl_t;
typedef unsigned char		amt_t;
typedef unsigned char		prg_t;
typedef char				interval_t;
typedef uint32				dur_t;
typedef int16				bend_t;
typedef int8				sysc_data_t;
typedef	uchar				bank_t;

// note class: my notes, midi noteon, keypress, noteoff
class Note: public Attributable {
public:
 	inline void
	Set(pitch_t p, vel_t v, dur_t d, cmd_t c)
	{
		pitch = p;
		dynamic = v;
		duration = d;
		cmd = c;
	}
	dur_t		duration;
	cmd_t		cmd;		/* raw cmd and channel */
    pitch_t		pitch;		/* midi note # */
    vel_t		dynamic;
};

class Ctrl {
public:
	inline void		Set(ctrl_t ct, amt_t a, cmd_t c)
	{
		cmd = c;
		amount = a;
		controller = ct;
	}
	cmd_t		cmd;		/* ... midi channel */
    ctrl_t		controller;
    amt_t		amount;
};

class SysC {
public:
	inline void
	Set(cmd_t c, sysc_data_t d1, sysc_data_t d2)
	{
		cmd = c;
		data1 = d1;
		data2 = d2;
	}
	cmd_t		cmd;
	sysc_data_t	data1;
	sysc_data_t	data2;
};

#define NON_PROG	0xff
class Prog {
public:
	inline void
	Set(prg_t p, bank_t b, bank_t sb, cmd_t c)
	{
		cmd = c;
		program = p;
		bank = b;
		subbank = sb;
	}
	cmd_t		cmd;		/* ... midi channel */
    prg_t		program;
    bank_t		bank;
    bank_t		subbank;
};

class Bend {
public:
	inline void
	Set(bend_t b, cmd_t c)
	{
		cmd = c;
		bend = b;
	}
	cmd_t		cmd;		/* ... midi channel */
	bend_t		bend;
};

class SysX {
public:
	inline void
	Set(char *dta, ulong len)
	{
		data = dta;
		length = len;
	}
	char		*data;
	int			length;
};

#endif
