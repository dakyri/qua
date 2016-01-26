#ifndef _BASEVAL
#define _BASEVAL

#include "qua_version.h"

#include "Time.h"
#include "QuasiStack.h"

//////////////////////////////////////////////////////////////////
//  base value class
//  a 64 bit value union. 
// ???????????????????????????
// big questions about the way some formerly small structures passed
// around the stack have grown ...
// dropping 64 bit quantities from base_val_t ->
//	base value 8 bytes
//	typed value 12 bytes
//	result value 12 bytes
//	lvalue 16 bytes
// otherwise base_val_t ->
//	base value 8 bytes
//	typed value 16 bytes ... to maintain alignment for 64bit int
//	result value 16 bytes ... to maintain alignment for 64bit int
//	lvalue 16 bytes

#include <stdio.h>

//typedef	int8	base_type_t;
typedef int8	ref_type_t;

class Voice;
class Application;
class Method;
class StreamItem;
class Stream;
class Pool;
class Sample;
class Time;
class Block;
class Qua;
class QuaObject;
class ResultValue;
class TypedValueList;
class Channel;
class StabEnt;
class QuaControllerBridge;
class Instance;
class Envelope;
class Note;
class Ctrl;
class SysX;
class SysC;
class Prog;
class Bend;
class Schedulable;
class Stacker;
class Template;
class QuaPort;
class TypedValueList;
class Stackable;
class Metric;
class Event;
class Input;
class Output;
class Executable;
class QuasiStack;
class VstPlugin;
class State;
class StabEnt;
class Stackable;
class Take;
class SampleTake;
class StreamTake;
class Clip;

#if defined(WIN32)
class BMessage;
#endif

class TypedValueList
{
public:
	//						TypedValueList();


	inline void Init()	// can't have blank constructor for union member
	{
		head = tail = nullptr;
	}

	class TypedValueListItem	*head;
	class TypedValueListItem	*tail;

	void				AddItem(void *I, short t);
	void				AddItem(class TypedValue *v);

	bool				AddToMessage(BMessage *m);

	void				Clear();
};

// our base value is 64 bit. all these structures should fit that.

struct StackAddress {
public:
	int32				offset;
	StabEnt				*context;
};

struct AState {
public:
	void Set(State *s, int32 v) { states = s; value = v; }
	int32				value;	// value can be set as a normal 4 byte value
	State				*states;
};

struct ALabel {
public:
	void Set(Block *b, StabEnt *c) { block = b; objContext = c; }
	Block				*block;
	StabEnt				*objContext;
};

struct ABuiltin {
public:
	void Set(Stackable *p, int16 t, int16 st) { stackable = p; type = t; subType = st; }

	Stackable			*stackable;
	int16				type;
	int16				subType;
};

struct AParamGroup {
	void Set(int8 en) { enable =  en; }
	int8				enable;
};

union base_val_t {
	StackAddress		stackAddress;
	AState				state;
	ALabel				label;
	ABuiltin			builtin;
	AParamGroup			paramGroup;
	unsigned char		byte;
	unsigned char		*byteP;
	int16				Short;
	int16				*shortP;
	int32				Int;
	int32				*intP;
	int64				Long;
	int64				*longP;
	float				*floatP;
	float				Float;
	double				Double;
	bool				Bool;
	bool				*boolP;
	void				*pointer;
	void				**pointerP;
	StreamItem			*streamItemList;
	Stream				*stream;
	class Time			*timeP;
	struct qua_time		time;
// probs with class Time in unions (constructor and operators)
// the qtime is hopefully a 64 bit structure
	Pool				*pool;
	Sample				*sample;
	Voice				*voice;
	Method				*method;
	Application			*application;
	Qua					*qua;
	Instance			*instance;
	Take				*take;
	Clip				*clip;
	Event				*event;
	Input				*input;
	Output				*output;
	QuaPort				*port;
	char				*string;
	class Note			*noteP;
	class Ctrl			*ctrlP;
	class SysX			*sysXP;
	class SysC			*sysCP;
	class Prog			*progP;
	class Bend			*bendP;
	class QuaJoy		*joyP;
	FILE				*file;
	class Block			**blockP;
	class Block			*block;
	Channel				*channel;
	Envelope			*envelope;
	Template			*qTemplate;
	TypedValueList		list;

#ifdef QUA_V_VST_HOST
	VstPlugin			*vst;
	int32				vstParam;
	int32				vstProgram;
#endif
#ifdef QUA_V_PORT_PARAM
	BParameter			*parameter;
#endif

#if defined(QUA_V_STREAM_MESG)
	BMessage			*mesgP;
#endif

};


