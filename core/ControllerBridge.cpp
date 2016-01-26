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

#if defined(WIN32)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <stdio.h>


#endif

#include "ControllerBridge.h"
#include "Sym.h"
#include "Block.h"
#include "Pool.h"
#include "Method.h"
#include "Pool.h"
#include "Sample.h"
#include "Envelope.h"
#include "Qua.h"


#ifdef QUA_V_APP_HANDLER
#include "include/Application.h"
#endif

#ifdef QUA_V_ARRANGER_INTERFACE
#if defined(WIN32)
#elif defined(_BEOS)
#include "MyCheckBox.h"
#include "EnvelopePanel.h"
#include "include/ArrangerObject.h"
#include "include/SequencerWindow.h"
#include "NumCtrl.h"
#include "NameCtrl.h"
#include "TimeCtrl.h"
#include "PoolSelect.h"
#include "BlockCtrl.h"
#endif
#endif

		
// set the diplay value on the mixer panel
// forward an update message to the arranger object
void
QuaControllerBridge::SetDisplayValue()
{
	fprintf(stderr, "Set display\n");
#if defined(WIN32)
#elif defined(_BEOS)
	if (envelope) {
		if (msg && msg->HasInt32("segment")) {
			int		seg = msg->FindInt32("segment");
			if (  stacker &&
				  stacker->arrangerObject &&
				  stacker->arrangerObject->displayedControlVariable == this) {
				ArrangerObject	*ao = stacker->arrangerObject;
				fprintf(stderr, "ao %x\n");
				BRect	r(	seg <= 0?0:ao->TimeToPixel(envelope->segment[seg-1].time.ticks),
							0,
							seg < envelope->nSeg-1?ao->TimeToPixel(envelope->segment[seg+1].time.ticks):ao->TimeToPixel(INFINITE_TICKS),
							stacker->arrangerObject->ObjectHeight()+1);
				BMessage	m(REDISPLAY_OBJECT);
				m.AddRect("rect", r);
				m.AddPointer("object", ((ObjectView*)ao));
				if (ao->Window())
					ao->Window()->PostMessage(&m, ao);			
//				a->Draw(r);
			}
			if (envelopePanel && envelopePanel->displayMode == DISPLAY_ENV) {
				EnvelopePanel	*ep = envelopePanel;
				if (ep->Window())
					ep->Window()->Lock();
				BRect		bnd = ep->editRegion;
				BRect	r(	seg <= 0?bnd.left:
								ep->TimeToPixel(envelope->segment[seg-1].time.ticks),
							bnd.top,
							seg < envelope->nSeg-1?ep->TimeToPixel(envelope->segment[seg+1].time.ticks):bnd.right,
							bnd.bottom);
				ep->SetLowColor(ep->bgColor);
				ep->FillRect(r, B_SOLID_LOW);
				ep->DrawNewEnvSegment(envelope, seg);
				if (ep->Window())
					ep->Window()->Unlock();
			}
		}
	} else {
		if (screenControl) {
			LValue	l = stabEnt->SetLValue(NULL, stacker, stackFrame);
			ResultValue	v	= l.CurrentValue();

			switch (stabEnt->type) {
			case S_POOL:
				break;
			
			case S_BOOL: {
				((MyCheckBox *)screenControl)->SetValue(v.BoolValue(NULL));
				break;
			}
			
			case S_TIME: {
				((TimeCtrl *)screenControl)->SetValue(v.TimeValue()->ticks);
				break;
			}
			
			case S_INT: {
				((NumCtrl *)screenControl)->SetValue(v.IntValue(NULL));
				break;
			}
			
			case S_BYTE: {
				((NumCtrl *)screenControl)->SetValue(v.FloatValue(NULL));
				break;
			}
			
			case S_FLOAT: {
				((NumCtrl *)screenControl)->SetValue(v.FloatValue(NULL));
				break;
			}
			
			case S_SHORT: {
				((NumCtrl *)screenControl)->SetValue(v.ShortValue(NULL));
				break;
			}
				
			case S_LONG: {
				((NumCtrl *)screenControl)->SetValue(v.LongValue(NULL));
				break;
			}
			
			default:
				break;
			}
		}
	}
#endif
}


void
QuaControllerBridge::SetSymbolValue()
{
	LValue	lval;
	sym->SetLValue(lval, 0, stacker, stackCtxt, stackFrame);
#if defined(WIN32)
#elif defined(_BEOS)
	switch (stabEnt->type) {
	case S_POOL: {
		PoolSelect	*PS = (PoolSelect *)screenControl;
		Pool	*P = PS->selected;
		stabEnt->SetValue(P);
		break;
	}

	case S_TIME:
		lval.StoreInt(((TimeCtrl *)screenControl)->TickValue());
		break;
		
	case S_BOOL:
		lval.StoreInt(((BCheckBox *)screenControl)->Value());
		break;
		
	case S_BYTE:
	case S_SHORT:
	case S_INT:
		lval.StoreInt(((NumCtrl *)screenControl)->Value());
		break;
		
	case S_LONG:
		lval.StoreLong(((NumCtrl *)screenControl)->Value());
		break;
		
	case S_FLOAT:
		lval.StoreFloat(((NumCtrl *)screenControl)->Value());
		break;
		
	case S_EXPRESSION: {
		BlockCtrl	*PS = (BlockCtrl *)screenControl;
		RWLock		*l = (RWLock *)(((int32)stabEnt->maxVal.PointerValue())
								+ ((int32)stacker));
		StabEnt		*s = (StabEnt *)stabEnt->iniVal.PointerValue();
		
		if (s == NULL) {
			reportError("Can't find all the right bits to compile this");
		} else {
			Block *b = PS->ExpressionValue(s);
			
			if (l) l->Lock();
			lval.StoreBlock(b);
			if (l) l->Unlock();
		}
		break;
	}

	default:
		reportError("Unlikely combination for control var");
		lval.StoreInt(((NumCtrl *)screenControl)->Value());
	}
#endif
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
#ifdef QUA_V_ARRANGER_INTERFACE
//	screenControl = NULL;
//	envelopePanel = NULL;
#endif
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
#ifdef QUA_V_ARRANGER_INTERFACE
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

#if defined(WIN32)
#elif defined(_BEOS)

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
#endif		
		return true;
	}
	return false;
}
*/
