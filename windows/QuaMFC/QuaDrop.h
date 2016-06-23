#pragma once

#include "afxole.h"

class Schedulable;
class Voice;
class Sample;
class Instance;
class StabEnt;
class Clip;

#include <vector>
#include <string>
using namespace std;

union u_quadrop_info
{
	vector<string> *filePathList;
	Voice		*voice;
	Clip		*clip;
	Sample		*sample;
	Instance	*instance;
	Schedulable	*schedulable;
	StabEnt		*symbol;
};

struct qua_symbol_clip
{
	StabEnt		*symbol;
};

class QuaDrop
{
public:
	QuaDrop(void);
	~QuaDrop(void);

	static bool		Initialize();

	static UINT		instanceFormat;
	static UINT		clipFormat;
	static UINT		voiceFormat;
	static UINT		sampleFormat;
	static UINT		schedulableFormat;
	static UINT		portFormat;
	static UINT		builtinFormat;
	static UINT		templateFormat;
	static UINT		vstpluginFormat;
	static UINT		methodFormat;

	bool			SetTo(COleDataObject *, DWORD);
	void			Clear();

	static bool		SetSymbolSource(COleDataSource *, UINT, StabEnt *);

	enum DropType {
		NOTHING = 0,
		FILES = 1,
		AUDIOFILES = 2,
		MIDIFILES = 3,
		APPLICATION = 4,
		VOICE = 5,
		SAMPLE = 6,
		SCHEDULABLE = 7,
		INSTANCE = 8,
		PORT = 9,
		VSTPLUGIN = 10,
		BUILTIN = 11,
		TEMPLATE = 12,
		METHOD = 13,
		CLIP = 14,
		UNKNOWN = 15
	};

	int				type;
	int				count;
	DWORD			keyState;
	DROPEFFECT		dropEffect;

	u_quadrop_info	data;
};


enum pixel_scale_t {
	TICK = 0,
	BEAT = 1,
	BAR =2
};

enum action_t {
	QUA_MOUSE_ACTION_NONE = 0,
	QUA_MOUSE_ACTION_MOVE_INSTANCE = 1,
	QUA_MOUSE_ACTION_SIZE_INSTANCE = 2,
	QUA_MOUSE_ACTION_DRAW_INSTANCE = 3,
	QUA_MOUSE_ACTION_REGION_SELECT = 4,
	QUA_MOUSE_ACTION_MOVE_EVENT = 5,
	QUA_MOUSE_ACTION_SIZE_EVENT = 6,
	QUA_MOUSE_ACTION_DRAW_EVENT = 7
};
