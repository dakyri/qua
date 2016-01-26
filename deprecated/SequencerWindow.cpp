
#ifdef __INTEL__ 
#include <SupportKit.h>
#include <StorageKit.h>
#include <InterfaceKit.h>
#include <stdlib.h>
#endif

#include <stdio.h>

#include "StdDefs.h"
#include "MyButton.h"
#include "Colors.h"
#include "KeyVal.h"
#include "NumCtrl.h"
#include "TiledView.h"

#include "include/GlblMessid.h"
#include "include/SequencerWindow.h"
#include "include/ArrangerObject.h"
#include "include/messid.h"
#include "include/Channel.h"
#include "include/Qua.h"
#include "TimeCtrl.h"
#include "include/AQuarium.h"
#include "include/QuaObject.h"
#include "include/Editor.h"
#include "include/Quapp.h"

#define MARGIN 10
#define INI_DISPLAY_CHAN	8

class SequencerBackView:public TiledView
{
public:
					SequencerBackView(BRect r, char *, uint32, uint32);
	virtual void	Pulse();
	virtual void	Draw(BRect);
};

SequencerBackView::SequencerBackView(BRect r, char *nm, uint32 rs, uint32 fl):
	TiledView(r,nm,rs,fl)
{
}

void
SequencerBackView::Draw(BRect r)
{
	TiledView::Draw(r);
	BView	*v;

// Block around controls.
	
	SequencerWindow	*w = (SequencerWindow *)Window();
	
	BPoint	p[6];
	p[0] = w->bGo->Frame().LeftTop()+BPoint(-2,-2);
	p[1] = w->dTempoCount->Frame().RightTop()+BPoint(2,-2);
	p[2] = w->dTempoCount->Frame().RightBottom()+BPoint(2,2);
	p[3] = BPoint(w->bRec->Frame().right+2,
						w->dTempoCount->Frame().bottom+2);
	p[4] = w->bRec->Frame().RightBottom()+BPoint(2,2);
	p[5] = w->bGo->Frame().LeftBottom()+BPoint(-2,2);
	SetHighColor(orange);
	FillPolygon(p, 6);
	SetHighColor(Darker(orange));
	SetLowColor(Lighter(orange));
	StrokeLine(p[0],p[1],B_SOLID_LOW);
	StrokeLine(p[1],p[2],B_SOLID_HIGH);
	StrokeLine(p[2],p[3],B_SOLID_HIGH);
	StrokeLine(p[3],p[4],B_SOLID_HIGH);
	StrokeLine(p[4],p[5],B_SOLID_HIGH);
	StrokeLine(p[5],p[0],B_SOLID_LOW);

// Boreders around major areas, and highlighting for focus view

	v = ((SequencerWindow *)Window())->aquarium;
	SetPenSize(2);
	if (Window()->CurrentFocus() == v) {
		LoweredBox(this, v->Frame(), green, false);
	} else {
		LoweredBox(this, v->Frame(), Darker(orange), false);
	}
	v = ((SequencerWindow *)Window())->channelView;
	if (Window()->CurrentFocus() == v) {
		LoweredBox(this, v->Frame(), green, false);
	} else {
		LoweredBox(this, v->Frame(), Darker(orange), false);
	}
	v = ((SequencerWindow *)Window())->arrange;
	BRect	fr = v->Frame();
	fr.top = ((SequencerWindow *)Window())->barView->Frame().top;
	if (Window()->CurrentFocus() == v) {
		LoweredBox(this, fr, green, false);
	} else {
		LoweredBox(this, fr, Darker(orange), false);
	}
}


void
SequencerBackView::Pulse()
{
	SequencerWindow	*sw = ((SequencerWindow *)Window());
	if (sw) {
		sw->SetTime(sw->uberQua->theTime);
		sw->uberQua->UpdateRecordDisplay();
	}
}

