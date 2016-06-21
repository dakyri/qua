#include "qua_version.h"

//////////////////////////////////////////////////////////////////////
// LValue stuff
//  lvalues are the main layer between symbols of
// the sequencers name space, and processing blocks
//   understands complex address expressions, array
// refs, plugin params. checks global tempo changes.
//
//   maybe should be the main hook for doing interface updates
//  currently this is StabEnt::UpdatateControlVar, referenced by
//  update active when an assignment is made.
//
//    The TypedValue hooks to do similar manipulations
// are lightweight and deal only with retrieval from a
// known bit of a known stack with very few checks.
// mainly for values, and hardwired pieces of known stack
/////////////////////////////////////////////////////////////////////
#if defined(WIN32)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <time.h>
#include <ctype.h>

#endif

///////////////// Application headers

#include "StdDefs.h"

#include "Sym.h"
#include "Block.h"
#include "Channel.h"
#include "ControllerBridge.h"
#include "Stackable.h"
#include "QuasiStack.h"
#include "Qua.h"
#include "Template.h"
#include "Voice.h"
#include "Pool.h"
#include "Sample.h"
#include "Clip.h"
#include "Method.h"
#include "Envelope.h"
#ifdef QUA_V_VST_HOST
#include "VstPlugin.h"
#endif
#include "Parse.h"
#include "QuaPort.h"

#ifdef QUA_V_APP_HANDLER
#include "include/Application.h"
#endif

flag	debug_lval=0;

//////////////////////////////////////////////////////////////////////////
// main hook for creating an lvalue from a symbol, given a particular
// stream and stack position
//////////////////////////////////////////////////////////////////////////
void
StabEnt::SetLValue(LValue &lval, StreamItem *items, Stacker *stacker, StabEnt *stackCtxt, QuasiStack *stack)
{
	lval.sym = this;
	lval.stack = stack;
	lval.addr = nullptr;
	lval.indirection = indirection;
	lval.stacker = stacker;
	lval.stackerSym = stackCtxt;
	BaseAddress(lval, items, stacker, stack);
	if (debug_lval >= 2) {
		fprintf(stderr, "SetLValue: %s addr %x indirection %d stacker %x\n",
			name, (unsigned) lval.addr, lval.indirection, (unsigned)lval.stacker);
	}
}


//////////////////////////////////////////////////////////////////////////
// main hook for creating an lvalue from a symbol, given a particular
// stream and stack position
// does offset calculation for symbols that are found implicitly
// within other structures that may be hidden
//////////////////////////////////////////////////////////////////////////

