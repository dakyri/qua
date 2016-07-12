#include "qua_version.h"

#include "StdDefs.h"

#include "Qua.h"
#include "Voice.h"
#include "Sample.h"
#include "Pool.h"
#include "Lambda.h"
#include "Sym.h"
#include "Block.h"
#include "Template.h"
#include "QuaFX.h"
#include "Envelope.h"
#include "Executable.h"
#include "Channel.h"
#include "QuasiStack.h"
#include "QuaPort.h"
#include "QuaJoy.h"
#include "Markov.h"
#include "BaseVal.h"
#include "VstPlugin.h"

#include "Dictionary.h"

std::unordered_map<std::string, int> builtinCommandIndex = {
	{ "find",	Block::BUILTIN_FIND },
	{ "delete",	Block::BUILTIN_DELETE },
	{ "insert",	Block::BUILTIN_INSERT },
	//	{"play",	Block::BUILTIN_PLAY},
	{ "roll",	Block::BUILTIN_ROLL },
	{ "ramp",	Block::BUILTIN_RAMP },
	{ "flush",	Block::BUILTIN_FLUSH },
	{ "select",	Block::BUILTIN_SELECT },
	{ "coalesce",	Block::BUILTIN_COALESCE },

#ifdef QUA_V_OLD_BUILTIN
	{ "mkNote",	Block::BUILTIN_NOTE },
	{ "mkCtrl",	Block::BUILTIN_CTRL },
	{ "mkBend",	Block::BUILTIN_BEND },
	{ "mkSysx",	Block::BUILTIN_SYSX },
	{ "mkProg",	Block::BUILTIN_PROG },
	{ "mkMsg",	Block::BUILTIN_MESG },

	{ "mkTimecode",	Block::BUILTIN_SYSC },
	{ "mkSongpos",	Block::BUILTIN_SYSC },
	{ "mkSongsel",	Block::BUILTIN_SYSC },
	{ "mkTunereq",	Block::BUILTIN_SYSC },
	{ "mkClk",		Block::BUILTIN_SYSC },
	{ "mkStrt",	Block::BUILTIN_SYSC },
	{ "mkCont",	Block::BUILTIN_SYSC },
	{ "mkStop",	Block::BUILTIN_SYSC },
	{ "mkSense",	Block::BUILTIN_SYSC },
	{ "mkRst",	Block::BUILTIN_SYSC },
#endif	
	{ "sin",		Block::BUILTIN_SIN },
	{ "cos",		Block::BUILTIN_COS },
	{ "tan",		Block::BUILTIN_TAN },
	{ "asin",	Block::BUILTIN_ASIN },
	{ "acos",	Block::BUILTIN_ACOS },
	{ "atan",	Block::BUILTIN_ATAN },
	{ "exp",		Block::BUILTIN_EXP },
	{ "log",		Block::BUILTIN_LOG },
	{ "sinh",	Block::BUILTIN_SINH },
	{ "cosh",	Block::BUILTIN_COSH },
	{ "tanh",	Block::BUILTIN_TANH },
	{ "pow",		Block::BUILTIN_POW } };

std::unordered_map<std::string, int> clipPlayerIndex = {
	{ "play2n",	Block::C_TUNEDSAMPLE_PLAYER },
	{ "sampleplay",	Block::C_SAMPLE_PLAYER },
	{ "streamplay",	Block::C_STREAM_PLAYER },
	{ "midiplay",	Block::C_MIDI_PLAYER },
	{ "markov",	Block::C_MARKOV_PLAYER },
	{ "play",	Block::C_GENERIC_PLAYER }
};

std::unordered_map<std::string, int> systemCommonCodeIndex = {
	{ "timecode",	MIDI_TIME_CODE },
	{ "songpos",	MIDI_SONG_POS },
	{ "songsel",	MIDI_SONG_SEL },
	{ "tunereq",	MIDI_TUNE_REQ },
	{ "clk",		MIDI_CLOCK },
	{ "strt",	MIDI_START },
	{ "cont",	MIDI_CONT },
	{ "stop",	MIDI_STOP },
	{ "sense",	MIDI_SENSING },
	{ "rst",		MIDI_SYS_RESET }
};