class TypedValue
{
public:
//	inline			TypedValue()
//						{ Set(S_UNKNOWN);}
//	inline			TypedValue(Time *v)
//						{ Set(S_TIME,0,0,0); Set(v); }
//	inline			TypedValue(Time v)
//						{ Set(S_TIME,0,0,0); Set(v); }
//	inline			TypedValue(base_type_t t, void *v)
//						{ Set(t,0,0,0); SetValue(v); }
//	inline			TypedValue(base_type_t t, float v)
//						{ Set(t,0,0,0); SetValue(v); }
//	inline			TypedValue(base_type_t t, int32 v)
//						{ Set(t,0,0,0); SetValue(v); }
//	inline			TypedValue(base_type_t t, int64 v)
//						{ Set(t,0,0,0); SetValue((int64)v); }
//	inline			TypedValue(base_type_t t, int16 ref, StabEnt *c, int32 off)
//						{ Set(t,ref,c, off); }

	enum {
		REF_STACK = 1,		// value on stack
		REF_VALUE = 0,		// value in symbol
		REF_POINTER = -1,	// value in symbol is a pointer
		REF_INSTANCE = -2	// value refers to an immediate value in the instance
	};

	enum {
		S_UNKNOWN	= -1,

		S_TEMPLATE	= 0,
		S_QUA		= 1,
		S_VOICE		= 2,
		S_POOL		= 3,
		S_SAMPLE	= 4,
		S_APPLICATION	= 5,
		S_METHOD		= 6,

		S_INSTANCE		= 8,
		S_STREAM		= 9,
		S_CHANNEL		= 10,
		S_STREAM_ITEM	= 11,
		S_VALUE			= 12,
		S_EXPRESSION	 = 13,
		S_BLOCK			= 14,
		S_TIME			= 15,

		S_FLOAT			= 16,
		S_LONG			= 17,
		S_INT			= 18,
		S_SHORT			= 19,
		S_BYTE			= 20,
		S_BOOL			= 21,
		S_STRING		= 22,

		S_LIST			= 23,
		S_STRUCT		= 24,

		S_NOTE			= 25,
		S_CTRL			= 26,
		S_SYSX			= 27,
		S_SYSC			= 28,
		S_MESSAGE		= 29,
		S_BEND			= 30,
		S_JOY			= 31,
		S_PROG			= 32,

		S_LOG_ENTRY 	= 33,
		S_STRANGE_POINTER = 34,
		S_FILE			= 35,
		S_PORT			= 36,
		S_PORTPARAM		= 37,
		S_PARAMGROUP	= 38,
		S_EVENT			= 39,

		S_INPUT			= 40,
		S_OUTPUT		= 41,
		S_TAKE			= 42,
		S_CLIP			= 43,
		S_LIST_ITEM		= 44,

		S_VST_PLUGIN	= 45,	// the loaded plugin
		S_VST_PARAM		= 46,	// vst plugin parameter
		S_VST_PROGRAM	= 47,
		S_LABEL			= 48,	// a label of a block also maybe an object sym reference. used to access instances of plugins, etc
		S_STATE			= 49,	// special state value ... used in state labels
		S_BUILTIN		= 50,	// one of my own builtins

		S_DOUBLE		= 51	// at the moment, stored directly not calculated with, used for inaccessible internals, all on stack
	};


	static inline TypedValue &
	BaseVal(base_type_t t)
	{
		static TypedValue		val;
		val.type = t;
		val.refType = REF_VALUE;
		val.flags = 0;
		val.indirection = 0;
		return val;
	}
	
