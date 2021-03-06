#include "qua_version.h"

#include "StdDefs.h"

#include "Block.h"
#include "Pool.h"
#include "Sym.h"
#include "Lambda.h"
//#include "include/QuaMidi.h"
#include "Channel.h"
#include "Executable.h"
#include "Qua.h"
//#include "include/Quapp.h"
#include "QuasiStack.h"
#include "QuaFX.h"
#ifdef QUA_V_VST_HOST
#include "VstPlugin.h"
#endif
#include "Parse.h"
#include "VstPlugin.h"
#include "AudioBuffer.h"
#include "Sample.h"
#include "Markov.h"

#include "Dictionary.h"

#include "QuaDisplay.h"

#if defined(QUA_V_AUDIO)
#include "QuaEnvironment.h"
#include "QuaAudio.h"
#endif

#include <sstream>

// ???? this is getting messy.... should be subclassed, rather than
// a mass of dodgy traversals....
#ifdef QUA_V_DEBUG_CONSOLE
flag	debug_block=0;
#define QDBMSG_BLK(X,Y,Z) if (debug_block) fprintf(stderr, X, Y, Z);
#else
#define QDBMSG_BLK(X,Y,Z)
#endif
extern flag	debug_save;
extern flag	debug_stack;

Block::Block()
{
	next = nullptr;
	type = subType = C_UNKNOWN;
	comment = nullptr;
	labelSym = nullptr;
}

Block::Block(ulong typ, ulong Sub)
{
	next = nullptr;
	type = typ;
	subType = Sub;
	Setup(true);
	comment = nullptr;
	labelSym = nullptr;
}

Block::Block(StabEnt *sym)
{
	next = nullptr;
	subType = C_UNKNOWN;
	comment = nullptr;
	labelSym = nullptr;
	if (sym) {
    	switch (sym->type) {
    	case TypedValue::S_LAMBDA: {
	    	type = C_CALL;
	    	crap.call.crap.lambda = sym->LambdaValue();
	    	break;
    	}
    	
    	case TypedValue::S_VST_PLUGIN: {	// should catch all vst's as they will be global to the application
	    	type = C_VST;
	    	crap.call.crap.vstplugin = sym->VstValue();
	    	break;
	    }
	    
	    default: {
	    	type = C_SYM;
	    	crap.sym = sym;
	    }}
	}
	Setup(true);
}

Block *
Block::Sibling(short n)
{
	Block *p=this;
	while (p!=nullptr && n > 0) {
		n--;
		p = p->next;
	}
	return p;
}



void
Block::Setup(bool setVars)
{
	switch(type) {

// language structures
	case C_FLUX:
		if (setVars)
			crap.flux.timeVar.Set(TypedValue::S_TIME,TypedValue::REF_STACK,0,0);
		break;

	case C_OUTPUT:
	case C_INPUT:
		break;

	case C_IFOP:
	case C_IF:
		if (setVars) {
			crap.iff.condVar.Set(TypedValue::S_BYTE,TypedValue::REF_STACK,0,0);
			crap.iff.doEvalVar.Set(TypedValue::S_BYTE,TypedValue::REF_STACK,0,0);
		}
		break;
		
	case C_GUARD:
		if (setVars) {
			crap.guard.condVar.Set(TypedValue::S_BYTE,TypedValue::REF_STACK,0,0);
			crap.guard.doEvalVar.Set(TypedValue::S_BYTE,TypedValue::REF_STACK,0,0);
		}
		break;
		
	case C_ASSIGN:
		break;
		
	case C_BREAK:
		break;
		
	case C_WAKE:
	case C_SUSPEND:
		break;
		
	case C_DIVERT:
		break;
	case C_STATEOF:
		break;
	case C_WAIT:
		break;
	case C_WITH:
		break;
	case C_FOREACH:
		break;
		
	case C_REPEAT:
		if (setVars) {
			crap.repeat.countVar.Set(TypedValue::S_INT,TypedValue::REF_STACK,0,0);
			crap.repeat.boundVar.Set(TypedValue::S_INT,TypedValue::REF_STACK,0,0);
		}
		break;
	
	case C_LIST:
		crap.list.block = nullptr;
		crap.list.nChildren = 0;
// should check that this is an audio unit block or do this later. ??
#ifdef QUA_V_AUDIO
		if (subType == LIST_FORK) {
			crap.list.tmpbuf1 = sample_buf_alloc(2, getAudioManager().bufferSize);
			crap.list.tmpbuf2 = sample_buf_alloc(2, getAudioManager().bufferSize);
		} else {
			crap.list.tmpbuf1 = nullptr;
			crap.list.tmpbuf2 = nullptr;
		}
#endif
		if (setVars) {
			crap.list.stuffVar.Set(TypedValue::S_BYTE,TypedValue::REF_STACK,0,0);
		}
		break;

	case C_BINOP:
	case C_UNOP:
		crap.op.l = crap.op.r = crap.op.m = nullptr;
		break;

	case C_BUILTIN: {
		crap.call.parameters = nullptr;
	}
	
	case C_GENERIC_PLAYER:  {
		crap.call.parameters = nullptr;
		crap.call.crap.player = new GenericPlayInfo;
		break;
	}
	case C_STREAM_PLAYER:  {
		crap.call.parameters = nullptr;
		crap.call.crap.mplayer = new StreamPlayInfo;
		break;
	}
	case C_MARKOV_PLAYER:  {
		crap.call.parameters = nullptr;
		crap.call.crap.markov = new MarkovInfo;
		break;
	}
	case C_SAMPLE_PLAYER: {
		crap.call.parameters = nullptr;
		crap.call.crap.splayer = new SamplePlayInfo;
		break;
	}
	case C_TUNEDSAMPLE_PLAYER: {
		crap.call.parameters = nullptr;
		crap.call.crap.tplayer = new TunedSamplePlayInfo;
		break;
	}
	case C_MIDI_PLAYER: {
		crap.call.parameters = nullptr;
		crap.call.crap.flayer = new MidiSamplePlayInfo;
		break;
	}
	
	case C_VST:
	case C_SCHEDULE:
	case C_CALL:
	case C_UNLINKED_CALL: {
		crap.call.parameters = nullptr;
		break;
	}
		
	case C_VALUE: {
		break;
	}
		
	case C_NAME:
		break;

	case C_SYM:
	case C_ARRAY_REF:
	case C_STRUCTURE_REF:
	case C_UNKNOWN:
	case C_CAST:
		break;

	}
}

