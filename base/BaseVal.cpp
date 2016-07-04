#include "qua_version.h"

#include "StdDefs.h"
#include "BaseVal.h"
#include "Sym.h"
#include "Pool.h"
#include "Voice.h"
#include "Sample.h"
#include "Lambda.h"
#include "QuaPort.h"
#include "Qua.h"
#include "Template.h"
#include "Channel.h"

/*
TODO XXXX todo xxxx possibly need to add proper constructor and destructor
most things are sage. i think strings will leak. or keep it as is, but look really closely
at strings. and perhaps sysx. I think other types are ok.
*/
int32
TypedValue::IntValue(QuasiStack *stack)
{
	if (refType == REF_STACK) {
		while (stack && stack->context != val.stackAddress.context) {
			fprintf(stderr, "stack has %s\n", stack->context->name.c_str());
			stack = stack->lowerFrame;
		}
		if (stack && stack->mulch) {
			uchar *addr = stack->stk.vars+val.stackAddress.offset;
			switch (type) {
			case S_BOOL:		return *((bool *)addr);
			case S_BYTE:		return *((uchar *)addr);
			case S_SHORT:		return *((int16 *)addr);
			case S_LONG:		return ((int32)*((int64 *)addr));
			case S_INT:			return *((int32 *)addr);
			case S_FLOAT:		return ((int32) *((float *)addr));
			case S_STRANGE_POINTER:	return (long)*((void **)addr);
			case S_TIME:		return ((Time*)addr)->ticks;
			}
		} else {
			fprintf(stderr, "IntValue: no stack matching ctxt %s\n", val.stackAddress.context?val.stackAddress.context->name.c_str() :"\"global\"");
			return 0;
		}
	} else if (refType == REF_INSTANCE) {
		internalError("boo");
		return 0;
	} else if (refType == REF_POINTER) {
		switch (type) {
		case S_BOOL:	return *val.boolP;
		case S_BYTE:	return *val.byteP;
		case S_SHORT:	return *val.shortP;
		case S_LONG:	return ((int32)*val.longP);
		case S_INT:		return *val.intP;
		case S_FLOAT:	return ((int32)*val.floatP);
		case S_TIME:		return val.timeP?val.timeP->ticks:0;
		default:
			return (int32)val.pointer;
		}
	} else if (refType == REF_VALUE) {
		switch (type) {
		case S_BOOL:		return val.Bool;
		case S_BYTE:		return val.byte;
		case S_SHORT:		return val.Short;
		case S_LONG:		return ((int32)val.Long);
		case S_INT:			return val.Int;
		case S_FLOAT:		return ((int32)val.Float);
		case S_TIME:		return val.time.ticks;
//		case S_TIME:		return ((Time*)(val.timeBuf))->ticks;
		default:
			return (int32)val.pointer;
		}
	} else {
		internalError("boo");
	}
	return 0;
}

bool
TypedValue::BoolValue(QuasiStack *stack)
{
	return IntValue(stack) != 0;
}

int8
TypedValue::ByteValue(QuasiStack *stack)
{
	return ((int8)IntValue(stack));
}

int16
TypedValue::ShortValue(QuasiStack *stack)
{
	return ((int16)IntValue(stack));
}

Clip *
TypedValue::ClipValue(QuasiStack *stack)
{
	return (type == S_CLIP)? (Clip *)PointerValue(stack):nullptr;
}

Time 
TypedValue::TimeValue(QuasiStack *stack)
{
	if (refType == REF_STACK) {
		while (stack && stack->context != val.stackAddress.context) {
			stack = stack->lowerFrame;
		}
		if (stack && stack->mulch) {
			uchar *addr = stack->stk.vars+val.stackAddress.offset;
			switch (type) {
			case S_TIME:		return *((Time*)addr);
			default:			return Time::zero;
			}
		} else {
			fprintf(stderr, "TimeValue: no stack matching ctxt %s\n", val.stackAddress.context?val.stackAddress.context->name.c_str() :"\"global\"");
			return Time::zero;
		}
	} else if (refType == REF_INSTANCE) {
		internalError("boo");
		return Time::zero;
	} else if (refType == REF_POINTER) {
		switch (type) {
		case S_TIME:		return (val.timeP?*val.timeP:Time::zero);
		default:			return Time::zero;
		}
	} else if (refType == REF_VALUE) {
		switch (type) {
		case S_TIME:		return *((Time *)&val.time);
		default: 			return Time::zero;
		}
	} else {
		internalError("boo");
	}
	return Time::zero;
}

float *
TypedValue::FloatPValue(QuasiStack *stack)
{
	if (refType == REF_STACK) {
		while (stack && stack->context != val.stackAddress.context) {
			stack = stack->lowerFrame;
		}
		if (stack && stack->mulch) {
			uchar *addr = stack->stk.vars+val.stackAddress.offset;
			switch (type) {
			case S_FLOAT:		return ((float *)addr);
			}
		} else {
			fprintf(stderr, "FloatValue: no stack matching ctxt %s\n", val.stackAddress.context?val.stackAddress.context->name.c_str() :"\"global\"");
			return 0;
		}
	} else if (refType == REF_INSTANCE) {
		return nullptr;
	} else if (refType == REF_POINTER) {
		switch (type) {
		case S_FLOAT:	return val.floatP;
		default:
			return nullptr;
		}
	} else if (refType == REF_VALUE) {
		return &val.Float;
	}
	return nullptr;
}