SequencerWindow::SequencerWindow(
						BRect inRect,
						float ovWidth, float cvWidth,
						const char* inTitle,
						ulong inFlags, Qua *qua)
	: BWindow( inRect, inTitle, B_TITLED_WINDOW, inFlags )
{
	fprintf(stderr, "Creating sequencer window %s...\n", inTitle);
	BRect		wrect = Bounds();
	menuBar = new BMenuBar(BRect(0,0,wrect.right,20), "mbar");
	BMenuItem *item;
	float		t;
	
	trackHeight = DFLT_TRACK_HEIGHT;
	
	uberQua = qua;

	mFileMenu = new BMenu("File");	
	mEditMenu = new BMenu("Edit");	
	mWinMenu = new BMenu("Window");
	
// FILE MENU
	menuBar->AddItem(mFileMenu);
	
	BMessage *msg = new BMessage(QUA_LOAD);
	item = new BMenuItem("Open", msg, 'O');
	mFileMenu->AddItem(item);
	item->SetTarget(be_app);
	
	item = new BMenuItem("New", new BMessage(QUA_NEW), 'N');
	mFileMenu->AddItem(item);
	item->SetTarget(be_app);
	
	msg = new BMessage(QUA_NEW);
	msg->AddInt32("load", 0);
	item = new BMenuItem("New from ...", msg, 'O');
	mFileMenu->AddItem(item);
	item->SetTarget(be_app);
	
	mFileMenu->AddSeparatorItem();

	mFileMenu->AddItem(new BMenuItem("Save", new BMessage(QUA_SAVE), 'S'));
	mFileMenu->AddItem(new BMenuItem("Save As", new BMessage(QUA_SAVE_AS)));
	mFileMenu->AddItem(new BMenuItem("Close", new BMessage(QUA_CLOSE), 'W'));
	
	mFileMenu->AddSeparatorItem();
	item = new BMenuItem("About Qua", new BMessage(B_ABOUT_REQUESTED));
	item->SetTarget(quapp);
	mFileMenu->AddItem(item);
	
	item = new BMenuItem("Quit Qua", new BMessage(B_QUIT_REQUESTED), 'Q');
	item->SetTarget(this);
	mFileMenu->AddItem(item);

// EDIT MENU
	menuBar->AddItem(mEditMenu);
	mEditMenu->AddItem(new BMenuItem("Edit", new BMessage(QUA_EDIT), 'E'));
	
// WINDOW MENU
	menuBar->AddItem(mWinMenu);
	
	msg = new BMessage(SET_WINDOW);
	msg->AddInt32("mixer", 1);
	item = new BMenuItem("Mixer", msg, 'M');
	item->SetTarget(be_app);
	item->SetMarked(TRUE);
	mWinMenu->AddItem(item);

	backView = new SequencerBackView(Bounds(),
							(char *)inTitle,
							B_FOLLOW_ALL,
							B_WILL_DRAW|B_PULSE_NEEDED);
	backView->SetViewColor(orange);
	
	backView->AddChild(menuBar);

	t = menuBar->Bounds().bottom;
	
	aquarium = new AQuarium(
						BRect(
							MARGIN, t + 30 + LABEL_HEIGHT,
							MARGIN+ovWidth, wrect.bottom -(MARGIN+B_H_SCROLL_BAR_HEIGHT)),
						"objview", qua);
						
	BRect	ovr = aquarium->Frame();
	
	channelView = new ChannelView(
						BRect(
							ovr.right+1, t + 30 + LABEL_HEIGHT,
							ovr.right+1+cvWidth, wrect.bottom -(MARGIN+B_H_SCROLL_BAR_HEIGHT)),
						"objview", qua);
	BRect	cvr = aquarium->Frame();
	
	arrange = new TimeArrangeView(
						BRect(
							cvr.right+1, t + 30 + LABEL_HEIGHT,
							wrect.right-(MARGIN+B_V_SCROLL_BAR_WIDTH), wrect.bottom -(MARGIN+B_H_SCROLL_BAR_HEIGHT)),
						"arranger", uberQua);
	
	barView = new TimeArrangeBarView(
						BRect(
							cvr.right+1, t + 30,
							wrect.right-(MARGIN+B_V_SCROLL_BAR_WIDTH), t+30 + LABEL_HEIGHT),
						arrange);
	
    arrangeScroll = new BScrollBar(
    						BRect(
    							cvr.right+1,wrect.bottom -(MARGIN+B_H_SCROLL_BAR_HEIGHT),
    							wrect.right-(MARGIN+B_V_SCROLL_BAR_WIDTH),wrect.bottom-MARGIN),
    						"", arrange,
							0, 30000, B_HORIZONTAL);
	
    vertScroll = new BScrollBar(
    					BRect(
    						wrect.right-(MARGIN+B_V_SCROLL_BAR_WIDTH)+1,t+30 + LABEL_HEIGHT,
    						wrect.right-MARGIN, wrect.bottom-MARGIN),
    					"", arrange,
						0, 30000, B_VERTICAL);
	


	msg = new BMessage(QUA_GO);

	bGo = new BButton( BRect( 5, t+5, 55, t+25 ), "",
						"Go", msg, (ulong)nullptr, (ulong)B_WILL_DRAW );

	msg = new BMessage(QUA_PAUSE);
	bPause = new BButton( BRect( 55, t+5, 105, t+25 ), "",
						"Stop", msg, (ulong)nullptr, (ulong)B_WILL_DRAW );

	
	msg = new BMessage(QUA_RECORD);
	BMessage	*mess2 = new BMessage(QUA_RECORD);
	mess2->AddInt32("audio", 0);
	bRec = new MyButton( BRect( 105, t+5, 155, t+25 ), "",
						"Rec", msg, mess2, nullptr, (ulong)nullptr, (ulong)B_WILL_DRAW );

	backView->AddChild(bGo);
	backView->AddChild(bPause);
	backView->AddChild(bRec);
	
	msg = new BMessage(QUA_TIME);
	msg->AddFloat("actual", 0);
	dTCount = new TimeCtrl(
		BRect( 155+MARGIN, t+5, 155+MARGIN+backView->StringWidth("Time")+70, t+21 ), "",
		"Time", msg, &mSecMetric, DRAW_HOR_LBL, 0, B_WILL_DRAW );
	dTCount->SetFontSize( 12 );
	dTCount->SetDivider(5+dTCount->StringWidth("Time"));
	dTCount->SetFont(be_bold_font);

	msg = new BMessage(QUA_TIME);
	msg->AddInt32("ticks", 0);

	dBBQCount = new TimeCtrl(
				  BRect( 155+110, t+5,
					155+110+backView->StringWidth("Clock")+70, t+21 ), "",
					"Clock", msg, uberQua->metric, DRAW_VER_LBL, 0, B_WILL_DRAW );
	dBBQCount->SetFontSize( 12 );
	dBBQCount->SetDivider(5+dBBQCount->StringWidth("Clock"));
	dBBQCount->SetFont(be_bold_font);
	
	msg = new BMessage(QUA_TIME);
	msg->AddFloat("tempo", 0);

	dTempoCount = new NumCtrl(
			BRect( 155+220, t+5,
				155+220+backView->StringWidth("Tempo")+30, t+21 ), "",
			"Tempo", 0, msg, 0, B_WILL_DRAW );
	dTempoCount->SetRanges(0,0,MAJORLY_BIG);
	dTempoCount->SetDivider(5+backView->StringWidth("Tempo"));
	dTempoCount->SetFont(be_bold_font);
	dTempoCount->SetFontSize( 12 );
	
	SetPulseRate(10000.0);

	AddChild(backView);
	backView->AddChild(dTCount);
	backView->AddChild(dBBQCount);
	backView->AddChild(dTempoCount);
	backView->AddChild(aquarium);
	backView->AddChild(barView);
	backView->AddChild(channelView);
	backView->AddChild(arrange);
	backView->AddChild(arrangeScroll);
	backView->AddChild(vertScroll);

	ArrangeChildren();
	
	fprintf(stderr, "Created sequencer window\n");
}