	static inline TypedValue &
	StackBaseVal(base_type_t t, ref_type_t ref, StabEnt *c, int32 off)
	{
		static TypedValue		val;
		val.type = t;
		val.refType = REF_STACK;
		val.indirection = 0;
		val.flags = 0;
		val.val.stackAddress.context = c;
		val.val.stackAddress.offset = off;
		return val;
	}
	
	static inline TypedValue &
	Bool(bool v)
	{
		static TypedValue		val;
		val.type = S_BOOL;
		val.refType = REF_VALUE;
		val.flags = 0;
		val.indirection = 0;
		val.val.Bool = v;
		return val;
	}
	
	static inline TypedValue &
	TimeV(Time t)
	{
		static TypedValue		val;
		val.type = S_TIME;
		val.refType = REF_VALUE;
		val.indirection = 0;
		val.flags = 0;
		val.val.time = t;
//		*((Time*)val.val.timeBuf) = t;
		return val;
	}
	
	static inline TypedValue &
	Float(float v)
	{
		static TypedValue		val;
		val.type = S_FLOAT;
		val.refType = REF_VALUE;
		val.flags = 0;
		val.indirection = 0;
		val.val.Float = v;
		return val;
	}

#ifdef QUA_V_VST_HOST
	static inline TypedValue &
	VstProgram(int32 v)
	{
		static TypedValue		val;
		val.type = S_VST_PROGRAM;
		val.refType = REF_VALUE;
		val.flags = 0;
		val.indirection = 0;
		val.val.vstProgram = v;
		return val;
	}
	static inline TypedValue &
	VstParam(int32 v)
	{
		static TypedValue		val;
		val.type = S_VST_PARAM;
		val.refType = REF_VALUE;
		val.flags = 0;
		val.indirection = 0;
		val.val.vstParam = v;
		return val;
	}
#endif
	static inline TypedValue &
	Double(double v)
	{
		static TypedValue		val;
		val.type = S_DOUBLE;
		val.refType = REF_VALUE;
		val.flags = 0;
		val.indirection = 0;
		val.val.Double = v;
		return val;
	}
	
	static inline TypedValue &
	Long(int64 v)
	{
		static TypedValue		val;
		val.type = S_LONG;
		val.refType = REF_VALUE;
		val.flags = 0;
		val.indirection = 0;
		val.val.Long = v;
		return val;
	}
	
	static inline TypedValue &
	Int(int32 v)
	{
		static TypedValue		val;
		val.type = S_INT;
		val.refType = REF_VALUE;
		val.flags = 0;
		val.indirection = 0;
		val.val.Int = v;
		return val;
	}
	
	static inline TypedValue &
	Short(int16 v)
	{
		static TypedValue		val;
		val.type = S_SHORT;
		val.refType = REF_VALUE;
		val.indirection = 0;
		val.flags = 0;
		val.val.Short = v;
		return val;
	}
	
	static inline TypedValue &
	Byte(int8 v)
	{
		static TypedValue		val;
		val.type = S_BYTE;
		val.refType = REF_VALUE;
		val.indirection = 0;
		val.flags = 0;
		val.val.byte = v;
		return val;
	}
	
	static inline TypedValue &
	Pointer(base_type_t t, void *v)
	{
		static TypedValue		val;
		val.type = t;
		val.flags = 0;
		val.refType = REF_VALUE;
		val.indirection = 0;
		val.val.pointer = v;
		return val;
	}

	
	static inline TypedValue &
	List()
	{
		static TypedValue		val;
		val.type = S_LIST;
		val.refType = REF_VALUE;
		val.indirection = 0;
		val.flags = 0;
		val.val.list.head = nullptr;
		val.val.list.tail = nullptr;
		return val;
	}
	
	char			*StringValue();

