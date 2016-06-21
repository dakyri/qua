#include "qua_version.h"

#ifdef __INTEL__ 
#include <SupportKit.h>
#include <StorageKit.h>
#include <InterfaceKit.h>
#endif

#include "math.h"

#include "StdDefs.h"
#include "Colors.h"

#include "include/SequencerWindow.h"
#include "include/ArrangerObject.h"
#include "include/QuaObject.h"
#include "include/messid.h"
#include "include/Qua.h"
#include "include/Instance.h"
#include "include/AQuarium.h"
#include "include/Schedulable.h"
#include "include/SchedulablePanel.h"
#include "include/MixerWindow.h"
#include "include/QuaPort.h"
#include "include/QuaAudio.h"

TimeArrangeBarView::TimeArrangeBarView(BRect area, TimeArrangeView *a):
	BView(area, "numbers", B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW)
{
	SetViewColor(mdGray);
	arrange = a;
}

TimeArrangeBarView::~TimeArrangeBarView()
{
	;
}

void
TimeArrangeBarView::Draw(BRect area)
{
	long numBeats = (area.right-area.left)/arrange->fPixelsPerBeat+2;
	long theBeat = area.left/arrange->fPixelsPerBeat;
	long thePos = theBeat*arrange->fPixelsPerBeat;
	long toAdd = arrange->fPixelsPerBeat;
	short bpBar = arrange->metric->beatsPerBar;
	short bpGroup = arrange->metric->barsPerGroup;
	char	buf[30];

	if (numBeats >= 8) {
		SetHighColor(black);
	
		theBeat = area.left/arrange->fPixelsPerBeat;
		thePos = theBeat*arrange->fPixelsPerBeat;
		for (int i=0;i<numBeats;i++) {
			if (theBeat % (8*bpBar) == 0) {
				sprintf(buf, "%d", theBeat);
				DrawString(buf, BPoint(thePos, LABEL_HEIGHT-1));
			}
			thePos += toAdd;
			theBeat += 1;
		}
	}
}

void
TimeArrangeView::ScrollTo(BPoint p)
{
	BRect bnd = Bounds();
	BView::ScrollTo(p);

	SequencerWindow	*w = (SequencerWindow *)Window();
	w->barView->ScrollTo(p.x, w->barView->Bounds().top);	
	w->aquarium->ScrollTo(w->aquarium->Bounds().left, p.y);	
	w->channelView->ScrollTo(w->channelView->Bounds().left, p.y);	
}

void
TimeArrangeBarView::KeyDown(const char *c, int32 nb)
{
	;
}

void
TimeArrangeBarView::MouseDown(BPoint where)
{
	;
}

void
TimeArrangeBarView::MessageReceived(BMessage *inMsg)
{
	BView::MessageReceived(inMsg);
}
void
TimeArrangeBarView::FrameResized(float width, float h)
{
	;
}

TimeCursor::TimeCursor(TimeArrangeView *tv)
{
	the_view = tv;
}

void
TimeCursor::SetTime(Time t)
{
	if (t == time)
		return;
	BRect		Box = the_view->Bounds();
	float		p1 = the_view->TimeToPixel(time),
				p2 = the_view->TimeToPixel(t);
	time = t;
	if ((abs(p1-p2)) < 2) {
		Box.left = Min(p1,p2)-1;
		Box.right = Max(p1,p2)+1;
		the_view->Draw(Box);
	} else {
		Box.left = floor(p1)-1;
		Box.right = ceil(p1)+1;
		the_view->Draw(Box);
		Box.left = floor(p2)-1;
		Box.right = ceil(p2)+1;
		the_view->Draw(Box);
	}		

}

void
TimeCursor::Draw(BRect wr)
{
	BPoint	p1,p2;
	float	curs_pt = the_view->TimeToPixel(time);

	p1.Set(curs_pt, wr.top);
	p2.Set(curs_pt, wr.bottom);
	the_view->SetHighColor(purple);
	the_view->StrokeLine(p1, p2, B_SOLID_HIGH);
}


void
TimeArrangeView::KeyDown(const char *c, int32 nb)
{
// perhaps this could be mda universal across
// class ObjectViewContainer????
	if (nb == 1) {
		char	theChar = c[0];
		if (theChar == B_DELETE) {
			BList	L(selectedObjects);
			DeSelectAll();
			for (short i=0; i<L.CountItems(); i++) {
				ArrangerObject *p = (ArrangerObject *)L.ItemAt(i);
				RemoveChild(p);
				RemoveArrangerObject(p, true);
			}
		} else 
			BView::KeyDown(c, nb);
	} else {
		BView::KeyDown(c, nb);
	}
}