void
SequencerWindow::ArrangeChildren()
{
//	fprintf(stderr, "rearranging\n");
	BRect	r = Bounds();
	BRect	of = aquarium->Frame();
	channelView->MoveTo(of.right+4, of.top);
	BRect	cf = channelView->Frame();
	
	arrange->ResizeTo(
			r.right - cf.right - B_V_SCROLL_BAR_WIDTH - MARGIN - 4,
			cf.bottom - cf.top);
	arrange->MoveTo(cf.right+4, of.top);
	BRect	tf = arrange->Frame();
	arrangeScroll->ResizeTo(
			r.right - cf.right - B_V_SCROLL_BAR_WIDTH - MARGIN,
			B_H_SCROLL_BAR_HEIGHT);
	arrangeScroll->MoveTo(cf.right, tf.bottom+2);
	vertScroll->ResizeTo(
			B_V_SCROLL_BAR_WIDTH,
			tf.bottom - tf.top);
	vertScroll->MoveTo(tf.right+3, tf.top);
	vertScroll->SetRange(0, uberQua->nChannel*trackHeight-arrange->Bounds().bottom);
	barView->ResizeTo(tf.right-tf.left,LABEL_HEIGHT);
	barView->MoveTo(tf.left, tf.top-LABEL_HEIGHT);
}


void
SequencerWindow::SetTarget(BHandler *hand)
{
	dBBQCount->SetTarget(this);
	dTCount->SetTarget(this);
	dTempoCount->SetTarget(this);
	bGo->SetTarget(hand);
	bPause->SetTarget(hand);
	bRec->SetTarget(hand);
}

void
SequencerWindow::FrameResized(float x, float y)
{
	vertScroll->SetRange(0, uberQua->nChannel*trackHeight-arrange->Bounds().bottom);
	backView->Invalidate();
	channelView->Invalidate();
	ArrangeChildren();
//	fprintf(stderr, "sz %g %g\n", x, y);
}

void
SequencerWindow::AddChannel(Channel *c)
{
	c->channelView = channelView;
	arrange->nChannel = uberQua->nChannel;
	Lock();
	c->ArrangeChildren();
	c->MoveTo(1,(c->chanId)*trackHeight+1);
	vertScroll->SetRange(0, uberQua->nChannel*trackHeight-arrange->Bounds().bottom);

	Unlock();
}

