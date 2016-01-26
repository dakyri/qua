
#ifdef __INTEL__ 
#include <SupportKit.h>
#include <StorageKit.h>
#include <InterfaceKit.h>
#endif

#include "SchedulablePanel.h"
#include "InstancePanel.h"
#include "Executable.h"
#include "messid.h"
#include "ControlVariable.h"
#include "Instance.h"
#include "NameCtrl.h"
#include "Colors.h"
#include "MixerWindow.h"
#include "SelectorPanel.h"
#include "Schedulable.h"


SchedulablePanel::SchedulablePanel(BRect rect, Schedulable *V, bool multi):
	ControlPanel(rect, rect.right - rect.left, "schedulable",
		multi? CP_TYPE_NORM:CP_TYPE_NOT,
		"Activate",
		((char *)(multi?V->sym->UniqueName():"")),
		V->status)
{
	multiInstance = multi;
	
	schedulable = V;
	V->controlPanel = this;
	BMessage	*msg = new BMessage(DISPLAY_INSTANCE);

	selector = new SelectorPanel(BRect(thePoint.x,
										MIXER_MARGIN,
										myWidth - MIXER_MARGIN,
										26),
										msg);
	selector->SetFontSize(12);
	selector->SetFont(be_bold_font);
	selector->SetViewColor(ViewColor());

	if (multiInstance) {
		AddChild(selector);
	}

//	myHeight = r.bottom + MIXER_MARGIN;
//	rowHeight = 26-MIXER_MARGIN;
//	thePoint.Set(myWidth,MIXER_MARGIN);

	short i;
//	myHeight = rect.bottom - rect.top;
//	myWidth = rect.right - rect.left;
	for (i=0; i<V->instances.CountItems(); i++) {
		Instance	*n = (Instance *)V->instances.ItemAt(i);
		AddInstance(n);
	}
	ArrangeChildren();
	fprintf(stderr, "%g %d\n", myHeight, V->instances.CountItems());
}


void
SchedulablePanel::FrameResized(float x, float y)
{
//	fprintf(stderr,"sc resized %g %g\n", y, x);
	
	MixerView *v;
	BRect	bnd = Frame();
	if ((v=((MixerView*)(Parent()->Parent()))) != nullptr) {
		v->ArrangeChildren();
	}

	myHeight = y;
	myWidth = x;
	
	Invalidate();
}

void
SchedulablePanel::ArrangeChildren()
{
	float	h = myHeight;
	float	w = myWidth;
	
	
	BRect	wrect;
	if (multiInstance) {
		wrect = selector->Frame();
	} else {
		if (CountChildren() == 0) {
			wrect = selector->Frame();
		} else {
			wrect.Set(0,0,0,0);
		}
	}
	myHeight = wrect.bottom + MIXER_MARGIN;
	myWidth = wrect.right + MIXER_MARGIN;
	rowHeight = MIXER_MARGIN;
	thePoint.Set(myWidth,MIXER_MARGIN);
	for (short i=0; i<CountChildren(); i++) {
		BView	*V = ChildAt(i);
		if	(strcmp(V->Name(), "panel") == 0 
		  || strcmp(V->Name(), "instance") == 0) {
			Panel	*p = (Panel *)V;
			GetItemRect(MIXER_MARGIN, p->myWidth, p->myHeight, 5, wrect);
			p->MoveTo(wrect.left,wrect.top);
		}
	}
	float nh = myHeight;
	float nw = myWidth;
//	myHeight = h;
//	myWidth = w;
	ResizeTo(nw, nh);
}

void
SchedulablePanel::Draw(BRect bnd)
{
	BRect	b(1,1,myWidth-1, myHeight-1);
//	fprintf(stderr, "draw %g %g\n", myWidth, myHeight);
	RaisedBox(this, b, ViewColor(), false);
	if (multiInstance)
		LoweredBox(this, selector->Frame(), ViewColor(), false);
}



void
SchedulablePanel::AddInstance(Instance *inst)
{
	int		i = selector->stuff.CountItems();
	selector->AddStuff(inst, inst->sym->defineCount, 0);
	if (multiInstance) {
	} else {
		if (i > 0) {
			reportError("Didn't expect multiple instances");
		}
		ZoomInstance(inst);
	}
}