	bool			BoolValue(QuasiStack *stack);
	int8			ByteValue(QuasiStack *stack);
	int16			ShortValue(QuasiStack *stack);
	int32			IntValue(QuasiStack *stack);
	int64			LongValue(QuasiStack *stack);
	float			FloatValue(QuasiStack *stack);
	float			*FloatPValue(QuasiStack *stack);
	double			DoubleValue(QuasiStack *stack);
	Time			TimeValue(QuasiStack *stack);
	Clip			*ClipValue(QuasiStack *stack);
	void			*PointerValue(QuasiStack *stack);
	void			*PointerValue();
	uchar			*StackAddressValue(QuasiStack *stack);
	int32			OffsetValue();
	AState			*StateValue(QuasiStack *stack);
	ALabel			*LabelValue();
	ABuiltin		*BuiltinValue();
	AParamGroup		*ParamGroupValue();
	Take			*TakeValue();
	SampleTake		*SampleTakeValue();
	StreamTake		*StreamTakeValue();

	Time			*TimeValue();
	Pool			*PoolValue();
	Instance		*InstanceValue();
	Sample			*SampleValue();
	StreamItem		*StreamItemValue();
	Note			*NoteValue();
	Qua				*QuaValue();
	Application		*ApplicationValue();
	Ctrl			*CtrlValue();
	SysX			*SysXValue();
	SysC			*SysCValue();
	Bend			*BendValue();
	Prog			*ProgValue();
	Method			*MethodValue();
	Voice			*VoiceValue();
	TypedValueList	*ListValue();
	Channel			*ChannelValue();
	Envelope		*EnvelopeValue();
	Block			*BlockValue();
	Block			**BlockPtrValue();
	Block			*ExpressionValue();
	FILE			*FileValue();
	Template		*TemplateValue();
	QuaPort			*PortValue();
	Event			*EventValue();
	Input			*InputValue();
	Output			*OutputValue();
	VstPlugin		*VstValue();

#if defined(QUA_V_VST_HOST)
	int32			VstParamValue(); // the id of a param
	int32			VstProgramValue(); // the value of a program
#endif
#if defined(QUA_V_PORT_PARAM)
#ifdef _BEOS
	BParameter		*PortParamValue();
#endif
#endif

#if defined(QUA_V_STREAM_MESG)
	BMessage		*MessageValue();
#endif
	
	Schedulable		*SchedulableValue();
	Executable		*ExecutableValue();
	Stackable		*StackableValue();
	
	void			SetValue(float v, class QuasiStack*p=nullptr);
	void			SetValue(int32 v, class QuasiStack*p=nullptr);
	void			SetValue(int64 v, class QuasiStack*p=nullptr);
	void			SetValue(Time *d, class QuasiStack*p=nullptr);
	void			SetValue(void *d, class QuasiStack*p=nullptr);
	void			SetValue(char *strval);
	void			SetValue(TypedValue *val);			
//	void			SetValue(Pool *d);
//	void			SetValue(Sample *d);
//	void			SetValue(Channel *d);
//	void			SetValue(FILE *d);
//	void			SetValue(Envelope *d);
//	void			SetValue(Block *d);
//	void			SetValue(Block **d);

	void			SetDoubleValue(double v, class QuasiStack*p=nullptr);

	void			SetStateValue(State *, int32);
	void			SetLabelValue(Block *, StabEnt *);
	void			SetBuiltinValue(Stackable *, int16, int16);
	
	void			Set(TypedValue *val);			
	void			Set(Time *val);			
	void			Set(Instance *d);
	void			Set(tick_t t, Metric *m);			
	void			Set(base_type_t type, ref_type_t ref, StabEnt *c, int32 off);
	void			Set(base_type_t, ref_type_t ref);

	void			SetToSymbol(char *nm, StabEnt *ctxt);
#ifndef OLD_LIST
	inline void Set(TypedValueList *l)
	{
		if (type == S_LIST) {
			val.list.Clear();
		}
		type = S_LIST;
		val.list.head = l->head;
		val.list.tail = l->tail;
		l->head = l->tail = nullptr;
		refType = REF_VALUE;
		indirection = 0;
	}
#endif
	