void
StabEnt::BaseAddress(LValue &lval, StreamItem *items, Stacker *stacker, QuasiStack *stack)
{
	StabEnt		*ctxt;

	lval.addr = nullptr;

	if (debug_lval >= 2) {
		(stderr, "BaseAddress(%s %d %d)\n", name, type, refType);
	}
	switch (refType) {
	
	case REF_POINTER: {
		switch (type) {
		case S_BOOL:	lval.addr = (char *) val.boolP; return;
		case S_BYTE:	lval.addr = (char *) val.byteP; return;
		case S_SHORT:	lval.addr = (char *) val.shortP; return;
		case S_INT:		lval.addr = (char *) val.intP; return;
		case S_LONG:	lval.addr = (char *) val.longP; return;
		case S_TIME:	lval.addr = (char *) val.timeP; return;
		case S_FLOAT:	lval.addr = (char *) val.floatP; return;
#ifdef QUA_V_VST_HOST
		case S_VST_PARAM:	lval.addr = (char *) stack->stk.afx; return;
		case S_VST_PROGRAM:	lval.addr = (char *) stack->stk.afx; return;
#endif
#ifdef QUA_V_PORT_PARAM
		case S_PORTPARAM:	lval.addr = (char *) val.parameter; return;
#endif
		default:
			lval.addr = (char *) val.pointer; return;
		}
		break;
	}
	
	case REF_VALUE: {
		switch (type) {
		case S_BOOL:	lval.addr = (char *) &val.Bool; return;
		case S_BYTE:	lval.addr = (char *) &val.byte; return;
		case S_SHORT:	lval.addr = (char *) &val.Short; return;
		case S_INT:		lval.addr = (char *) &val.Int; return;
		case S_LONG:	lval.addr = (char *) &val.Long; return;
		case S_FLOAT:	lval.addr = (char *) &val.Float; return;
		case S_TIME:	lval.addr = (char *) &val.time; return;
//		case S_TIME:	lval.addr = val.timeBuf; return;
		case S_STRANGE_POINTER:	lval.addr = (char *) val.pointer; return;
		case S_LABEL:	lval.addr = (char *) &val.label; return;
		case S_BUILTIN:	lval.addr = (char *) &val.builtin; return;
		case S_STATE:	lval.addr = (char *) &val.state; return;
		default:
			lval.addr = (char *) &val.pointer; return;
		}
		break;
	}
	
	case REF_INSTANCE: {
		char	*addr = nullptr;
		switch (type) {
		case S_SHORT:
		case S_BOOL:
		case S_BYTE:
		case S_INT:
		case S_LONG:
		case S_FLOAT:
		case S_STRANGE_POINTER:
		case S_TIME:
		case S_BLOCK:
		case S_EXPRESSION:
		case S_CHANNEL:
			addr = (char *)val.stackAddress.offset;
		}
		if (addr == 0) {
			internalError("Unusual null data reference");
		}
		lval.addr = stacker? ((uint32)stacker) + addr: nullptr;
		lval.indirection = 1;
		return;
	}

	case REF_STACK:
	default: {
		char *addr = (char *)val.stackAddress.offset;
		base_type_t		streamItemType = type;

//		fprintf(stderr, "so %d %d\n", addr, val.stackAddress.offset);

		ctxt = context;
		while (ctxt != nullptr) {
//			fprintf(stderr, "context %x %s\n", ctxt, ctxt->name);
			if (ctxt->refType == REF_STACK) {
				addr += (int) ctxt->val.stackAddress.offset;
				
//				fprintf(stderr, "so %d %d\n", addr, ctxt->val.stackAddress.offset);
				
				switch(ctxt->type) {
				case S_CTRL:
				case S_SYSX:
				case S_SYSC:
				case S_BEND:
				case S_NOTE:
				case S_JOY:
				case S_MESSAGE:
					streamItemType = ctxt->type;
				break;
				}
			} else if (ctxt->refType == REF_INSTANCE) {
				lval.addr = stacker? ((uint32)stacker) + addr: nullptr;
				return;

			} else { // REF_VALUE, REF_POINTER
				switch (ctxt->type) {
					case S_PORT:
						lval.addr = addr;
						return;

					case S_PARAMGROUP:
						break;
						
					case S_TEMPLATE:
					case S_VOICE:
					case S_METHOD:
					case S_POOL:
					case S_APPLICATION:
					case S_QUA:
					case S_CHANNEL:
					case S_SAMPLE:
					case S_INSTANCE:
					case S_BUILTIN:
						for (;stack!=nullptr; stack=stack->lowerFrame) {
							if (stack->context == ctxt)
								break;
						}
						if (stack == nullptr || stack->mulch == nullptr)
							return;
						lval.addr = addr + (uint32) stack->stk.vars;
						lval.stack = stack;
						return;
			
					case S_STREAM_ITEM: {
						StreamItem *p = items;
	//					if (items)
	//						fprintf(stderr, "stream item %x %x ittype %d\n", items, items->type, streamItemType);
	//					else
	//						fprintf(stderr, "no items...\n");
						if (p) {
							if (p->type == S_VALUE)	{
								StreamValue *vp = ((StreamValue *)p);
								if (vp->value.type == S_STREAM_ITEM)
									items = vp->value.val.streamItemList;
							}
						}
				
						if ( 	items == nullptr ||
								items->type != streamItemType) {
							return;
						}
	//					fprintf(stderr, "addr %x\n", (char *)items + (int) addr);
						lval.addr =  (char *)items + (int) addr;
						return;
					}

					case S_CTRL:
					case S_SYSX:
					case S_SYSC:
					case S_BEND:
					case S_NOTE:
					case S_JOY:
					case S_MESSAGE:
						lval.addr =	addr + (int) ctxt->val.pointer;
						return;
						
					default:
						internalError("BaseAddress: surprising context %d for %s\n", ctxt->type, name);
						return;//addr;
				}
			}
			
			ctxt = ctxt->context;
		}

		break;
	}
	}
	
	internalError("Qua::BaseAddress(): severe misunderstanding in context for %s\n", name);
		
	return;
}