float
TypedValue::FloatValue(QuasiStack *stack)
{
	if (refType == REF_STACK) {
		while (stack && stack->context != val.stackAddress.context) {
			stack = stack->lowerFrame;
		}
		if (stack && stack->mulch) {
			uchar *addr = stack->stk.vars+val.stackAddress.offset;
			switch (type) {
			case S_BOOL:		return *((bool *)addr);
			case S_BYTE:		return *((uchar *)addr);
			case S_SHORT:		return *((int16 *)addr);
			case S_LONG:		return ((float)*((int64 *)addr));
			case S_INT:			return ((float)*((int32 *)addr));
			case S_FLOAT:		return *((float *)addr);
			case S_TIME:		return ((float)((Time*)addr)->ticks);
			case S_STRANGE_POINTER:	return 0;
			}
		} else {
			fprintf(stderr, "FloatValue: no stack matching ctxt %s\n", val.stackAddress.context?val.stackAddress.context->name.c_str() :"\"global\"");
			return 0;
		}
	} else if (refType == REF_INSTANCE) {
		internalError("boo");
		return 0;
	} else if (refType == REF_POINTER) {
		switch (type) {
		case S_BOOL:	return *val.boolP;
		case S_BYTE:	return *val.byteP;
		case S_SHORT:	return *val.shortP;
		case S_LONG:	return ((float)*val.longP);
		case S_INT:		return ((float)*val.intP);
		case S_FLOAT:	return *val.floatP;
		case S_TIME:		return ((float)(val.timeP?val.timeP->ticks:0));
		default:
			return (float)((int32)val.pointer);
		}
	} else if (refType == REF_VALUE) {
		switch (type) {
		case S_BOOL:		return val.Bool;
		case S_BYTE:		return val.byte;
		case S_SHORT:		return val.Short;
		case S_LONG:		return ((float)val.Long);
		case S_INT:			return ((float)val.Int);
		case S_FLOAT:		return val.Float;
		case S_TIME:		return val.time.ticks;
//		case S_TIME:		return ((float)((Time*)(val.timeBuf))->ticks);
		default: {
			return ((float)(int32)val.pointer);
		}
		}
	} else {
		internalError("boo");
	}
	return 0;
}

double
TypedValue::DoubleValue(QuasiStack *stack)
{
	if (refType == REF_STACK) {
		while (stack && stack->context != val.stackAddress.context) {
			stack = stack->lowerFrame;
		}
		if (stack && stack->mulch) {
			uchar *addr = stack->stk.vars+val.stackAddress.offset;
			switch (type) {
			case S_DOUBLE:		return *((double *)addr);
			case S_BOOL:		return *((bool *)addr);
			case S_BYTE:		return *((uchar *)addr);
			case S_SHORT:		return *((int16 *)addr);
			case S_LONG:		return ((float)*((int64 *)addr));
			case S_INT:			return ((float)*((int32 *)addr));
			case S_FLOAT:		return *((float *)addr);
			case S_TIME:		return ((float)((Time*)addr)->ticks);
			case S_STRANGE_POINTER:	return 0;
			}
		} else {
			fprintf(stderr, "FloatValue: no stack matching ctxt %s\n", val.stackAddress.context?val.stackAddress.context->name.c_str() :"\"global\"");
			return 0;
		}
	} else if (refType == REF_INSTANCE) {
		internalError("double val of instance var???");
		return 0;
	} else if (refType == REF_POINTER) {
		internalError("double val of pointer var???");
		return 0;
	} else if (refType == REF_VALUE) {
		internalError("double val of value var???");
		return 0;
	} else {
		internalError("boo");
	}
	return 0;
}

int64
TypedValue::LongValue(QuasiStack *stack)
{
	if (refType == REF_STACK) {
		while (stack && stack->context != val.stackAddress.context)
			stack = stack->lowerFrame;
		if (stack && stack->mulch) {
			uchar *addr = stack->stk.vars+val.stackAddress.offset;
			switch (type) {
			case S_BOOL:		return *((bool *)addr);
			case S_BYTE:		return *((uchar *)addr);
			case S_SHORT:		return *((short *)addr);
			case S_LONG:		return *((long *)addr);
			case S_INT:			return *((int32 *)addr);
			case S_FLOAT:		return ((int64)*((float *)addr));
			case S_TIME:		return ((Time*)addr)->ticks;
			case S_STRANGE_POINTER:	return (int64)*((void **)addr);
			}
		} else {
			fprintf(stderr, "LongValue: no stack matching ctxt %s\n", val.stackAddress.context?val.stackAddress.context->name.c_str() :"\"global\"");
			return 0;
		}
	} else if (refType == REF_INSTANCE) {
		internalError("boo");
		return 0;
	} else if (refType == REF_POINTER) {
		switch (type) {
		case S_BOOL:	return *val.boolP;
		case S_BYTE:	return *val.byteP;
		case S_SHORT:	return *val.shortP;
		case S_LONG:	return *val.longP;
		case S_INT:		return *val.intP;
		case S_FLOAT:	return ((int64)*val.floatP);
		case S_TIME:		return val.timeP?val.timeP->ticks:0;
		default:
			return (int32)val.pointer;
		}
	} else if (refType == REF_VALUE) {
		switch (type) {
		case S_BOOL:		return val.Bool;
		case S_BYTE:		return val.byte;
		case S_SHORT:		return val.Short;
		case S_LONG:		return val.Long;
		case S_INT:			return val.Int;
		case S_FLOAT:		return ((int64)val.Float);
		case S_TIME:		return val.time.ticks;
//		case S_TIME:		return ((Time*)(val.timeBuf))->ticks;
		default:
			return (int32)val.pointer;
		}
	} else {
		internalError("boo");
	}
	return 0;
}

