
#ifdef __INTEL__ 
#include <SupportKit.h>
#include <StorageKit.h>
#include <InterfaceKit.h>
#endif

#include "DataEditor.h"
#include "Colors.h"
#include "StdDefs.h"
#include "QuaTypes.h"
#include "ToggleButton.h"
#include "MyCheckBox.h"
#include "TimeCtrl.h"
#include "NumCtrl.h"
#include "Stacker.h"
#include "TimeKeeper.h"
#include "Metric.h"
#include "Schedulable.h"
#include "Method.h"
#include "Qua.h"
#include "SymEditBlockView.h"
#include "InstancePanel.h"
#include "Block.h"
#include "BlockDrawBits.h"

DataEditor::DataEditor(BRect rec, float maxw, Schedulable *sched, Metric *m, long length):
	Panel(rec, maxw, "realdata"),
	Stacker(sched->uberQua),
	TimeKeeper(m)
{
	schedulable = sched;
	offlineFilter = new Method("Filter", sched->sym, sched->uberQua,
			false, false, false, false, false, false);
	filterStack = new QuasiStack(offlineFilter->sym,
								 this,
								 nullptr,
								 nullptr,
								 sched->uberQua->theStack,
								 sched->uberQua, nullptr);
// ?????? no stackable: dummy stack...
//	if (!offlineFilter->StackOMatic(filterStack, 3))
//		reportError("can't build stack for channel");

	short swid = StringWidth("Preview");
	BMessage	*m1 = new BMessage(PREVIEW_REGION);
	BMessage	*m2 = new BMessage(PREVIEW_REGION);
	m1->AddInt32("state", STATUS_RUNNING);
	m2->AddInt32("state", STATUS_SLEEPING);

	BRect wrect(MIXER_MARGIN, MIXER_MARGIN, MIXER_MARGIN+swid+5, MIXER_MARGIN+20);
	previewButton = new ToggleButton( wrect, "dross",
		"Preview",
		"Stop",
		 m1,
		 m2,
		 STATUS_SLEEPING,
		 STATUS_RUNNING,
		 (ulong)B_FOLLOW_TOP|B_FOLLOW_LEFT, (ulong)B_WILL_DRAW );
	previewButton->SetFont(be_bold_font);
	previewButton->SetFontSize( 12 );
	AddChild(previewButton);
	BRect r = previewButton->Frame();

	swid = StringWidth("Loop");
	wrect.Set(r.right+MIXER_MARGIN, MIXER_MARGIN, r.right+MIXER_MARGIN+swid+15, MIXER_MARGIN+20);
	loopButton = new MyCheckBox(wrect, "dross", "Loop",
						new BMessage(PREVIEW_LOOP),
						(ulong)B_FOLLOW_TOP|B_FOLLOW_LEFT, (ulong)B_WILL_DRAW);
	loopButton->SetFont(be_bold_font);
	loopButton->SetFontSize( 12 );
	AddChild(loopButton);
	BRect s = loopButton->Frame();
	
	wrect.Set(s.right+MIXER_MARGIN, MIXER_MARGIN+2, s.right+MIXER_MARGIN+115, MIXER_MARGIN+18);
	durView = new TimeCtrl(wrect, "dross", "Duration",
						new BMessage(SAMPLE_DURATION),
						metric, DRAW_VER_LBL,
						(ulong)B_FOLLOW_TOP|B_FOLLOW_LEFT, (ulong)B_WILL_DRAW);
	durView->SetValue(length);
	durView->SetBounds(0,length,length);
	AddChild(durView);
	BRect df = durView->Frame();

	swid = durView->StringWidth("Selected");	
	wrect.Set(df.right+5, MIXER_MARGIN+2, df.right+5+swid+80, MIXER_MARGIN+18);
	lSelView = new TimeCtrl(wrect, "dross", "Selected",
						new BMessage(REGION_SELECT),
						metric, DRAW_VER_LBL,
						(ulong)B_FOLLOW_TOP|B_FOLLOW_LEFT, (ulong)B_WILL_DRAW);
	lSelView->SetValue(length);
	lSelView->SetBounds(0,0,length);
	AddChild(lSelView);
	BRect lf = lSelView->Frame();
	
	wrect.Set(lf.right, MIXER_MARGIN+2, lf.right+83, MIXER_MARGIN+18);
	rSelView = new TimeCtrl(wrect, "dross", "-",
						new BMessage(REGION_SELECT),
						m, DRAW_VER_LBL,
						(ulong)B_FOLLOW_TOP|B_FOLLOW_LEFT, (ulong)B_WILL_DRAW);
	rSelView->SetValue(length);
	rSelView->SetBounds(0,length,length);
	rSelView->SetDivider(7);
	AddChild(rSelView);
	BRect rf = rSelView->Frame();
	
	wrect.Set(MIXER_MARGIN, r.bottom+MIXER_MARGIN,
			 40, r.bottom+MIXER_MARGIN+18);
	editBar = new BMenuBar(wrect, "edit",
						(ulong)B_FOLLOW_TOP|B_FOLLOW_LEFT, B_ITEMS_IN_ROW, false);
	AddChild(editBar);
	BRect ebf = editBar->Frame();

	editMenu = new BMenu("Edit");
	BMenuItem	*it;
	it = new BMenuItem("Cut", new BMessage(CUT_REGION));		editMenu->AddItem(it);
	it = new BMenuItem("Copy", new BMessage(COPY_REGION));   	editMenu->AddItem(it);
	it = new BMenuItem("Paste", new BMessage(PASTE_REGION));	editMenu->AddItem(it);
	it = new BMenuItem("Overwrite", new BMessage(OVERWRITE_REGION));	editMenu->AddItem(it);
	it = new BMenuItem("Clear", new BMessage(CLEAR_REGION));	editMenu->AddItem(it);
	it = new BMenuItem("Delete", new BMessage(DELETE_REGION));	editMenu->AddItem(it);
	it = new BMenuItem("Crop", new BMessage(CROP_REGION));	editMenu->AddItem(it);
	it = new BSeparatorItem();	editMenu->AddItem(it);
	it = new BMenuItem("Render", new BMessage(RENDER_REGION));	editMenu->AddItem(it);
	it = new BMenuItem("Halt Render", new BMessage(HALT_RENDER));	editMenu->AddItem(it);
	it = new BSeparatorItem();	editMenu->AddItem(it);
	it = new BMenuItem("Select All", new BMessage(SELECT_ALL));	editMenu->AddItem(it);
	editBar->AddItem(editMenu);
	
	rowHeight = s.bottom+MIXER_MARGIN;

	filterEdit = new SymEditBlockView(BRect(0,0,20,20), sched->uberQua);

//	myBlock->SetValue(E->mainBlock);
//	myBlock->ResizeToFit();
	AddChild(filterEdit);
	filterEdit->MoveTo(ebf.right+MIXER_MARGIN, r.bottom+MIXER_MARGIN);
	BRect	t = filterEdit->Frame();
	filterEdit->SetResizingMode(B_FOLLOW_TOP|B_FOLLOW_LEFT);

	wrect.Set(0,0,20,20);
	filterControls = new FramePanel(
									wrect, myWidth-2*MIXER_MARGIN-t.right,
									filterStack,
									sched->uberQua);

	BRect	nvr = filterControls->Frame();
	filterControls->MoveTo(t.right + MIXER_MARGIN, t.top);
	filterControls->SetResizingMode(B_FOLLOW_TOP|B_FOLLOW_LEFT);
	AddChild(filterControls);

}

