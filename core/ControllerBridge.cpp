#include "qua_version.h"

/*
 * control variables. these are constructed on stack for any used stack for any
 * dynamic variable/symbol that is either a controller or envelope driven.
 * these hold references to interface controllers for the component, and
 * references to envelopes.... envelopes are held and updated by the instance
 * object when it is run.
 * provides a bridge for
 *   - controllers to update system and qua variables
 *   - record changes to variables within envelopes
 *	 - display changes that running qua schedulable instances make to their variables
 */

#include <stdio.h>

#include "Sym.h"
#include "Block.h"
#include "Pool.h"
#include "Lambda.h"
#include "Pool.h"
#include "Sample.h"
#include "Envelope.h"
#include "Qua.h"
#include "ControllerBridge.h"
#include "QuaDisplay.h"

void
QuaControllerBridge::SetDisplayValue()
{
	// deleted stuff from the original that set the display value of the screenControl display thingy according to current value
	// also did stuff with onscreen envelopes
}


void
QuaControllerBridge::SetSymbolValue()
{
	LValue	lval;
	sym->SetLValue(lval, 0, stacker, stackCtxt, stackFrame);
	// deleted stuff from the original that set the value according to the screenControl var
}

QuaControllerBridge::QuaControllerBridge(TimeKeeper *timeKeeper,
						Stacker *i,
						QuasiStack *stack,
						StabEnt *controlsym,
						StabEnt *stackCtx)	
{
	sym = controlsym;
	stacker = i;
	envelope = NULL;
	stackCtxt = stackCtx;
	stackFrame = stack;

	if (sym->controlMode == StabEnt::DISPLAY_ENV) {
		envelope = new Envelope(sym, /*this,*/
								timeKeeper?timeKeeper->metric:&Metric::std,
								true,
								sym->iniVal.FloatValue(stack),
								stacker,
								stackCtxt,
								stackFrame);
	}
}



status_t
QuaControllerBridge::Save(FILE *fp, short indent)
{
	bool		comma = false;
	status_t	err=B_NO_ERROR;
	if (envelope)
		envelope->Save(fp);
	else
		sym->SaveValue(fp, stacker, stackFrame);
	return err;
}

QuaControllerBridge::~QuaControllerBridge()
{
	if (envelope)
		delete envelope;
}

void
QuaControllerBridge::DrawNewEnvSegment(Envelope *e, long ind)
{
	;
}

void
QuaControllerBridge::DrawNewEnvSegPosition(Envelope *e, long ind, bool erase)
{
	;
}

bool
QuaControllerBridge::SetValue(Block *b)
{
	if (envelope) {
		envelope->SetValue(b);
	} else if (sym->type == TypedValue::S_EXPRESSION ||
		sym->type == TypedValue::S_BLOCK) {
#ifdef XXXX
		BlockCtrl	*PS = (BlockCtrl *)screenControl;
		RWLock		*l = (RWLock *)(((int32)sym->maxVal.PointerValue())
								+ ((int32)stacker));
		StabEnt		*s = (StabEnt *)sym->iniVal.PointerValue();
		
		if (s == NULL) {
			reportError("Can't find all the right bits to compile this");
		} else {
			if (l) l->Lock();
			
			Block	*n = b->next;
			b->next = NULL;
			Block	*newB = (b->type != C_UNKNOWN)? new Block(b, s, false):NULL;
			b->next = n;
			if (newB) {
				glob.PushContext(s);
				if (!newB->Init(NULL)) {
					reportError("initialisation fails: bad luck");
					delete newB;
					newB = NULL;
				}
				glob.PopContext();
			}
			LValue	lval = sym->SetLValue(0, stacker, stackFrame);
			lval.StoreBlock(newB);

			if (l) l->Unlock();
		}
#endif
	} else {
		ResultValue v = EvaluateExpression(b);
		LValue	lval;
		sym->SetLValue(lval, 0, stacker, NULL, stackFrame);
		lval.StoreValue(&v);
	}
	return true;
}

/////////////////////////////////////////////////////////
// control var related routines from stabent
/////////////////////////////////////////////////////////
/*bool
StabEnt::UpdateControllerBridge(Time *timep, TypedValue *v, QuasiStack *stack)
{
	QuaControllerBridge	*p;
	StabEnt				*truectxt = context;

	while (truectxt) {
		if (truectxt->type != S_PARAMGROUP) {
			break;
		}
		truectxt = truectxt->context;
	}
	
	for (;stack!=NULL; stack=stack->lowerFrame) {
		if (stack->context == truectxt)
			break;
	}
	if (	controlMode != StabEnt::DISPLAY_NOT &&
			stack != NULL &&
			(p=stack->ControllerBridgeFor(this))) {


		BMessage	*msg = new BMessage(UPDATE_CONTROL_VAR);
		msg->AddPointer("control var", p);
		if (p->envelope && timep) {		// add points...
			Time	theTime = *timep;
			if (stack->stacker) {
				theTime = stack->stacker->RelativeTime(theTime);
			}
			int		seg = p->envelope->Segment(*timep);
			if (seg >= 0 && p->envelope->segment[seg].time == theTime) {
				p->envelope->MoveSegment(seg, theTime, v->FloatValue(NULL), false);
				msg->AddInt32("segment", seg);
			} else {
				seg = p->envelope->AddSegment(theTime, v->FloatValue(NULL), ENV_SEG_TYPE_FIXED, false);
				msg->AddInt32("segment", seg);
			}
		} else {
			;
		}
		
		if (stack->controlPanel) {
			BWindow *w = stack->controlPanel->Window();
			w->PostMessage(msg, stack->controlPanel);
		}
		delete msg;
	
		return true;
	}
	return false;
}
*/