void *
TypedValue::PointerValue(QuasiStack *stack)
{
	if (refType == REF_STACK) {
		while (stack && stack->context != val.stackAddress.context) {
			fprintf(stderr, "%s not %s\n", val.stackAddress.context->name.c_str(), stack->context->name.c_str());
			stack = stack->lowerFrame;
		}
		if (stack && stack->mulch) {
			uchar *addr = stack->stk.vars+val.stackAddress.offset;
			switch (type) {
			case S_BOOL:
			case S_BYTE:
			case S_SHORT:
			case S_LONG:
			case S_INT:
			case S_FLOAT:
				return nullptr;
			case S_STRANGE_POINTER:
			default:
				return *((void **)addr);
			}
		} else {
			fprintf(stderr, "PointerValue: no stack matching ctxt %s\n", val.stackAddress.context?val.stackAddress.context->name.c_str() :"\"global\"");
			return 0;
		}
	} else if (refType == REF_INSTANCE) {
		internalError("PointerValue: instance reference unexpected on stack");
		return 0;
	} else if (refType == REF_POINTER) {
		return (void *)val.pointer;
	} else if (refType == REF_VALUE) {
		switch (type) {
		case S_BOOL:
		case S_BYTE:
		case S_SHORT:
		case S_LONG:
		case S_INT:
		case S_FLOAT:
			return nullptr;
		default:
			return (void *)val.pointer;
		}
	} else {
		internalError("PointerValue: unexpected reference type");
		return 0;
	}
	return 0;
}


void
TypedValue::SetValue(int64 v, class QuasiStack *stack)
{
	if (refType == REF_STACK) {
		if (stack == nullptr || stack->mulch == nullptr)
			return;
		uchar *addr = stack->stk.vars + val.stackAddress.offset;
//		fprintf(stderr, "set long: addr %x\n", addr);
		switch (type) {
		case S_BOOL:		*((uchar*)addr) = (v!=0);	break;
		case S_FLOAT:		*((float*)addr) = ((float)v);	break;
		case S_BYTE:		*((uchar*)addr) = ((uchar)v); break;
		case S_SHORT:		*((int16*)addr) = ((int16)v); break;
		case S_INT:			*((int32*)addr) = ((int32)v); break;
		case S_LONG:		*((int64*)addr) = v; break;
		case S_TIME:		((Time*)addr)->ticks = ((tick_t)v); break;
		case S_STRANGE_POINTER:
							*((void**)addr) = (void *)v;
							break;
		default:
			internalError("strange ... ref type %d set to int64", type);
		}
	} else if (refType == REF_INSTANCE) {
		internalError("boo");
	} else if (refType == REF_POINTER) {
		switch (type) {
		case S_BOOL:	*val.boolP = (v!=0); break;
		case S_FLOAT:	*val.floatP = ((float)v); break;
		case S_BYTE:	*val.byteP = ((uchar)v); break;
		case S_SHORT:	*val.shortP = ((int16)v); break;
		case S_INT:		*val.intP = ((int32)v); break;
		case S_LONG:	*val.longP = v; break;
		case S_TIME:	if (val.timeP) val.timeP->ticks = ((int32)v); break;
		default:
			internalError("strange ... type %d set to int64", type);
		}
	} else if (refType == REF_VALUE) {
		switch (type) {
		case S_BOOL:		val.Bool = (v!=0);	break;
		case S_FLOAT:		val.Float = ((float)v);	break;
		case S_BYTE:		val.byte = ((uchar)v); break;
		case S_SHORT:		val.Short = ((int16)v); break;
		case S_INT:			val.Int = ((int32)v); break;
		case S_LONG:		val.Long = v; break;
		default:
			internalError("strange ... type %d set to int64", type);
		}
	}
}

