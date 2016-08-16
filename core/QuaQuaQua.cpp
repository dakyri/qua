#include "qua_version.h"


/*
 * Qua:
 * the hacked Be version
 *  more of the same. the guts of it
 */
#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <stdlib.h>

#include <string>

#include "StdDefs.h"

#include "Block.h"
#include "Voice.h"
#include "Sym.h"
#include "QuaMidi.h"
#include "QuaParallel.h"
#include "Qua.h"
#include "Expression.h"
#include "Note.h"
#include "Pool.h"
#include "Lambda.h"
#include "QuaJoystick.h"
#include "Template.h"
#include "MidiDefs.h"
#include "Metric.h"
#ifdef QUA_V_AUDIO
#include "QuaAudio.h"
#endif
#ifdef QUA_V_VST_HOST
#include "VstPlugin.h"
#endif
#include "Parse.h"
#include "Sym.h"
#include "OSCMessage.h"


flag		debug_gen = 0;

#include <iostream>


/*
 * PutNextNote:
 */

ResultValue
CreateNewNote(int type, Block *P,
			StreamItem *items,
			Stacker *stacker,
			StabEnt *stackCtxt,
			QuasiStack *stack)
{
    static 			Note	noteTo;
    ResultValue		val,
    				ret_val;
    

    noteTo.cmd = MIDI_NOTE_ON;
    
    ret_val.Set(TypedValue::S_NOTE, TypedValue::REF_POINTER);
	ret_val.SetPointerValue((void *)NULL);
	ret_val.flags  = ResultValue::COMPLETE;

    val = EvaluateExpression(P, items, stacker, stackCtxt, stack);
	if (val.Blocked()) {
		ret_val.flags |= ResultValue::BLOCKED;
		return ret_val;
	}
	if (!val.Complete()) {
		ret_val.flags |= ~ResultValue::COMPLETE;
	}
    if (val.type == TypedValue::S_STRING)
    	noteTo.pitch = stringToNote(val.StringValue());
    else
    	noteTo.pitch = val.ByteValue(NULL);

    val = EvaluateExpression(P->next, items, stacker, stackCtxt, stack);
	if (val.Blocked()) {
		ret_val.flags |= ResultValue::BLOCKED;
		return ret_val;
	}
	if (!val.Complete()) {
		ret_val.flags |= ~ResultValue::COMPLETE;
	}
	noteTo.dynamic = val.ByteValue(NULL);

    val = EvaluateExpression(P->next->next, items, stacker, stackCtxt, stack);
	if (val.Blocked()) {
		ret_val.flags |= ResultValue::BLOCKED;
		return ret_val;
	}
	if (!val.Complete()) {
		ret_val.flags |= ~ResultValue::COMPLETE;
	}
    noteTo.duration = val.FloatValue(NULL);

// this was never a good way of doing things
//	noteTo.attributes.add(P->next->next->next);
	ret_val.SetPointerValue(&noteTo);
	if (debug_gen)
		cerr << "creating type "<< ret_val.type <<": pitch "<< noteTo.pitch <<" dynamic " << noteTo.dynamic << ": duration " << noteTo.duration << endl;
   
    return ret_val;
}


ResultValue
CreateNewCtrl(Block *P,
			StreamItem *items,
			Stacker *stacker,
			StabEnt *stackCtxt,
			QuasiStack *stack)
{
    static Ctrl		ctrlTo;
//    float			dur;
    ResultValue		val,
    				ret_val;
    bool			comp = true;
    
    ret_val.Set(TypedValue::S_CTRL, TypedValue::REF_POINTER);
	ret_val.flags = ResultValue::COMPLETE;

    val = EvaluateExpression(P, items, stacker, stackCtxt, stack);
	if (val.Blocked()) {
		ret_val.flags |= ResultValue::BLOCKED;
		return ret_val;
	}
	if (!val.Complete()) {
		ret_val.flags |= ~ResultValue::COMPLETE;
	}
   	ctrlTo.controller = (ctrl_t)val.IntValue(NULL);

    val = EvaluateExpression(P->next, items, stacker, stackCtxt, stack);
	if (val.Blocked()) {
		ret_val.flags |= ResultValue::BLOCKED;
		return ret_val;
	}
	if (!val.Complete()) {
		ret_val.flags |= ~ResultValue::COMPLETE;
	}
    ctrlTo.amount = (amt_t)val.IntValue(NULL);

	if (debug_gen)
		cerr << "creating "<< ret_val.type <<": " << ctrlTo.controller << " " << ctrlTo.amount <<  endl;
	ret_val.SetPointerValue(&ctrlTo);
   
    return ret_val;
}

ResultValue
CreateNewBend(Block *P,
			StreamItem *items,
			Stacker *stacker,
			StabEnt *stackCtxt,
			QuasiStack *stack)
{
    static Bend		bendTo;
    ResultValue		val,
    				ret_val;
    bool			comp = true;
    
	ret_val.Set(TypedValue::S_BEND, TypedValue::REF_POINTER);
	ret_val.SetPointerValue((void*)NULL);
	ret_val.flags = ResultValue::COMPLETE;

    val = EvaluateExpression(P, items, stacker, stackCtxt, stack);
	if (val.Blocked()) {
		ret_val.flags |= ResultValue::BLOCKED;
		return ret_val;
	}
	if (!val.Complete()) {
		ret_val.flags |= ~ResultValue::COMPLETE;
	}
   	bendTo.bend = (bend_t)val.IntValue(NULL);

	if (debug_gen)
		cerr << "creating "<< ret_val.type <<": " << bendTo.bend << endl;
	ret_val.SetPointerValue(&bendTo);
   
    return ret_val;
}

