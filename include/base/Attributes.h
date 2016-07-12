#ifndef _PROPERTY
#define _PROPERTY

#include <unordered_map>
#include <string>
using namespace std;

class Attribute {
public:
	union value_u {
		;
	};

	typedef short idcode;
	typedef union value_u value;

	Attribute(idcode ty, value val);
	~Attribute();
	idcode id;
	value val;
	Attribute *next=nullptr;

	enum {
		NODE = 1,
		MODAL = 2,
		ONCER = 3,
		EXCEPT = 4,
		RESET = 5,
		TOGGLE = 6,
		HELD = 7,
		RESETTO = 8,
		FIXED = 9,
		LABEL = 10,
		RANGE = 11,
		TRIGGER = 12,
		ENVELOPE = 13,
		RETURN = 14,
		MIME = 16,

		ENABLE = 17,	// whether the vst/whatever component is loadable
		DISABLE = 18,
		LOAD = 19,	// load this dll or not, and define it and its parameters, etc
		NOLOAD = 20,

		INS = 21,
		OUTS = 22,
		ID = 23,

		MIDITHRU = 24,
		AUDIOTHRU = 25,
		NO_MIDITHRU = 26,
		NO_AUDIOTHRU = 27,

		MAP_VSTPARAM = 28,
		SYNTH = 29,

		NPARAM = 30,
		NPROGRAM = 31,

		DEVICE_AUDIO = 32,
		DEVICE_MIDI = 33,
		DEVICE_JOYSTICK = 34,
		DEVICE_PARALLEL = 35,
		DEVICE_OSC = 36,
		DEVICE_SENSOR = 37,

		CELL_START = 40,
		PHRASE_END = 41,

		PATH = 90,	// following need to be out of range of regular types
		TAKES = 91,
		STREAM = 92,
		SAMPLE = 93,

		BUILTIN_EVENT = 100,
		EVENT_RX = BUILTIN_EVENT + 0,
		EVENT_TX = BUILTIN_EVENT + 1,
		EVENT_RECEIVE = BUILTIN_EVENT + 2,
		EVENT_WAKE = BUILTIN_EVENT + 3,
		EVENT_SLEEP = BUILTIN_EVENT + 4,
		EVENT_START = BUILTIN_EVENT + 5,
		EVENT_STOP = BUILTIN_EVENT + 6,
		EVENT_RECORD = BUILTIN_EVENT + 7,
		EVENT_CUE = BUILTIN_EVENT + 8,
		EVENT_INIT = BUILTIN_EVENT + 9
	};

};

extern	Attribute::idcode AttributeId(char *);
class Block;

class AttributeList
{
public:
	~AttributeList() {
		if (items != nullptr) {
			delete items;
		}
	}
	void clear();
	void add(Attribute::idcode, Attribute::value val=Attribute::value());
	void add(Block *b);
	bool has(Attribute::idcode);
	Attribute::value &getValue(Attribute::idcode id);

	inline bool
	has(char *pr)
	{
		return has(AttributeId(pr));
	}
	
	inline Attribute::value
	getValue(char *pr)
	{
		return getValue(AttributeId(pr));
	}
	
	inline void
	add(char *pr, Attribute::value val)
	{
		add(AttributeId(pr), val);
	}
	
	Attribute *items=nullptr;
};


#endif
