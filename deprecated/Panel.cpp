
#ifdef __INTEL__ 
#include <SupportKit.h>
#include <StorageKit.h>
#include <InterfaceKit.h>
#endif

#include "Panel.h"
#include "Colors.h"
#include "ControlVariable.h"
#include "StdDefs.h"
#include "messid.h"

Panel::Panel(BRect rect, float maxw, char *nm, ulong flags)
	: BView(rect, nm?nm:"panel", (ulong)B_FOLLOW_LEFT|B_FOLLOW_TOP, (ulong)B_FRAME_EVENTS|B_WILL_DRAW|flags)
{
	SetViewColor(mdGray);
	SetFontSize(12);
	SetFont(be_bold_font);
	myHeight = rect.bottom - rect.top;
	myWidth = rect.right - rect.left;
	maxWidth = maxw;
	thePoint.Set(MIXER_MARGIN, MIXER_MARGIN);
	rowHeight = MIXER_MARGIN;
	
	SetDisplayMode(PANEL_DISPLAY_BIG);
}


void
Panel::SetControlDefaults(BControl *n, float swid, ControlVariable *p, BMessage *msg)
{
	if (p) {
		msg->AddPointer("control var", p);
		p->screenControl = n;
	}
	if (msg)
		n->SetMessage(msg);
	n->SetFont(be_bold_font);
	n->SetFontSize(12);
	n->SetViewColor(mdGray);
}

void
Panel::FrameResized(float x, float y)
{
	Panel *v;
	myHeight = y;
	myWidth = x;
	if ((v=(Panel*)Parent()) != nullptr) {
		v->ArrangeChildren();
	}
//	fprintf(stderr, "resize panel\n");
	Invalidate();
}

void
Panel::ArrangeChildren()
{
}

void
Panel::SetDisplayMode(short md)
{
	displayMode = md;
}

void
Panel::MouseDown(BPoint where)
{
	long		channel, quant;
	BRect		area = Bounds();

	ulong		mods = modifiers(); // Key mods???
	ulong		buts;
	BMessage	*msg;
	BPoint		pt;
	drawing_mode	cur_mode = DrawingMode();
	long		clicks;
	
	GetMouse(&pt, &buts);
	msg = Window()->CurrentMessage();
	
	fprintf(stderr, "%x %d\n", buts, buts & B_SECONDARY_MOUSE_BUTTON);
	if ((clicks=msg->FindInt32("clicks")) == 1) {
		if (buts & B_SECONDARY_MOUSE_BUTTON) {
			RunControlMenu(where);
		} else {
//			msg = new BMessage(MOVE_OBJECT);
//		 	msg->AddPointer("object_view", this);
		 	
			if (mods & B_SHIFT_KEY) {
//				((ObjectViewContainer *)Parent())->AddSelection(this);
			} else {
//				((ObjectViewContainer *)Parent())->Select(this);
			}
		
//			DragMessage(msg, area);
		}
	} else if (clicks > 1) {	// rescale
		if (displayMode==PANEL_DISPLAY_SMALL)
			SetDisplayMode(PANEL_DISPLAY_BIG);
		else
			SetDisplayMode(PANEL_DISPLAY_SMALL);
	} else {
	}
}

void
Panel::RunControlMenu(BPoint where)
{
	BPopUpMenu	*qMenu = new BPopUpMenu("env sel", true, FALSE);
	
	BPoint			orig = where;
	ConvertToScreen(&where);
	
	BMessage	*msg;
	BMenuItem	*item;
	
	if (displayMode != PANEL_DISPLAY_SMALL) {
		msg = new BMessage(SET_DISPLAY_MODE);
		msg->AddInt32("display mode", PANEL_DISPLAY_SMALL);
		item = new BMenuItem("Close", msg);
		qMenu->AddItem(item);
		item->SetTarget(this);
	}
	if (displayMode != PANEL_DISPLAY_BIG) {
		msg = new BMessage(SET_DISPLAY_MODE);
		msg->AddInt32("display mode", PANEL_DISPLAY_BIG);
		item = new BMenuItem("Open", msg);
		qMenu->AddItem(item);
		item->SetTarget(this);
	}
		
	qMenu->SetAsyncAutoDestruct(true);
	qMenu->Go(where, true, false, true);
}

void
Panel::MessageReceived(BMessage *inMsg)
{
	if (inMsg->what == SET_DISPLAY_MODE) {
		SetDisplayMode(inMsg->FindInt32("display mode"));
	} else {
		BView::MessageReceived(inMsg);
	}
}

void
Panel::GetItemRect(
			float indent,
			float item_wid, float item_hite,
			float seperation_wid,
			BRect &wrect)
{
	if (thePoint.x + item_wid > myWidth) {
		if (thePoint.x + item_wid < maxWidth) {	// can expand in wid
			myWidth = thePoint.x + item_wid+indent;
			seperation_wid = indent;
			if (rowHeight < item_hite + MIXER_MARGIN)
				rowHeight = item_hite + MIXER_MARGIN;
			if (thePoint.y + rowHeight > myHeight) {
				myHeight = thePoint.y + rowHeight;
			}
		} else {								// gotta go down
			thePoint.x = indent;
			thePoint.y = myHeight;
			rowHeight = item_hite + MIXER_MARGIN;
			myHeight = thePoint.y + rowHeight;
			myWidth = Max(myWidth, item_wid+2*indent);
			if (myWidth > maxWidth)
				myWidth = maxWidth;
		}
	} else {									// fits in this row.
		if (rowHeight < item_hite + MIXER_MARGIN)
			rowHeight = item_hite + MIXER_MARGIN;
		if (thePoint.y + rowHeight > myHeight) {
			myHeight = thePoint.y + rowHeight;
		}
	}
//	fprintf(stderr, "%s %g %g %g %g\n", Name(), myWidth, myHeight, item_wid, item_hite);
	ResizeTo(myWidth, myHeight);
	wrect.Set(thePoint.x, thePoint.y, thePoint.x+item_wid, thePoint.y+item_hite);
	thePoint.x += item_wid + seperation_wid;
}