std::unordered_map<std::string, int>  note_nf = {
	{ "noteon",	MIDI_NOTE_ON },
	{ "noteoff",	MIDI_NOTE_OFF },
	{ "keypress",	MIDI_KEY_PRESS },
	{ "chanpress",	MIDI_CHAN_PRESS }
};

std::unordered_map<std::string, int>  deviceTypeIndex = {
	{ "midi",	QUA_DEV_MIDI },
	{ "audio",	QUA_DEV_AUDIO },
	{ "joystick",	QUA_DEV_JOYSTICK },
	{ "parallel",	QUA_DEV_PARALLEL }
};

std::unordered_map<std::string, int> streamTypeIndex = {
	{ "joy",		TypedValue::S_JOY },
	{ "note",	TypedValue::S_NOTE },
	{ "ctrl",	TypedValue::S_CTRL },
	{ "bend",	TypedValue::S_BEND },
	{ "sysx",	TypedValue::S_SYSX },
	{ "sysc",	TypedValue::S_SYSC },
	{ "prog",	TypedValue::S_PROG },
	{ "msg",		TypedValue::S_MESSAGE },

	{ "noteon",	TypedValue::S_NOTE },
	{ "noteoff",	TypedValue::S_NOTE },
	{ "keypress",	TypedValue::S_NOTE },
	{ "chanpress",	TypedValue::S_NOTE },

	{ "timecode",	TypedValue::S_SYSC },
	{ "songpos",	TypedValue::S_SYSC },
	{ "songsel",	TypedValue::S_SYSC },
	{ "tunereq",	TypedValue::S_SYSC },
	{ "clk",		TypedValue::S_SYSC },
	{ "strt",	TypedValue::S_SYSC },
	{ "cont",	TypedValue::S_SYSC },
	{ "stop",	TypedValue::S_SYSC },
	{ "sense",	TypedValue::S_SYSC },
	{ "rst",		TypedValue::S_SYSC }
};