void
TypedValue::SetValue(int32 v, class QuasiStack *stack)
{
	if (refType == REF_STACK) {
		if (stack == nullptr || stack->mulch == nullptr)
			return;
		uchar *addr = stack->stk.vars + val.stackAddress.offset;
//		fprintf(stderr, "set int: addr %x\n", addr);
		switch (type) {
		case S_BOOL:		*((uchar*)addr) = (uchar)v;	break;
		case S_FLOAT:		*((float*)addr) = (float)v;	break;
		case S_BYTE:		*((uchar*)addr) = (uchar)v; break;
		case S_SHORT:		*((int16*)addr) = (int16)v; break;
		case S_INT:			*((int32*)addr) = (int32)v; break;
		case S_LONG:		*((int64*)addr) = (int64)v; break;
		case S_TIME:		((Time*)addr)->ticks = v; break;
		case S_STRANGE_POINTER:
							*((void**)addr) = (void *)v;
							break;
		default:
			internalError("strange ... ref type %d set to long", type);
		}
	} else if (refType == REF_INSTANCE) {
		internalError("boo");
	} else if (refType == REF_POINTER) {
		switch (type) {
		case S_BOOL:	*val.boolP = (v!=0); break;
		case S_FLOAT:	*val.floatP = ((float)v); break;
		case S_BYTE:	*val.byteP = ((uchar)v); break;
		case S_SHORT:	*val.shortP = ((int16)v); break;
		case S_INT:		*val.intP = v; break;
		case S_LONG:	*val.longP = v; break;
		case S_TIME:	if (val.timeP) val.timeP->ticks = v; break;
		default:
			internalError("strange ... type %d set to long", type);
		}
	} else if (refType == REF_VALUE) {
		switch (type) {
		case S_BOOL:		val.Bool = (v!=0);	break;
		case S_FLOAT:		val.Float = ((float)v);	break;
		case S_BYTE:		val.byte = ((uchar)v); break;
		case S_SHORT:		val.Short = ((int16)v); break;
		case S_INT:			val.Int = v; break;
		case S_LONG:		val.Long = v; break;
		case S_TIME:		val.time.ticks = v; break;
//		case S_TIME:		((Time*)(val.timeBuf))->ticks = v; break;
		default:
			internalError("strange ... type %d set to long", type);
		}
	}
}

void
TypedValue::SetValue(float v, class QuasiStack *stack)
{
	if (refType == REF_STACK) {
		if (stack == nullptr || stack->mulch == nullptr)
			return;
		uchar *addr = stack->stk.vars + val.stackAddress.offset;
//		fprintf(stderr, "set float: addr %x\n", addr);
		switch (type) {
		case S_BOOL:		*((uchar*)addr) = (v!=0);	break;
		case S_FLOAT:		*((float*)addr) = v;	break;
		case S_BYTE:		*((uchar*)addr) = ((uchar)v); break;
		case S_SHORT:		*((int16*)addr) = ((int16)v); break;
		case S_INT:			*((int32*)addr) = ((int32)v); break;
		case S_LONG:		*((int64*)addr) = ((int64)v); break;
		case S_TIME:		((Time*)addr)->ticks = ((tick_t)v); break;
		default:
			internalError("strange ... ref type %d set to int64", type);
		}
	} else if (refType == REF_INSTANCE) {
		internalError("boo");
	} else if (refType == REF_POINTER) {
		switch (type) {
		case S_BOOL:	*val.boolP = (v!=0); break;
		case S_FLOAT:	*val.floatP = v; break;
		case S_BYTE:	*val.byteP = ((int8)v); break;
		case S_SHORT:	*val.shortP = ((int16)v); break;
		case S_INT:		*val.intP = ((int32)v); break;
		case S_LONG:	*val.longP = ((int64)v); break;
		case S_TIME:	if (val.timeP) val.timeP->ticks = ((tick_t)v); break;
		default:
			internalError("strange ... type %d set to float", type);
		}
	} else if (refType == REF_VALUE) {
		switch (type) {
		case S_BOOL:		val.Bool = (v!=0);	break;
		case S_FLOAT:		val.Float = v;	break;
		case S_BYTE:		val.byte = ((uchar)v); break;
		case S_SHORT:		val.Short = ((int16)v); break;
		case S_INT:			val.Int = ((int32)v); break;
		case S_LONG:		val.Long = ((int64)v); break;
		case S_TIME:		val.time.ticks = v; break;
//		case S_TIME:		((Time*)(val.timeBuf))->ticks = ((tick_t)v); break;
		default:
			internalError("strange ... type %d set to float", type);
		}
	}
}