TimeArrangeView::TimeArrangeView(BRect area, char *name, Qua *qua) :
	ObjectViewContainer(area, name)
{
	metric = &stdMetric;
	SetResizingMode(B_FOLLOW_ALL);
	SetFlags(B_WILL_DRAW|B_FRAME_EVENTS);
	nChannel = qua->nChannel;
	maxTime = 600;
	current_time_cursor = new TimeCursor(this);
	uberQua = qua;
	scale = 1;
	fPixelsPerBeat = PIXELS_PER_QUANT * scale;
	SetFont( be_plain_font );
	SetFontSize(8);
	arrangerObjects = nullptr;
}

void
TimeArrangeView::SetMetric(class Metric *m)
{
	metric = m; // mebe redraw
}

ArrangerObject *
TimeArrangeView::FindArrangerObject(Instance *i)
{
	ArrangerObject *p = arrangerObjects;
	while (p != nullptr) {
		if (p->instance == i)
			break;
	}
	return p;
}

void
TimeArrangeView::AddArrangerObject(QuaObject *model, BPoint where)
{
	short channel = PointToChannel(where);
	if (!model->schedulable)
		return;
	
	if ((channel) < 0)
		return;

	where.y = channel*TrackHeight();
//	where.x -= Bounds().left;
	Time	x =	~PixelToTime(where.x);
	ArrangerObject *obj = model->schedulable->CloneArrangerObject(channel,
					x,
					infiniteTime, this);
	if (!obj)
		return;		//method......
//	AddChild(obj);
}

void
TimeArrangeView::AddArrangerObject(ArrangerObject *model, BPoint where)
{
	short channel = PointToChannel(where);
	
	if ((channel) < 0)
		return;

	where.y = channel*TrackHeight();
//	where.x -= Bounds().left;
	Time	x =	~PixelToTime(where.x);

	ArrangerObject *obj = model->Clone(channel, x);
}


void
TimeArrangeView::MoveArrangerObject(ArrangerObject *obj, BPoint where)
{
	if (where == obj->Frame().LeftTop())
		return;

	short channel = PointToChannel(where);
	
	if ((channel) < 0)
		return;

	where.y = channel*TrackHeight();
	Time	x =	~PixelToTime(where.x);
	where.x = TimeToPixel(x);
	where.x -= Bounds().left;
	obj->instance->startTime = x;
	obj->instance->SetChannel(channel);
	obj->MoveTo(where);
}

void
TimeArrangeView::RemoveArrangerObject(QuaObject *p, bool andD)
{
	ArrangerObject		*q, **qp;
	
	qp = &arrangerObjects;
	q = arrangerObjects;
	while (q != nullptr) {
		if (q->instance->schedulable == p->schedulable) {
			*qp = q->next;
			RemoveChild(q);
			if (andD) {
				glob.DeleteSymbol(q->instance->sym);
				fprintf(stderr, "deleted symbol\n");
				delete q;
			}
			q = *qp;
		} else {
			qp = &q->next;
			q = q->next;
		}
	}
}

void
TimeArrangeView::RemoveArrangerObject(ArrangerObject *p, bool andD)
{
	ArrangerObject		*q, **qp;
	
	qp = &arrangerObjects;
	q = arrangerObjects;
	while (q != nullptr) {
		if (q == p) {
			*qp = q->next;
			RemoveChild(p);
			if (andD) {
				glob.DeleteSymbol(p->instance->sym);
				delete (p);
			}
			break;
		}
		qp = &q->next;
		q = q->next;
	}
}


TimeArrangeView::~TimeArrangeView()
{
	delete current_time_cursor;
}


void
TimeArrangeView::FrameResized(float x, float y)
{
}

void
TimeArrangeView::SetScale(float scale)
{
	scale = Max(scale, 0.1);
	fPixelsPerBeat = PIXELS_PER_QUANT * scale;
}

void
TimeArrangeView::Draw(BRect area)
{
	BRegion	oldReg; oldReg.Include(area);
	ConstrainClippingRegion(&oldReg);
	
	SetLowColor(ViewColor());
	FillRect(area, B_SOLID_LOW);
	SetDrawingMode(B_OP_COPY);
	
	DrawGrid(this,
		area,
		fPixelsPerBeat,
		TrackHeight(),
		area.left/fPixelsPerBeat,
		(area.right-area.left)/fPixelsPerBeat + 2,
		0,
		nChannel,
		ltGray,
		mdGray, metric->beatsPerBar, 
		dkGray, metric->barsPerGroup, 
		mdGray,
		mdGray, 0, 
		mdGray, 0
	);
	current_time_cursor->Draw(area);
	
	ConstrainClippingRegion(nullptr);
}