	inline void	
	SetStackReference(StabEnt *newc, int32 off)
	{
		if (refType == REF_STACK) {
			val.stackAddress.context = newc;
			val.stackAddress.offset = off;
		}
	}
	
	inline void	
	SetStackContext(StabEnt *newc)
	{
		if (refType == REF_STACK)
			val.stackAddress.context = newc;
	} 
			
	TypedValue	&	operator >= (TypedValue &t2);
	TypedValue	&	operator == (TypedValue &t2);
	TypedValue	&	operator != (TypedValue &t2);
	TypedValue	&	operator <= (TypedValue &t2);
	TypedValue	&	operator > (TypedValue &t2);
	TypedValue	&	operator < (TypedValue &t2);
	TypedValue	&	operator && (TypedValue &t2);
	TypedValue	&	operator || (TypedValue &t2);
	
	TypedValue	&	operator + (TypedValue &t2);
	TypedValue	&	operator - (TypedValue &t2);
	TypedValue	&	operator * (TypedValue &t2);
	TypedValue	&	operator / (TypedValue &t2);
	TypedValue	&	operator % (TypedValue &t2);
	TypedValue	&	operator - ();
	TypedValue	&	operator ! ();
	TypedValue	&	operator ~ ();
	base_type_t		BigEnoughType(TypedValue t2);
	
friend StabEnt	*DefineSymbol(std::string , base_type_t typ, int8 nd,
						void *val,
						StabEnt *context,
						ref_type_t ref,
						bool, bool,
						short dm);
friend StabEnt	*DupSymbol(StabEnt *n, StabEnt *m);
friend class StabEnt;
friend class ResultValue;
friend class LValue;
friend class Qua;

protected:
	base_val_t		val;			// 64 bits
public:
	base_type_t		type;			// 8 bits
	int8			refType;		// 8 bits
	int8			indirection;	// 8 bits
	uint8			flags;
};


class TypedValueListItem
{
public:
	inline TypedValueListItem()
	{
		next = nullptr;
		value.type = TypedValue::S_UNKNOWN;
	}
	
	inline ~TypedValueListItem()
	{
		if (next)
			delete next;
	}
	
	TypedValueListItem	*next;
	TypedValue			value;
};

inline void *
TypedValue::PointerValue()
{
	return type == S_BYTE || type == S_SHORT ||
			type == S_INT || type == S_LONG ||
			type == S_FLOAT? nullptr : val.pointer;
}

inline Pool *
TypedValue::PoolValue()
{
	return type == S_POOL && refType == REF_VALUE? val.pool: nullptr;
}

inline Instance *
TypedValue::InstanceValue()
{
	return type == S_INSTANCE && refType == REF_VALUE? val.instance: nullptr;
}

inline Sample *
TypedValue::SampleValue()
{
	return type == S_SAMPLE && refType == REF_VALUE? val.sample: nullptr;
}

inline Qua *
TypedValue::QuaValue()
{
	return type == S_QUA && refType == REF_VALUE? val.qua: nullptr;
}

inline Bend *
TypedValue::BendValue()
{
	return type == S_BEND && (refType == REF_VALUE || refType == REF_POINTER)? val.bendP: nullptr;
}

inline Prog *
TypedValue::ProgValue()
{
	return type == S_PROG && (refType == REF_VALUE || refType == REF_POINTER)? val.progP: nullptr;
}

inline SysX *
TypedValue::SysXValue()
{
	return type == S_SYSX && (refType == REF_VALUE || refType == REF_POINTER)? val.sysXP: nullptr;
}

inline SysC *
TypedValue::SysCValue()
{
	return type == S_SYSC && (refType == REF_VALUE || refType == REF_POINTER)? val.sysCP: nullptr;
}

inline Application *
TypedValue::ApplicationValue()
{
	return type == S_APPLICATION && refType == REF_VALUE? val.application: nullptr;
}

inline StreamItem *
TypedValue::StreamItemValue()
{
	return type == S_STREAM_ITEM && refType == REF_VALUE? val.streamItemList: nullptr;
}


