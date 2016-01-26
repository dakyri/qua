
#ifdef __INTEL__ 
#include <SupportKit.h>
#include <StorageKit.h>
#include <InterfaceKit.h>
#endif

#include "ChannelPanel.h"
#include "Sym.h"
#include "Channel.h"
#include "Colors.h"
#include "messid.h"
#include "Method.h"
#include "Qua.h"
#include "StdDefs.h"
#include "ControlVariable.h"
#include "MixerWindow.h"
#include "SelectorPanel.h"
#include "InstancePanel.h"

ChannelPanel::ChannelPanel(BRect rect, float maxw, Channel *A):
	ControlPanel(rect, maxw, "channel",
				CP_TYPE_ONOFF,
				"Activate",
				A->sym->name,
				A->status)
{
	BRect	wrect;
	myChannel = A;
	rxPortPanel = txPortPanel = nullptr;
	rxBlockPanel = txBlockPanel = nullptr;
	A->controlPanel = this;
	
//	short swid = StringWidth("RX");
//	GetItemRect(MIXER_MARGIN, swid + 35, 17, 5, wrect);
//	BMessage	*msg = new BMessage(DESTINATION_SELECT);
//	msg->AddPointer("destination", &A->streamRX);
//
//	dr = new DestinationSelect(
//						wrect, "", "RX",
//						&A->streamRX,
//						nullptr, A->uberQua, nullptr, B_WILL_DRAW);
//	SetControlDefaults(dr, swid, nullptr, msg);
//	dr->SetDivider(swid+5);
//	AddChild(dr);
//
//	swid = StringWidth("TX");
//	GetItemRect(MIXER_MARGIN, swid + 35, 17, 5, wrect);
//	msg = new BMessage(DESTINATION_SELECT);
//	msg->AddPointer("destination", &A->streamTX);
//	dt = new DestinationSelect(
//						wrect, "", "TX",
//						&A->streamTX,
//						nullptr, A->uberQua, nullptr, B_WILL_DRAW);
//	SetControlDefaults(dt, swid, nullptr, msg);
//	dt->SetDivider(swid+5);
//	AddChild(dt);

	AddControlVariables(A->parameters, A->uberQua, nullptr);
	if (A->rx.block)
		rxBlockPanel = AddFramePanel(A->rxStack);
	if (A->tx.block)
		txBlockPanel = AddFramePanel(A->txStack);
}

FramePanel *
ChannelPanel::AddFramePanel(QuasiStack *n)
{
	if (n->higherFrame.CountItems() || n->controlVariables) {
		BRect	wrect;
		
		wrect.Set(	MIXER_MARGIN, MIXER_MARGIN,
					2*MIXER_MARGIN, 2*MIXER_MARGIN);
		FramePanel *NV = new FramePanel(wrect,
										maxWidth-2*MIXER_MARGIN,
										n,
										myChannel->uberQua);
		
		wrect=NV->Bounds();
		GetItemRect(MIXER_MARGIN,
				wrect.right - wrect.left,
				wrect.bottom - wrect.top, MIXER_MARGIN,
				wrect);
		NV->MoveTo(wrect.left,wrect.top);
		AddChild(NV);
		return NV;
	}
	return nullptr;
}

void
ChannelPanel::MouseDown(BPoint pt)
{
}

void
ChannelPanel::MessageReceived(BMessage *msg)
{
	switch(msg->what) {

//	case DESTINATION_SELECT:
//		if (myChannel->Window())
//			myChannel->Window()->Lock();
//		myChannel->Invalidate();
//		if (myChannel->Window())
//			myChannel->Window()->Unlock();
//		// check what other little bits may be needed
//		
//		break;

	case CTRL_BANG:
		break;
		
	case SET_GRANULARITY:
		break;
		
	case SET_OFFSET:
		break;
		
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
	};
}

void
ChannelPanel::AttachedToWindow()
{
	ControlPanel::AttachedToWindow();
//	dt->SetTarget(this);
//	dr->SetTarget(this);
}



ChannelPanel::~ChannelPanel()
{
}

void
ChannelPanel::Draw(BRect bnd)
{
	BRect	b(1,1,myWidth-1, myHeight-1);
	RaisedBox(this, b, ViewColor(), false);
}

void
ChannelPanel::FrameResized(float x, float y)
{
	Panel *v;
	fprintf(stderr,"panel resized %g %g\n", y, x);
	myHeight = y;
	myWidth = x;
	if ((v=(Panel*)Parent()) != nullptr) {
		v->ArrangeChildren();
	}
	Invalidate();
}

void
ChannelPanel::ArrangeChildren()
{
// leave where they are...
//	vName
//	dr
//	dt
	BRect	base(MIXER_MARGIN,MIXER_MARGIN,
				MIXER_MARGIN,MIXER_MARGIN);
	BRect	bd = bBangButton->Frame();
	thePoint = bd.RightTop();
	maxWidth = myWidth;
	float		oldWid = maxWidth;
	myHeight = bd.bottom + 2*MIXER_MARGIN;

//	rxPortPanel && txPortPanel
	if (rxPortPanel) {
		base = rxPortPanel->Bounds();
		GetItemRect(MIXER_MARGIN,
				base.right-base.left,
				base.bottom-base.top, MIXER_MARGIN,
				base);
		rxPortPanel->MoveTo(base.LeftTop());
	}
	if (txPortPanel) {
		base = txPortPanel->Bounds();
		GetItemRect(MIXER_MARGIN,
				base.right-base.left,
				base.bottom-base.top, MIXER_MARGIN,
				base);
		txPortPanel->MoveTo(base.LeftTop());
	}
//	rxBlockPanel && txBlockPanel
	if (rxBlockPanel) {
		base = rxBlockPanel->Bounds();
		GetItemRect(MIXER_MARGIN,
				base.right-base.left,
				base.bottom-base.top, MIXER_MARGIN,
				base);
		rxBlockPanel->MoveTo(base.LeftTop());
	}
	if (txBlockPanel) {
		base = txBlockPanel->Bounds();
		GetItemRect(MIXER_MARGIN,
				base.right-base.left,
				base.bottom-base.top, MIXER_MARGIN,
				base);
		txBlockPanel->MoveTo(base.LeftTop());
	}
	maxWidth = oldWid;
//	myHeight = base.bottom+MIXER_MARGIN;
	ResizeTo(myWidth,myHeight);
}