std::unordered_map<std::string, int> midiNoteIndex = {
	{ "c0",	0 },
	{ "c#0",	1 },
	{ "d0",	2 },
	{ "d#0",	3 },
	{ "e0",	4 },
	{ "f0",	5 },
	{ "f#0",	6 },
	{ "g0",	7 },
	{ "g#0",	8 },
	{ "a0",	9 },
	{ "a#0",	10 },
	{ "b0",	11 },
	{ "c1",	12 },
	{ "c#1",	13 },
	{ "d1",	14 },
	{ "d#1",	15 },
	{ "e1",	16 },
	{ "f1",	17 },
	{ "f#1",	19 },
	{ "g1",	19 },
	{ "g#1",	20 },
	{ "a1",	21 },
	{ "a#1",	22 },
	{ "b1",	23 },
	{ "c2",	24 },
	{ "c#2",	25 },
	{ "d2",	26 },
	{ "d#2",	27 },
	{ "e2",	28 },
	{ "f2",	29 },
	{ "f#2",	30 },
	{ "g2",	31 },
	{ "g#2",	32 },
	{ "a2",	33 },
	{ "a#2",	34 },
	{ "b2",	35 },
	{ "c3",	36 },
	{ "c#3",	37 },
	{ "d3",	38 },
	{ "d#3",	39 },
	{ "e3",	40 },
	{ "f3",	41 },
	{ "f#3",	42 },
	{ "g3",	43 },
	{ "g#3",	44 },
	{ "a3",	45 },
	{ "a#3",	46 },
	{ "b3",	47 },
	{ "c4",	48 },
	{ "c#4",	49 },
	{ "d4",	50 },
	{ "d#4",	51 },
	{ "e4",	52 },
	{ "f4",	53 },
	{ "f#4",	54 },
	{ "g4",	55 },
	{ "g#4",	56 },
	{ "a4",	57 },
	{ "a#4",	58 },
	{ "b4",	59 },
	{ "c5",	60 },
	{ "c#5",	61 },
	{ "d5",	62 },
	{ "d#5",	63 },
	{ "e5",	64 },
	{ "f5",	65 },
	{ "f#5",	66 },
	{ "g5",	67 },
	{ "g#5",	68 },
	{ "a5",	69 },
	{ "a#5",	70 },
	{ "b5",	71 },
	{ "c6",	72 },
	{ "c#6",	73 },
	{ "d6",	74 },
	{ "d#6",	75 },
	{ "e6",	76 },
	{ "f6",	77 },
	{ "f#6",	78 },
	{ "g6",	79 },
	{ "g#6",	80 },
	{ "a6",	81 },
	{ "a#6",	82 },
	{ "b6",	83 },
	{ "c7",	84 },
	{ "c#7",	85 },
	{ "d7",	86 },
	{ "d#7",	87 },
	{ "e7",	88 },
	{ "f7",	89 },
	{ "f#7",	90 },
	{ "g7",	91 },
	{ "g#7",	92 },
	{ "a7",	93 },
	{ "a#7",	94 },
	{ "b7",	95 },
	{ "c8",	96 },
	{ "c#8",	97 },
	{ "d8",	98 },
	{ "d#8",	99 },
	{ "e8",	100 },
	{ "f8",	101 },
	{ "f#8",	102 },
	{ "g8",	103 },
	{ "g#8",	104 },
	{ "a8",	105 },
	{ "a#8",	106 },
	{ "b8",	107 },
	{ "c9",	108 },
	{ "c#9",	109 },
	{ "d9",	110 },
	{ "d#9",	111 },
	{ "e9",	112 },
	{ "f9",	113 },
	{ "f#9",	114 },
	{ "g9",	115 },
	{ "g#9",	116 },
	{ "a10",	117 },
	{ "a#10", 118 },
	{ "b10",	119 },
	{ "c10",	120 },
	{ "c#10", 121 },
	{ "d10",	122 },
	{ "d#10", 123 },
	{ "e10",	124 },
	{ "f10",	125 },
	{ "f#10", 126 },
	{ "g10",	127 }
};

std::unordered_map<std::string, int> objectSubTypeIndex = {
	{ "node",		Attribute::NODE },
	{ "modal",		Attribute::MODAL },
	{ "oncer",		Attribute::ONCER },
	{ "exception",	Attribute::EXCEPT },
	{ "reset",		Attribute::RESET },
	{ "toggle",		Attribute::TOGGLE },
	{ "held",		Attribute::HELD },
	{ "resetto",	Attribute::RESETTO },
	{ "fixed",		Attribute::FIXED },
	{ "label",		Attribute::LABEL },
	{ "range",		Attribute::RANGE },
	{ "trigger",	Attribute::TRIGGER },
	{ "mime",		Attribute::MIME },
	{ "enable",		Attribute::ENABLE },
	{ "disable",	Attribute::DISABLE },
	{ "load",		Attribute::LOAD },
	{ "noload",		Attribute::NOLOAD },
	{ "ins",		Attribute::INS },
	{ "outs",		Attribute::OUTS },
	{ "id",			Attribute::ID },
	{ "midithru",	Attribute::MIDITHRU },
	{ "audiothru",	Attribute::AUDIOTHRU },
	{ "noMidithru",	Attribute::NO_MIDITHRU },
	{ "noAudiothru", Attribute::NO_AUDIOTHRU },
	{ "map",		Attribute::MAP_VSTPARAM },
	{ "synth",		Attribute::SYNTH },
	{ "nparam",		Attribute::NPARAM },
	{ "nprogram",	Attribute::NPROGRAM },
	{ "audio",		Attribute::DEVICE_AUDIO },
	{ "midi",		Attribute::DEVICE_MIDI },
	{ "joystick",	Attribute::DEVICE_JOYSTICK },
	{ "parallel",	Attribute::DEVICE_PARALLEL },
	{ "osc",		Attribute::DEVICE_OSC },
	{ "sensor",		Attribute::DEVICE_SENSOR },
	{ "path",		Attribute::PATH },
	{ "takes",		Attribute::TAKES },
	{ "stream",		Attribute::STREAM },
	{ "sample",		Attribute::SAMPLE }

};

