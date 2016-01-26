#include "PoolController.h"
#include "Sym.h"
#include "Pool.h"
#include "Colors.h"
#include "messid.h"
#include "KeyVal.h"
#include "NumCtrl.h"
#include "Source.h"
#include "QuaQua.h"
#include "StdDefs.h"
#include "FilterBlockCtl.h"
#include "Block.h"
#include "GlblMessid.h"
#include "NameCtrl.h"
#include "ControlVar.h"

PoolControlView::PoolControlView(Pool *A, BRect rect):
	ControlPanel(rect, A->controlVars,
				TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, CP_TYPE_NORM, 5,
				"Activate", A->Name, A->Status, A->uberQua, A->mainStack, nullptr, nullptr, nullptr)
{
	myPool = A;
}

void
PoolControlView::MouseDown(BPoint pt)
{
}

void
PoolControlView::MessageReceived(BMessage *msg)
{
	switch(msg->what) {
	case CTRL_BANG: {
		break;
	}
		
	case CTRL_STOP:
		break;
		
	case SET_GRANULARITY:
		break;
		
	case SET_OFFSET:
		break;
		
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
		myPool->SetName(nm);
	}
		
	default:
		BView::MessageReceived(msg);
	};
}

void
PoolControlView::AttachedToWindow()
{
	ControlPanel::AttachedToWindow();

}



PoolControlView::~PoolControlView()
{
}


PoolController::PoolController(Pool *A, BRect inRect)
	: Controller(inRect, A->Name)
{
	BRect		wrect(0,0,400,35);
	short		cnt=0;
	float		len=0,
				wid=400;
	BRect		Box = Bounds();

	myPool = A;
	edit = new PoolControlView(A, wrect);
	Lock();
	wrect = edit->Bounds();
	AddControlPanel(edit);
	A->controlPanel = edit;
	A->controlWindow = this;
	ResizeTo(wrect.right, wrect.bottom);
	edit->MoveTo(0,0);
	Unlock();

	FilterBlockCtlView		*WV = new FilterBlockCtlView(
						"offline",
						A->Sym,
						A->olEditor,
						A->uberQua,
						wrect,
						&A->olLock,
						FC_MODE_CLEAR,
						&A->Stream,
						A->mainStack);
	AddControlPanel(WV);

	Show();
}

PoolController::~PoolController()
{
	myPool->controlWindow = nullptr;
}

void
PoolController::MessageReceived(BMessage *msg)
{
	switch(msg->what) {
	default:
		BWindow::MessageReceived(msg);
	};
}


void
PoolController::MenusBeginning()
{
}

bool
PoolController::QuitRequested()
{

	return TRUE;
}