DataEditor::~DataEditor()
{
	glob.DeleteSymbol(offlineFilter->sym);
}

bool
DataEditor::QuitRequested()
{
	return true;
}

void
DataEditor::Draw(BRect r)
{
	BRect	b(1,1,myWidth-1, myHeight-1);
	RaisedBox(this, b, ViewColor(), false);
}


void
DataEditor::MessageReceived(BMessage *inMsg)
{
	switch (inMsg->what) {

	case COMPILE_BLOCK: {
		Block		*b = filterEdit->BlockValue(schedulable->sym);
		filterControls->RemoveHigherFrames();
		
		schlock.Lock();
		int cnt = filterStack->higherFrame.CountItems();
		for (short j=0; j<cnt; j++) {
			QuasiStack	*q = (QuasiStack *)filterStack->higherFrame.RemoveItem((int32)0);
			delete q;
		}
		if (b)
			b->StackOMatic(filterStack,1);
		Block	*oldBlock = offlineFilter->mainBlock;
		offlineFilter->mainBlock = b;
		
		schlock.Unlock();
		if (oldBlock)
			oldBlock->DeleteAll();
		
		BRect po =
			filterControls->bBangButton? filterControls->bBangButton->Frame():
			filterControls->vName? filterControls->vName->Frame():
			BRect(0,0,0,0);
		filterControls->thePoint.Set(po.right+MIXER_MARGIN, MIXER_MARGIN);
		filterControls->myHeight = po.bottom+MIXER_MARGIN;
		filterControls->myWidth = po.right+MIXER_MARGIN;
		filterControls->rowHeight = MIXER_MARGIN;
		filterControls->AddHigherFrame(filterStack->higherFrame, schedulable->uberQua);
		filterControls->ArrangeChildren();
		filterControls->Invalidate();
		break;
	}

	default:
		Panel::MessageReceived(inMsg);

	}
}

void
DataEditor::MouseDown(BPoint p)
{
}

void
DataEditor::ArrangeChildren()
{
	origH = myHeight;
	origW = myWidth;
}

void
DataEditor::ReScale(float x, float y)
{
	origH = y;
	origW = x;
	maxWidth = x;
	ResizeTo(x,y);
	Invalidate();
}

void
DataEditor::FrameResized(float x, float y)
{
	myHeight = y;
	myWidth = x;
	BView *v;
	if ((v=Parent()) == nullptr) {
	} else if (strcmp(v->Name(),"back") == 0) {
		if (myHeight == origH) {
		} else {
//			Window()->ResizeBy(0/*myWidth-origW*/, myHeight-origH);
		}
	}
	Invalidate();
}
