#include "SampleController.h"
#include "Sym.h"
#include "Sample.h"
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

SampleControlView::SampleControlView(Sample *A, BRect rect):
	ControlPanel(rect, A->controlVars,
				TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, CP_TYPE_NORM, 5,
				"Activate", A->Name, A->Status, A->uberQua,
				A->mainStack, nullptr, nullptr, nullptr)
{
	mySample = A;
}

void
SampleControlView::MouseDown(BPoint pt)
{
}

void
SampleControlView::MessageReceived(BMessage *msg)
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
	
	case B_SAVE_REQUESTED: {
		if (msg->HasString("name")) {	// do it!
			BDirectory	*the_dir = new BDirectory();
			BEntry		sampleFile(mySample->sampleFilePathName);
			char		saveName[MAX_QUA_NAME_LENGTH];
			status_t	err=sampleFile.InitCheck();
			
			if (err != B_NO_ERROR) {
				reportError("Doh! I've lost the sample file: %s", ErrorStr(err));
				break;
			}
			if (mySample->uberQua->savePanel) {
				delete mySample->uberQua->savePanel;
				mySample->uberQua->savePanel = nullptr;
			}
			strcpy(saveName, msg->FindString("name"));
 			msg->FindRef("directory", &mySample->uberQua->sampleSaveDir);
			if ((err=the_dir->SetTo(&mySample->uberQua->sampleSaveDir))
										!= B_NO_ERROR) {
				reportError("Save: Can't set directory: %s", ErrorStr(err));
				break;
			}
			if ((err=the_dir->SetTo(&mySample->uberQua->sampleSaveDir))
										!= B_NO_ERROR) {
				reportError("Save: Can't set directory: %s", ErrorStr(err));
				break;
			}
			if ((err=sampleFile.MoveTo(the_dir, saveName, true)) != B_NO_ERROR) {
				reportError("Save: can't move file: %s", ErrorStr(err));
				break;
			}
			BPath		filePath;
			if ((err=sampleFile.GetPath(&filePath)) != B_NO_ERROR) {
				reportError("Save: can't get path: %s", ErrorStr(err));
				break;
			}
			delete [] mySample->sampleFilePathName;
			mySample->sampleFilePathName = new char[strlen(filePath.Path())+1];
			strcpy(mySample->sampleFilePathName, filePath.Path());
		}
		break;
	}
		
	case SET_NAME: {
		char	*nm;
		msg->FindString("name", &nm);
		mySample->SetName(nm);
		
		if (mySample->uberQua->savePanel) {
			delete mySample->uberQua->savePanel;
			mySample->uberQua->savePanel = nullptr;
		}
	
		BPath	save_dir_path(mySample->sampleFilePathName);
		save_dir_path.GetParent(&save_dir_path);
		if (save_dir_path.InitCheck() == B_NO_ERROR) {
			entry_ref	dir_ref;
			status_t	err = get_ref_for_path(save_dir_path.Path(),
												&dir_ref);
			mySample->uberQua->savePanel = new BFilePanel(B_SAVE_PANEL,
							&RightHere,	&dir_ref, FALSE, FALSE);
						
			mySample->uberQua->savePanel->SetButtonLabel(B_CANCEL_BUTTON, "eat my toes");
			mySample->uberQua->savePanel->SetSaveText(nm);
			mySample->uberQua->savePanel->Window()->SetTitle("qua: save sample information");
			mySample->uberQua->savePanel->Refresh();
			mySample->uberQua->savePanel->Window()->Show();
		} else {
			reportError("Chaos strikes, in the form of a disappearing directory");
		}
		break;
	}
		
	default:
		BView::MessageReceived(msg);
	};
}

void
SampleControlView::AttachedToWindow()
{
	ControlPanel::AttachedToWindow();
	RightHere = BMessenger(this);
}



SampleControlView::~SampleControlView()
{
}


SampleController::SampleController(Sample *A, BRect inRect)
	: Controller(inRect, A->Name)
{
	BRect		wrect(0,0,400,35);
	short		cnt=0;
	float		len=0,
				wid=400;
	BRect		Box = Bounds();

	mySample = A;
	edit = new SampleControlView(A, wrect);
	Lock();
	wrect = edit->Bounds();
	AddControlPanel(edit);
	A->controlPanel = edit;
	A->controlWindow = this;
	ResizeTo(wrect.right, wrect.bottom);
	edit->MoveTo(0,0);
	Unlock();

//	FilterBlockCtlView		*WV = new FilterBlockCtlView(
//		"offline", A->Sym, A->olEditor, A->uberQua,
//			A->olEditorLocus->subLocus, wrect,
//			&A->olLock,
//			FC_MODE_CLEAR, &A->Stream, A->olEditorLocus);
//	AddControlPanel(WV);

	Show();
}

SampleController::~SampleController()
{
	mySample->controlWindow = nullptr;
}

void
SampleController::MessageReceived(BMessage *msg)
{
	switch(msg->what) {
	default:
		BWindow::MessageReceived(msg);
	};
}


void
SampleController::MenusBeginning()
{
}

bool
SampleController::QuitRequested()
{

	return TRUE;
}

