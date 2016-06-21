#include "VoiceController.h"
#include "Colors.h"
#include "Voice.h"
#include "QuaQua.h"
#include "Block.h"
#include "Source.h"
#include "messid.h"
#include "StdDefs.h"
#include "GlblMessid.h"
#include "NameCtrl.h"
#include "QuasiStack.h"
#include "ControlVar.h"

VoiceController::VoiceController(Voice *V, BRect rect)
	: Controller( rect, V->Sym->UniqueName() )
{
	BRect		wrect(0,0,400,35);
	VoiceCtlView	*VV;
	LocusCtlView	*NV;
	short		cnt=0;
	float		len=0,
				wid=400;

	fprintf(stderr, "about to do ctrl %x %x\n", V, V->mainStack);
	ResizeTo(0,400);
	VV = new VoiceCtlView(V, wrect);
	fprintf(stderr, "done vcv\n");
	Lock();
	wrect = VV->Bounds();
	AddControlPanel(VV);
	ResizeTo(wrect.right, wrect.bottom);
	VV->MoveTo(0,0);
	Unlock();
	
	V->controlPanel = VV;
	V->controlWindow = this;
	myVoice = V;

	Show();
}

void
VoiceController::MessageReceived(BMessage *msg)
{
	BWindow::MessageReceived(msg);
}

VoiceController::~VoiceController()
{
	myVoice->controlPanel = nullptr;
	myVoice->controlWindow = nullptr;
}

bool VoiceController::QuitRequested( )
{
	return TRUE;
}


VoiceCtlView::VoiceCtlView(Voice *V, BRect rect):
	ControlPanel(rect, V->mainStack->formalParams,
		TRUE, TRUE, TRUE, TRUE, TRUE, FALSE,
		CP_TYPE_NORM, 5,
		"Activate", V->Name, V->Status, V->uberQua, V->mainStack,
		nullptr, nullptr, nullptr)
{
	BRect wrect = Bounds();
	myVoice = V;
	V->mainStack->controlPanel = this;
	
	float	pos = wrect.bottom - wrect.top;
	float	wid = wrect.right - wrect.left;
	
	for (short i=0; i<V->mainStack->higherFrame.CountItems(); i++) {
		QuasiStack	*n = (QuasiStack *)V->mainStack->higherFrame.ItemAt(i);
		
		ControlPanel	*NV = new LocusCtlView(n, wrect);

		BRect	nvr = NV->Bounds();
		NV->MoveTo(0, len);
		BackView->AddChild(p);
	//	AddChild(p);
		len += (wrect.bottom - wrect.top);
		wid = Max(wid, wrect.right - wrect.left);
		ResizeTo(wid,len);
		BackView->MoveTo(0,0);
	}

}

void
VoiceCtlView::MessageReceived(BMessage *msg)
{
	switch (msg->what) {
	case CTRL_BANG: {
		myVoice->Trigger();
		break;
	}
	case CTRL_STOP: {
		myVoice->UnTrigger();
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
	
	case SET_NAME: {
		char	*nm;
		msg->FindString("name", &nm);
		myVoice->SetName(nm);
		break;
	}
		
	default: {
		BView::MessageReceived(msg);
		break;
	}}
}



VoiceCtlView::~VoiceCtlView()
{
}