bool
TimeArrangeView::MessageDropped(BMessage *message,BPoint where,
								BPoint delta)
{
	if (message->what == MOVE_OBJECT) {
		bool				add_obj = FALSE;

		where.x = where.x - delta.x;
		if (message->HasPointer("arranger_object")) {		
			ArrangerObject		*obj;

			message->FindPointer("arranger_object", (void **)&obj);
			MoveArrangerObject(obj, where);
		} else if (message->HasPointer("qua_object")) {
			QuaObject *model;

			message->FindPointer("qua_object", (void **)&model);
			AddArrangerObject(model, where);
		} else if (message->HasPointer("sym_object")) {
			QuaSymbolBridge *model;
			
			short channel = PointToChannel(where);
			if ((channel) < 0)
				return true;
			where.y = channel*TrackHeight();
			//	where.x -= Bounds().left;
			Time	x =	~PixelToTime(where.x);

			message->FindPointer("sym_object", (void **)&model);
			if (model->sym->type == S_PORT) {
				QuaPort	*p = ((PortObject *)model)->quaPort;
				ArrangerObject *obj =
						p->CloneArrangerObject(
											channel,
											x,
											infiniteTime,
											this);
				fprintf(stderr, "%x %x...\n", model->sym->PortValue(),
						p);
				if (obj) {
					uberQua->AddSchedulable(p);
					SchedulablePanel		*SC = nullptr;
					if (uberQua->mixerView->Window())
						uberQua->mixerView->Window()->Lock();
					BRect		r = uberQua->mixerView->back->Frame();
					if (uberQua->mixerView->Window())
						uberQua->mixerView->Window()->Unlock();
					BRect		CRect(0,0,r.right-r.left,2*MIXER_MARGIN);
    				SC = new SchedulablePanel(CRect, p, p->IsMultiSchedulable());
    				uberQua->mixerView->AddSchedulablePanel(SC);
				}
			}
		} else {
//			reportError("wierd message\n");
			return FALSE;
		}
		return TRUE;
	} else if (message->what == COPY_OBJECT) {
		if (message->HasPointer("arranger_object")) {
			ArrangerObject *model;
			
			message->FindPointer("arranger_object", (void **)&model);
			AddArrangerObject(model, where);
		}
		return true;
	} else if (message->what == B_SIMPLE_DATA) {
		entry_ref ref;
		status_t err= message->FindRef("refs", &ref);
//		where.x = where.x - delta.x;
		BPoint	aqpt = where;
		BRect aqr = uberQua->sequencerWindow->aquarium->Bounds(); 
		aqpt.x = aqr.right/2;
		
		BPath	path;
		BEntry	ent(&ref);
		ent.GetPath(&path);
		
		QuaObject *newO = uberQua->LoadFile(&path, aqpt);
		if (newO) {
			AddArrangerObject(newO, where);
		}
		return true;
	} else
		return FALSE;

	return FALSE;
}

BRect TimeArrangeView::ObjectRect(Time &startt, Time &dur, short chan)
{
	Time	endt = startt + dur;
	BRect	B(	TimeToPixel(startt), chan * DFLT_OBJECT_HEIGHT,
				TimeToPixel(endt), (chan + 1)*DFLT_OBJECT_HEIGHT);

	return B;
}

Time
TimeArrangeView::PixelToTime(float d)
{
	Time	t((d/fPixelsPerBeat) * metric->granularity, metric);
	return t;
}

float
TimeArrangeView::TimeToPixel(Time &t)
{
	if (t.ticks == INFINITE_TICKS)
		return 100000;
	return t.BeatValue() * fPixelsPerBeat;
}

short
TimeArrangeView::PointToChannel(BPoint point)
{
	long channel = (point.y) / TrackHeight();

	if (point.y < 0)
		return -1;
	if (channel >= nChannel)
		return -1;
	return channel;
}

float
TimeArrangeView::ChannelToPixel(short c)
{
	return c*TrackHeight();
}



void
TimeArrangeView::MessageReceived(BMessage *inMsg)
{
	if (inMsg->WasDropped()) {
		BPoint delta(0, 0);
		BPoint where = inMsg->DropPoint(&delta);
		ConvertFromScreen(&where);
		if (MessageDropped(inMsg, where, delta)) {
			return;
		}
	}
	switch (inMsg->what) {
	
	case REDISPLAY_OBJECT: {
		Instance	*i = nullptr;
		if (inMsg->FindPointer("instance", (void **)&i) != B_ERROR && i != nullptr) {
			ArrangerObject	*a = i->arrangerObject;
			BPoint	where(
						TimeToPixel(i->startTime),
						ChannelToPixel(i->channel));
			where -= Bounds().LeftTop();
			a->MoveTo(where);
			a->Invalidate();
		}
		break;
	}
	
	case RESIZE_OBJECT: {
		Instance	*i = nullptr;
		if (inMsg->FindPointer("instance", (void **)&i) != B_ERROR && i != nullptr) {
			ArrangerObject	*a = i->arrangerObject;
			BRect			bnd = a->Bounds();
			a->ResizeTo(TimeToPixel(i->duration),
						bnd.bottom - bnd.top);
			a->Draw(a->Bounds());
		}
	}

	default:
		ObjectViewContainer::MessageReceived(inMsg);
	}
}


void
TimeArrangeView::MouseDown(BPoint where)
{
	MakeFocus(TRUE);
	Window()->Activate();
	DeSelectAll();
}