void
ChannelPanel::AddDestinationPanel(bool rx, ControlPanel *p)
{
	if (rx)
		rxPortPanel = p;
	else
		txPortPanel = p;
	AddChild(p);
	ArrangeChildren();
}

void
ChannelPanel::RemoveDestinationPanel(bool rx)
{
	if (rx) {
		if (rxPortPanel)
			RemoveChild(rxPortPanel);
		rxPortPanel = nullptr;
	} else {
		if (txPortPanel)
			RemoveChild(txPortPanel);
		txPortPanel = nullptr;
	}
	ArrangeChildren();
}


ChannelSelector::ChannelSelector(BRect r, Qua *uq):
	ControlPanel(r, r.right - r.left, "channel",
				CP_TYPE_NOT,
				"Activate",
				"Channel",
				STATUS_RUNNING)
{
	uberQua = uq;
	
	BList		stuff;
	BMessage	*msg = new BMessage(DISPLAY_CHANNEL);
	
	selector = new SelectorPanel(BRect(thePoint.x,
										MIXER_MARGIN,
										myWidth - MIXER_MARGIN,
										26),
										msg);
	selector->SetFontSize(12);
	selector->SetFont(be_bold_font);
	selector->SetViewColor(ViewColor());
	AddChild(selector);
	myHeight = 32;
	ResizeTo(myWidth, myHeight);
	thePoint.y = MARGIN;
	thePoint.x = myWidth;
	
	for (short i = 0; i<uq->nChannel; i++) {
		AddChannel(uq->channel[i]);
	}
}

void
ChannelSelector::AddChannel(Channel *c)
{
	char		buf[20];
	short i = c->chanId;
	selector->AddStuff(c, i, 0);
	selector->Invalidate();
}

ChannelSelector::~ChannelSelector()
{
}

void
ChannelSelector::MessageReceived(BMessage *inMsg)
{
	switch (inMsg->what) {
	case DISPLAY_CHANNEL: {
		Stacker	*s = nullptr;
		inMsg->FindPointer("stacker", (void **)&s);
		if (s) {
			int mode = inMsg->FindInt32("mode");
			Channel		*C = (Channel*)s;
			if (mode)
				DisplayChannel(C);
			else
				UndisplayChannel(C);
		}
		break;
	}
	default:
		Panel::MessageReceived(inMsg);
		break;
	}
}

void
ChannelSelector::Draw(BRect r)
{
	RaisedBox(this, BRect(1,1,myWidth-1, myHeight-1), ViewColor(), false);
	LoweredBox(this, selector->Frame(), ViewColor(), false);
}

void
ChannelSelector::FrameResized(float x, float y)
{
	fprintf(stderr,"chan sel resized %g %g\n", x, y);
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
ChannelSelector::ArrangeChildren()
{
	thePoint.Set(myWidth, MARGIN);
	float	h = 31, w = myWidth;
	for (short i=0; i<selector->stuff.CountItems(); i++) {
		StuffInfo	*it = (StuffInfo *)selector->stuff.ItemAt(i);
		if (it->mode) {
			Channel *C = (Channel *)it->stacker;
			if (C->controlPanel) {
				BRect	wrect=C->controlPanel->Bounds();
				float	item_hite = wrect.bottom - wrect.top;
				float	item_wid = wrect.right - wrect.left + MIXER_MARGIN;
				if (thePoint.x + item_wid > myWidth) {
					if (w + item_wid < maxWidth) {
						w += item_wid;
						if (thePoint.y + item_hite > h) {
							h = thePoint.y + item_hite + MIXER_MARGIN;
						}
					} else {
						thePoint.x = MIXER_MARGIN;
						thePoint.y = h - MIXER_MARGIN;
						h += item_hite + MIXER_MARGIN;
					}
				} else {
					if (thePoint.y + item_hite > h) {
						h = thePoint.y + item_hite + MIXER_MARGIN;
					}
				}
			
			
				C->controlPanel->MoveTo(thePoint.x, thePoint.y+2);
				thePoint.x += item_wid;
			}
		}
	}
	ResizeTo(w, h);
}

void
ChannelSelector::DisplayChannel(Channel *C)
{
	BRect	wrect(	MIXER_MARGIN, MIXER_MARGIN,
					2*MIXER_MARGIN, 2*MIXER_MARGIN);
	C->controlPanel = new ChannelPanel(wrect,
							 maxWidth-2*MIXER_MARGIN,
							 C);
	
	AddChild(C->controlPanel);
	ArrangeChildren();
}

void
ChannelSelector::UndisplayChannel(Channel *C)
{
	if (C && C->controlPanel) {
		RemoveChild(C->controlPanel);
		delete C->controlPanel;
		C->controlPanel = nullptr;
		ArrangeChildren();
	}
}

void
ChannelSelector::AttachedToWindow()
{
	ControlPanel::AttachedToWindow();
	selector->SetTarget(this);
}