QuasiStack	*
FindStackFor(QuasiStack *from, StabEnt *context, StabEnt *label, StabEnt *obj, long frameind)
{
	if (from != nullptr) {
// if this we're running at the stack level we want or our ancestors are ...
		QuasiStack *qs = from;
		while (qs != nullptr) {
			if (qs->context == context) {
				QuasiStack *callstk = qs->frameAt(frameind);
				if (callstk && (callstk->context == label || callstk->context == obj)) {
					return callstk;
				}
			}
			qs = qs->lowerFrame;
		}
// or my own descendants
// ?????????????????????????
// ... need to include invisible stacks like rx, tx, wake, sleep etc. in my descendents 
		for (QuasiStack *qs: from->higherFrame) {
			for (QuasiStack *ps: qs->higherFrame) {
				if (ps->context == context) {
					QuasiStack *callstk = ps->frameAt(frameind);
					if (callstk && (callstk->context == label || callstk->context == obj)) {
						return callstk;
					}
				}
			}
		}
// okay, what about siblings and cousins
		qs = from->lowerFrame;
		while (qs != nullptr) {
			for (int i=0; i<qs->countFrames(); i++) {
				QuasiStack	*ps = qs->frameAt(i);
				if (ps->context == context) {
					QuasiStack *callstk = ps->frameAt(frameind);
					if (callstk && (callstk->context == label || callstk->context == obj)) {
						return callstk;
					}
				}
			}
			qs = qs->lowerFrame;
		}
	}
	fprintf(stderr, "... a quick hunt around and i couldn't find stack for %s.(%s/%s/%d) from %s\n",
		context?context->name:"<null>",
		label?label->name:"<null>",
		obj?obj->name:"<null>",
		frameind,
		from?from->context->name:"<null>");
	return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
//  calls the hooks to initialise a variable. presumably initialisations
// won't refer to a stream.
/////////////////////////////////////////////////////////////////////////////
void
StabEnt::Initialize(QuasiStack *stack)
{
	if (  iniVal.type != S_UNKNOWN &&
		  		!(refType == REF_INSTANCE && stack->stacker == nullptr)) {
		LValue	lval;
		SetLValue(lval, nullptr, stack->stacker, stack->stackerSym, stack);
		lval.Initialize();
		
		if (debug_lval)
			fprintf(stderr, "Symbol %s initialised\n", UniqueName());
	} else {
		if (debug_lval)
			fprintf(stderr, "Symbol %s not initialised\n", UniqueName());
	}
}

/////////////////////////////////////////////////////////////////////////
// LValueAte
//   the principal hook for finding retrieving calculated lvalues
//  ... struct references arrays item. item lists done by LvalueFor
////////////////////////////////////////////////////////////////////////
void
LValueAte(LValue &lval, Block *b, StreamItem *items, Stacker *stacker, StabEnt *stackCtxt, QuasiStack *stack)
{
	lval.addr = nullptr;
	lval.sym = nullptr;
	lval.stacker = stacker;
	lval.indirection = 0;
	lval.stackerSym = stackCtxt;
	lval.stack = stack;
	switch (b->type) {
	
	case Block::C_SYM: {
		if (debug_lval >= 2) {
			fprintf(stderr, "sym lval %s\n", b->crap.sym?b->crap.sym->name:"<>");
		}
		if (b->crap.sym == nullptr) {
			return;
		}
		b->crap.sym->SetLValue(lval, items, stacker, stackCtxt, stack);
		switch (lval.sym->type) {
			case TypedValue::S_LABEL: {
				ALabel *lb = lval.sym->LabelValue();
				long frameIndex = lb->block->crap.call.frameIndex;
				if (stack == nullptr) {
					lval.addr = nullptr;
					return;
				}
				lval.stack = FindStackFor(stack, lval.sym->context, lval.sym,
										lb->objContext, frameIndex);
				lval.addr = (char*)lval.stack;
				break;
			}
			case TypedValue::S_INSTANCE: {
				lval.stack = lval.sym->InstanceValue()->mainStack;
				lval.addr = (char *)lval.stack;
				break;
			}
		}
		return;
	}
	
	case Block::C_STRUCTURE_REF: {
		if (debug_lval >= 2) {
			fprintf(stderr, "struct ref lval\n");
		}
		if (	b->crap.structureRef.member->refType == TypedValue::REF_VALUE ||
				b->crap.structureRef.member->refType == TypedValue::REF_POINTER) {
			b->crap.structureRef.member->SetLValue(
					lval, items, stacker, stackCtxt, stack);
			return;
		}
		StabEnt		*base = nullptr;
		LValue bval;
		LValueAte(bval,
				b->crap.structureRef.base,
				items,
				stacker,
				stackCtxt,
				stack);
		lval = bval;
		base = lval.sym;

		if (base) {
			switch(base->type) {
				case TypedValue::S_LABEL: {
					ALabel *lb = lval.sym->LabelValue();
					long frameIndex = lb->block->crap.call.frameIndex;
					if (stack == nullptr) {
						lval.addr = nullptr;
						lval.sym = b->crap.structureRef.member;
						return;
					}
					if (b->crap.structureRef.member->type == TypedValue::S_LABEL) {
						QuasiStack	*callStack = (QuasiStack *) lval.addr;
						for (short i=0; i<callStack->countFrames(); i++) {
							QuasiStack	*qs = callStack->frameAt(i);
							if (qs->context == b->crap.structureRef.member) {
								lval.stack = qs;
								lval.addr = (char*)qs;
							}
						}
						lval.sym = b->crap.structureRef.member;
						lval.indirection = b->crap.structureRef.member->indirection;
						return;
					} else if (lb->objContext != nullptr &&
							lb->block != nullptr) {
						QuasiStack	*callFrame = (QuasiStack *)lval.addr;
						switch (lb->objContext->type) { 
							case TypedValue::S_VST_PLUGIN: {
								lval.addr = (char *)callFrame->stk.afx;;
								lval.sym = b->crap.structureRef.member;
								return;
							}
							case TypedValue::S_METHOD:
							case TypedValue::S_BUILTIN: {
								lval.stack = callFrame;
								lval.addr = (char *)callFrame->stk.vars;
								break;
							}
						}
					}
					lval.sym = b->crap.structureRef.member;
					lval.indirection = b->crap.structureRef.member->indirection;
					break;
				}

				case TypedValue::S_INSTANCE: {
					Instance	*inst = (Instance*)lval.addr;
					lval.stack = inst->mainStack;
					lval.addr = (char*)inst->mainStack;
					lval.stacker = inst;
					break;
				}

				case TypedValue::S_PARAMGROUP:
				case TypedValue::S_PORT:
//					return addr;
				case TypedValue::S_TEMPLATE:
				case TypedValue::S_VOICE:
				case TypedValue::S_METHOD:
				case TypedValue::S_POOL:
				case TypedValue::S_APPLICATION:
				case TypedValue::S_QUA:
				case TypedValue::S_CHANNEL:
				case TypedValue::S_SAMPLE:
				case TypedValue::S_BUILTIN: {
					if (b->crap.structureRef.member->type == TypedValue::S_LABEL) {
						Schedulable	*sch;
						Stackable	*stk;
						QuasiStack	*callStack = (QuasiStack *) stack;

						if (lval.sym->ChannelValue() && (stk = lval.sym->ChannelValue())) {
							callStack = stk->stack(0);
						} else if (sch=lval.sym->SchedulableValue()) {
							callStack = sch->instanceAt(0)?sch->instanceAt(0)->mainStack:nullptr;
						}
						for (short i=0; i<callStack->countFrames(); i++) {
							QuasiStack	*qs = callStack->frameAt(i);
							if (qs->context == b->crap.structureRef.member) {
								lval.stack = qs;
								lval.addr = (char*)qs;
								lval.sym = b->crap.structureRef.member;
								lval.indirection = b->crap.structureRef.member->indirection;
								return;
							}
						}
						lval.addr = nullptr;
						lval.sym = b->crap.structureRef.member;
						lval.indirection = b->crap.structureRef.member->indirection;
						return;
					}
					// ?????????? might have to go on a scrounge for this ....
					for (;stack!=nullptr; stack=stack->lowerFrame) {
						if (stack->context == lval.sym)
							break;
					}
					if (stack->mulch == nullptr) {
						lval.addr = nullptr;
						return;
					}
					lval.addr = (char*)(stack? stack->stk.vars : nullptr);
					break;
				}
			}
		}

// if we fall through to here, we have a normal stack or stacker reference,
// set the address to the members offset and head back up the line
		lval.sym = b->crap.structureRef.member;
		if (lval.addr && lval.sym) {
			if (lval.sym->refType == TypedValue::REF_INSTANCE && lval.stacker) {
				lval.addr = ((char*)lval.stacker) + lval.sym->OffsetValue();
			} else {
				lval.addr += lval.sym->OffsetValue();
			}
			lval.indirection = lval.sym->indirection;
		}
		return;
	}
	
	case Block::C_ARRAY_REF: {
		if (debug_lval >= 2) {
			fprintf(stderr, "aref lval\n");
		}
		LValue	bval;
		LValueAte(bval,
			b->crap.arrayRef.base,
			items, 
			stacker,
			stackCtxt,
			stack);
		lval = bval;
		ResultValue		ret_val =
				EvaluateExpression(b->crap.arrayRef.index,
					items, stacker, stackCtxt, stack);
		if (ret_val.Blocked()) {
			lval.addr = nullptr;
		} else {
			int		ind = ret_val.IntValue(nullptr);
			switch (lval.sym->type) {
				case TypedValue::S_STREAM: {
					break;
				}
				default: {
					if (ind >= 0 && ind < lval.sym->Size(
									lval.sym->indirection -
									lval.indirection + 1)) {
						lval.addr +=  ind * lval.sym->Size(
									lval.sym->indirection - lval.indirection);
					} else {
						// maybe block???
						// lval.addr = nullptr ....
					}
					lval.indirection--;
				}
			}
		}
		return;
	}}
	
	return;
}


//////////////////////////////////////////////
// LValue::*
//////////////////////////////////////////////

bool
LValue::SpecialAssignment(int value)
{
	if (sym->type == TypedValue::S_CHANNEL) {
		if (sym->refType == TypedValue::REF_INSTANCE) {
			Instance	*i = (Instance *)stacker;	
			if (value >= 0 && value < i->uberQua->nChannel) {
				i->SetChannel(i->uberQua->channel[value], true);
			}
			return true;
		}
	}
	return false;	
}

bool
LValue::SpecialAssignment(TypedValue *vp)
{
	if (sym->type == TypedValue::S_CHANNEL) {
		if (sym->refType == TypedValue::TypedValue::REF_INSTANCE) {
			Instance	*i = (Instance *)stacker;	
			if (vp->type == TypedValue::S_CHANNEL && vp->val.channel) {
				i->SetChannel(vp->val.channel, true);
			}
			return true;
		}
	}
	return false;	
}

/*
 * extras associated with any particular assignment that operates normally,
 * called after the actual assignment. in particular, there may well be
 * calls out here to update the interface??
 */
void
LValue::AssignmentExtras()
{
	if (sym == nullptr || addr == nullptr)
		return;
	if (sym->refType == TypedValue::REF_INSTANCE) {
		Instance	*i = (Instance *)stacker;	

		if (addr == (char *)&i->duration) {
			i->SetDuration(i->duration, true);	// force an update msg to windo
		} else if (addr == (char *)&i->startTime) {
			i->SetStartTime(i->startTime, true);	// force an update msg to windo
		}
	} else if (sym->refType == TypedValue::REF_POINTER) {
		if (stacker) {
			Qua	*q = stacker->uberQua;
		    if (addr == (char *)&q->metric->tempo &&
						q->metric->tempo != 0) {
				q->SetTempo(q->metric->tempo, true);
	
			}
		}
	} else if (sym->IsControlVar()) {
		if (stacker) {
			Qua	*q = stacker->uberQua;
			q->bridge.display->UpdateControllerDisplay(stackerSym, stack, sym);
		}
	}
}


void
LValue::StoreBlock(Block *v)
{
	if (addr != nullptr && sym != nullptr) {
		if (debug_lval)
			fprintf(stderr, "StoreBlock(): addr %s %x\n", sym->name, (unsigned) addr);
		switch(sym->type) {
		case TypedValue::S_BLOCK:
		case TypedValue::S_EXPRESSION:
			if (*((Block**)addr) != nullptr)
				delete *((Block**)addr);
			*((Block**)addr) = v;
			break;
		}
	}
}

#if defined(QUA_V_VST_HOST)
bool
LValue::VstParamCheck(float v)
{
	if (sym->type == TypedValue::S_VST_PARAM) {
		if (addr != nullptr) {
			AEffect	*afx = (AEffect *)addr;
			afx->setParameter(afx, sym->val.vstParam, v);
			return true;
		}
	}
	return false;
}
#endif

void
LValue::StoreInt(int32 v)
{
	if (sym == nullptr || sym->isDeleted)
		return;
	if (
#if defined(QUA_V_VST_HOST)
	  !VstParamCheck(v) &&
#endif
	  !SpecialAssignment(v) &&
	  addr != nullptr) {
		if (1)//debug_lval)
			fprintf(stderr, "store_int %s.%s (%d): addr %x = %d. %x %x\n",
				sym->context?sym->context->name:"<glbl>",
				sym->name, sym->context?sym->context->type:-1,
				(unsigned) addr, v,
				(unsigned) sym->context, (unsigned) sym->val.stackAddress.context);
		switch(sym->type) {
			case TypedValue::S_BOOL:	*(bool*)addr = (v!=0); break;
			case TypedValue::S_BYTE:	*(int8*)addr = (int8)v; break;
			case TypedValue::S_FLOAT:	*(float*)addr = (float)v; break;
			case TypedValue::S_DOUBLE:	*(double*)addr = v; break;
			case TypedValue::S_INT:		*(int32*)addr = v; break;
			case TypedValue::S_SHORT:	*(int16*)addr = (int16)v; break;
			case TypedValue::S_LONG:	*(int64*)addr = v; break;
			case TypedValue::S_TIME:	((Time*)addr)->ticks = v; break;
			case TypedValue::S_STRANGE_POINTER:	*(void**)addr = (void*)v; break;
		}
		AssignmentExtras();
	}
}

void
LValue::StoreFloat(float v)
{
	if (sym == nullptr || sym->isDeleted)	
		return;
	if (
#if defined(QUA_V_VST_HOST)
	  !VstParamCheck(v) &&
#endif
	  !SpecialAssignment(v) &&
	  addr != nullptr) {
		if (debug_lval)
			fprintf(stderr, "store_float %s.%s (%d): addr %x = %g. %x %x\n",
				sym->context?sym->context->name:"<glbl>",
				sym->name, sym->context?sym->context->type:-1,
				(unsigned) addr, v,
				(unsigned)sym->context, (unsigned)sym->val.stackAddress.context);
		switch(sym->type) {
			case TypedValue::S_BOOL:	*(bool*)addr = (v!=0); break;
			case TypedValue::S_BYTE:	*(int8*)addr = (int8)v; break;
			case TypedValue::S_FLOAT:	*(float*)addr = v; break;
			case TypedValue::S_DOUBLE:	*(double*)addr = v; break;
			case TypedValue::S_INT:		*(int32*)addr = (int32)v; break;
			case TypedValue::S_SHORT:	*(int16*)addr = (int16)v; break;
			case TypedValue::S_LONG:	*(int64*)addr = (int64)v; break;
			case TypedValue::S_TIME:	((Time*)addr)->ticks = (int32)v; break;
			case TypedValue::S_STRANGE_POINTER:
				internalError("StoreFloat: strange pointer not expected");
				*(void**)addr = nullptr; break;
		}
		AssignmentExtras();
	} else {
//		fprintf(stderr, "StoreFloat():: null addr\n");
	}
}

void
LValue::StoreLong(int64 v)
{
	if (sym == nullptr || sym->isDeleted)	
		return;

	if (
#if defined(QUA_V_VST_HOST)
	  !VstParamCheck(v) &&
#endif
	  !SpecialAssignment(v) &&
	  addr != nullptr) {
		if (debug_lval)
			fprintf(stderr, "store_long %s.%s (%d): addr %x = %lld. %x %x\n",
				sym->context?sym->context->name:"<glbl>", sym->name, sym->context?sym->context->type:-1,
				(unsigned)addr, v, (unsigned)sym->context, (unsigned) sym->val.stackAddress.context);
		switch(sym->type) {
			case TypedValue::S_BOOL:	*(bool*)addr = (v!=0); break;
			case TypedValue::S_BYTE:	*(int8*)addr = (int8)v; break;
			case TypedValue::S_FLOAT:	*(float*)addr = (float)v; break;
			case TypedValue::S_DOUBLE:	*(double*)addr = v; break;
			case TypedValue::S_INT:		*(int32*)addr = (int32)v; break;
			case TypedValue::S_SHORT:	*(int16*)addr = (int16)v; break;
			case TypedValue::S_LONG:	*(int64*)addr = v; break;
			case TypedValue::S_TIME:	((Time*)addr)->ticks = (int32)v; break;
			case TypedValue::S_STRANGE_POINTER:	*(void**)addr = (void*)v; break;
			}
		AssignmentExtras();
	}
}

void
LValue::StoreValue(TypedValue *valp)
{
	switch(valp->type)  {
	case TypedValue::S_BOOL:	StoreInt(valp->val.Bool); break;
	case TypedValue::S_BYTE:	StoreInt(valp->val.byte); break;
	case TypedValue::S_SHORT:	StoreInt(valp->val.Short); break;
	case TypedValue::S_INT:		StoreInt(valp->val.Int); break;
	case TypedValue::S_LONG:	StoreLong(valp->val.Long); break;
	case TypedValue::S_FLOAT:	StoreFloat(valp->val.Float); break;
	case TypedValue::S_DOUBLE:	StoreFloat(valp->val.Double); break;
	default:
		if (addr && sym  && !sym->isDeleted && !SpecialAssignment(valp)) {
			switch (sym->type) {
			case TypedValue::S_TIME: {
				if (valp->type == TypedValue::S_TIME) {
					// do appropriate checks & conversions.!!!!!!!!!!
					*((Time*)addr) = *valp->TimeValue();
				} else {
					if (addr) {
						((Time*)addr)->ticks = valp->IntValue(nullptr);
					}
				}
				break;
			}
			
			case TypedValue::S_NOTE: {
				if (valp->type == TypedValue::S_NOTE) {
				}
				break;
			}
			case TypedValue::S_CLIP: {
				if (valp->type == TypedValue::S_CLIP || valp->type == TypedValue::S_STRANGE_POINTER) {
					*((void**)addr) = valp->val.pointer;
				}
				break;
			}
			case TypedValue::S_TAKE: {
				if (valp->type == TypedValue::S_TAKE || valp->type == TypedValue::S_STRANGE_POINTER) {
					*((void**)addr) = valp->val.pointer;
				}
				break;
			}
			default:
				if (valp->type == sym->type) {
					sym->val = valp->val;
				}
				break;
			}
			AssignmentExtras();
		}
	}
}

// these are awful
bool
LValue::StoreData(char *data, int length)
{
	return FALSE;
}

void
LValue::FindData(char**data, int *length)
{
//	char	buf[MAX_SYSX_DATA], *p=buf;
//	StabEnt				*child;
//	int					len = 0;
//	for (child=children; child!=nullptr; child=child->sibling) {
//		switch (child->type) {
//		case TypedValue::S_BYTE:	*((int8 *)p) = val.Byte;	p+=1; len += 1; break;
//		case TypedValue::S_SHORT:	*((int16 *)p) = val.Short; p+=2; len += 2; break;
//		case TypedValue::S_INT:		*((int32 *)p) = val.Int;	p+=4; len += 4; break;
//		case TypedValue::S_LONG:	*((int64 *)p) = val.Long;	p+=8; len += 8; break;
//		case TypedValue::S_FLOAT:	*((float *)p) = val.Float;	p+=4; len += 4; break;
//		case TypedValue::S_METHOD: {
//			char		*nested_data;
//			int			nested_len;
//			child->val.Method->sym->FindData(&nested_data, &nested_len);
//			if (nested_len) {
//				if (len + nested_len > MAX_SYSX_DATA) {
//					reportError("sysx data overload");
//					break;
//				}				
//				memcpy(p, nested_data, nested_len);
//				len += nested_len;
//				delete nested_data;
//			}
//			break;
//		}
//		case TypedValue::S_VOICE: {
//			char		*nested_data;
//			int			nested_len;
//			val.Voice->sym->FindData(&nested_data, &nested_len);
//			if (nested_len) {
//				if (len + nested_len > MAX_SYSX_DATA) {
//					reportError("sysx data overload");
//					break;
//				}				
//				memcpy(p, nested_data, nested_len);
//				len += nested_len;
//				delete nested_data;
//			}
//			break;
//		}
//		default: {
//			break;
//		}}
//	}
//	if (len) {
//		*data = new char[len];
//		memcpy(*data, buf, len);
//	} else {
//		*data = nullptr;
//	}
//	*length = len;
}




void
LValue::Initialize()
{
	long	nobj=1;
	int		i = 0;
	
	if (sym->refType != TypedValue::REF_STACK) {
		return;
	}
	if (debug_lval)
		fprintf(stderr, "lvalue: initialize %s-> %d %s\n", sym->name, sym->iniVal.type, sym->iniVal.StringValue());
	for (i=1; i<=sym->indirection; i++)
		nobj *= sym->size[i];
	for (i=0; i<nobj; i++) {
		switch (sym->type) {
			case TypedValue::S_DOUBLE:
			case TypedValue::S_FLOAT:
			case TypedValue::S_BOOL:
			case TypedValue::S_BYTE:
			case TypedValue::S_SHORT:
			case TypedValue::S_INT:
			case TypedValue::S_TIME:
			case TypedValue::S_LONG: {
				StoreValue(&sym->iniVal);
				addr += sym->size[0];
				break;
			}
			case TypedValue::S_TAKE:
			case TypedValue::S_CLIP: {
				StoreValue(&sym->iniVal);
				addr += sym->size[0];
				break;
			}
			case TypedValue::S_STRUCT: {
				for (StabEnt *p = sym->children; p!=nullptr; p=p->sibling) {
					LValue(	p, addr + p->OffsetValue(),
							p->indirection, stacker, stackerSym, stack).Initialize();
				}
				break;
			}
			default:
				break;
		}
	}
}

bool
LValue::SetToString(const char *strval)
{
//	reportError("set t o string %s %s", sym->name, strval);
	if (addr == nullptr) {
		return false;
	}
	switch (sym->type) {
		case TypedValue::S_FLOAT:	*((float*)addr) = atof(strval); break;
		case TypedValue::S_DOUBLE:	*((double*)addr) = atof(strval); break;
		case TypedValue::S_BYTE:	*((int8*)addr) = atoi(strval); break;
		case TypedValue::S_SHORT:	*((int16*)addr) = atoi(strval); break;
		case TypedValue::S_INT:		*((int32*)addr) = atoi(strval); break;
		case TypedValue::S_LONG:	*((int64*)addr) = atoi(strval); break;
		case TypedValue::S_TIME:	((Time*)addr)->Set(strval); break;
		case TypedValue::S_BOOL: {
			if (strcmp(strval,"true")==0) {
				*((int8*)addr) = 1;
			} else if (strcmp(strval,"false")==0) {
				*((int8*)addr) = 0;
			} else {
				*((int8*)addr) = ((atoi(strval)!=0));
			}
			break;
					 }
		case TypedValue::S_VST_PROGRAM: {
#ifdef QUA_V_VST_HOST
			VstPlugin *vst = nullptr;
			if (stack && stack->context && (vst=stack->context->VstValue())) {
				int		prog = atoi(strval);
				vst->SetProgram(stack->stk.afx, prog);
			}
#endif
			break;
		}
#ifdef QUA_V_VST_HOST
		case TypedValue::S_VST_PARAM: {
			if (stack != nullptr && stack->context != nullptr && (stack->stk.afx != nullptr)) {
				double val = atof(strval);
				stack->stk.afx->setParameter(stack->stk.afx, sym->VstParamValue(), val);
			}
			break;
		}
#endif
		case TypedValue::S_CHANNEL: {
//			reportError("channel set to string '%s'", strval);
			if (indirection > 0) {
				Channel	*c = findChannel(strval, -1);
				if (c == nullptr) {
					;
				} else {
					*((Channel**)addr) = c;
				}
			}
			break;
		}
		case TypedValue::S_CLIP: {
			if (indirection > 0) {
				StabEnt *s = FindTypedSymbolInCtxt(strval, TypedValue::S_CLIP, stackerSym?stackerSym->context:nullptr);
				if (s != nullptr) {
					*((Clip**)addr) = s->ClipValue(nullptr);
				} else {
					*((Clip**)addr) = nullptr;
				}
			}
			break;
		}
		case TypedValue::S_TAKE: {
//			reportError("take set to string '%s'", strval);
			if (indirection > 0) {
				StabEnt *s = FindTypedSymbolInCtxt(strval, TypedValue::S_TAKE, stackerSym?stackerSym->context:nullptr);
				if (s != nullptr) {
					*((Take**)addr) = s->TakeValue();
				} else {
					*((Take**)addr) = nullptr;
				}
			}
			break;
		}
	}
	AssignmentExtras();
	return true;
}


ResultValue
LValue::CurrentValue()
{
	ResultValue		ret_val;
	
	ret_val.flags = ResultValue::COMPLETE;
	ret_val.refType = TypedValue::REF_VALUE;
	ret_val.indirection = 0;
	if (sym == nullptr || addr == nullptr) {
		ret_val.flags = ResultValue::BLOCKED;
		return ret_val;
	}
	switch (sym->type) {
	
#if defined(QUA_V_VST_HOST)

	case TypedValue::S_VST_PARAM: {
		if (stack && sym) {
			AEffect *afx = stack->stk.afx;
			ret_val.type = TypedValue::S_FLOAT;
			ret_val.val.Float = afx->getParameter(afx, sym->val.vstParam);
		} else {
			ret_val.flags = ResultValue::BLOCKED;
		}
		break;
	}
	case TypedValue::S_VST_PROGRAM: {
		if (stack && sym) {
			AEffect *afx = stack->stk.afx;
			ret_val.type = TypedValue::S_INT;
			ret_val.val.Int = 0;
		} else {
			ret_val.flags = ResultValue::BLOCKED;
		}
		break;
	}
#endif

			
	case TypedValue::S_UNKNOWN:
		break;
	case TypedValue::S_METHOD:
	case TypedValue::S_VOICE:
	case TypedValue::S_POOL:
	case TypedValue::S_SAMPLE:
	case TypedValue::S_PORT:
	case TypedValue::S_QUA:
	case TypedValue::S_APPLICATION:
		ret_val.type = sym->type;
		ret_val.val = sym->val;
		break;
	case TypedValue::S_FLOAT:
		ret_val.type = TypedValue::S_FLOAT;
		ret_val.val.Float = *((float *)addr);
		break;
	case TypedValue::S_LONG:
		ret_val.type = TypedValue::S_LONG;
		ret_val.val.Long = *((int64 *)addr);
		break;
	case TypedValue::S_INT:
		ret_val.type = TypedValue::S_INT;
		ret_val.val.Int = *((int32 *)addr);
		break;
	case TypedValue::S_SHORT:
		ret_val.type = TypedValue::S_SHORT;
		ret_val.val.Short = *((int16 *)addr);
		break;
	case TypedValue::S_BYTE:
		ret_val.type = TypedValue::S_BYTE;
		ret_val.val.byte = *((unsigned char *)addr);
		break;
	case TypedValue::S_BOOL:
		ret_val.type = TypedValue::S_BOOL;
		ret_val.val.Bool = *((bool *)addr);
		break;
	case TypedValue::S_TIME:
		ret_val.type = TypedValue::S_TIME;
		ret_val.SetValue(((Time *)addr));
		break;
	case TypedValue::S_BLOCK:
		ret_val.type = TypedValue::S_BLOCK;
		ret_val.val.block = *((Block**)addr);
		break;
	case TypedValue::S_EXPRESSION:
		ret_val.type = TypedValue::S_EXPRESSION;
		ret_val.val.block = *((Block**)addr);
		break;
	case TypedValue::S_CHANNEL:
		if (indirection == 0) {
			ret_val.val = sym->val;
			ret_val.type = TypedValue::S_CHANNEL;
			ret_val.refType = sym->refType;
		} else if (indirection == 1) { // a channel controller
			ret_val.type = TypedValue::S_CHANNEL;
			ret_val.refType = TypedValue::REF_VALUE;
			ret_val.val.channel = *((Channel**)addr);
		}
		break;
	case TypedValue::S_CLIP:
		if (indirection == 0) {
			ret_val.val = sym->val;
			ret_val.type = TypedValue::S_CLIP;
			ret_val.refType = sym->refType;
		} else if (indirection == 1) { // a clip controller
			ret_val.type = TypedValue::S_CLIP;
			ret_val.refType = TypedValue::REF_VALUE;
			ret_val.val.clip = *((Clip**)addr);
		}
		break;
	case TypedValue::S_TAKE:
		if (indirection == 0) {
			ret_val.val = sym->val;
			ret_val.type = TypedValue::S_TAKE;
			ret_val.refType = sym->refType;
		} else if (indirection == 1) { // a take controller
			ret_val.type = TypedValue::S_TAKE;
			ret_val.refType = TypedValue::REF_VALUE;
			ret_val.val.take = *((Take**)addr);
		}
		break;
	default:
		ret_val.val = sym->val;
		ret_val.type = sym->type;
		ret_val.refType = sym->refType;
		break;
	}
	return ret_val;
}