void
TypedValue::SetDoubleValue(double v, class QuasiStack *stack)
{
	if (refType == REF_STACK) {
		if (stack == nullptr || stack->mulch == nullptr)
			return;
		uchar *addr = stack->stk.vars + val.stackAddress.offset;
//		fprintf(stderr, "set float: addr %x\n", addr);
		switch (type) {
		case S_DOUBLE:		*((double*)addr) = v; break;
		case S_BOOL:		*((uchar*)addr) = (v!=0);	break;
		case S_FLOAT:		*((float*)addr) = v;	break;
		case S_BYTE:		*((uchar*)addr) = ((uchar)v); break;
		case S_SHORT:		*((int16*)addr) = ((int16)v); break;
		case S_INT:			*((int32*)addr) = ((int32)v); break;
		case S_LONG:		*((int64*)addr) = ((int64)v); break;
		case S_TIME:		((Time*)addr)->ticks = ((tick_t)v); break;
		default:
			internalError("strange ... ref type %d set to double", type);
		}
	} else if (refType == REF_INSTANCE) {
		internalError("strange ... instance type %d set to double", type);
	} else if (refType == REF_POINTER) {
		switch (type) {
		default:
			internalError("strange ... type %d set to double", type);
		}
	} else if (refType == REF_VALUE) {
		switch (type) {
		default:
			internalError("strange ... type %d set to double", type);
		}
	}
}
void
TypedValue::SetValue(Time &d, class QuasiStack *stack)
{
	if (type != S_TIME) {
		internalError("must be a valid time");
		return;
	}
	if (refType == REF_STACK) {
		if (stack == nullptr || stack->mulch == nullptr)
			return;
		uchar *addr = stack->stk.vars + val.stackAddress.offset;
		switch (type) {
		case S_TIME:		*((Time*)addr) = d; return;
		default:			return;
		}
	} else if (refType == REF_VALUE) {
		val.time = d;
	} else if (refType == REF_POINTER) {
		if (val.timeP) *val.timeP = d;
	}
}

void
TypedValue::SetPointerValue(void *d, class QuasiStack *stack)
{
	if (refType == REF_STACK) {
		if (stack == nullptr || stack->mulch == nullptr)
			return;
		void **addr = (void**)(stack->stk.vars + val.stackAddress.offset);
		*addr = d;
		fprintf(stderr, "set %x in %x\n", (unsigned) *addr, (unsigned) addr);
	} else if (refType == REF_VALUE) {	// should be cool for all pointer types
		val.pointer = d;
	} else if (refType == REF_POINTER) {
		val.pointer = d;
	// or ?????
	// if (val.pointerP) *val.pointerP = d;
	}
}

/*
void
TypedValue::SetValue(Pool *d)
{
	if (type != S_POOL) {
		internalError("must be a valid pool");
	} else {
		val.pool = d;
	}
}

void
TypedValue::SetValue(FILE *d)
{
	if (type != S_FILE) {
		internalError("must be a valid file ref");
	} else {
		val.file = d;
	}
}
void
TypedValue::SetValue(Channel *d)
{
	if (type != S_CHANNEL) {
		internalError("must be a valid channel");
	} else {
		val.channel = d;
	}
}
*/

void
TypedValue::SetValue(TypedValue *vp)
{
	if (type == vp->type) {
		val = vp->val;
	} else if (refType == REF_VALUE) {
		switch (type) {
		case S_BOOL:		val.Bool = vp->BoolValue(nullptr); break;
		case S_BYTE:		val.byte = vp->ByteValue(nullptr); break;
		case S_SHORT:		val.Short = vp->ShortValue(nullptr); break;
		case S_LONG:		val.Long = vp->LongValue(nullptr); break;
		case S_INT:			val.Int = vp->IntValue(nullptr); break;
	    case S_FLOAT:		val.Float = vp->FloatValue(nullptr); break;
		}
	}
//	fprintf(stderr, "SetValue %d %d :: %d %d\n", type, val.Int,
//			vp->type, vp->val.Int); 
}

void
TypedValue::SetLabelValue(Block *b, StabEnt *sym)
{
	if (type == S_LABEL) {
		val.label.block = b;
		val.label.objContext = sym;
	}
}

void
TypedValue::SetBuiltinValue(Stackable *p, int16 t, int16 st)
{
	if (type == S_BUILTIN) {
		val.builtin.stackable = p;
		val.builtin.type = t;
		val.builtin.subType = st;
	}
}

void
TypedValue::SetStateValue(State *s, int32 state)
{
	if (type == S_STATE) {
		val.state.states = s;
		val.state.value = state;
	}
}


class Schedulable *
	TypedValue::SchedulableValue()
{
	switch (type) {
	case S_SAMPLE:	return val.sample;
	case S_VOICE: 	return val.voice;
	case S_POOL: 	return val.pool;
		//	case S_PORT: 	return val.port;
#ifdef QUA_V_APP_HANDLER
	case S_APPLICATION: return val.application;
#endif
	}
	return nullptr;
}

Executable *
TypedValue::ExecutableValue()
{
	switch (type) {
	case S_SAMPLE:	return val.sample;
	case S_VOICE: 	return val.voice;
	case S_POOL: 	return val.pool;
		//	case S_PORT: 	return val.port;
#ifdef QUA_V_APP_HANDLER
	case S_APPLICATION: return val.application;
#endif
	case S_LAMBDA:	return val.lambda;
	case S_TEMPLATE: return val.qTemplate;
	}
	return nullptr;
}