Block::Block(Block *model, StabEnt *newContext, bool unwind)
{
	labelSym = model->labelSym;
	type = model->type;
	subType = model->subType;
	crap = model->crap;
	Setup(false);
	
	switch(type) {
	case C_FLUX:
		crap.flux.timeVar.SetStackContext(newContext);
		if (model->crap.flux.rateExp) crap.flux.rateExp = new Block(model->crap.flux.rateExp, newContext, unwind);
		if (model->crap.flux.lengthExp) crap.flux.lengthExp = new Block(model->crap.flux.lengthExp, newContext, unwind);
		if (model->crap.flux.block) crap.flux.block = new Block(model->crap.flux.block, newContext, unwind);
		break;
		
	case C_OUTPUT:
	case C_INPUT:
		break;
		
	case C_IFOP:
	case C_IF:
		crap.iff.doEvalVar.SetStackContext(newContext);
		crap.iff.condVar.SetStackContext(newContext);
		if (model->crap.iff.ifBlock) crap.iff.ifBlock = new Block(model->crap.iff.ifBlock, newContext, unwind);
		if (model->crap.iff.elseBlock) crap.iff.elseBlock = new Block(model->crap.iff.elseBlock, newContext, unwind);
		if (model->crap.iff.condition) crap.iff.condition = new Block(model->crap.iff.condition, newContext, unwind);
		break;
		
	case C_GUARD:
		crap.guard.doEvalVar.SetStackContext(newContext);
		crap.guard.condVar.SetStackContext(newContext);
		if (model->crap.guard.block) crap.guard.block = new Block(model->crap.guard.block, newContext, unwind);
		if (model->crap.guard.condition) crap.guard.condition = new Block(model->crap.guard.condition, newContext, unwind);
		break;
		
	case C_ASSIGN:
		if (model->crap.assign.atom) crap.assign.atom = new Block(model->crap.assign.atom, newContext, unwind);
		if (model->crap.assign.exp) crap.assign.exp = new Block(model->crap.assign.exp, newContext, unwind);
		break;
		
	case C_CALL	: {
		crap.call.crap.lambda =	model->crap.call.crap.lambda;
		if (model->crap.call.parameters) crap.call.parameters = new Block(model->crap.call.parameters, newContext, unwind);
		break;
	}
		
	case C_BREAK:
		break;
		
	case C_SUSPEND:
	case C_WAKE	:
		crap.call.crap.sym = model->crap.call.crap.sym;
		if (model->crap.call.parameters) crap.call.parameters = new Block(model->crap.call.parameters, newContext, unwind);
		break;
		
	case C_DIVERT:
		if (model->crap.divert.block) crap.divert.block = new Block(model->crap.divert.block, newContext, unwind);
		if (model->crap.divert.clockExp) crap.divert.clockExp = new Block(model->crap.divert.clockExp, newContext, unwind);
		break;
		
	case C_STATEOF:
		break;
		
	case C_WAIT	:
		if (model->crap.block) crap.block = new Block(model->crap.block, newContext, unwind);
		break;
		
	case C_WITH	:
		if (model->crap.with.withBlock) crap.with.withBlock = new Block(model->crap.with.withBlock, newContext, unwind);
		if (model->crap.with.withoutBlock) crap.with.withoutBlock = new Block(model->crap.with.withoutBlock, newContext, unwind);
		if (model->crap.with.condition) crap.with.condition = new Block(model->crap.with.condition, newContext, unwind);
		break;
	
// List elements

	case C_LIST: {
		if (model->crap.list.block) crap.list.block = new Block(model->crap.list.block, newContext, unwind);
		crap.list.nChildren = model->crap.list.nChildren;
		crap.list.stuffVar = model->crap.list.stuffVar;
		crap.list.stuffVar.SetStackContext(newContext);
		break;
	}
		
// atomic operations
	case C_VALUE:
		break;
		
	case C_ARRAY_REF	:
		if (model->crap.arrayRef.base) crap.arrayRef.base = new Block(model->crap.arrayRef.base, newContext, unwind);
		if (model->crap.arrayRef.index) crap.arrayRef.index = new Block(model->crap.arrayRef.index, newContext, unwind);
		break;
		
	case C_SYM:
		if (unwind) {
			type = C_NAME;
			name = model->crap.sym->name;
			QDBMSG_BLK("new Block() clone unwind symbol %s\n", crap.name,0);
		}
		break;
		
	case C_NAME	:
		name = model->name;
		break;
		
		
	case C_STRUCTURE_REF:
		if (model->crap.structureRef.base) crap.structureRef.base = new Block(model->crap.structureRef.base, newContext, unwind);
//		if (model->crap.structureRef.member) crap.structureRef.member= new Block(model->crap.structureRef.member, newContext, unwind);
		break;

// operators
	case C_UNOP:
	case C_BINOP:
		if (model->crap.op.l) crap.op.l = new Block(model->crap.op.l, newContext, unwind);
		if (model->crap.op.r) crap.op.r = new Block(model->crap.op.r, newContext, unwind);
		if (model->crap.op.m) crap.op.m = new Block(model->crap.op.m, newContext, unwind);
		break;
	
	case C_CAST	:
		if (model->crap.cast.block) crap.cast.block = new Block(model->crap.cast.block, newContext, unwind);
		break;

	case C_REPEAT:
		if (model->crap.repeat.Exp) crap.repeat.Exp = new Block(model->crap.repeat.Exp, newContext, unwind);
		if (model->crap.repeat.block) crap.repeat.block = new Block(model->crap.repeat.block, newContext, unwind);
		crap.repeat.countVar.SetStackContext(newContext);
		crap.repeat.boundVar.SetStackContext(newContext);
		break;

	case C_BUILTIN: {
		if (model->crap.call.parameters) crap.call.parameters = new Block(model->crap.call.parameters, newContext, unwind);
		break;
	}
	
	case C_SCHEDULE: {
		if (model->crap.call.parameters) crap.call.parameters = new Block(model->crap.call.parameters, newContext, unwind);
		break;
	}
	
	case C_STREAM_PLAYER:
	case C_MIDI_PLAYER:
	case C_MARKOV_PLAYER:
	case C_GENERIC_PLAYER:
	case C_TUNEDSAMPLE_PLAYER:
	case C_SAMPLE_PLAYER: {
		if (model->crap.call.parameters) crap.call.parameters = new Block(model->crap.call.parameters, newContext, unwind);
		break;
	}
	
	case C_VST: {
		if (model->crap.call.parameters) crap.call.parameters = new Block(model->crap.call.parameters, newContext, unwind);
		break;
	}
	default:
		internalError("new Block copy: bad block %d", type);
	}
	if (model->next)
		next = new Block(model->next, newContext, unwind);
	else
		next = nullptr;
}


// gotta delete all the bits && pieces!
Block::~Block()
{
//	if (label)
//		delete label;
	switch (type) {

	case C_BUILTIN: {
		break;
	}

	case C_LIST: {
		if (crap.list.tmpbuf1)
			sample_buf_free(crap.list.tmpbuf1, 2);
		if (crap.list.tmpbuf2)
			sample_buf_free(crap.list.tmpbuf2, 2);
		break;
	}

#ifdef QUA_V_VST_HOST
	case C_VST: {
		// vst is a reference to a global object
		break;
	}
#endif
	case C_GENERIC_PLAYER: {
		if (crap.call.crap.player) {
			delete crap.call.crap.player;
		}
		break;
	}
	case C_SAMPLE_PLAYER: {
		if (crap.call.crap.splayer) {
			delete crap.call.crap.splayer;
		}
		break;
	}
	case C_TUNEDSAMPLE_PLAYER: {
		if (crap.call.crap.tplayer) {
			delete crap.call.crap.tplayer;
		}
		break;
	}
	case C_STREAM_PLAYER: {
		if (crap.call.crap.mplayer) {
			delete crap.call.crap.mplayer;
		}
		break;
	}
	case C_MARKOV_PLAYER: {
		if (crap.call.crap.markov) {
			delete crap.call.crap.markov;
		}
		break;
	}
	case C_MIDI_PLAYER: {
		if (crap.call.crap.flayer) {
			delete crap.call.crap.flayer;
		}
		break;
	}
	case C_VALUE: {
		//		if (crap.constant.Value.type == TypedValue::S_TIME) {
		//			delete crap.constant.Value.TimeValue();
		//		}
		break;
	}
	case C_SET_ATTRIB: {
		if (crap.setAttrib.attributes) {
			delete crap.setAttrib.attributes;
		}
		break;
	}

	case C_NAME: {
	}
	default: {
	}
	}
}

StabEnt *
Block::TypeSymbol()
{
	if (type == C_SYM) {
		QDBMSG_BLK("TypeSymbol: %s %d\n", crap.sym?crap.sym->name:"nul", crap.sym?crap.sym->type:0);
		return crap.sym?crap.sym->TypeSymbol():nullptr;
	} else if (type == C_NAME) {
		return nullptr;
	} else {
		return nullptr;
	}
}

//////////////////////////////////////////////////////////////////////////////
// recursively delete all block elements in the block, including the past
// initialization. all the per block stuff done by ~Block
//////////////////////////////////////////////////////////////////////////////
// XXXX I think when I did this initially, I thought it funny. Now it just smells funny.
// It seems to work, but is full of all danger and badness.
void
Block::DeleteAll()
{
	Traverse(&Block::DoDelete, nullptr, (VBlockFnPtr)nullptr, nullptr, 0);
}

bool
Block::DoDelete(void *x, void *B, int z)
{
	delete this;
	return true;
}