void
SchedulablePanel::DeleteInstance(Instance *i)
{
	UnzoomInstance(i);
	selector->DeleteStuff(i);
	
//	ControlPanel	*NV = i->controlPanel;
//
//	if (Window()) Window()->Lock();
//	
//	if (i == selectedInstance) {
//		short	j;
//		Instance *ns = nullptr;
//		for (j=0; j<schedulable->instances.CountItems(); j++) {
//			Instance	*k = (Instance *)schedulable->instances.ItemAt(j);
//			if (k != i) {
//				ns = k;
//				break;
//			}
//		}
//		if (ns) {
//			Select(ns);
//		} else {
//			selectedInstance = nullptr;
//		}
//	}
//
//	if (NV) {
//		BRect			wrect = NV->Frame();
//		float			len = wrect.bottom - wrect.top + 3;
//		RemoveChild(NV);
//		delete NV;
//		for (int i=0; i<CountChildren(); i++) {
//			BView *p = ChildAt(i);
//			BRect	r=p->Frame();
//			if (r.top > wrect.top) {
//				p->MoveBy(0,-len);
//			}
//		}
//		ResizeBy(0,-len);
////		delete i->controlPanel;
//	}
//	if (Window()) Window()->Unlock();
}


void
SchedulablePanel::AddFramePanel(QuasiStack *n)
{
	BRect	wrect(0,0,2*MIXER_MARGIN,2*MIXER_MARGIN);
	ControlPanel	*NV = new FramePanel(
									wrect, myWidth-2*MIXER_MARGIN,
									n,
									schedulable->uberQua);

	BRect	nvr = NV->Bounds();
	GetItemRect(MIXER_MARGIN,
			wrect.right - wrect.left,
			wrect.bottom - wrect.top, MIXER_MARGIN,
			wrect);
	NV->MoveTo(wrect.left, wrect.top);
	AddChild(NV);
}

void
SchedulablePanel::AttachedToWindow()
{
	ControlPanel::AttachedToWindow();
	selector->SetTarget(this);
}


void
SchedulablePanel::MessageReceived(BMessage *msg)
{
	switch (msg->what) {
	case DISPLAY_INSTANCE: {
		Stacker	*s = nullptr;
		msg->FindPointer("stacker", (void **)&s);
		if (s) {
			int mode = msg->FindInt32("mode");
			Instance		*C = (Instance *)s;
			if (mode)
				ZoomInstance(C);
			else
				UnzoomInstance(C);
		}
		break;
	}
	case CTRL_BANG: {
		schedulable->Trigger();
		break;
	}
//	case CTRL_STOP: {
//		schedulable->UnTrigger();
//		break;
//	}
	case UPDATE_CONTROL_VAR: {
		ControlVariable *p;
		status_t err = msg->FindPointer("control var", (void **)&p);
		p->SetDisplayValue(msg);
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
	
	case SET_NAME: {
		char	*nm;
		msg->FindString("name", (const char **)&nm);
		schedulable->SetName(nm);
		break;
	}
		
	default: {
		Panel::MessageReceived(msg);
		break;
	}}
}



SchedulablePanel::~SchedulablePanel()
{
}

void
SchedulablePanel::ZoomInstance(Instance *i)
{
	if (i->controlPanel == nullptr) {
		BRect			wrect(0,0,myWidth-2*MIXER_MARGIN, 2*MIXER_MARGIN);
		ControlPanel	*NV = new InstancePanel(wrect, i, !multiInstance);

		if (Window()) Window()->Lock();
		AddChild(NV);
		ArrangeChildren();
		if (Window()) Window()->Unlock();
	}
}

void	
SchedulablePanel::UnzoomInstance(Instance *i)
{
	if (Window()) Window()->Lock();
	if (i->controlPanel) {
		RemoveChild(i->controlPanel);
		delete i->controlPanel;
		i->controlPanel = nullptr;
	}
	ArrangeChildren();
	if (Window()) Window()->Unlock();
}

void
SchedulablePanel::Select(Instance *i)
{
	Window()->Lock();

	if (selectedInstance == nullptr) {
	} else {
		selectedInstance->controlPanel->SetViewColor(mdGray);
		selectedInstance->controlPanel->Invalidate();
	}
	selectedInstance = i;
	selectedInstance->controlPanel->SetViewColor(orange);
	selectedInstance->controlPanel->Invalidate();
	Window()->Unlock();
}