Stackable *
TypedValue::StackableValue()
{
	switch (type) {
	case S_SAMPLE:	return val.sample;
	case S_VOICE: 	return val.voice;
	case S_POOL: 	return val.pool;
	case S_PORT: 	return val.port;
	case S_LAMBDA:	return val.lambda;
	case S_TEMPLATE: return val.qTemplate;
	case S_BUILTIN:	return val.builtin.stackable;
	case S_QUA: 	return val.qua;
	case S_EVENT:	return val.event;
	case S_CHANNEL: return val.channel;
#ifdef QUA_V_APP_HANDLER
	case S_APPLICATION: return val.application;
#endif
#ifdef QUA_V_VST_HOST
	case S_VST_PLUGIN: return val.vst;
#endif
	}
	return nullptr;
}


/*
* SetValue(*)
*   set the value of a BaseValueType according to its type
*/
void
TypedValue::SetTo(const string &strval)
{
	if (strval.size()) {
		switch (type) {
		case S_BOOL:		val.Bool = (atoi(strval.c_str()) != 0); break;
		case S_BYTE:		val.byte = atoi(strval.c_str()); break;
		case S_INT:			val.Int = atoi(strval.c_str()); break;
		case S_SHORT:		val.Short = atoi(strval.c_str()); break;
		case S_LONG:		val.Long = atoi(strval.c_str()); break;
		case S_FLOAT:		val.Float = atof(strval.c_str()); break;
		case S_POOL:		val.pool = findPool(strval.c_str()); break;
		case S_SAMPLE:		val.sample = findSample(strval.c_str()); break;
//		case S_STRING:		val.string = strval; break;
		default:
			internalError("can't set value to string, type = %d", type);
		}
		//		fprintf(stderr, "SetVal: %s %d %d\n", strval, type, val.Int);
	}
}



/*
 * Set(*)
 *   set the value and type of a BaseValueType
 */

void
TypedValue::Set(TypedValue *vp)
{
	type = vp->type;
	val = vp->val;
	refType = vp->refType;
	indirection = vp->indirection;
	flags = vp->flags;
}

void
TypedValue::Set(base_type_t typ, ref_type_t ref, StabEnt *c, int32 off)
{
	type = typ;
	refType = ref;
	val.stackAddress.context = c;
	val.stackAddress.offset = off;
	indirection = 0;
	flags = 0;
}

void
TypedValue::Set(base_type_t typ, ref_type_t reft)
{
	type = typ;
	refType = reft;
	val.pointer = 0;
	indirection = 0;
	flags = 0;
}

void
TypedValue::Set(Time *t)
{
	type = S_TIME;
	val.timeP = t;
	refType = REF_POINTER;
	indirection = 0;
	flags = 0;
}

void
TypedValue::Set(tick_t ticks, Metric *m)
{
	type = S_TIME;
	val.time.ticks = ticks;
	val.time.metric = m;
	refType = REF_VALUE;
	indirection = 0;
	flags = 0;
}

void
TypedValue::Set(Instance *d)
{
	type = S_INSTANCE;
	indirection = 0;
	refType = REF_VALUE;
	val.instance = d;
	flags = 0;
}

//void
//TypedValue::Set(Time t)
//{
//	type = S_TIME;
//	*((Time*)val.timeBuf) = t;
//	refType = REF_VALUE;
//}

//void
//TypedValue::Set(int32 typ, int64 val)
//{
//	type = typ;
//    switch (typ) {
//    case S_LONG:		val.Long = val;
//    	break;
//    default:
//		internalError("Set(int64): Unexpected type %d", typ);
//    }
//}
//
//void
//TypedValue::Set(int32 typ, void *val)
//{
// 	type = typ;
//	switch (typ) {
//	case S_UNKNOWN:		val.pointer = val;
//    case S_QUA:		val.Qua = (Qua *) val; break;
//    case S_POOL:		val.Pool = (Pool *) val; break;
//    case S_SAMPLE:		val.Sample = (Sample *) val; break;
//    case S_INSTANCE:	val.Instance = (Instance *) val; break;
//    case S_VOICE:		val.Voice = (Voice *) val; break;
//    case S_APPLICATION:	val.Application = (Application *) val; break;
//    case S_CHANNEL:		val.Channel = (Channel *) val; break;
//    case S_OUTPUT:	val.Output = (Output *) val; break;
//    case S_LAMBDA:		val.Lambda = (Lambda *) val; break;
//    case S_STREAM_ITEM:val.StreamItemList = (StreamItem *) val; break;
//    case S_STREAM:		val.Stream = (Stream *) val; break;
//    case S_STRING:
//   		val.String = new char[strlen((char *)val) + 1];
//    	strcpy(val.String, (char *)val);
//    	break;
//    case S_LIST:		val.List = (TypedValueList *) val; break;
//    case S_MESSAGE:	val.MesgP = (BMessage *) val; break;
//    case S_EXPRESSION:	val.Block = (Block **) val; break;
//    case S_BLOCK:		val.Block = (Block **) val; break;
//	case S_NOTE:	val.NoteP = (Note *) val; break;
//	case S_CTRL:	val.CtrlP = (Ctrl *) val; break;
//	case S_PROG:	val.ProgP = (Prog *) val; break;
//	case S_BEND:	val.BendP = (Bend *) val; break;
//	case S_SYSC:	val.SysCP = (SysC *) val; break;
//	case S_SYSX:	val.SysXP = (SysX *) val; break;
//	case S_JOY:		val.JoyP = (QuaJoy *) val; break;
//	case S_TIME:	val.TimeP = (Time *) val; break;
//    case S_BYTE_PTR:	val.ByteP = (int8 *) val; break;
//    case S_SHORT_PTR:	val.ShortP = (int16 *) val; break;
//    case S_INT_PTR:		val.IntP = (int32 *) val; break;
//    case S_LONG_PTR:	val.LongP = (int64 *) val; break;
//    case S_FLOAT_PTR:	val.FloatP = (float *) val; break;
//    case S_BOOL:		val.Bool = (bool) val; break;
//    case S_BOOL_PTR:	val.BoolP = (bool *) val; break;
//    case S_BYTE:		val.Byte = (int8) val; break;
//    case S_SHORT:		val.Short = (int16) val; break;
//    case S_INT:			val.Int = (int32) val; break;
//    case S_LONG:		val.Long = (int64) val; break;
//    case S_STRUCT:		break;
//    case S_FLOAT:
//    	val.Float = *((float*) &val);
//    	internalError("Set(void *)invalid float conversion?? %g\n", *((float *) &val));
//    	break;
//    default:
//		internalError("Set(void *):Unexpected type %d", typ);
//    }
//}
//
//void
//TypedValue::Set(int32 typ, int32 val)
//{
//	type = typ;
//    switch (typ) {
//    case S_BOOL:		val.Bool = (bool) val; break;
//    case S_BYTE:		val.Byte = (int8) val; break;
//    case S_SHORT:		val.Short = (int16) val; break;
//    case S_INT:			val.Int = (int32) val; break;
//    case S_LONG:		val.Long = (int64) val; break;
//    case S_FLOAT:		val.Float = (float) val; break;
//    default:
//		internalError("Set(int32):Unexpected type %d", typ);
//    }
//}