void
SequencerWindow::MenusBeginning()
{
	mFileMenu->SetTargetForItems(uberQua);
}

void
SequencerWindow::MessageReceived( BMessage* inMsg )
{
	
	if (	inMsg->what == QUA_TIME) {
		BMessage		*fwdMsg=new BMessage(QUA_TIME);
		int				val;
							
//		if (theApp->appWindow->CurrentFocus() == nullptr)
//			MakeFocus();
		if		  (inMsg->HasFloat("tempo")) {
			float val = atof(dTempoCount->Text());
			fwdMsg->AddFloat("tempo", val);
		}
		if (inMsg->HasInt32("ticks")) {
			long tickval = dBBQCount->TickValue();
			float actval = dBBQCount->ActualValue();
			fwdMsg->AddInt32("ticks", tickval);
			fwdMsg->AddFloat("actual", actval);
		}
		if (inMsg->HasFloat("actual")) {
			long tickval = dTCount->TickValue();
			float val = dTCount->ActualValue();
			fwdMsg->AddInt32("msticks", tickval);
			fwdMsg->AddFloat("actual", val);
		}
		quapp->PostMessage(fwdMsg, (BHandler *)uberQua);
		delete fwdMsg;
	} else {
		BWindow::MessageReceived(inMsg);
	}
		
}

SequencerWindow::~SequencerWindow()
{
	fprintf(stderr, "Deleted sequencer window\n");
}
	
bool
SequencerWindow::QuitRequested( )
{
	short		val;
	BAlert*		about = new BAlert( "", "Save this shgite?",
		"Cancel", "Don't Save", "Save" );
		
	if (CurrentMessage()->HasInt32("le bombe"))
		return true;
		
	if ((val=about->Go()) == 2) {
		CloseSubwindows();
		sem_id		qsem = uberQua->mySem;
		BMessage	m(QUA_SAVE);
		m.AddInt32("terminally", 0);
		quapp->PostMessage(&m, uberQua );
		fprintf(stderr, "SequencerWindow::QuitRequested() aquiring uberQua sem\n");
		acquire_sem(qsem);	// wait till qua thread goner	
		fprintf(stderr, "SequencerWindow::QuitRequested() kissing its ass goodbye\n");
		return TRUE;
	} else if (val == 1) {
		CloseSubwindows();
		sem_id		qsem = uberQua->mySem;
		fprintf(stderr, "posted close message\n");
		quapp->PostMessage(QUA_CLOSE);
		fprintf(stderr, "SequencerWindow::QuitRequested() acuiring uberQua sem\n");
		acquire_sem(qsem);	// wait till qua thread goner	
		fprintf(stderr, "SequencerWindow::QuitRequested() kissing its ass goodbye\n");
		return TRUE;
	} else
		return FALSE;

}

void
SequencerWindow::CloseSubwindows()
{
	for (short i=0; i<aquarium->CountChildren(); i++) {
		BView	*vo = aquarium->ChildAt(i);
		if (strcmp(vo->Name(), "QuaObject") == 0) {
			QuaObject	*qo = (QuaObject *)vo;
			if (qo->editor) {
				fprintf(stderr, "quitting editor, like it or not\n");
				thread_id	winThread = qo->editor->Thread();
				qo->editor->PostMessage(B_QUIT_REQUESTED);
				status_t	err;
				wait_for_thread(winThread, &err);
			}
		}
	}
}

void
SequencerWindow::SetTime(Time theTime)
{
	BPoint		TimerPos;
	BRect		DispArea;
	char		buf[30];
	static long			ClockCounter=INFINITE_TICKS;

	if (ClockCounter == theTime.ticks) {
		return;
	}

	ClockCounter = theTime.ticks;

	dTCount->SetValue(theTime.SecsValue()*1000);
	dBBQCount->SetValue(theTime);
	arrange->current_time_cursor->SetTime(theTime);

	if (uberQua->status != STATUS_SLEEPING && arrange->TimeToPixel(theTime) > arrange->Bounds().right-10) {
		arrangeScroll->SetValue(arrange->Bounds().right-10);
	}
}

void
SequencerWindow::SetTempo(float temp)
{
	
	static float			Tempo=-1;
	char		buf[30];
	Lock();
	if (temp != Tempo) {
		Tempo = temp;
		sprintf(buf, "%g", Tempo);
		dTempoCount->SetText(buf);
	}
	Unlock();
}

void
SequencerWindow::SetMetric(class Metric *m)
{
	SetTempo(m->tempo);
	arrange->SetMetric(m);
}	