//////////////////////////////////////////////////////////////////////////////
// do all first time initializations of the stack elements
//////////////////////////////////////////////////////////////////////////////
void *
Block::DoInitPre(void *V, void *B, int z, long & status)
{
	switch (type) {
	case C_SYM: {
		break;
	}
	
	case C_STRUCTURE_REF: {
		if (crap.structureRef.base) {
			StabEnt *s = crap.structureRef.base->TypeSymbol();
			if (s) {
				glob.PushContext(s);
			} else {
				QDBMSG_BLK("pre-init: type symbol not found\n",0,0);
			}
		}
		break;
	}
	
	case C_NAME: {
	    StabEnt	*S;
	    S = glob.findSymbol(name);
	    if (S == nullptr) {
			internalError("Qua linkage error: \"%s\" not found", name.c_str());
			status = B_ERROR;
			return B;
	    }
	    QDBMSG_BLK("found symbol %s in %s\n", S->name, S->context?S->context->name.c_str():"<glbl>");
		if (S->type == TypedValue::S_LAMBDA) {
			type = C_CALL;
			crap.call.parameters = nullptr;
	    	crap.call.crap.lambda = S->LambdaValue();
			glob.PushContext(crap.call.crap.lambda->sym);
		} else {
		    type = C_SYM;
		    crap.sym = S;
		}
	    break;
	}
	
	case C_UNLINKED_CALL: {
	    QDBMSG_BLK("Setting up unlinked call element: %s\n", crap.call.crap.name,0);
    
    	StabEnt *S = glob.findSymbol(name);

	    if (S == nullptr) {
			internalError("Init: call symbol \"%s\" not found\n", name);
			status = B_ERROR;
			return B;
	    }
	    
	    switch (S->type) {
		case TypedValue::S_VOICE:
		case TypedValue::S_SAMPLE:
		case TypedValue::S_POOL:
   		case TypedValue::S_PORT:
	    	crap.call.crap.sym = S;
			glob.PushContext(crap.call.crap.sym);
			type = C_SCHEDULE;
			break;
		case TypedValue::S_VST_PLUGIN:
	    	crap.call.crap.sym = S;
			glob.PushContext(crap.call.crap.sym);
			type = C_VST;
			break;
		case TypedValue::S_EVENT:
	    	crap.call.crap.sym = S;
			glob.PushContext(crap.call.crap.sym);
			type = C_CALL;
			break;
	    case TypedValue::S_LAMBDA:
	    	crap.call.crap.lambda = S->LambdaValue();
			glob.PushContext(crap.call.crap.lambda->sym);
	    	type = C_CALL;
	    	break;

	    default:
  			internalError("Init: call symbol \"%s\" not a method\n", name.c_str());
  			crap.call.crap.sym = S;
			return B;
		}

	    break;
	}

	case C_CALL:
	    QDBMSG_BLK("DoInitPre: call element:\n",0,0);
    
		if (crap.call.crap.lambda && crap.call.crap.lambda->sym)
			glob.PushContext(crap.call.crap.lambda->sym);
		else {
			internalError("Strange call...");
			type = C_UNLINKED_CALL;
		}
		break;
	
	case C_VST: {
	    QDBMSG_BLK("DoInitPre: vst element:\n",0,0);
		glob.PushContext(crap.call.crap.vstplugin->sym);
		break;
	}

	case C_GENERIC_PLAYER: {
		GenericPlayInfo	*inf = crap.call.crap.player;
		if (inf->playerSym) {
			if (labelSym) {
				labelSym->LabelValue()->objContext = inf->playerSym;
			}
			glob.PushContext(inf->playerSym);
		}
		break;
	}
	case C_STREAM_PLAYER: {
		StreamPlayInfo	*inf = crap.call.crap.mplayer;
		if (inf->playerSym) {
			if (labelSym) {
				labelSym->LabelValue()->objContext = inf->playerSym;
			}
			glob.PushContext(inf->playerSym);

			inf->clipVar.SetToSymbol("clip", inf->playerSym);
			inf->mediaVar.SetToSymbol("media", inf->playerSym);
			inf->posVar.SetToSymbol("position", inf->playerSym);
			inf->rateVar.SetToSymbol("rate", inf->playerSym);
			inf->loopmodeVar.SetToSymbol("loop", inf->playerSym);
			inf->stateVar.SetToSymbol("state", inf->playerSym);
			inf->looptimeVar.SetToSymbol("looptime", inf->playerSym);
			inf->loopstartVar.SetToSymbol("loopstarttime", inf->playerSym);
//			inf->directionVar.SetToSymbol("dir", inf->playerSym);
//			inf->gainVar.SetToSymbol("gain", inf->playerSym);
//			inf->panVar.SetToSymbol("pan", inf->playerSym);
		}
		break;
	}

	case C_SAMPLE_PLAYER: {
		SamplePlayInfo	*inf = crap.call.crap.splayer;
		if (inf->playerSym) {
			if (labelSym) {
				labelSym->LabelValue()->objContext = inf->playerSym;
			}
			glob.PushContext(inf->playerSym);

			inf->clipVar.SetToSymbol("clip", inf->playerSym);
			inf->mediaVar.SetToSymbol("media", inf->playerSym);
			inf->posVar.SetToSymbol("position", inf->playerSym);
//			inf->pitchVar.SetToSymbol("pitch", inf->playerSym);
			inf->gainVar.SetToSymbol("gain", inf->playerSym);
			inf->panVar.SetToSymbol("pan", inf->playerSym);
			inf->loopmodeVar.SetToSymbol("loop", inf->playerSym);
			inf->directionVar.SetToSymbol("direction", inf->playerSym);
			inf->stateVar.SetToSymbol("state", inf->playerSym);
		}
		break;
	}

	case C_TUNEDSAMPLE_PLAYER: {
		TunedSamplePlayInfo	*inf = crap.call.crap.tplayer;
		if (inf->playerSym) {
			if (labelSym) {
				labelSym->LabelValue()->objContext = inf->playerSym;
			}
			glob.PushContext(inf->playerSym);

			inf->clipVar.SetToSymbol("clip", inf->playerSym);
			inf->mediaVar.SetToSymbol("media", inf->playerSym);
			inf->posVar.SetToSymbol("position", inf->playerSym);
			inf->pitchVar.SetToSymbol("pitch", inf->playerSym);
			inf->gainVar.SetToSymbol("gain", inf->playerSym);
			inf->panVar.SetToSymbol("pan", inf->playerSym);
			inf->loopmodeVar.SetToSymbol("loop", inf->playerSym);
			inf->directionVar.SetToSymbol("direction", inf->playerSym);
			inf->stateVar.SetToSymbol("state", inf->playerSym);
		}
		break;
	}

	case C_MIDI_PLAYER: {
		MidiSamplePlayInfo	*inf = crap.call.crap.flayer;
		if (inf->playerSym) {
			if (labelSym) {
				labelSym->LabelValue()->objContext = inf->playerSym;
			}
			glob.PushContext(inf->playerSym);
		}
		break;
	}
		
	case C_MARKOV_PLAYER: {
		MarkovInfo	*inf = crap.call.crap.markov;
		if (inf->playerSym) {
			if (labelSym) {
				labelSym->LabelValue()->objContext = inf->playerSym;
			}
			glob.PushContext(inf->playerSym);
		}
		break;
	}
		
	case C_SCHEDULE: {
	    QDBMSG_BLK("Going through schedule element:\n",0,0);
		glob.PushContext(crap.call.crap.sym);
		break;
	}
		
	case C_WAKE:
	case C_SUSPEND: {
	    QDBMSG_BLK("Going through wake/suspend element:\n",0,0);
		glob.PushContext(crap.call.crap.sym);
		Schedulable	*S = crap.call.crap.sym->SchedulableValue();
	    break;
	}
	}
	return B;
}



bool
Block::DoInitPost(void *V, void *B, int z)
{
	switch (type) {
	
	case C_STRUCTURE_REF: {
		if (crap.structureRef.base) {
			StabEnt *s = crap.structureRef.base->TypeSymbol();
			if (s) {
				glob.PopContext(s);
			} else {
				QDBMSG_BLK("post-init: type symbol not found\n",0,0);
			}
		}
		break;
	}
	
	case C_BUILTIN: {
		    break;
	}
		
	case C_VST: {
		glob.PopContext(crap.call.crap.vstplugin->sym);
		break;
	}

	case C_CALL: {
		glob.PopContext(crap.call.crap.sym);
		break;
	}

	case C_TUNEDSAMPLE_PLAYER:
	case C_SAMPLE_PLAYER:
	case C_STREAM_PLAYER:
	case C_MARKOV_PLAYER:
	case C_MIDI_PLAYER:
	case C_GENERIC_PLAYER: {
		StabEnt	*playerSym = nullptr;
		if (labelSym && labelSym->LabelValue()->objContext) {
		}
		glob.PopContext(playerSym);
		break;
	}
		
	case C_SCHEDULE:
	case C_WAKE:
	case C_SUSPEND: {
		glob.PopContext(crap.call.crap.sym);
		Schedulable	*S = crap.call.crap.sym->SchedulableValue();
	    break;
	}
	}
	    
	return true;
}

bool
Block::Init(Executable *V)
{
	return Traverse(&Block::DoInitPost,
					(void *)V,
					&Block::DoInitPre,
					nullptr,
					0);
}


//////////////////////////////////////////////////////////////////////////////
// set every variable and created structure to its standard pre-run condition
//////////////////////////////////////////////////////////////////////////////
bool
Block::Reset(QuasiStack *s)
{
	return Traverse(&Block::DoReset, nullptr, (VBlockFnPtr)nullptr, (void *)s, 0);
}