ResultValue
CreateNewProg(Block *P,
			StreamItem *items,
			Stacker *stacker,
			StabEnt *stackCtxt,
			QuasiStack *stack)
{
    static Prog		progTo;
    ResultValue		val,
    				ret_val;
    bool			comp = true;
    
	ret_val.Set(TypedValue::S_PROG, TypedValue::REF_POINTER);
	ret_val.SetPointerValue((void*)NULL);
	ret_val.flags = ResultValue::COMPLETE;

    val = EvaluateExpression(P, items, stacker, stackCtxt, stack);
	if (val.Blocked()) {
		ret_val.flags |= ResultValue::BLOCKED;
		return ret_val;
	}
	if (!val.Complete()) {
		ret_val.flags |= ~ResultValue::COMPLETE;
	}
   	progTo.program = (prg_t)val.IntValue(NULL);
	progTo.bank = NON_PROG;
	progTo.subbank = NON_PROG;
	if (debug_gen)
		cerr << "creating " << ret_val.type << ": " << progTo.program << endl;
	ret_val.SetPointerValue(&progTo);
   
    return ret_val;
}

ResultValue
CreateNewSysX(Block *P,
			StreamItem *items,
			Stacker *stacker,
			StabEnt *stackCtxt,
			QuasiStack *stack)
{
     static SysX	sysXTo;
     ResultValue	val,
    				ret_val;
    bool			comp = true;
    
	ret_val.Set(TypedValue::S_SYSX, TypedValue::REF_POINTER);
	ret_val.SetPointerValue((void*)NULL);
	ret_val.flags = ResultValue::COMPLETE;

    val = EvaluateExpression(P, items, stacker, stackCtxt, stack);
	if (val.Blocked()) {
		ret_val.flags |= ResultValue::BLOCKED;
		return ret_val;
	}
	if (!val.Complete()) {
		ret_val.flags |= ~ResultValue::COMPLETE;
	}
    if (val.type == TypedValue::S_LAMBDA) {
   		Lambda *S = val.LambdaValue();
//   		S->sym->FindData(&SysXTo.data, &SysXTo.length);
   	} else {
   		sysXTo.length = 0;
   		sysXTo.data = 0;
   	}

	if (debug_gen)
		cerr << "creating "<< ret_val.type <<": " << sysXTo.length << endl;
	ret_val.SetPointerValue(&sysXTo);
   
    return ret_val;
}

ResultValue
CreateNewSysC(int which, Block *P,
			StreamItem *items,
			Stacker *stacker,
			StabEnt *stackCtxt,
			QuasiStack *stack)
{
    static SysC		sysCTo;
    ResultValue		val,
    				ret_val;
    bool			comp = true;
    
	ret_val.Set(TypedValue::S_SYSC, TypedValue::REF_POINTER);
	ret_val.SetPointerValue((void*)NULL);
	ret_val.flags = ResultValue::COMPLETE;

	sysCTo.cmd = which;

    val = EvaluateExpression(P, items, stacker, stackCtxt, stack);
	if (val.Blocked()) {
		ret_val.flags |= ResultValue::BLOCKED;
		return ret_val;
	}
	if (!val.Complete()) {
		ret_val.flags |= ~ResultValue::COMPLETE;
	}
   	sysCTo.data1 = (int8)val.IntValue(NULL);

    val = EvaluateExpression(P, items, stacker, stackCtxt, stack);
	if (val.Blocked()) {
		ret_val.flags |= ResultValue::BLOCKED;
		return ret_val;
	}
	if (!val.Complete()) {
		ret_val.flags |= ~ResultValue::COMPLETE;
	}
   	sysCTo.data2 = (int8)val.IntValue(NULL);

	if (debug_gen)
		cerr << "creating "<< ret_val.type <<": " << sysCTo.cmd << endl;
	ret_val.SetPointerValue(&sysCTo);
   
    return ret_val;
}

/*
 xxxx todo careful with ownership
 assumption now is that the stream processor takes ownership, or if not added to a stream, then deleted by updateactive
*/
ResultValue
CreateNewMesg(Block *P,
			StreamItem *items,
			Stacker *stacker,
			StabEnt *stackCtxt, QuasiStack *stack)
{
	OSCMessage *msg;
    ResultValue val, ret_val;
    char *path;

	ret_val.Set(TypedValue::S_MESSAGE, TypedValue::REF_POINTER);
	ret_val.SetPointerValue((void*)nullptr);
	ret_val.flags = ResultValue::COMPLETE;

    val = EvaluateExpression(P, items, stacker, stackCtxt, stack);
	if (val.Blocked()) {
		ret_val.flags |= ResultValue::BLOCKED;
		return ret_val;
	}
	if (!val.Complete()) {
		ret_val.flags |= ~ResultValue::COMPLETE;
	}

    path = val.StringValue();
	msg = new OSCMessage(path);
// items to add!	
	P = P->next;
	while (P != NULL) {
	    val = EvaluateExpression(P, items, stacker, stackCtxt, stack);
	    if (!val.Blocked()) {
			if (val.type == TypedValue::S_LIST) {
				val.ListValue().AddToMessage(msg);
				if (!val.Complete()) {
					ret_val.flags |= ~ResultValue::COMPLETE;
				}
			} else {
				msg->add(val);
			}
	    }

	    P = P->next;
	}
	ret_val.SetPointerValue(msg);
   
    return ret_val;
}

ResultValue
Find(Stream *S, Block *Query)
{
	ResultValue ret_val;
	
	ret_val = TypedValue::List();

	for (StreamItem *p=S->head; p!=NULL; p=p->next) {
		ResultValue cond_val = EvaluateExpression(Query, p, NULL, NULL);
		if (cond_val.IntValue(NULL)) {
			ret_val.ListValue().AddItem(p, TypedValue::S_STREAM_ITEM);
			break;
		}
	}
	return ret_val;
}