std::unordered_map<std::string, int> dfltEventIndex = {
	{ "wake",		Attribute::EVENT_WAKE },
	{ "sleep",		Attribute::EVENT_SLEEP },
	{ "start",		Attribute::EVENT_START },
	{ "stop",		Attribute::EVENT_STOP },
	{ "record",		Attribute::EVENT_RECORD },
	{ "cue",		Attribute::EVENT_CUE },
	{ "rx",			Attribute::EVENT_RX },
	{ "Rx",			Attribute::EVENT_RX },
	{ "RX",			Attribute::EVENT_RX },
	{ "tx",			Attribute::EVENT_TX },
	{ "Tx",			Attribute::EVENT_TX },
	{ "TX",			Attribute::EVENT_TX }
};

std::unordered_map<std::string, int> typeIndex = {
	// confusion ... type or constructor builtin?
	{ "joy",		TypedValue::S_JOY },
	{ "note",	TypedValue::S_NOTE },
	{ "ctrl",	TypedValue::S_CTRL },
	{ "bend",	TypedValue::S_BEND },
	{ "sysx",	TypedValue::S_SYSX },
	{ "sysc",	TypedValue::S_SYSC },
	{ "prog",	TypedValue::S_PROG },

	{ "clip",	TypedValue::S_CLIP },
	{ "take",	TypedValue::S_TAKE },
	{ "builtin",	TypedValue::S_BUILTIN },
	{ "vst",		TypedValue::S_VST_PLUGIN },
	{ "vstparam",	TypedValue::S_VST_PARAM },

	{ "template",	TypedValue::S_TEMPLATE },
	{ "voice",		TypedValue::S_VOICE },
	{ "qua",			TypedValue::S_QUA },
	{ "channel",		TypedValue::S_CHANNEL },
	{ "instance",	TypedValue::S_INSTANCE },
	{ "pool",		TypedValue::S_POOL },
	{ "application",	TypedValue::S_APPLICATION },
	{ "block",		TypedValue::S_BLOCK },
	{ "expression",	TypedValue::S_EXPRESSION },
	{ "sample",		TypedValue::S_SAMPLE },
	{ "bool",		TypedValue::S_BOOL },
	{ "byte",		TypedValue::S_BYTE },
	{ "short",		TypedValue::S_SHORT },
	{ "int",			TypedValue::S_INT },
	{ "long",		TypedValue::S_LONG },
	{ "float",		TypedValue::S_FLOAT },
	{ "struct",		TypedValue::S_STRUCT },
	{ "lambda",		TypedValue::S_LAMBDA },
	{ "file",		TypedValue::S_FILE },
	{ "time",		TypedValue::S_TIME },
	{ "port",		TypedValue::S_PORT },
	{ "message",		TypedValue::S_MESSAGE },
	{ "input",		TypedValue::S_INPUT },
	{ "output",		TypedValue::S_OUTPUT },
	{ "list",		TypedValue::S_LIST } };