inline Note	 *
TypedValue::NoteValue()
{
	return type == S_NOTE && (refType == REF_VALUE || refType == REF_POINTER)? val.noteP: nullptr;
}

inline Ctrl	 *
TypedValue::CtrlValue()
{
	return type == S_CTRL && (refType == REF_VALUE || refType == REF_POINTER)? val.ctrlP: nullptr;
}

inline Method *
TypedValue::MethodValue()
{
	return type == S_METHOD && refType == REF_VALUE? val.method: nullptr;
}

#ifdef QUA_V_VST_HOST
inline VstPlugin *
TypedValue::VstValue()
{
	return type == S_VST_PLUGIN && refType == REF_VALUE? val.vst: nullptr;
}
#endif

inline Voice *
TypedValue::VoiceValue(){
	return type == S_VOICE && refType == REF_VALUE? val.voice: nullptr;
}

inline TypedValueList	 *
TypedValue::ListValue()
{
#ifdef OLD_LIST
	return type == S_LIST? val.list: nullptr;
#else
	return type == S_LIST? &val.list: nullptr;
#endif
}

inline Channel *
TypedValue::ChannelValue()
{
	return type == S_CHANNEL && refType == REF_VALUE? val.channel: nullptr;
}

inline Time *
TypedValue::TimeValue()
{
	return (type == S_TIME)?
				(refType > 0?
					val.timeP:
					(Time *)&val.time)
				: 0;
}

inline Block *
TypedValue::BlockValue()
{
	return (type == S_BLOCK)?
		((refType == REF_POINTER && val.blockP)? *val.blockP:
						((refType == REF_VALUE)? val.block:0)):0;
}

inline ALabel *
TypedValue::LabelValue()
{
	return (type == S_LABEL)? &val.label: nullptr;
}

inline ABuiltin *
TypedValue::BuiltinValue()
{
	return (type == S_BUILTIN)? &val.builtin: nullptr;
}

inline AParamGroup *
TypedValue::ParamGroupValue()
{
	return (type == S_PARAMGROUP)?&val.paramGroup: nullptr;
}

inline Block *
TypedValue::ExpressionValue()
{
	return (type == S_EXPRESSION)?
		((refType == REF_POINTER && val.blockP)? *val.blockP:
						((refType == REF_VALUE)? val.block:0)):
				0;
}

inline Block **
TypedValue::BlockPtrValue()
{
	return (type == S_BLOCK || type == S_EXPRESSION)?
		((refType == REF_POINTER && val.blockP)? val.blockP:
						((refType == REF_VALUE)? &val.block:0))
			:0;
}

inline FILE *
TypedValue::FileValue()
{
	return type == S_FILE?val.file: nullptr;
}

inline uchar *
TypedValue::StackAddressValue(QuasiStack *stack)
{
	return (refType == REF_STACK) && stack?
				stack->stk.vars + val.stackAddress.offset: 0;
}

inline int32
TypedValue::OffsetValue()
{
	return (refType == REF_STACK || refType == REF_INSTANCE)?
							val.stackAddress.offset: 0;
}

inline Template *
TypedValue::TemplateValue()
{
	return (type == S_TEMPLATE)?val.qTemplate: nullptr;
}

inline QuaPort *
TypedValue::PortValue()
{
	return ((type == S_PORT)?val.port: nullptr);
}

inline Event *
TypedValue::EventValue()
{
	return (type == S_EVENT)?val.event: nullptr;
}

inline Input *
TypedValue::InputValue()
{
	return (type == S_INPUT)?val.input: nullptr;
}

inline Output *
TypedValue::OutputValue()
{
	return (type == S_OUTPUT)?val.output: nullptr;
}

#if defined(QUA_V_VST_HOST)
inline int32
TypedValue::VstParamValue()
{
	return (type == S_VST_PARAM)?val.vstParam: 0;
}

inline int32
TypedValue::VstProgramValue()
{
	return (type == S_VST_PROGRAM)?val.vstProgram: 0;
}
#endif

inline Take *
TypedValue::TakeValue()
{
	return (type == S_TAKE)?val.take: nullptr;
}

#endif
