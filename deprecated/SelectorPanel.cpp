
#ifdef __INTEL__ 
#include <SupportKit.h>
#include <StorageKit.h>
#include <InterfaceKit.h>
#endif

#include "QuasiStack.h"
#include "SelectorPanel.h"
#include "Colors.h"

SelectorPanel::SelectorPanel(BRect r, BMessage *msg):
	BControl(r, "selector", "", msg, B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW)
{
	tracking = MOUSE_TRACK_NOT;
}


SelectorPanel::~SelectorPanel()
{
}

void
SelectorPanel::Draw(BRect r)
{
	for (short i=0; i<stuff.CountItems(); i++) {
		DrawItem((StuffInfo *)stuff.ItemAt(i));
	}
}

void
SelectorPanel::MouseDown(BPoint p)
{
	uint32	buttons;
	BPoint	pos;

	for (short i=0; i<stuff.CountItems(); i++) {
		StuffInfo	*it = (StuffInfo *)stuff.ItemAt(i);
		if (it->rect.Contains(p)) {
			track_button = i;
		}
	}
	
	if (track_button >= 0) {
		tracking = MOUSE_TRACK_DO_SQUAT;
		SetMouseEventMask(B_POINTER_EVENTS, B_NO_POINTER_HISTORY);
	} else {
		tracking = MOUSE_TRACK_SCROLL;
		track_point = p;
		SetMouseEventMask(B_POINTER_EVENTS, B_NO_POINTER_HISTORY);
	}
}

void
SelectorPanel::MouseMoved(BPoint pt, uint32 wh, const BMessage *msg)
{
	if (tracking == MOUSE_TRACK_NOT)
		return;
	if (tracking == MOUSE_TRACK_DO_SQUAT) {
	} else if (tracking == MOUSE_TRACK_SCROLL) {
		BRect		bnd = Bounds();
		float		left = bnd.left + track_point.x - pt.x;
		ScrollTo(left, 0);
	}
}

void
SelectorPanel::MouseUp(BPoint where)
{
	if (tracking == MOUSE_TRACK_NOT)
		return;
	if (tracking == MOUSE_TRACK_DO_SQUAT) {
		StuffInfo	*it = (StuffInfo *)stuff.ItemAt(track_button);
		if (it->rect.Contains(where)) {
			it->mode = !it->mode;
			if (Message()->HasPointer("stacker")) {
				Message()->ReplacePointer("stacker", it->stacker);
			} else {
				Message()->AddPointer("stacker", it->stacker);
			}
			if (Message()->HasInt32("mode")) {
				Message()->ReplaceInt32("mode", it->mode);
			} else {
				Message()->AddInt32("mode", it->mode);
			}
			Invoke();
			DrawItem(it);
		}
	}
	tracking = MOUSE_TRACK_NOT;
}

void
SelectorPanel::DrawItem(StuffInfo *it)
{
	if (it) {
		rgb_color	col;
		
		if (it->mode)
			col = RedEr(ViewColor(), -20);
		else
			col = BlueEr(RedEr(ViewColor(), -20));
		RaisedBox(this, it->rect, col, true);
		char	buff[20];
		sprintf(buff, "%d", it->lblind);
		DrawString(buff, BPoint(it->rect.left+2, it->rect.bottom - 2));
	}
}

void
SelectorPanel::AddStuff(Stacker *S, int ind, int m)
{
	int	i = stuff.CountItems();
	BRect	r(5+30*i, 3,25+30*i,20);
//	fprintf(stderr, "add %x\n", S);
	StuffInfo	*it = new StuffInfo(r, S, ind, m);
	stuff.AddItem(it);
	if (Window()) Window()->Lock();
	Invalidate();
	if (Window()) Window()->Unlock();
}

void
SelectorPanel::DeleteStuff(Stacker *s)
{
	short		i;
	for (i=0; i<stuff.CountItems(); i++) {
		StuffInfo *it = (StuffInfo*)stuff.ItemAt(i);
		if (it->stacker == s) {
			delete stuff.RemoveItem(i);
			break;
		}
	}
	for (;i<stuff.CountItems(); i++) {
		StuffInfo *it = (StuffInfo*)stuff.ItemAt(i);
		it->lblind = i;		
		it->rect.Set(5+30*i, 3,25+30*i,20);
	}
	if (Window()) Window()->Lock();
	Invalidate();
	if (Window()) Window()->Unlock();
}

StuffInfo::StuffInfo(BRect &r, Stacker *s, int lbi, int m)
{
	rect = r;
	stacker = s;
	lblind = lbi;
	mode = m;
}

StuffInfo::~StuffInfo()
{
}