bool
Block::DoReset(void *V, void *B, int x)
{
	QuasiStack	*stack = (QuasiStack *)B;
	switch (type) {
	case C_FLUX: {
		QDBMSG_BLK("Resetting flux element\n",0,0);
		Time		*nextTP = (Time*)crap.flux.timeVar.StackAddressValue(stack);
		nextTP->Reset();
		break;
	}
	case C_GUARD:
		QDBMSG_BLK("Resetting guard element\n",0,0);
		crap.guard.doEvalVar.SetValue((int32)TRUE,stack);
		crap.guard.condVar.SetValue((int32)TRUE,stack);
		break;
		
	case C_IFOP:
	case C_IF:
		QDBMSG_BLK("Resetting if element %d %d\n",
						crap.iff.doEvalVar.type, crap.iff.condVar.type);
		crap.iff.doEvalVar.SetValue((int32)TRUE,stack);
		crap.iff.condVar.SetValue((int32)TRUE,stack);
		break;
		
	case C_LIST: {
		if (subType == LIST_NORM) {
			QDBMSG_BLK("Reset plein list element\n",0,0);
		    uchar	*p = crap.list.stuffVar.StackAddressValue(stack);
		    for (short i=0; i <crap.list.nChildren; i++)
		    	p[i] = false;
		} else if (subType == LIST_SEQ) {
			QDBMSG_BLK("Reset seq list element: %x\n",crap.list.block,0);
			crap.list.stuffVar.SetValue((int32)crap.list.block,stack);
		}
	    break;
	}
	
	case C_CALL: {
	    QDBMSG_BLK("Reset call element: %s\n",
	    			crap.call.crap.lambda->sym->name,0);
	    			
		QuasiStack	*higherFrame = stack->frameAt(crap.call.frameIndex);
//		if (crap.call.crap.lambda->isInit) {
		    if (higherFrame) {
   	 			crap.call.crap.lambda->mainBlock->Reset(higherFrame);
				higherFrame->stacker->uberQua->bridge.DisplayStatus(higherFrame, higherFrame->locusStatus);
		    }
//		}

		if (crap.call.crap.lambda->resetVal.type != TypedValue::S_UNKNOWN) {
			higherFrame->locusStatus = (short)crap.call.crap.lambda->resetVal.IntValue(nullptr);
		}
	    break;
	}
	case C_REPEAT: {
		QDBMSG_BLK("Reset repeat element\n",0,0);
		crap.repeat.boundVar.SetValue((int32)0,stack);
		crap.repeat.countVar.SetValue((int32)0,stack);
		break;
	}

	case C_GENERIC_PLAYER: {
		break;
	}
	case C_STREAM_PLAYER: {
		QDBMSG_BLK( "Reset stream element\n",0,0);
		QuasiStack	*higherFrame = stack->frameAt(crap.call.frameIndex);
		StreamPlayInfo	*inf = crap.call.crap.mplayer;

		Block *bp			= crap.call.parameters;
		if (bp && bp->type == C_SYM && bp->crap.sym) {
			StabEnt	*sym = bp->crap.sym;
			Take	*media=nullptr;
			Clip	*clip=nullptr;
			if (sym->type == TypedValue::S_CLIP) {
				clip = (Clip *)sym->PointerValue(higherFrame);
				media = clip->media;
				int	state = 0;
				if (media && media->type == Take::STREAM) {
					StreamItem *pos = ((StreamTake*)media)->stream.IndexTo(clip->start);
					inf->posVar.SetValue((int32)pos, higherFrame);
					inf->stateVar.SetValue((int32)0, higherFrame);
				}
			} else if (sym->type == TypedValue::S_TAKE) {
				media = (Take*)sym->PointerValue(stack);
				int	state = 0;
				if (media && media->type == Take::STREAM) {
					StreamItem *pos = ((StreamTake*)media)->stream.head;
					inf->posVar.SetValue((int32)pos, higherFrame);
					inf->stateVar.SetValue((int32)0, higherFrame);
				}
			} 
			inf->clipVar.SetPointerValue(clip, higherFrame);
			inf->mediaVar.SetPointerValue(media, higherFrame);
// processed by main loop... do parameters there
		}
		break;
	}

	case C_MARKOV_PLAYER: {
		QDBMSG_BLK("Reset midi markov element\n",0,0);
		QuasiStack	*higherFrame = stack->frameAt(crap.call.frameIndex);
		MarkovInfo	*inf = crap.call.crap.markov;

		Block *bp			= crap.call.parameters;
		if (bp && bp->type == C_SYM && bp->crap.sym) {
			StabEnt	*sym = bp->crap.sym;
			Take	*media=nullptr;
			Clip	*clip=nullptr;
			if (sym->type == TypedValue::S_CLIP) {
				clip = sym->ClipValue(higherFrame);
				media = clip->media;
				int	state = 0;
				if (media && media->type == Take::STREAM) {
					inf->stateVar.SetValue((int32)0, higherFrame);
				}
			} else if (sym->type == TypedValue::S_TAKE) {
				media = sym->TakeValue();
				int	state = 0;
				if (media && media->type == Take::STREAM) {
					inf->stateVar.SetValue((int32)0, higherFrame);
				}
			} 
			inf->clipVar.SetPointerValue(clip, higherFrame);
			inf->mediaVar.SetPointerValue(media, higherFrame);
// processed by main loop... do parameters there
// ..... but also recalculate markov stuff if we think it has changed
		}
		break;
	}

	case C_SAMPLE_PLAYER: {
		QDBMSG_BLK("Reset sample player element\n",0,0);
		QuasiStack	*higherFrame = stack->frameAt(crap.call.frameIndex);
		SamplePlayInfo	*inf = crap.call.crap.splayer;
		Block *bp			= crap.call.parameters;
		if (bp && bp->type == C_SYM && bp->crap.sym) {
			StabEnt	*sym = bp->crap.sym;
			Take	*media=nullptr;
			Clip	*clip=nullptr;
			if (sym->type == TypedValue::S_CLIP) {
				clip = sym->ClipValue(higherFrame);
				media = clip->media;
				int	state = 0;
				if (media && media->type == Take::SAMPLE) {
					long pos = ((SampleTake*)media)->sample->ResetClip(clip);
					inf->posVar.SetValue((int32)pos, higherFrame);
					inf->stateVar.SetValue((int32)0, higherFrame);
				}
			} else if (sym->type == TypedValue::S_TAKE) {
				media = sym->TakeValue();
				int	state = 0;
				if (media && media->type == Take::SAMPLE) {
					long pos = ((SampleTake*)media)->sample->ResetTake((SampleTake*)media);
					inf->posVar.SetValue((int32)0, higherFrame);
					inf->stateVar.SetValue((int32)0, higherFrame);
				}
			} 
			inf->clipVar.SetPointerValue(clip, higherFrame);
			inf->mediaVar.SetPointerValue(media, higherFrame);
			bp = bp->next;
			Stackable *stackable = inf->playerSym->
					BuiltinValue()->stackable;
			int npar = 0;
			for (; bp!=nullptr; bp=bp->next) {
// set param values, incase audio loop pops thru
				ResultValue the_val = EvaluateExpression(bp, nullptr, nullptr, nullptr, higherFrame);
				if (!the_val.Blocked()) {
					StabEnt	*ctl = stackable->controller(npar);
					if (ctl) {
						if (bp->type != C_ASSIGN) {
							LValue		lval;
							ctl->SetLValue(lval, nullptr,
												nullptr, nullptr, 
												higherFrame);
							lval.StoreValue(&the_val);
						}
					}
					npar++;
				}
			}
		}
		break;
	}

	case C_TUNEDSAMPLE_PLAYER: {
		QDBMSG_BLK("Reset sample player element\n",0,0);
		QuasiStack	*higherFrame = stack->frameAt(crap.call.frameIndex);
		TunedSamplePlayInfo	*inf = crap.call.crap.tplayer;
		Block *bp			= crap.call.parameters;
		if (bp && bp->type == C_SYM && bp->crap.sym) {
			StabEnt	*sym = bp->crap.sym;
			Take	*media=nullptr;
			Clip	*clip=nullptr;
			if (sym->type == TypedValue::S_CLIP) {
				clip = sym->ClipValue(higherFrame);
				media = clip->media;
				int	state = 0;
				if (media && media->type == Take::SAMPLE) {
					long pos = ((SampleTake*)media)->sample->ResetClip(clip);
					inf->posVar.SetValue((int32)pos, higherFrame);
					inf->stateVar.SetValue((int32)0, higherFrame);
				}
			} else if (sym->type == TypedValue::S_TAKE) {
				media = sym->TakeValue();
				int	state = 0;
				if (media && media->type == Take::SAMPLE) {
					long pos = ((SampleTake*)media)->sample->ResetTake((SampleTake*)media);
					inf->posVar.SetValue((int32)0, higherFrame);
					inf->stateVar.SetValue((int32)0, higherFrame);
				}
			} 
			inf->clipVar.SetPointerValue(clip, higherFrame);
			inf->mediaVar.SetPointerValue(media, higherFrame);
			bp = bp->next;
			Stackable *stackable = inf->playerSym->
					BuiltinValue()->stackable;
			int npar = 0;
			for (; bp!=nullptr; bp=bp->next) {
// set param values, incase audio loop pops thru
				ResultValue the_val = EvaluateExpression(bp, nullptr, nullptr, nullptr, higherFrame);
				if (!the_val.Blocked()) {
					StabEnt	*ctl = stackable->controller(npar);
					if (ctl) {
						if (bp->type != C_ASSIGN) {
							LValue		lval;
							ctl->SetLValue(lval, nullptr,
												nullptr, nullptr,
												higherFrame);
							lval.StoreValue(&the_val);
						}
					}
					npar++;
				}
			}
		}
		break;
	}

	case C_MIDI_PLAYER: {
		QDBMSG_BLK("Reset midi sample player element\n",0,0);
		QuasiStack	*higherFrame = stack->frameAt(crap.call.frameIndex);
		MidiSamplePlayInfo	*inf = crap.call.crap.flayer;
		Block *bp			= crap.call.parameters;
		if (bp && bp->type == C_SYM && bp->crap.sym) {
			StabEnt	*sym = bp->crap.sym;
			Take	*media=nullptr;
			Clip	*clip=nullptr;
			if (sym->type == TypedValue::S_CLIP) {
				clip = sym->ClipValue(stack);
				media = clip->media;
				int	state = 0;
				if (media && media->type == Take::SAMPLE) {
					long pos = ((SampleTake*)media)->sample->ResetClip(clip);
					inf->posVar.SetValue((int32)pos, higherFrame);
					inf->stateVar.SetValue((int32)0, higherFrame);
				}
			} else if (sym->type == TypedValue::S_TAKE) {
				media = sym->TakeValue();
				int	state = 0;
				if (media && media->type == Take::SAMPLE) {
					inf->posVar.SetValue((int32)0, higherFrame);
					inf->stateVar.SetValue((int32)0, higherFrame);
				}
			} 
			inf->clipVar.SetPointerValue(clip, higherFrame);
			inf->mediaVar.SetPointerValue(media, higherFrame);
			bp = bp->next;
			Stackable *stackable = inf->playerSym->
					BuiltinValue()->stackable;
			int npar = 0;
			for (; bp!=nullptr; bp=bp->next) {
// set param values, incase audio loop pops thru
				ResultValue the_val = EvaluateExpression(bp,nullptr, nullptr, nullptr, higherFrame);
				if (!the_val.Blocked()) {
					StabEnt	*ctl = stackable->controller(npar);
					if (ctl) {
						if (bp->type != C_ASSIGN) {
							LValue		lval;
							ctl->SetLValue(lval,nullptr,
												nullptr, nullptr,
												higherFrame);
							lval.StoreValue(&the_val);
						}
					}
					npar++;
				}
			}
// turn off oscillator, ready for a note-on
		}
		break;
	}


#if defined(QUA_V_VST_HOST)
	case C_VST: {
		QDBMSG_BLK("Reset vst element\n",0,0);
		QuasiAFXStack *higherFrame = dynamic_cast<QuasiAFXStack*>(stack->frameAt(crap.call.frameIndex));
		if (higherFrame != nullptr) {
			AEffect *afx = higherFrame->afx;
			// ?????????????????? reset to what and how ???????????
			// default param vals will be held in the plugin
			//		crap.call.crap.afx->Reset(afx);
		}
		break;
	}
#endif
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////
// allocate stack based variables within stack elements.
// this is only done for re-allocation due to symbol edits for data.
// at the moment, the first time allocation is done by the parser itself
// ???????? something looks broken ... looks like 
// ???????? context would be mucked up ... ????
//////////////////////////////////////////////////////////////////////////////

bool
Block::AllocateExecStack(Stackable *s)
{
	return Traverse(&Block::DoAlloc, nullptr, (VBlockFnPtr)nullptr, (void *)s, 0);
}

bool
Block::DoAlloc(void *V, void *B, int x)
{
	Stackable	*s = (Stackable *)B;
	switch (type) {
	case C_FLUX: {
		QDBMSG_BLK("Allocing flux element\n",0,0);
		crap.flux.timeVar.SetStackReference(s->sym->context,
			AllocStack(s->sym->context,TypedValue::S_BYTE,sizeof(Time)));
		break;
	}
	case C_GUARD:
		QDBMSG_BLK("Allocing guard element\n",0,0);
		crap.guard.doEvalVar.SetStackReference(s->sym->context,
			AllocStack(s->sym->context,TypedValue::S_BYTE));
		crap.guard.condVar.SetStackReference(s->sym->context,
			AllocStack(s->sym->context,TypedValue::S_BYTE));
		break;
		
	case C_IFOP:
	case C_IF:
		QDBMSG_BLK("Allocing if element\n",0,0);
		crap.iff.doEvalVar.SetStackReference(s->sym->context,
			AllocStack(s->sym->context,TypedValue::S_BYTE));
		crap.iff.condVar.SetStackReference(s->sym->context,
			AllocStack(s->sym->context,TypedValue::S_BYTE));
		break;
		
	case C_LIST: {
		if (subType == LIST_NORM) {
			QDBMSG_BLK("Allocing plein list element\n",0,0);
			crap.list.stuffVar.SetStackReference(s->sym->context,
				AllocStack(s->sym->context, TypedValue::S_BYTE, crap.list.nChildren));
		} else if (subType == LIST_SEQ) {
			QDBMSG_BLK("Allocing seq list element: %x\n",crap.list.block,0);
			crap.list.stuffVar.SetStackReference(s->sym->context,
				AllocStack(s->sym->context, TypedValue::S_INT, 1));
		}
	    break;
	}
	
	case C_CALL: {
	    break;
	}
	case C_REPEAT: {
		QDBMSG_BLK("Alloc repeat element\n",0,0);
		crap.repeat.boundVar.SetStackReference(s->sym->context,
			(long)AllocStack(s->sym->context,TypedValue::S_INT));
		crap.repeat.countVar.SetStackReference(s->sym->context,
			(long)AllocStack(s->sym->context,TypedValue::S_INT));
		break;
	}
	case C_GENERIC_PLAYER: {
		break;
	}

	case C_TUNEDSAMPLE_PLAYER:
	case C_SAMPLE_PLAYER: {
//		crap.call.crap.splayer->clipVar.Set(TypedValue::S_STRANGE_POINTER, TypedValue::REF_STACK,	s->sym->context,
//				(long)AllocStack(s->sym->context,TypedValue::S_STRANGE_POINTER, 1));
//		crap.call.crap.splayer->posVar.Set(TypedValue::S_INT, TypedValue::REF_STACK,	s->sym->context,
//				(long)AllocStack(s->sym->context,TypedValue::S_INT, 1));
//		crap.call.crap.splayer->pitchVar.Set(TypedValue::S_FLOAT, TypedValue::REF_STACK,	s->sym->context,
//				(long)AllocStack(s->sym->context,TypedValue::S_FLOAT, 1));
//		crap.call.crap.splayer->gainVar.Set(TypedValue::S_FLOAT, TypedValue::REF_STACK,	s->sym->context,
//				(long)AllocStack(s->sym->context,TypedValue::S_FLOAT, 1));
//		crap.call.crap.splayer->panVar.Set(TypedValue::S_FLOAT, TypedValue::REF_STACK,	s->sym->context,
//				(long)AllocStack(s->sym->context,TypedValue::S_FLOAT, 1));
//		crap.call.crap.splayer->loopmodeVar.Set(TypedValue::S_BYTE, TypedValue::REF_STACK,	s->sym->context,
//				(long)AllocStack(s->sym->context,TypedValue::S_BYTE, 1));
//		crap.call.crap.splayer->directionVar.Set(TypedValue::S_BYTE, TypedValue::REF_STACK,	s->sym->context,
//				(long)AllocStack(s->sym->context,TypedValue::S_BYTE, 1));
//		crap.call.crap.splayer->stateVar.Set(TypedValue::S_BYTE, TypedValue::REF_STACK,	s->sym->context,
//				(long)AllocStack(s->sym->context,TypedValue::S_BYTE, 1));
		break;
	}
	case C_STREAM_PLAYER: {
		break;
	}

	case C_MIDI_PLAYER: {
		break;
	}

	case C_MARKOV_PLAYER: {
		break;
	}

	case C_VST: { // no allocation ... params are vst nos ... cookie allocated elsewhere
		break;
	}
	}
	return true;
}

////////////////////////////////////////////////////////////////
// recursively create all the stack elements
////////////////////////////////////////////////////////////////

bool
Block::StackOMatic(QuasiStack *V, short preDepth)
{
	QDBMSG_BLK("StackOMatic(%x,%d)\n", V, preDepth);
	return Traverse((BlockFnPtr)nullptr, (void *)V, &Block::DoStack, nullptr, preDepth);
}

void *
Block::DoStack(void *V, void *B, int y, long&status)
{
// B is enclosing list
// V is parent frame
	QDBMSG_BLK("DoStack: parent %x list %x\n", V, B);
	switch (type) {
	
	case C_FLUX: {
		QDBMSG_BLK("Init stack for flux element\n",0,0);
		Time		*nextTP = (Time*)crap.flux.timeVar.StackAddressValue((QuasiStack*)V);
		nextTP->Set(0,&Metric::std);
		break;
	}
	case C_LIST: {
		QDBMSG_BLK("Inherit list element\n",0,0);
		return this;
	    break;
	}

	case C_WAKE:
	case C_SUSPEND:
	case C_VST:
	case C_CALL:
	case C_TUNEDSAMPLE_PLAYER:
	case C_SAMPLE_PLAYER:
	case C_STREAM_PLAYER:
	case C_MIDI_PLAYER:
	case C_MARKOV_PLAYER:
	case C_GENERIC_PLAYER:
    	StabEnt		*S = nullptr;
    	if (type == C_CALL) {
    		if (crap.call.crap.lambda) 
    			S = crap.call.crap.lambda->sym;
#if defined(QUA_V_VST_HOST)
    	} else if (type == C_VST) {
			if (crap.call.crap.vstplugin) {
				S = crap.call.crap.vstplugin->sym;
			}
#endif
    	} else if (type == C_SAMPLE_PLAYER) {
 			S = crap.call.crap.tplayer->playerSym;
   		} else if (type == C_SAMPLE_PLAYER) {
			S = crap.call.crap.splayer->playerSym;
    	} else if (type == C_GENERIC_PLAYER) {
			S = crap.call.crap.player->playerSym;
    	} else if (type == C_STREAM_PLAYER) {
			S = crap.call.crap.mplayer->playerSym;
    	} else if (type == C_MIDI_PLAYER) {
			S = crap.call.crap.flayer->playerSym;
    	} else if (type == C_MARKOV_PLAYER) {
			S = crap.call.crap.markov->playerSym;
    	} else if (type == C_WAKE) {
    		S = crap.call.crap.sym;
    	} else if (type == C_SUSPEND) {
    		S = crap.call.crap.sym;
	   	} else {
   			S = crap.call.crap.sym;
    	}

   		QuasiStack	*parent = (QuasiStack *)V;
		QuasiStack *qs = nullptr;
	   	if (parent) {
			if (type == C_VST) {
				qs = new QuasiAFXStack(S, parent->stacker, parent->stackerSym,
					this, (Block *)B, parent, parent->timeKeeper, nullptr);
				parent->addFrame(qs);

				crap.call.frameIndex = parent->countFrames() - 1;
				qs->isLeaf = true;
				QDBMSG_BLK("StackOMatic(), C_CALL: parent %x %s\n", parent, parent->context ? parent->context->name : "<no name>");
			} else {
				qs = new QuasiStack(S, parent->stacker, parent->stackerSym,
					this, (Block *)B, parent, parent->timeKeeper, nullptr);
				parent->addFrame(qs);

				crap.call.frameIndex = parent->countFrames() - 1;
				QDBMSG_BLK("StackOMatic(), C_CALL: parent %x %s\n", parent, parent->context ? parent->context->name : "<no name>");
				switch (type) {

				case C_STREAM_PLAYER:
				case C_GENERIC_PLAYER:
				case C_BUILTIN:
					qs->isLeaf = true;
					break;

				case C_VST: {
					qs->isLeaf = true;
					break;
				}

				case C_TUNEDSAMPLE_PLAYER:
				case C_SAMPLE_PLAYER: {
					qs->isLeaf = true;
					break;
				}

				case C_MIDI_PLAYER: {
					qs->isLeaf = true;
					break;
				}

				case C_MARKOV_PLAYER: {
					qs->isLeaf = true;
					break;
				}

				case C_WAKE:
				case C_SUSPEND: {
					qs->isLeaf = true;
					//	    		Schedulable		*sched = S->SchedulableValue();
					//		    	if (sched && y>0) {
					//		    		sched->mainBlock->StackOMatic(qs,y-1);
					//		    		if (qs->higherFrame.CountItems() == 0)	// no context to build, try again
					//		    			qs->isLeaf = true;
					//		    	}
					break;
				}

				default: {
					if (y>0) {
						((Lambda *)qs->stackable)->mainBlock->StackOMatic(qs, y - 1);
						if (qs->countFrames() == 0)	// no context to build, try again
							qs->isLeaf = true;
					}
				}

				}

			}
    	} else {
    		QDBMSG_BLK("StackOMatic(), C_CALL: no parent stack!\n",0,0);
    	}
	
	    break;
	}
	return B;
}


///////////////////////////////////////////////////////////////////////////
// recursively do shit
//////////////////////////////////////////////////////////////////
bool
Block::Traverse(BlockFnPtr f, void *x, VBlockFnPtr g, void *y, int z)
{
	void *newy=y;
	if (g) {
		long		status=0;
		newy = (this->*g)(x,y,z,status);
		if (status < 0) {
			return false;
		}
	}
	
	switch (type) {
	case C_UNKNOWN:
		break;
		
	case C_WAIT:
		QDBMSG_BLK("Travering wait element\n",0,0);
		if (crap.block && !crap.block->Traverse(f,x,g,newy,z))
			return false;
		break;

	case C_FLUX:
		QDBMSG_BLK("Travering flux element\n",0,0);
		if (crap.flux.block && !crap.flux.block->Traverse(f,x,g,newy,z)) return false;
		if (crap.flux.rateExp && !crap.flux.rateExp->Traverse(f,x,g,newy,z)) return false;
	    if (crap.flux.lengthExp && !crap.flux.lengthExp->Traverse(f,x,g,newy,z)) return false;
		break;

	case C_LIST:
		QDBMSG_BLK("Travering block list %d\n", type,0);
	    if (crap.list.block &&
	    	!crap.list.block->Traverse(f,x,g,newy,z))
	    		 return false;
	    break;
	
	case C_DIVERT:
		QDBMSG_BLK("Travering diversion\n",0,0);
		if (crap.divert.clockExp && !crap.divert.clockExp->Traverse(f,x,g,newy,z)) return false;
		if (crap.divert.block && !crap.divert.block->Traverse(f,x,g,newy,z)) return false;
		break;
		
	case C_SET_ATTRIB:
		QDBMSG_BLK("Travering diversion\n", 0, 0);
		if (crap.setAttrib.block && !crap.divert.block->Traverse(f, x, g, newy, z)) return false;
		break;

	case C_BUILTIN: {
		QDBMSG_BLK("Travering up builtin call element\n",0,0);
	    if (crap.call.parameters && !crap.call.parameters->Traverse(f,x,g,newy,z)) return false;
		break;
	}
	
	case C_SAMPLE_PLAYER: {
		QDBMSG_BLK("Travering up builtin sample player element\n",0,0);
	    if (crap.call.parameters && !crap.call.parameters->Traverse(f,x,g,newy,z)) return false;
		break;
	}
	case C_TUNEDSAMPLE_PLAYER: {
		QDBMSG_BLK("Travering up builtin tuned sample player element\n",0,0);
	    if (crap.call.parameters && !crap.call.parameters->Traverse(f,x,g,newy,z)) return false;
		break;
	}
	case C_STREAM_PLAYER: {
		QDBMSG_BLK("Travering up builtin stream player element\n",0,0);
	    if (crap.call.parameters && !crap.call.parameters->Traverse(f,x,g,newy,z)) return false;
		break;
	}
	case C_MIDI_PLAYER: {
		QDBMSG_BLK("Travering up builtin midi sample element\n",0,0);
	    if (crap.call.parameters && !crap.call.parameters->Traverse(f,x,g,newy,z)) return false;
		break;
	}
	case C_MARKOV_PLAYER: {
		QDBMSG_BLK("Travering up builtin markov element\n",0,0);
	    if (crap.call.parameters && !crap.call.parameters->Traverse(f,x,g,newy,z)) return false;
		break;
	}
	case C_GENERIC_PLAYER: {
		QDBMSG_BLK("Travering up builtin call element\n",0,0);
	    if (crap.call.parameters && !crap.call.parameters->Traverse(f,x,g,newy,z)) return false;
		break;
	}
	case C_OUTPUT:
		QDBMSG_BLK("Travering up output element\n",0,0);
		 break;
		 
	case C_INPUT:
		QDBMSG_BLK("Travering up input element\n",0,0);
		break;

	case C_VST:
		QDBMSG_BLK("Travering up vst element %d\n", type,0);
	    if (	crap.call.parameters &&
	    		!crap.call.parameters->Traverse(f,x,g,newy,z))
	    	return false;
		break;

	case C_UNLINKED_CALL:
	case C_CALL:
		QDBMSG_BLK("Travering up call element %d\n", type,0);
	    if (	crap.call.parameters &&
	    		!crap.call.parameters->Traverse(f,x,g,newy,z))
	    	return false;
		break;

	case C_BREAK:
		QDBMSG_BLK("Travering up break element\n",0,0);
		 break;

	case C_SCHEDULE:
	case C_WAKE:
	case C_SUSPEND:
		QDBMSG_BLK("Travering up signal element\n",0,0);
	    if (	crap.call.parameters &&
	    		!crap.call.parameters->Traverse(f,x,g,newy,z))
	    	return false;
	    break;

	case C_ASSIGN:
		QDBMSG_BLK("Travering up assign element\n",0,0);
	    if (crap.assign.atom && !crap.assign.atom->Traverse(f,x,g,newy,z)) return false;
	    if (crap.assign.exp && !crap.assign.exp->Traverse(f,x,g,newy,z)) return false;
	    break;

	case C_GUARD:
		QDBMSG_BLK("Travering up guard element\n",0,0);
	    if (crap.guard.block && !crap.guard.block->Traverse(f,x,g,newy,z)) return false;
	    if (crap.guard.condition && !crap.guard.condition->Traverse(f,x,g,newy,z)) return false;
	    break;

	case C_REPEAT:
		QDBMSG_BLK( "Travering up repeat element\n",0,0);
	    if (crap.repeat.block && !crap.repeat.block->Traverse(f,x,g,newy,z)) return false;
	    if (crap.repeat.Exp && !crap.repeat.Exp->Traverse(f,x,g,newy,z)) return false;
	    break;

	case C_IFOP:
	case C_IF:
		QDBMSG_BLK( "Travering up if block\n",0,0);
	    if (crap.iff.ifBlock &&!crap.iff.ifBlock->Traverse(f,x,g,newy,z)) return false;
	    if (crap.iff.elseBlock && !crap.iff.elseBlock->Traverse(f,x,g,newy,z)) return false;
	    if (crap.iff.condition && !crap.iff.condition->Traverse(f,x,g,newy,z)) return false;
	    break;

	case C_FOREACH:
		QDBMSG_BLK("Travering up foreach block\n",0,0);
	    if (crap.foreach.ifBlock &&!crap.foreach.ifBlock->Traverse(f,x,g,newy,z)) return false;
	    if (crap.foreach.elseBlock && !crap.foreach.elseBlock->Traverse(f,x,g,newy,z)) return false;
	    if (crap.foreach.condition && !crap.foreach.condition->Traverse(f,x,g,newy,z)) return false;
	    break;

	case C_WITH:
		QDBMSG_BLK( "Travering up with block\n",0,0);
	    if (crap.with.withBlock &&!crap.with.withBlock->Traverse(f,x,g,newy,z)) return false;
	    if (crap.with.withoutBlock &&!crap.with.withoutBlock->Traverse(f,x,g,newy,z)) return false;
	    if (crap.with.condition && !crap.with.condition->Traverse(f,x,g,newy,z)) return false;
	    break;

	case C_UNOP:
	case C_BINOP:
		QDBMSG_BLK("Travering operator %d\n", subType,0);
	    if (crap.op.l && !crap.op.l->Traverse(f,x,g,newy,z)) return false;
	    if (crap.op.r && !crap.op.r->Traverse(f,x,g,newy,z)) return false;
	    if (crap.op.m && !crap.op.m->Traverse(f,x,g,newy,z)) return false;
	    break;
	    
	case C_VALUE:
		QDBMSG_BLK("Travering constant <%s>\n", (char *)crap.constant.value.StringValue(),0);
	    break;
	    
	case C_SYM:
		QDBMSG_BLK("Travering sym %s\n", crap.sym->name,0);
		break;

	case C_NAME:
		QDBMSG_BLK( "Travering name %s\n", crap.name,0);
	    break;
	    
	case C_STRUCTURE_REF:
		QDBMSG_BLK("Travering sref\n",0,0);
		if (crap.structureRef.base && !crap.structureRef.base->Traverse(f,x,g,newy,z)) return false;
//		if (crap.structureRef.member && !crap.structureRef.member->Traverse(f,x,g,newy,z)) return false;
		break;
		
	case C_ARRAY_REF:
		QDBMSG_BLK("Travering aref\n",0,0);
		if (crap.arrayRef.base && !crap.arrayRef.base->Traverse(f,x,g,newy,z)) return false;
		if (crap.arrayRef.index && !crap.arrayRef.index->Traverse(f,x,g,newy,z)) return false;
		break;

	case C_CAST:
		QDBMSG_BLK("Travering cast\n",0,0);
		if (crap.cast.block && !crap.cast.block->Traverse(f,x,g,newy,z)) {
			return false;
		}
		break;
		
	default:
	    internalError("Traverse: Bad block element %d", type);
	    return false;
	}

	if (next && ! next->Traverse(f,x,g,newy,z)) {
		return false;
	}
	
	return f?(this->*f)(x,newy,z):true;
}

/////////////////////////////////////////////////////////////////////////
// recursively flush stream data 
////////////////////////////////////////////////////////////////////////

void
Block::FlushOutput(bool ClrFlg)
{
	QDBMSG_BLK("flush...\n",0,0);
 	Traverse(&Block::DoFlush, (void *)ClrFlg, (VBlockFnPtr)nullptr, nullptr,0);
}

bool
Block::DoFlush(void * clrFlg, void *z, int y)
{
	switch (type) {
	case C_OUTPUT:
		crap.channel->ClearOutBuffers((bool)(clrFlg!= nullptr));
	    break;
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
// odd miscellaneous sods
/////////////////////////////////////////////////////////////////////////
status_t WriteHandlerBlock(ostream &out, char *which, short indent);
status_t
WriteHandlerBlock(ostream &out, char *which, Block *b, short indent)
{
	long		len;
	status_t	err = B_OK;

	if (b == nullptr) {
		return B_OK;
	}
	out << tab(indent) << which << " ";
	if (b->type != Block::C_LIST) {
		out << "{" << endl;
	}
	len=0;
	if (!Esrap(b, out, true, indent+1, 0)) {
		internalError("block size too large...");
		return B_ERROR;
	}
	if (b->type != Block::C_LIST) {
		out << endl;
		out << tab(indent);
		out << "}";
	}
	out << endl;
	return err;
}

status_t
SaveHandlers(ostream &out, short indent, StabEnt *sym, bool force_brace)
{
	status_t	err = B_OK;
	long		len;
	int			i;

	Schedulable *sched;
	Channel		*channel;
	if ((sched=sym->SchedulableValue()) != nullptr) {
		if (sched->wake.block) {
			out << endl;
			WriteHandlerBlock(out, "wake", sched->wake.block, indent);
		}
		if (sched->sleep.block) {
			out << endl;
			WriteHandlerBlock(out, "sleep", sched->sleep.block, indent);
		}
		if (sched->rx.block) {
			out << endl;
			WriteHandlerBlock(out, "rx", sched->rx.block, indent);
		}
		if (sched->start.block) {
			WriteHandlerBlock(out, "start", sched->start.block, indent);
		}
		if (sched->stop.block) {
			WriteHandlerBlock(out, "stop", sched->stop.block, indent);
		}
		if (sched->record.block) {
			WriteHandlerBlock(out, "record", sched->record.block, indent);
		}
	} else if ((channel=sym->ChannelValue()) != nullptr) {
//		if (sym->children) {
//			err=sym->children->Save(fp, indent+1);
//		}
		i = 0;
		for (auto ci: channel->inputs) {
			ci->Save(out, indent, i++);
		}
		i = 0;
		for (auto ci: channel->outputs) {
			ci->Save(out, indent, i++);
		}
		if (channel->rx.block) {
			WriteHandlerBlock(out, "rx", channel->rx.block, indent);
		}
				
		if (channel->tx.block) {
			WriteHandlerBlock(out, "tx", channel->tx.block, indent);
		}
	}
	return B_OK;
}

status_t
SaveMainBlock(Block *b, ostream &out, short indent, StabEnt *sym, bool force_brace, bool saveInits, Stacker *stacker, QuasiStack *stack)
{
	status_t	err;
	bool		forceblock = (sym->type != TypedValue::S_CHANNEL);


	if (b == nullptr) {
		if (sym->children || force_brace) {
			QDBMSG_BLK("save main: sym %d block %x", sym->type, b);
			out << " {" << endl;
			if (sym->children) {
				sym->children->SaveScript(out, indent+1, false, false);
			}
			SaveHandlers(out, indent+1, sym, force_brace);
#ifndef QUA_V_SAVE_INITASXML
			if (saveInits && sym->children) {
				sym->children->SaveInitialAssigns(fp, indent+1, stacker, stack);
			}
#endif
			out << tab(indent) << "}" << endl;
		} else {
			out << endl;
		}
		return B_NO_ERROR;
	} else if (b->type == Block::C_LIST) {
		out << endl << tab(indent);
		string os;
		stringstream ssos(os);
		if (!Esrap(b, ssos, true, indent, 0)) {
			internalError("block size too large...");
			return B_ERROR;
		}
		int l2 = os.find_last_of(' ');

		out.write(os.c_str(), l2 + 1);
		if (!out.good()) {
			err = B_ERROR;
		}

		if (force_brace)
			out << endl;
		if (sym->children) {
			sym->children->SaveScript(out, indent+1, false, false);
		}
		SaveHandlers(out, indent+1, sym, force_brace);
		if (force_brace)
			out << tab(indent+1);

		out.write(os.c_str() + l2 + 1, os.size() - (l2 + 1));
		if (!out.good()) {
			err = B_ERROR;
		}
		out << endl;
		return B_NO_ERROR;
	} else if (sym->children || force_brace) {
		out << "{" << endl;
		if (sym->children)
			sym->children->SaveScript(out, indent+1, false, false);
		SaveHandlers(out, indent+1, sym, force_brace);
		
		if (!Esrap(b, out, true, indent + 1, 0)) {
			internalError("block size too large...");
			return B_ERROR;
		}

		out << tab(indent) << "}" << endl;
		return B_NO_ERROR;
	} else {	
		out << "\t";
		if (!Esrap(b, out, true, indent+1, 0)) {
			internalError("block size too large...");
			return B_ERROR;
		}
		out << endl;
		return B_NO_ERROR;
	}
	return B_ERROR;
}

void
Block::dump(ostream &os, short indent)
{
	switch (type) {

	case C_WAIT:
		os << "wait(";
		if (crap.block) {
			crap.block->dump(os, 0);
		}
		os << ")\n";
		break;

	case C_FLUX:
		os << "<-";
	    if (crap.flux.lengthExp) {
	    	os << "(";
	    	crap.flux.lengthExp->dump(os, 0);
	    	os << ")";
	    }
		if (crap.flux.block)
			crap.flux.block->dump(os, indent+1);
		if (crap.flux.rateExp) {
			os << " : ";
			crap.flux.rateExp->dump(os, 0);
		}
		break;

	case C_LIST:
		os << (subType == LIST_SEQ? "[":
				subType == LIST_FORK? "{&":
				subType == LIST_PAR? "{|": "{");
	    if (crap.list.block)
	    	crap.list.block->dump(os, indent+1);
		os << (subType == LIST_SEQ?"]":"}");
	    break;
	
	case C_DIVERT:
		if (crap.divert.block)
			crap.divert.block->dump(os, indent+1);
		os << " @ ";
		if (crap.divert.clockExp)
			crap.divert.clockExp->dump(os, indent+1);
		break;
		
	case C_BUILTIN: {
		int t = subType;
		std::string s = qut::unfind(builtinCommandIndex, t);
		os << s;
		Block *params=crap.call.parameters;

		if (params) {
			os << "(";
			params->dump(os, 0);
			os << ")";
 	    }
 		break;
		os << "\n";
	}
	
	case C_STREAM_PLAYER:
	case C_GENERIC_PLAYER:
	case C_MIDI_PLAYER:
	case C_SAMPLE_PLAYER:
	case C_TUNEDSAMPLE_PLAYER: 
	case C_MARKOV_PLAYER: {
		std::string s =findClipPlayer(subType);
		os << s;
		Block *params = crap.call.parameters;

		if (params) {
			os << "(";
			params->dump(os, 0);
			os << ")";
 	    }
 		break;
		os << "\n";
	}
	
	case C_OUTPUT: {
		os << ">> " << crap.channel->chanId << endl;
		break;
	}
		 
	case C_INPUT: {
		os << "<< " << crap.channel->chanId << endl;
		break;
	}


	case C_UNLINKED_CALL:
	    if (name.size()) {
			os << name;
		    if (crap.call.parameters) {
		    	os << "(";
	    		crap.call.parameters->dump(os, 0);
		    	os << ")\n";
		    }
 	    }
		break;

	case C_CALL:
	    if (crap.call.crap.lambda) {
			os << crap.call.crap.lambda->sym->name;
		    if (crap.call.parameters) {
		    	os << "(";
	    		crap.call.parameters->dump(os, 0);
		    	os << ")\n";
		    }
 	    }
 	    break;

	case C_BREAK:
		os << "break\n";
		break;

	case C_WAKE:
	case C_SUSPEND:
	    if (crap.call.crap.sym) {
			os << crap.call.crap.sym->name;
		    if (crap.call.parameters) {
		    	os << "(";
		    	if (crap.call.parameters)
		    		crap.call.parameters->dump(os, indent+1);
		    	os << ")\n";
		    }
 	    }
	    break;

	case C_ASSIGN:
	    if (crap.assign.atom)
	    	crap.assign.atom->dump(os, indent+1);
		os << " = ";
	    if (crap.assign.exp)
	    	crap.assign.exp->dump(os, indent+1);
    	os << "\n";
	    break;

	case C_GUARD:
	    if (crap.guard.condition)
	    	crap.guard.condition->dump(os, indent+1);
		os << "::";
	    if (crap.guard.block)
	    	crap.guard.block->dump(os, indent+1);
	    break;

	case C_REPEAT:
		os << "repeat(";
	    if (crap.repeat.Exp)
	    	crap.repeat.Exp->dump(os, indent+1);
		os << ")";
	    if (crap.repeat.block)
	    	crap.repeat.block->dump(os, indent+1);
	    break;

	case C_IF:
		os << "if (";
	    if (crap.iff.condition)
	    	crap.iff.condition->dump(os, 0);
	    os << ")";
	    if (crap.iff.ifBlock)
	    	crap.iff.ifBlock->dump(os, indent+1);
	    if (crap.iff.elseBlock) {
	    	os << "else";
			crap.iff.elseBlock->dump(os, indent+1);
		}
	    break;

	case C_FOREACH:
		os << "foreach (";
	    if (crap.foreach.condition)
	    	crap.foreach.condition->dump(os, 0);
	    os << ")";
	    if (crap.foreach.ifBlock)
	    	crap.foreach.ifBlock->dump(os, indent+1);
	    if (crap.foreach.elseBlock) {
	    	os << "else";
			crap.foreach.elseBlock->dump(os, indent+1);
		}
	    break;

	case C_WITH:
		internalError("Unimplimented dump");
//		    if (crap.with.withBlock &&!crap.with.withBlock->->dump(os, indent+1);buf,pos,len, do_indent, indent)) return false;
//		    if (crap.with.withoutBlock &&!crap.with.withoutBlock->->dump(os, indent+1);buf,pos,len, do_indent, indent)) return false;
//		    if (crap.with.condition && !crap.with.condition->->dump(os, indent+1);buf,pos,len, do_indent, indent)) return false;
	    break;

	case C_UNOP:
	    if (crap.op.l) {
	    	switch(subType) {
	    	case OP_UMINUS:	os << "-"; break;
	    	case OP_NOT:	os << "!"; break;
	    	case OP_BNOT:	os << "~"; break;
	    	default:
				internalError("Unimplimented Esrap: unop");
	    	}
			if (crap.op.l->isOperator()) os << "(";
			crap.op.l->dump(os, 0);
			if (crap.op.l->isOperator()) os << ")";
	    }
	    break;
	    
	case C_BINOP:
		if (crap.op.l && crap.op.r) {
			if (crap.op.l->isOperator())
	    		os << "(";
	    	crap.op.l->dump(os, 0);
			if (crap.op.l->isOperator())
	    		os << ")";
	    	switch(subType) {
			case OP_LT:		os << "<"; break;
			case OP_GT:		os << ">"; break;
			case OP_EQ:		os << "=="; break;
			case OP_NEQ:	os << "!="; break;
			case OP_LE:		os << "<="; break;
			case OP_GE:		os << ">="; break;
			case OP_MUL:	os << "*"; break;
			case OP_DIV:	os << "/"; break;
			case OP_ADD:	os << "+"; break;
			case OP_SUB:	os << "-"; break;
			case OP_MOD:	os << "%%"; break;
			case OP_AND:	os << "&&"; break;
			case OP_OR:		os << "||"; break;
			case OP_BAND:	os << "&"; break;
			case OP_BOR:	os << "|"; break;
	    	default:
				internalError("Unimplimented Esrap: unop");
	    	}
	
			if (crap.op.r->isOperator()) os << "(";
	    	crap.op.r->dump(os, 0);
	    	
			if (crap.op.r->isOperator()) os << ")";
		}
	    break;
	    
	case C_IFOP:
		if (crap.iff.condition)
			crap.iff.condition->dump(os, indent+1);
		os << "?";
	    if (crap.iff.ifBlock)
			crap.iff.ifBlock->dump(os, indent+1);
		os << ":";
	    if (crap.iff.elseBlock)
			crap.iff.elseBlock->dump(os, indent+1);
	    break;
	    
	case C_VALUE:
		if (crap.constant.value.type == TypedValue::S_STRING)
			os << "\"";
		os << crap.constant.value.StringValue();
		if (crap.constant.value.type == TypedValue::S_STRING)
			os << "\"";
		os << " ";
	    break;
	    
	case C_SYM:
		os << crap.sym->name;
		os << " ";
		break;

	case C_NAME:
		os << name;
		os << " ";
	    break;
	    
	case C_STRUCTURE_REF:
		if (crap.structureRef.base)
			crap.structureRef.base->dump(os, 0);
		os << ".";
		if (crap.structureRef.member) {
			crap.structureRef.member->dump(os, 0);
		}
		break;
		
	case C_ARRAY_REF:
		if (crap.arrayRef.base && crap.arrayRef.index) {
			crap.arrayRef.base->dump(os, 0);
			os << "[";
			crap.arrayRef.index->dump(os, 0);
			os << "]";
		}
		break;

	case C_CAST: {
		std::string s =findTypeName(subType);
		os << "(#";
		os << s;
		os << " ";
		crap.cast.block->dump(os, 0);
		os << ")";
		break;
	}
	
	default:
		internalError("Esrap: Bad block element %d", type);
	    return;
	}
	
	if (next)
		next->dump(os, indent);
}

void
Block::Set(ulong t, ulong st)
{
	type = t;
	subType = st;
}

void
Block::SetConstValue(TypedValue &v, char*nm)
{
	type = C_VALUE;
	crap.constant.value = v;
	if (nm) name = nm;
}

void
TypedValue::SetToSymbol(char *nm, StabEnt *ctxt)
{
	StabEnt *sym;
	if ((sym=glob.findContextSymbol(nm, ctxt)) != nullptr) {
		Set(sym);
		QDBMSG_BLK("set %d %s\n", val.stackAddress.offset, val.stackAddress.context->name);
	} else {
		internalError("internal error: %s var not found in %s", nm, ctxt->name);
	}
}