TypedValue &
TypedValue::operator >= (TypedValue &t2)
{
	if (type == S_FLOAT || t2.type == S_FLOAT)
		return Bool((FloatValue(nullptr) >= t2.FloatValue(nullptr)));
	if (type == S_LONG || t2.type == S_LONG)
		return Bool((LongValue(nullptr) >= t2.LongValue(nullptr)));
	return Bool((IntValue(nullptr) >= t2.IntValue(nullptr)));
}
TypedValue &
TypedValue::operator == (TypedValue &t2)
{
	if (type == S_FLOAT || t2.type == S_FLOAT)
		return Bool((FloatValue(nullptr) == t2.FloatValue(nullptr)));
	if (type == S_LONG || t2.type == S_LONG)
		return Bool((LongValue(nullptr) == t2.LongValue(nullptr)));
	return Bool((IntValue(nullptr) == t2.IntValue(nullptr)));
}
TypedValue &
TypedValue::operator != (TypedValue &t2)
{
	if (type == S_FLOAT || t2.type == S_FLOAT)
		return Bool((FloatValue(nullptr) != t2.FloatValue(nullptr)));
	if (type == S_LONG || t2.type == S_LONG)
		return Bool((LongValue(nullptr) != t2.LongValue(nullptr)));
	return Bool((IntValue(nullptr) != t2.IntValue(nullptr)));
}
TypedValue	 &
TypedValue::operator <= (TypedValue &t2)
{
	if (type == S_FLOAT || t2.type == S_FLOAT)
		return Bool((FloatValue(nullptr) <= t2.FloatValue(nullptr)));
	if (type == S_LONG || t2.type == S_LONG)
		return Bool((LongValue(nullptr) <= t2.LongValue(nullptr)));
	return Bool((IntValue(nullptr) <= t2.IntValue(nullptr)));
}
TypedValue	&
TypedValue::operator > (TypedValue &t2)
{
	if (type == S_FLOAT || t2.type == S_FLOAT)
		return Bool((FloatValue(nullptr) > t2.FloatValue(nullptr)));
	if (type == S_LONG || t2.type == S_LONG)
		return Bool((LongValue(nullptr) > t2.LongValue(nullptr)));
	return Bool((IntValue(nullptr) > t2.IntValue(nullptr)));
}
TypedValue	&
TypedValue::operator < (TypedValue &t2)
{
	if (type == S_FLOAT || t2.type == S_FLOAT)
		return Bool((FloatValue(nullptr) < t2.FloatValue(nullptr)));
	if (type == S_LONG || t2.type == S_LONG)
		return Bool((LongValue(nullptr) < t2.LongValue(nullptr)));
	return Bool((IntValue(nullptr) < t2.IntValue(nullptr)));
}
TypedValue	&
TypedValue::operator && (TypedValue &t2)
{
	return Bool((BoolValue(nullptr) && t2.BoolValue(nullptr)));
}

TypedValue	&
TypedValue::operator || (TypedValue &t2)
{
	return Bool((BoolValue(nullptr) && t2.BoolValue(nullptr)));
}

