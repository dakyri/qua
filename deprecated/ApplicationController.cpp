#include "ApplicationController.h"
#include "Sym.h"
#include "KeyVal.h"
#include "Application.h"
#include "Colors.h"
#include "messid.h"
#include "PoolSelect.h"
#include "Source.h"
#include "QuaQua.h"
#include "StdDefs.h"
#include "Block.h"
#include "FilterBlockCtl.h"
#include "VoiceController.h"
#include "GlblMessid.h"
#include "QuasiStack.h"
#include "NameCtrl.h"
#include "ControlVar.h"

ApplicationView::ApplicationView(BRect rect, Application *A):
	ControlPanel(rect, A->mainStack->formalParams,
				TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, CP_TYPE_NORM, 5,
				"Activate", A->Name, A->Status, A->uberQua,
				A->mainStack, nullptr, nullptr, nullptr)
{
	myApplication = A;
}

void
ApplicationView::MouseDown(BPoint pt)
{
}

void
ApplicationView::MessageReceived(BMessage *msg)
{
	switch(msg->what) {
	case CTRL_BANG:
		myApplication->Trigger();
		break;
		
	case CTRL_STOP:
		myApplication->UnTrigger();
		break;
		
	case SET_NAME: {
		char	*nm;
		msg->FindString("name", &nm);
		myApplication->SetName(nm);
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
	};
}

void
ApplicationView::AttachedToWindow()
{
	ControlPanel::AttachedToWindow();

}



ApplicationView::~ApplicationView()
{
	myApplication->controlPanel = nullptr;
}


ApplicationController::ApplicationController(BRect inRect, Application *A)
	: Controller(inRect, A->Sym->UniqueName())
{
	BRect		wrect(0,0,400,35);
	short		cnt=0;
	float		len=0,
				wid=400;

	BRect		Box = Bounds();
	edit = new ApplicationView(wrect, A);
	Lock();
	wrect = edit->Bounds();
	AddControlPanel(edit);
	A->controlPanel = edit;
	ResizeTo(wrect.right, wrect.bottom);
	edit->MoveTo(0,0);
	Unlock();

	
	wrect.Set(0,0,wid,25);
	
//	FilterBlockCtlView		*WV = new FilterBlockCtlView("wake", A->wakeBlock, A->uberQua, A->WakeLocus->subLocus, wrect);
//	AddControlPanel(WV);
//	FilterBlockCtlView		*SV = new FilterBlockCtlView("sleep", A->sleepBlock, A->uberQua, A->SleepLocus->subLocus, wrect);
//	AddControlPanel(SV);
//	FilterBlockCtlView		*MV = new FilterBlockCtlView("main", A->mainBlock, A->uberQua, A->MainLocus->subLocus, wrect);
//	AddControlPanel(MV);
//	FilterBlockCtlView		*IV = new FilterBlockCtlView("init", A->initBlock, A->uberQua, A->InitLocus->subLocus, wrect);
//	AddControlPanel(IV);
//	FilterBlockCtlView		*STV = new FilterBlockCtlView("start", A->seqStartBlock, A->uberQua,A->SeqStartLocus->subLocus,  wrect);
//	AddControlPanel(STV);
//	FilterBlockCtlView		*DV = new FilterBlockCtlView("stop", A->seqStopBlock, A->uberQua,A->SeqStopLocus->subLocus,  wrect);
//	AddControlPanel(DV);
//	FilterBlockCtlView		*RV = new FilterBlockCtlView("record", A->seqRecBlock, A->uberQua,A->SeqRecLocus->subLocus,  wrect);
//	AddControlPanel(RV);
//

//	if (A->Locuss) {
//		for (short i=0; i<A->Locuss->Count; i++) {
//			wrect.Set(0,0,400,25);
//			cnt++;fprintf(stderr, "%x\n", A->Locuss->List[i]);
//			LocusCtlView *NV = new LocusCtlView(A->Locuss->List[i], wrect);
//			AddControlPanel(NV);
//		}
//	}


	A->controlWindow = this;
	myApplication = A;

	Show();
}

ApplicationController::~ApplicationController()
{
	myApplication->controlWindow = nullptr;
}

void
ApplicationController::MessageReceived(BMessage *msg)
{
	BWindow::MessageReceived(msg);
}
	
bool
ApplicationController::QuitRequested()
{

	return TRUE;
}