std::unordered_map<std::string, int> constantIndex = {
	{ "forever", INFINITE_TICKS },
	// Midi System Constnts
	{ "MidiSysXStart",	MIDI_SYSX_START },	// 0xf0;
	{ "MidiTimecode",	MIDI_TIME_CODE },	// 0xf1
	{ "MidiSongPosition", MIDI_SONG_POS },	// 0xf2
	{ "MidiSongSelect",	MIDI_SONG_SEL },	// 0xf3
	{ "MidiCableMessage",	MIDI_CABLE_MSG },	// 0xf5;
	{ "MidiTuneRequest",	MIDI_TUNE_REQ },	// 0xf6
	{ "MidiSysXStart",	MIDI_SYSX_END },	// 0xf7;
	{ "MidiClock",	MIDI_CLOCK },	// 0xf8
	{ "MidiStart",	MIDI_START },	// 0xfa
	{ "MidiContinue", MIDI_CONT },	// 0xfb
	{ "MidiStop",	MIDI_STOP },	// 0xfc
	{ "MidiSense",	MIDI_SENSING },	// 0xfe
	{ "MidiReset",	MIDI_SYS_RESET },	// 0xff
										// Midi Channel message constants
	{ "MidiNoteOn",	MIDI_NOTE_OFF },		// 0x80
	{ "MidiNoteOff",	MIDI_NOTE_ON },		// 0x90
	{ "MidiKeyPressure",	MIDI_KEY_PRESS },	// 0xa0
	{ "MidiControlChange",	MIDI_CTRL },	// 0xb0
	{ "MidiProgramChange",	MIDI_PROG },	// 0xc0
	{ "MidiChannelPressure", MIDI_CHAN_PRESS },	// 0xd0
	{ "MidiPitchBend",	MIDI_BEND },	// 0xe0
#ifdef QUA_NEW_JOY
	{ "JoyNot",		QUA_JOY_NOT },
	{ "JoyAxis",		QUA_JOY_AXIS },
	{ "JoyButton",	QUA_JOY_BUTTON },
	{ "JoyHat",		 QUA_JOY_HAT },
	{ "JoyHatCentred", QUA_JOY_HAT_CENTRED },
	{ "JoyHatUp",	QUA_JOY_HAT_UP },
	{ "JoyHatUpR",	QUA_JOY_HAT_UP_RIGHT },
	{ "JoyHatR",		QUA_JOY_HAT_RIGHT },
	{ "JoyHatDnR",	QUA_JOY_HAT_DOWN_RIGHT },
	{ "JoyHatDn",	QUA_JOY_HAT_DOWN },
	{ "JoyHatDnL",	QUA_JOY_HAT_DOWN_LEFT },
	{ "JoyHatL",		QUA_JOY_HAT_LEFT },
	{ "JoyHatUpL",	QUA_JOY_HAT_UP_LEFT },
#endif
	{ "true",		1 },
	{ "false",		0 }
};


int32 findType(const char *s)
{
	auto v = typeIndex.find(s);

	return v == typeIndex.end() ? TypedValue::S_UNKNOWN : v->second;
}

int32 findConstant(const char *s)
{
	auto v = constantIndex.find(s);

	return v == constantIndex.end() ? INT_MIN : v->second;
}


int32 findMidiNote(const char *s)
{
	auto v = midiNoteIndex.find(s);

	return v == midiNoteIndex.end() ? TypedValue::S_UNKNOWN : v->second;
}

int32 findDfltEventType(const char *s)
{
	auto v = dfltEventIndex.find(s);

	return v == dfltEventIndex.end() ? TypedValue::S_UNKNOWN : v->second;
}


int32 findTypeAttribute(const char *s)
{
	auto v = objectSubTypeIndex.find(s);

	return v == objectSubTypeIndex.end() ? TypedValue::S_UNKNOWN : v->second;

}
/*
auto it = find_if(typeIndex.begin(), typeIndex.end(),
[this](pair<string,int> vt) { return vt.second == value.type; }
);

*/

string findTypeName(int type) {
	return qut::unfind(typeIndex, type);
}

string findAttributeName(int type) {
	return qut::unfind(objectSubTypeIndex, type);
}

string findClipPlayer(int type) {
	return qut::unfind(clipPlayerIndex, (int)type);
}