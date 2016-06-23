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
#include "Method.h"
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


flag		debug_gen = 0;


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
	ret_val.SetValue((void *)NULL);
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
    	noteTo.pitch = StringToNote(val.StringValue());
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

	noteTo.properties = NULL;
	noteTo.AddProperties(P->next->next->next);
	ret_val.SetValue(&noteTo);
	if (debug_gen)
		fprintf(stderr, "creating type %d: pitch %d dynamic %d duration %d\n", ret_val.type,
					noteTo.pitch,
					noteTo.dynamic,
					noteTo.duration
					);
   
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
		fprintf(stderr, "creating %d: %d %d\n", ret_val.type,
					ctrlTo.controller,
					ctrlTo.amount);
	ret_val.SetValue(&ctrlTo);
   
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
	ret_val.SetValue((void*)NULL);
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
		fprintf(stderr, "creating %d: %d\n", ret_val.type,
					bendTo.bend);
	ret_val.SetValue(&bendTo);
   
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
	ret_val.SetValue((void*)NULL);
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
		fprintf(stderr, "creating %d: %d\n", ret_val.type,
					progTo.program);
	ret_val.SetValue(&progTo);
   
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
	ret_val.SetValue((void*)NULL);
	ret_val.flags = ResultValue::COMPLETE;

    val = EvaluateExpression(P, items, stacker, stackCtxt, stack);
	if (val.Blocked()) {
		ret_val.flags |= ResultValue::BLOCKED;
		return ret_val;
	}
	if (!val.Complete()) {
		ret_val.flags |= ~ResultValue::COMPLETE;
	}
    if (val.type == TypedValue::S_METHOD) {
   		Method *S = val.MethodValue();
//   		S->sym->FindData(&SysXTo.data, &SysXTo.length);
   	} else {
   		sysXTo.length = 0;
   		sysXTo.data = 0;
   	}

	if (debug_gen)
		fprintf(stderr, "creating %d: %d\n", ret_val.type,
					sysXTo.length);
	ret_val.SetValue(&sysXTo);
   
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
	ret_val.SetValue((void*)NULL);
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
		fprintf(stderr, "creating %d: %d\n", ret_val.type,
					sysCTo.cmd);
	ret_val.SetValue(&sysCTo);
   
    return ret_val;
}

#ifdef QUA_V_APP_HANDLER
ResultValue
CreateNewMesg(Block *P,
			StreamItem *items,
			Stacker *stacker, QuasiStack *stack)
{
	BMessage		*msg;
    ResultValue		val,
    				ret_val;
    int				type;

	ret_val.Set(TypedValue::S_MESSAGE, REF_POINTER);
	ret_val.SetValue((void*)NULL);
	ret_val.complete = TRUE;

    val = EvaluateExpression(P, items, stacker, stackCtxt, stack);
	if (val.Blocked()) {
		ret_val.flags |= ResultValue::BLOCKED;
		return ret_val;
	}
	if (!val.Complete()) {
		ret_val.flags |= ~ResultValue::COMPLETE;
	}
// mess id!
    type = val.IntValue(NULL);

	msg = new BMessage(type);
// items to add!	
	P = P->next;
	while (P != NULL) {
	    val = EvaluateExpression(P, items, stacker, stackCtxt, stack);
	    if (!val.blocked && val.type == TypedValue::S_LIST) {
	    	val.ListValue()->AddToMessage(msg);
	    	delete val.ListValue();
	    }
    	if (!val.complete) ret_val.complete = FALSE;
	    P = P->next;
	}
	ret_val.SetValue(msg);
   
    return ret_val;
}
#endif

ResultValue
Find(Stream *S, Block *Query)
{
	ResultValue ret_val;
	
	ret_val = TypedValue::List();

	for (StreamItem *p=S->head; p!=NULL; p=p->next) {
		ResultValue cond_val = EvaluateExpression(Query, p, NULL, NULL);
		if (cond_val.IntValue(NULL)) {
			ret_val.ListValue()->AddItem(p, TypedValue::S_STREAM_ITEM);
			break;
		}
	}
	return ret_val;
}
