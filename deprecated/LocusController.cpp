#include "Colors.h"
#include "KeyVal.h"
#include "NumCtrl.h"
#include "PoolSelect.h"
#include "Voice.h"
#include "QuaQua.h"
#include "Block.h"
#include "Source.h"
#include "Locus.h"
#include "messid.h"
#include "StdDefs.h"
#include "GlblMessid.h"
#include "QuasiStack.h"
#include "ControlVar.h"

FormalParamList *
first_unpassed_param(Locus *V)
{
	if (V->CallingBlock) {
		FormalParamList *par = V->Source->formalParams;
		for (Block *p=V->CallingBlock->Crap.Call.Params; p!= nullptr;p=p->Next) {
			par = par->Next;
		}
		return par;
	} else
		return nullptr;
}

LocusCtlView::LocusCtlView(QuasiStack *V, BRect rect):
	ControlPanel(rect,V->formalParams,
		!((Source*)V->executable)->isFixed,
		!(((Source*)V->executable)->isOncer || ((Source*)V->executable)->isFixed),
		!((Source*)V->executable)->isOncer,
		FALSE,
		!(((Source*)V->executable)->isToggled || ((Source*)V->executable)->isOncer || ((Source*)V->executable)->isFixed),
		FALSE,
		((Source*)V->executable)->isToggled? CP_TYPE_TOGGLE:
		((Source*)V->executable)->isHeld? CP_TYPE_HELD: CP_TYPE_NORM,
		15,
		"Bang",
		V->callingBlock->Crap.Call.Label?
			V->callingBlock->Crap.Call.Label:((Source*)V->executable)->Name,
		V->callingBlock->Crap.Call.Status, ((Source*)V->executable)->uberQua,
		V, &V->higherFrame, nullptr, nullptr)
{
	myStack = V;
	V->controlPanel = this;
}

LocusCtlView::~LocusCtlView()
{
	myStack->controlPanel = nullptr;
}


void
LocusCtlView::MessageReceived(BMessage *msg)
{
	switch (msg->what) {
	case CTRL_BANG: {
		myStack->Trigger();
		break;
	}
	
	case CTRL_STOP: {
		myStack->UnTrigger();
		break;
	}
	
	case PARAM_DISPLAY: {
		ControlVar *p;
		status_t err = msg->FindPointer("which", &p);
		char *val = msg->FindString("string_value");
		p->SetDisplayValue(val);
		break;
	}
	
	case PARAM_SET: {
		ControlVar *p;
		status_t err = msg->FindPointer("which", &p);
		if (p != nullptr) {
			p->SetSymbolValue();
		}
		break;
	}
	
	default:
		BView::MessageReceived(msg);
	}
}
