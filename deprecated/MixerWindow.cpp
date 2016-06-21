
#ifdef __INTEL__ 
#include <SupportKit.h>
#include <StorageKit.h>
#include <InterfaceKit.h>
#endif

#include <stdio.h>

#include "StdDefs.h"
#include "MixerWindow.h"
#include "messid.h"
#include "Colors.h"
#include "GlblMessid.h"
#include "TiledView.h"
#include "Controller.h"
#include "ChannelPanel.h"
#include "Qua.h"
#include "MyButton.h"

extern FILE *dbfp;

MixerWindow::MixerWindow(	BRect r,
							const char* inTitle,
							ulong inFlags,
							MixerView *mv)
	: BWindow( r, inTitle, B_TITLED_WINDOW, inFlags )
{
	AddChild(mv);
	SetPulseRate(100000);
}

MixerWindow::~MixerWindow()
{
	fprintf(stderr, "deleted mixer window\n");
}
	
bool
MixerWindow::QuitRequested( )
{
	fprintf(dbfp, "mixerWindow signing off\n");
	return TRUE;
}


void
MixerWindow::FrameResized(float x, float y)
{
	BRect	r = Frame();
	if (r.bottom <= 0)
		MoveTo(0,0);
}


MixerView::MixerView(BRect rect, Qua *uq)
	: TiledView(rect, "mixerview", B_FOLLOW_ALL, B_WILL_DRAW|B_FRAME_EVENTS)
	
{
	uberQua = uq;
	BRect 		ctrlRect;
	BMessage	*msg;
	int			i;
	
	SetViewColor(mdGray);
	
	SetFont(be_bold_font);
	SetFontSize( 12 );
	BRect	bnd = Bounds();
	back = new BView(BRect(MARGIN,MARGIN,
							rect.right-rect.left-MARGIN-B_V_SCROLL_BAR_WIDTH,
							rect.bottom-rect.top-MARGIN),
					"mix back",
					B_FOLLOW_ALL, (uint32)nullptr);
	back->SetViewColor(mdGray);
	AddChild(back);
	channelSelector = new ChannelSelector(
							BRect(0,0,rect.right-rect.left-2*MARGIN,30),
							uberQua);
	back->AddChild(channelSelector);
	BRect	bfr	= back->Frame();
    verticalScroll = new BScrollBar(
    							BRect(bfr.right+2,bfr.top,bfr.right+2+B_V_SCROLL_BAR_WIDTH,bfr.bottom),
    							"", back,
              					0, bfr.bottom-bfr.top, B_VERTICAL);
	AddChild(verticalScroll);
	lastPanelBottom = channelSelector->Frame().bottom;
}

void
MixerView::ArrangeChildren()
{
	BRect	f1 = channelSelector->Frame();
	for (short i=0; i<back->CountChildren(); i++) {
		BView	*v = back->ChildAt(i);
		if (strcmp(v->Name(), "schedulable") == 0) {
			v->MoveTo(f1.left,f1.bottom+1);
			f1 = v->Frame();
		}
	}
	lastPanelBottom = f1.bottom;
	BRect	f0 = back->Bounds();
	float	x = lastPanelBottom-(f0.bottom-f0.top);
	if (x < 0)
		x = 0;
	verticalScroll->SetRange(0,x);
}
		

void
MixerView::AttachedToWindow()
{
	SetTargets();
}

void
MixerView::SetTargets()
{
//	bGo->SetTarget(uberQua);
//	bPause->SetTarget(uberQua);
//	bRec->SetTarget(uberQua);
}
	
void MixerView::MessageReceived( BMessage* inMsg )
{
	TiledView::MessageReceived(inMsg);
}

void MixerView::Draw(BRect region)
{
	TiledView::Draw(region);
}


MixerView::~MixerView()
{
	fprintf(stderr, "deleted mixer view\n");
}

void
MixerView::DeleteSchedulablePanel(Panel *P)
{
	if (Window())
		Window()->Lock();
	BRect	bnd = P->Frame();
	float	toGo = P->myHeight + 1;
	fprintf(stderr, "delete sc %g\n", toGo);
	if (back->RemoveChild(P)) {
		ArrangeChildren();
	}
	if (Window())
		Window()->Unlock();
}

void
MixerView::AddSchedulablePanel(Panel *p)
{
	if (Window())
		Window()->Lock();
	BRect wrect = Bounds();
	int			len = wrect.bottom - wrect.top;
	int			wid = wrect.right - wrect.left;
	
	wrect = p->Bounds();
//	fprintf(stderr, "%g %g %g %g\n",
//					wrect.left, wrect.top, wrect.right, wrect.bottom);
	p->MoveTo(0, len);
	back->AddChild(p);
	ArrangeChildren();

//	len += (wrect.bottom - wrect.top)+1;
//	wid = Max(wid, wrect.right - wrect.left + 2*MARGIN);
//	ResizeTo(wid,len);
	if (Window())
		Window()->Unlock();
}

void
MixerView::AddChannel(Channel *c)
{
	if (Window())
		Window()->Lock();
	channelSelector->AddChannel(c);
	ArrangeChildren();
	if (Window())
		Window()->Unlock();
}

void
MixerView::FrameResized(float x, float y)
{
	float	z = lastPanelBottom-(y-2*MARGIN);
	if (z < 0)
		z = 0;
	verticalScroll->SetRange(0,z);
}
