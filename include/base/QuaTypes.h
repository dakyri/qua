#ifndef _QUA_TYPES
#define _QUA_TYPES

#include "StdDefs.h"

typedef short qua_status;
typedef short qua_chan_id;
typedef	uint32 q_size_t;
typedef	int8 base_type_t;
typedef int16 port_chan_id;

enum {
	STATUS_UNKNOWN = -1,
	STATUS_DEAD	= 0,
	STATUS_UNLOADED = 0,		// same meaning, but for a file/object not found
	STATUS_RUNNING = 1,
	STATUS_LOADED = 1,		// same meaning, but for a file/object loaded externally e.g. a sample file
	STATUS_SLEEPING	= 2,
	STATUS_RECORDING = 3,
	STATUS_CLEANING_UP = 4
};

enum {
	MOUSE_TRACK_NOT = 0,
	MOUSE_TRACK_RESIZE = 1,
	MOUSE_TRACK_RESIZE_NOTE = 2,
	MOUSE_TRACK_MOVE_ENV_SEG = 3,
	MOUSE_TRACK_MOVE_POINT = 4,
	MOUSE_TRACK_MOVE_KEYPRESS = 5,
	MOUSE_TRACK_MOVE_NOTE = 6,
	MOUSE_TRACK_MOVE_CTRL = 7,
	MOUSE_TRACK_MOVE_LOGENTRY = 8,
	MOUSE_TRACK_SELECT_REGION = 9,
	MOUSE_TRACK_DO_SQUAT = 10,
	MOUSE_TRACK_SCROLL = 11
};


#endif