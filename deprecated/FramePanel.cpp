
#ifdef __INTEL__ 
#include <SupportKit.h>
#include <StorageKit.h>
#include <InterfaceKit.h>
#endif

#include "InstancePanel.h"
#include "QuasiStack.h"
#include "Sym.h"
#include "Method.h"
#include "Block.h"
#include "messid.h"
#include "MyCheckBox.h"
#include "ControlVariable.h"
#include "Colors.h"

FramePanel::FramePanel(BRect rect, float maxw, QuasiStack *V, Qua *uq):
	ControlPanel(rect, maxw, "frame",
		( V->stackable == nullptr ||
			 V->stackable->sym->type != S_METHOD)? CP_TYPE_NOT:
			((Method*)V->stackable)->isFixed? CP_TYPE_NOT:
			((Method*)V->stackable)->isHeld? CP_TYPE_HELD: CP_TYPE_ONOFF,
		"Bang",
		((char *)(V->stackable == nullptr? " ":
			(V->callingBlock && V->callingBlock->crap.call.label)?
				V->callingBlock->crap.call.label:
				V->stackable->sym->name)),
		V->locusStatus)
{
	if (V->stackable) {
		AddGroupPanel(V->stackable->sym);
	}
	AddControlVariables(V->controlVariables, uq, V);
	AddHigherFrame(V->higherFrame, uq);
	ArrangeChildren();
	frame = V;
	V->controlPanel = this;
}

FramePanel::~FramePanel()
{
	frame->controlPanel = nullptr;
}


void
FramePanel::MessageReceived(BMessage *msg)
{
	switch (msg->what) {
	case CTRL_BANG: {
		if (bBangType == CP_TYPE_ONOFF) {
			MyCheckBox	*n = (MyCheckBox *)bBangButton;
			if (n->Value()) {
//				fprintf(stderr, "bang...\n");
				frame->Trigger();
			}
			else {
				frame->UnTrigger();
			}
		}
		else {
			frame->Trigger();
		}
		break;
	}
	
//	case CTRL_STOP: {
//		frame->UnTrigger();
//		break;
//	}
//	
	case DISPLAY_STATUS: {
		DisplayStatus(frame->locusStatus);
		break;
	}
	
	case UPDATE_CONTROL_VAR: {
		ControlVariable *p;
		status_t err = msg->FindPointer("control var", (void **)&p);
		if (p) {
			p->SetDisplayValue(msg);
		}
		break;
	}
	
	case PARAM_SET: {
		ControlVariable *p;
		status_t err = msg->FindPointer("control var", (void **)&p);
		if (p != nullptr) {
			p->SetSymbolValue();
		}
		break;
	}
	
	default:
		Panel::MessageReceived(msg);
	}
}

void
FramePanel::Draw(BRect bnd)
{
	BRect	b(1,1,myWidth-1, myHeight-1);
	RaisedBox(this, b, ViewColor(), false);
}


void
FramePanel::AttachedToWindow()
{
	ControlPanel::AttachedToWindow();

}

GroupPanel::GroupPanel(BRect rect, float maxw, StabEnt *sym):
	ControlPanel(rect, maxw, "group",
		CP_TYPE_NOT,
		"",
		sym->name,
		0)
{
	groupSym = sym;
	AddGroupPanel(sym);
}


void
GroupPanel::Draw(BRect bnd)
{
	BRect	b(1,1,myWidth-1, myHeight-1);
	RaisedBox(this, b, ViewColor(), false);
}

void
GroupPanel::ArrangeChildren()
{
	BRect	wrect; 
	if (Window()) Window()->Lock();

	wrect = vName->Frame();
	myHeight = wrect.bottom+MIXER_MARGIN;
	myWidth = wrect.right+MIXER_MARGIN;
	rowHeight = MIXER_MARGIN;
	thePoint.Set(myWidth,MIXER_MARGIN);

	for (short i=0; i<CountChildren(); i++) {
		BView	*V = ChildAt(i);
		if		(strcmp(V->Name(), "frame") == 0 ||
				 strcmp(V->Name(), "instance") == 0 ||
				 strcmp(V->Name(), "schedulable") == 0 ||
				 strcmp(V->Name(), "panel") == 0) {
			Panel	*panel = (Panel *)V;
			GetItemRect(MIXER_MARGIN, panel->myWidth, panel->myHeight, 5, wrect);
			panel->MoveTo(wrect.left,wrect.top);
		} else if	(strcmp(V->Name(), "group") == 0) {
			GroupPanel	*panel = (GroupPanel *)V;
			panel->ArrangeChildren();
			GetItemRect(MIXER_MARGIN, panel->myWidth, panel->myHeight, 5, wrect);
			panel->MoveTo(wrect.left,wrect.top);
		} else if	(strcmp(V->Name(), "name") == 0) {
		} else {
			BRect	wect = V->Bounds();	
			GetItemRect(MIXER_MARGIN, wect.right, wect.bottom, 5, wrect);
			V->MoveTo(wrect.left,wrect.top);
		}
	}
	ResizeTo(myWidth, myHeight);
	if (Window()) Window()->Unlock();
}

