
#ifdef __INTEL__ 
#include <SupportKit.h>
#include <StorageKit.h>
#include <InterfaceKit.h>
#endif


#include <stdio.h>

#include "StdDefs.h"
#include "SequencerWindow.h"
#include "Colors.h"
#include "TiledView.h"
#include "Qua.h"
#include "Channel.h"

extern FILE *dbfp;

ChannelView::ChannelView(BRect area, char *name, Qua *q) :
	ObjectViewContainer(area, name, (ulong)nullptr, (ulong)B_FRAME_EVENTS|B_WILL_DRAW)
{
	SetResizingMode(B_FOLLOW_TOP_BOTTOM);
	SetViewColor(Darker(orange));
	channelWidth = DFLT_CHANNELVIEW_WIDTH;
	uberQua = q;
}


ChannelView::~ChannelView()
{
	fprintf(dbfp, "deleted channel view\n");
}

void
ChannelView::Draw(BRect area)
{
}

bool
ChannelView::MessageDropped(BMessage *message,BPoint where,
								BPoint delta)
{
	return FALSE;
}


void
ChannelView::MouseDown(BPoint where)
{
}

void
ChannelView::FrameResized(float x, float y)
{
//fprintf(stderr, "frsz\n");
	((SequencerWindow*)Window())->ArrangeChildren();
//	((SequencerWindow*)Window())->backView->Invalidate();
}

void
ChannelView::MessageReceived(BMessage *message)
{
	;
}


void
ChannelView::ArrangeChildren()
{
	for (short i=0; i<uberQua->nChannel; i++) {
		Channel *c = uberQua->channel[i];
		c->MoveTo(1,(c->chanId)*TrackHeight()+1);
	}
	BRect	r = Bounds();
	if (r.right - r.left < channelWidth) {
		ResizeBy(channelWidth+2-(r.right-r.left), 0);
	}
}