//void
//TypedValue::operator ++
//{
//	if (type == S_FLOAT)
//		val.Float = FloatValue(nullptr) + 1;
//	if (type == S_LONG || t2.type == S_LONG)
//		return Long((int64)(LongValue(nullptr) + t2.LongValue(nullptr)));
//	return Int( IntValue(nullptr) + t2.IntValue(nullptr));
//}
//
//void
//TypedValue::operator --
//{
//	if (type == S_FLOAT || t2.type == S_FLOAT)
//		return Float( FloatValue(nullptr) - t2.FloatValue(nullptr));
//	if (type == S_LONG || t2.type == S_LONG)
//		return Long((int64)(LongValue(nullptr) - t2.LongValue(nullptr)));
//	return Int( IntValue(nullptr) - t2.IntValue(nullptr));
//}

TypedValue	&
TypedValue::operator + (TypedValue &t2)
{
	if (type == S_TIME && t2.type == S_TIME) {
		Time	*ti1 = TimeValue();
		Time	*ti2 = t2.TimeValue();
		return TimeV(*ti1 + *ti2);
	}
	if (type == S_TIME) {
		Time	*ti1 = TimeValue();
		int		ti2 = t2.IntValue(nullptr);
		return TimeV(*ti1 + ti2);
	}
	if (t2.type == S_TIME) {
		int		ti1 = IntValue(nullptr);
		Time	*ti2 = t2.TimeValue();
		return TimeV(*ti2 + ti1);
	}
	if (type == S_FLOAT || t2.type == S_FLOAT)
		return Float( FloatValue(nullptr) + t2.FloatValue(nullptr));
	if (type == S_LONG || t2.type == S_LONG)
		return Long((int64)(LongValue(nullptr) + t2.LongValue(nullptr)));
	return Int( IntValue(nullptr) + t2.IntValue(nullptr));
}

TypedValue	&
TypedValue::operator - (TypedValue &t2)
{
	if (type == S_TIME && t2.type == S_TIME) {
		Time	*ti1 = TimeValue();
		Time	*ti2 = t2.TimeValue();
		return TimeV(*ti1 - *ti2);
	}
	if (type == S_TIME) {
		Time	*ti1 = TimeValue();
		int		ti2 = t2.IntValue(nullptr);
		return TimeV(*ti1 - ti2);
	}
	if (t2.type == S_TIME) {
		int		ti1 = IntValue(nullptr);
		Time	*ti2 = t2.TimeValue();
		return TimeV(Time(ti1) - *ti2);
	}
	if (type == S_FLOAT || t2.type == S_FLOAT)
		return Float( FloatValue(nullptr) - t2.FloatValue(nullptr));
	if (type == S_LONG || t2.type == S_LONG)
		return Long((int64)(LongValue(nullptr) - t2.LongValue(nullptr)));
	return Int( IntValue(nullptr) - t2.IntValue(nullptr));
}

TypedValue	&
TypedValue::operator * (TypedValue &t2)
{
	if (type == S_FLOAT || t2.type == S_FLOAT) {
		return Float( FloatValue(nullptr) * t2.FloatValue(nullptr));
	}
	if (type == S_LONG || t2.type == S_LONG)
		return Long((int64)(LongValue(nullptr) * t2.LongValue(nullptr)));
	return Int( IntValue(nullptr) * t2.IntValue(nullptr));
}

TypedValue	&
TypedValue::operator / (TypedValue &t2)
{
	if (type == S_FLOAT || t2.type == S_FLOAT) {
		return Float( FloatValue(nullptr) / t2.FloatValue(nullptr));
	}
	if (type == S_LONG || t2.type == S_LONG)
		return Long((int64)(LongValue(nullptr) / t2.LongValue(nullptr)));
	return Int( IntValue(nullptr) / t2.IntValue(nullptr));
}

TypedValue	&
TypedValue::operator % (TypedValue &t2)
{
	if (type == S_FLOAT || t2.type == S_FLOAT) {
		internalError("Dodgy mod: float to float");
		return Int((int32)-1);
	}
	if (type == S_LONG || t2.type == S_LONG)
		return Long((int64)(LongValue(nullptr) % t2.LongValue(nullptr)));
	return Int( IntValue(nullptr) % t2.IntValue(nullptr));
}

TypedValue	&
TypedValue::operator - ()
{
	if (type == S_FLOAT) {
		return Float( - FloatValue(nullptr));
	}
	if (type == S_LONG)
		return Long((int64)(- LongValue(nullptr)));
	return Int(- IntValue(nullptr));
}

TypedValue	&
TypedValue::operator ! ()
{
	if (type == S_FLOAT) {
		return Int((int32)(! FloatValue(nullptr)));
	}
	if (type == S_LONG)
		return Long((int64)(! LongValue(nullptr)));
	return Int((int32)(! IntValue(nullptr)));
}

TypedValue	&
TypedValue::operator ~ ()
{
	if (type == S_FLOAT) {
		internalError("dodgy bitwise operand");
		return Int((int32)-1);
	}
	if (type == S_LONG)
		return Long((int64)(~ LongValue(nullptr)));
	return Int(~ IntValue(nullptr));
}
