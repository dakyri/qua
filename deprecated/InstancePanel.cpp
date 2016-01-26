
#ifdef __INTEL__ 
#include <SupportKit.h>
#include <StorageKit.h>
#include <InterfaceKit.h>
#endif

#include "Instance.h"
#include "InstancePanel.h"
#include "ControlVariable.h"
#include "Qua.h"
#include "messid.h"
#include "QuasiStack.h"
#include "SequencerWindow.h"
#include "ArrangerObject.h"
#include "Method.h"
#include "Block.h"
#include "KeyVal.h"
#include "NumCtrl.h"
#include "Colors.h"
#include "MyCheckBox.h"
#include "SchedulablePanel.h"
#include "Schedulable.h"

InstancePanel::InstancePanel(BRect rect, Instance *I, bool is):
	ControlPanel(rect, rect.right - rect.left, "instance",
				is?CP_TYPE_NOT:CP_TYPE_ONOFF,
				"", "", I->status)
{
	isSingular = is;
	
	if (!isSingular) {
		AddControlVariables(I->controlVariables,I->schedulable->uberQua,nullptr);
	} else {
		myHeight = MIXER_MARGIN;
	}
	
	instance = I;
	I->controlPanel = this;

	float	len = myHeight;
	float	wid = myWidth;

	if (I->mainStack && (I->mainStack->IsInteresting())) {
		AddFramePanel(I->mainStack, len, wid);
	}
	if (I->wakeStack && (I->wakeStack->IsInteresting())) {
		AddFramePanel(I->wakeStack, len, wid);
	}
	if (I->sleepStack && (I->sleepStack->IsInteresting())) {
		AddFramePanel(I->sleepStack, len, wid);
	}
	if (I->rxStack && (I->rxStack->IsInteresting())) {
		AddFramePanel(I->rxStack, len, wid);
	}
	if (I->startStack && (I->startStack->IsInteresting())) {
		AddFramePanel(I->startStack, len, wid);
	}
	if (I->stopStack && (I->stopStack->IsInteresting())) {
		AddFramePanel(I->stopStack, len, wid);
	}
	if (I->recordStack && (I->recordStack->IsInteresting())) {
		AddFramePanel(I->recordStack, len, wid);
	}
	
	myHeight = len;
	myWidth = wid;
}


void
InstancePanel::MouseDown(BPoint pt)
{
//	((SchedulablePanel*)Window())->Select(instance);
}


void
InstancePanel::MessageReceived(BMessage *msg)
{
	switch(msg->what) {
	case CTRL_BANG:
//		instance->Trigger();
		break;
		
//	case CTRL_STOP:
////		instance->UnTrigger();
//		break;
//		
	case SET_GRANULARITY:
		break;
		
	case SET_OFFSET:
		break;
		
	case UPDATE_CONTROL_VAR: {
		ControlVariable *p;
		status_t err = msg->FindPointer("control var", (void **)&p);
		p->SetDisplayValue(msg);
		break;
	}
	
	case PARAM_SET: {
		ControlVariable *p=nullptr;
		status_t err = msg->FindPointer("control var", (void **)&p);
		if (p != nullptr) {
			p->SetSymbolValue();
		}
		break;
	}
	default:
		Panel::MessageReceived(msg);
	};
}


void
InstancePanel::AttachedToWindow()
{
	ControlPanel::AttachedToWindow();

}


InstancePanel::~InstancePanel()
{
}


void
InstancePanel::AddFramePanel(QuasiStack *n, float &len, float &wid)
{
	BRect	wrect(0,0,myWidth-2*MIXER_MARGIN,2*MIXER_MARGIN);
	ControlPanel	*NV = new FramePanel(
									wrect, myWidth-2*MIXER_MARGIN,
									n,
									instance->schedulable->uberQua);

	BRect	nvr = NV->Bounds();
	NV->MoveTo(MIXER_MARGIN, len+1);
	AddChild(NV);
	len += (nvr.bottom - nvr.top)+ 4;
	wid = Max(wid, nvr.right - nvr.left);
	ResizeTo(wid,len);
}


void
InstancePanel::Draw(BRect bnd)
{
	BRect	b(1,1,myWidth-1, myHeight-1);
	RaisedBox(this, b, ViewColor(), false);
}


