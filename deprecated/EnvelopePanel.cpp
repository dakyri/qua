
#ifdef __INTEL__ 
#include <SupportKit.h>
#include <StorageKit.h>
#include <InterfaceKit.h>
#endif

#include "EnvelopePanel.h"
#include "Envelope.h"
#include "ModeCtrl.h"
#include "TimeCtrl.h"
#include "KeyVal.h"
#include "NumCtrl.h"
#include "NameCtrl.h"
#include "messid.h"
#include "Qua.h"
#include "Colors.h"
#include "GlblMessid.h"
#include "Sym.h"
#include "ControlVariable.h"
#include "TPot.h"
#include "Instance.h"
#include "Schedulable.h"

KeyVal	envt[] = {
	{"not", 		ENV_RST_NOT},
	{"immediate",	ENV_RST_IMMEDIATE},
	{"at loop",		ENV_RST_LOOP_START}
};

KeyIndex	envtIndex(envt,sizeof(envt)/sizeof(KeyVal));

KeyVal envptt[] = {
	{"fixed",		ENV_SEG_TYPE_FIXED},
	{"linear",		ENV_SEG_TYPE_LINEAR}
};

KeyIndex	envptIndex(envptt,sizeof(envptt)/sizeof(KeyVal));

KeyVal dispmd[] = {
	{"not",			DISPLAY_NOT},
	{"pot",			DISPLAY_POT},
	{"envelope",	DISPLAY_ENV}
};

#define POT_OFFSET 10

class PotEnvelope: public TeaPot19
{
public:
	bool		inMD;
	PotEnvelope(BRect r, char *name, char *label,
				 BMessage *message, BStringView *sv, ulong resizeMode,
				 ulong flags ):
		TeaPot19(r,name,label,message,sv,resizeMode,flags)
	{
		inMD = false;
	}
	
	virtual void
	MouseDown(BPoint p)
	{
		inMD = true;
		TeaPot19::MouseDown(p);
		inMD = false;
	}
};

EnvelopePanel::EnvelopePanel(BRect er, BRect pr, float maxw, Envelope *e, short mode):
	Panel(mode==DISPLAY_POT?pr:er, maxw, "envelope", B_PULSE_NEEDED),
	EnvelopeDrawer(this, white, blue, red, green)	
{
	tracking = MOUSE_TRACK_NOT;

	er.OffsetTo(0,0);
	pr.OffsetTo(0,0);

	envRect = er;
	potRect = pr;
	
	envelope = e;
	if (e->controlVar)
		e->controlVar->envelopePanel = this;

	float swid = 0;
	
	editRegion.bottom = envRect.bottom-7;
	editRegion.right = envRect.right - 2;
	editRegion.left = 200;
	editRegion.top = 5;

	baseX = 0;
	baseY = 0;
	scaleX = 1;
	scaleY = 1;

	bool	isFloat = e->sym->type == S_FLOAT;
	
	float min = e->sym->minVal.FloatValue(nullptr);
	float ini = e->sym->iniVal.FloatValue(nullptr);
	float max = e->sym->maxVal.FloatValue(nullptr);
	
	displayOptionTab.Set(1,1,6,6);
	SetEnvelope(e);
	
	BMessage	*msg = new BMessage(SET_ENV_SEG_VAL);
	swid = StringWidth(envelope->sym->UniqueName());
	tPot = new PotEnvelope(BRect(POT_OFFSET,0,potRect.right-POT_OFFSET,potRect.bottom),
					"", envelope->sym->UniqueName(),
					msg, nullptr, (ulong)B_FOLLOW_LEFT|B_FOLLOW_TOP, (ulong)B_WILL_DRAW );
	SetControlDefaults(tPot, swid);
	tPot->SetDivider(swid+3);
	tPot->SetBounds(min, ini, max);
	tPot->SetFloatValue(e->cueValue);
	
	envName = new BStringView(BRect(10, 4, 10+swid, 22),	"", envelope->sym->name);
	envName->SetFont(be_bold_font);
	envName->SetFontSize( 12 );
	float nwid = envName->StringWidth(envelope->sym->UniqueName())+15;
	envName->ResizeTo(nwid,18);
	
	swid = StringWidth("Reset");
	envType = new ModeCtrl(
		BRect( nwid, 4, nwid+swid+25, 16 ),
		"Reset", envelope->rstFlags, &envtIndex,
					new BMessage(SET_ENV_TYPE),
		(ulong)B_FOLLOW_LEFT|B_FOLLOW_TOP, (ulong)B_WILL_DRAW );
	SetControlDefaults(envType, swid);
	envType->SetFont((BFont *)be_plain_font);
//	envType->SetDivider(swid+10);
		
	ptType = new ModeCtrl(
		BRect( nwid+swid+28, 4, nwid+swid+78, 16 ),
		"segment", envelope->segment[selectedPt].type, &envptIndex,
				new BMessage(SET_ENV_SEG_TYPE),
		(ulong)B_FOLLOW_LEFT|B_FOLLOW_TOP, (ulong)B_WILL_DRAW );
	swid = StringWidth("segment");
	SetControlDefaults(ptType, swid);
	ptType->SetFont((BFont *)be_plain_font);
//	ptType->SetDivider(swid+10);

	ptTime = new TimeCtrl(BRect( nwid, 18, nwid+90, 33 ), "",
						"Time",
						new BMessage(SET_ENV_SEG_TIME),
						e->valueMetric, DRAW_HOR_LBL,
						(ulong)B_FOLLOW_LEFT|B_FOLLOW_TOP, (ulong)B_WILL_DRAW);
	swid = StringWidth("Time");
	SetControlDefaults(ptTime, swid);
	ptTime->SetValue(envelope->segment[selectedPt].time.ticks);
	ptTime->SetFont(be_plain_font);
	ptTime->SetDivider(swid+2);
						

	ptVal = new NumCtrl(BRect( nwid, 35, nwid+90, 50 ),
						 "", "Val",
						envelope->segment[selectedPt].val,
						new BMessage(SET_ENV_SEG_VAL),
						(ulong)B_FOLLOW_LEFT|B_FOLLOW_TOP, (ulong)B_WILL_DRAW,
						!isFloat);
	swid = StringWidth("Val");
	SetControlDefaults(ptVal, swid);
	ptVal->SetFont(be_plain_font);
	ptVal->SetDivider(swid+2);
	ptVal->SetRanges(min, ini, max);
	
	editRegion.left = ptType->Frame().right + 6;

	if (isFloat) {
		ptVal->SetInc((max-min)/100);
	} else {
		ptVal->SetInc(1);
	}
	
	displayMode = DISPLAY_NOT;
	SetDisplayMode(mode);
}

EnvelopePanel::~EnvelopePanel()
{
	if (envelope->controlVar)
		envelope->controlVar->envelopePanel = nullptr;
}

void
EnvelopePanel::SetEnvelope(Envelope *e)
{
//	fprintf(stderr, "env panel setting bounds, env %x %s!\n", e, (e && e->sym)?e->sym->name:"non");
	float min = e->sym->minVal.FloatValue(nullptr);
	float max = e->sym->maxVal.FloatValue(nullptr);

	baseX = 0;
	baseY = min;
	scaleX = 1;
	scaleY = (max-min);
}

void
EnvelopePanel::DrawEnvEdit()
{
	char		buf[20];
	BRegion		regi;

	SetFontSize(8);
	SetLowColor(ViewColor());
	SetHighColor(black);

	ConstrainClippingRegion(nullptr);
	BRect		marge;

	marge.Set(editRegion.left-17,editRegion.top,editRegion.left,editRegion.bottom);
	regi.Include(marge);
	
	ConstrainClippingRegion(&regi);
	
	FillRect(marge, B_SOLID_LOW);
	sprintf(buf, "%g", baseY);
	DrawString(buf, BPoint(editRegion.left-StringWidth(buf),
					editRegion.bottom));
	sprintf(buf, "%g", PixelToVal(editRegion.top));
	DrawString(buf, BPoint(editRegion.left-StringWidth(buf),
					editRegion.top+6));
	
	marge.Set(editRegion.left,editRegion.bottom,editRegion.right,editRegion.bottom+8);
	regi.Include(marge);
	ConstrainClippingRegion(&regi);
	FillRect(marge, B_SOLID_LOW);
	
	Time		printTime;
	printTime.Set(baseX, envelope->valueMetric);
	DrawString(	printTime.StringValue(),
				BPoint(editRegion.left, editRegion.bottom+8));
	printTime.Set(PixelToTime(editRegion.right), envelope->valueMetric);
	char	*cp = printTime.StringValue();
	DrawString(	cp,
				BPoint(editRegion.right-StringWidth(cp)-2, editRegion.bottom+8));

	ConstrainClippingRegion(nullptr);

	SetLowColor(bgColor);
	FillRect(editRegion, B_SOLID_LOW);
	LoweredBox(this, editRegion, ViewColor(), false);
	
	regi.MakeEmpty();
	regi.Include(editRegion);

	ConstrainClippingRegion(&regi);
	
	for (short i=0; i<envelope->nSeg; i++) {
		DrawEnvSegment(envelope, i);
	}

	ConstrainClippingRegion(&regi);
	for (short i=0; i<envelope->nSeg; i++) {
		DrawEnvSegmentPt(envelope, i);
	}
	ConstrainClippingRegion(nullptr);
}

float
EnvelopePanel::TimeToPixel(float x)
{
	return editRegion.left + (x-baseX) / scaleX;
}

float
EnvelopePanel::ValToPixel(float x)
{
	return editRegion.bottom - (editRegion.bottom-editRegion.top) * (x-baseY) / scaleY;
}

float
EnvelopePanel::PixelToTime(float x)
{
	return baseX + (x-editRegion.left)*scaleX;
}

float
EnvelopePanel::PixelToVal(float x)
{
	return baseY + (editRegion.bottom-x)*scaleY/(editRegion.bottom-editRegion.top);
}

void
EnvelopePanel::UpdateDisplay(Time &t)
{
}

void
EnvelopePanel::KeyDown(const char *cb, int32 ncb)
{
	if (displayMode == DISPLAY_ENV){
		float	val;
		char	*txt;
		char	achar=cb[0];
		
		BRect	marge = editRegion;
		BRegion	regi; regi.Include(marge);
		ConstrainClippingRegion(&regi);
	
		switch (achar) {
		case B_DELETE:
			if (selectedPt >= 0 && envelope->nSeg > 1) {
				envelope->DelSegment(selectedPt);
				if (selectedPt >= envelope->nSeg) {
					SelectEnvSegment(envelope->nSeg-1);
				}
				DrawEnvEdit();
			}
			break;
			
		case B_INSERT:
		case B_END:
			break;
	
		case B_HOME:
			break;
					
		case B_UP_ARROW:
		case B_DOWN_ARROW:
		case B_LEFT_ARROW:
		case B_RIGHT_ARROW:{
			long	mods = modifiers();
			
			if (mods & B_SHIFT_KEY) {
				switch (achar) {
				case B_UP_ARROW:	baseY += 1; break;
				case B_DOWN_ARROW:	baseY -= 1; break;
				case B_LEFT_ARROW:	baseX += 1; break;
				case B_RIGHT_ARROW:	baseX -= 1; break;
				}
			} else {
				switch (achar) {
				case B_UP_ARROW:	scaleY *= 2; break;
				case B_DOWN_ARROW:	scaleY /= 2; break;
				case B_LEFT_ARROW:	scaleX /= 2; break;
				case B_RIGHT_ARROW:	scaleX *= 2; break;
				}
				if (scaleX == 0) scaleX = 1;
				if (scaleY == 0) scaleX = 1;
			}
			DrawEnvEdit();
			break;
		}
			
		case B_ENTER:
			break;
			
		case B_TAB: {
			int lastSel = selectedPt;
			selectedPt++;
			if (selectedPt >= envelope->nSeg) selectedPt = 0;
			DrawEnvSegmentPt(envelope, lastSel);
			DrawEnvSegmentPt(envelope, selectedPt);
			DisplaySelected();
			break;
		}
			
		default: {
			BView::KeyDown(cb, ncb);
		}}
		ConstrainClippingRegion(nullptr);
	} else {
		BView::KeyDown(cb, ncb);
	}
}

void
EnvelopePanel::AttachedToWindow()
{
	ptVal->SetTarget(this);
	ptTime->SetTarget(this);
	ptType->SetTarget(this);
	envType->SetTarget(this);
	tPot->SetTarget(this);
}

void
EnvelopePanel::SelectEnvSegment(int i)
{
	if (i >= 0 && i<envelope->nSeg) {
		selectedPt = i;
		DisplaySelected();
	}
}

void
EnvelopePanel::DisplaySelected()
{
	ptTime->SetValue(envelope->segment[selectedPt].time.ticks);
	if (ptType->Value() != envelope->segment[selectedPt].type)
		ptType->SetValue(envelope->segment[selectedPt].type);
	ptVal->SetValue(envelope->segment[selectedPt].val);
}


void
EnvelopePanel::MessageReceived(BMessage *msg)
{
	if (msg->what == SET_DISPLAY_MODE) {
		SetDisplayMode(msg->FindInt32("mode"));
	} else if (displayMode == DISPLAY_POT) {
		switch (msg->what) {
		case SET_ENV_SEG_VAL: {
			float	val = msg->FindFloat("control value");
//			fprintf(stderr, "val %g\n", val);
			Qua *q = envelope->controlVar->stacker->uberQua;
			Time	theTime =  envelope->controlVar->stacker->RelativeTime(q->theTime);
			int		seg = envelope->Segment(theTime);
			
//fprintf(stderr, "%d %d %d\n", seg, envelope->segment[seg].time.ticks, theTime.ticks);

			if (seg < 0) {
					// insert new first point
				seg = envelope->AddSegment(theTime,
									val,
									ENV_SEG_TYPE_FIXED);
			} else if (envelope->segment[seg].time == theTime) {
				if (!envelope->MoveSegment(seg,
									theTime,
									val)) {
					reportError("Can't move segment");
				}
			} else {
				seg = envelope->AddSegment(theTime,
									val,
									ENV_SEG_TYPE_FIXED);
			}
			
			if (seg >= 0) {
				LValue	lval =
					envelope->controlVar->stabEnt->LValueFor(
							nullptr, envelope->controlVar->stacker,
							envelope->controlVar->stackFrame);
				lval.StoreFloat(val);
			}
			break;
		}
		default:
			Panel::MessageReceived(msg);
			break;
		}
	} else if (displayMode == DISPLAY_ENV){
		switch (msg->what) {
		case SET_NAME:
			envelope->SetName(envName->Text());
			break;
		case SET_ENV_TYPE:
			envelope->rstFlags = envType->Value();
			break;
		case SET_ENV_SEG_TYPE: {
			if (selectedPt >= 0 && selectedPt < envelope->nSeg) {
				envelope->segment[selectedPt].type = ptType->Value();
				envelope->MoveSegment(selectedPt,
						envelope->segment[selectedPt].time,
						envelope->segment[selectedPt].val);
			}
			break;
		}
		case SET_ENV_SEG_TIME:
			if (selectedPt >= 0 && selectedPt < envelope->nSeg) {
				envelope->MoveSegment(selectedPt,
									ptTime->textview->theTime,
									envelope->segment[selectedPt].val);
			}
			break;
			
		case SET_ENV_SEG_VAL:
			if (selectedPt >= 0 && selectedPt < envelope->nSeg) {
				envelope->MoveSegment(selectedPt,
						envelope->segment[selectedPt].time,
						ptVal->Value());
			}
			break;
		default:
			Panel::MessageReceived(msg);
			break;
		}
	} else {
		Panel::MessageReceived(msg);
	}
}

void
EnvelopePanel::FrameResized(float x, float y)
{
	Panel::FrameResized(x,y);
	editRegion.bottom = y-7;
	editRegion.right = x-5;
	Draw(Bounds());
}

void
EnvelopePanel::DrawNewEnvSegment(Envelope *e, short pt)
{
	if (displayMode == DISPLAY_ENV) {
		if (Window()) Window()->Lock();
		BRegion	regi;regi.Include(editRegion);
		ConstrainClippingRegion(&regi);
	
		DrawEnvSegment(envelope, pt-1,
				pt==envelope->nSeg-1?
					-2:pt+1, true);
		// new segs
		DrawEnvSegment(envelope, pt-1);
		DrawEnvSegment(envelope, pt);
		DrawEnvSegmentPt(envelope, pt-1);
		DrawEnvSegmentPt(envelope, pt);
		if (Window()) Window()->Unlock();
	}
}

			
void
EnvelopePanel::DrawNewEnvSegPosition(Envelope *e, short pt, bool scrub)
{
	if (displayMode == DISPLAY_ENV) {
		if (Window()) Window()->Lock();
		BRegion	regi;regi.Include(editRegion);
		ConstrainClippingRegion(&regi);
	
		if (scrub) {
			DrawEnvSegment(e, pt-1, -1, true);
			DrawEnvSegment(e, pt, -1, true);
			DrawEnvSegmentPt(e, pt, true);
		} else {
			// redraw everything
			
	//		Draw(editRegion);
			
			DrawEnvSegment(e, pt-1);
			DrawEnvSegment(e, pt);
			DrawEnvSegmentPt(e, pt);
		
			if (pt == selectedPt)
				DisplaySelected();
		}
		if (Window()) Window()->Unlock();
	}
}	// redraw everything

void
EnvelopePanel::SetDisplayMode(short mode)
{
	if (displayMode != mode) {
		switch (displayMode) {
		case DISPLAY_POT:
			RemoveChild(tPot);
			break;
		case DISPLAY_ENV:
			RemoveChild(envName);
			RemoveChild(envType);
			RemoveChild(ptTime);
			RemoveChild(ptType);
			RemoveChild(ptVal);
			break;
		}
		
		displayMode = mode;
		
		switch (displayMode) {
		case DISPLAY_POT:
			AddChild(tPot);
			tPot->SetTarget(this);
			ResizeTo(potRect.right-potRect.left, potRect.bottom-potRect.top);
			break;
		case DISPLAY_ENV:
			AddChild(envName);
			AddChild(envType);
			AddChild(ptTime);
			AddChild(ptType);
			AddChild(ptVal);
			ptVal->SetTarget(this);
			ptTime->SetTarget(this);
			ptType->SetTarget(this);
			envType->SetTarget(this);
			ResizeTo(envRect.right-envRect.left, envRect.bottom-envRect.top);
			break;
		}
		
		Invalidate();
		if (Parent()) {
			((Panel *)Parent())->ArrangeChildren();
		}
	}
}

void
EnvelopePanel::Draw(BRect region)
{
	RaisedBox(this, displayOptionTab, BlueEr(ViewColor()), true);
	if (displayMode == DISPLAY_ENV) {
		DrawEnvEdit();
	} else if (displayMode == DISPLAY_POT) {
	}
}


void
EnvelopePanel::MouseDown(BPoint where)
{
	if (displayOptionTab.Contains(where)) {
		BPopUpMenu	*qMenu = new BPopUpMenu("env sel", true, FALSE);
		
		BPoint			orig = displayOptionTab.LeftTop();
		ConvertToScreen(&orig);

		for (short i=0;i<sizeof(dispmd)/sizeof(KeyVal); i++) {
			BMessage	*msg = new BMessage(SET_DISPLAY_MODE);
			msg->AddInt32("mode", dispmd[i].Value);
			BMenuItem	*item = new BMenuItem(dispmd[i].Key, msg);
			qMenu->AddItem(item);
			item->SetTarget(this);
			
		}
	
		qMenu->SetAsyncAutoDestruct(true);
		qMenu->Go(orig, true, false, true);
	} else if (displayMode == DISPLAY_POT) {
	} else if (displayMode == DISPLAY_ENV) {
		MakeFocus();
		if (editRegion.Contains(where)) {
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
			
			if ((clicks=msg->FindInt32("clicks")) == 1) {
				Time tTo;
				tTo = PixelToTime(where.x);
				float valTo = PixelToVal(where.y);
				int lastSel = selectedPt;
	
				BRegion	regi;regi.Include(editRegion);
				ConstrainClippingRegion(&regi);
	
				if (	 buts & B_PRIMARY_MOUSE_BUTTON
					  && mods & B_SHIFT_KEY) {
					// add point
					selectedPt = envelope->AddSegment(tTo, valTo, ENV_SEG_TYPE_LINEAR);
					// scrub old
					if (selectedPt != -1) {
						if (lastSel >= selectedPt)
							lastSel++;
						
						if (lastSel != selectedPt-1) // undo marking 
							DrawEnvSegmentPt(envelope, lastSel);
					} else {
						reportError("cannot add identical points to envelope");
						return;
					}
				} else {
					// select point
					if (!SelectEnvSegmentPoint(envelope, where))
						return;
					DisplaySelected();
					DrawEnvSegmentPt(envelope, lastSel);
					DrawEnvSegmentPt(envelope, selectedPt);
				}
				DisplaySelected();

				tracking = MOUSE_TRACK_MOVE_ENV_SEG;
				SetMouseEventMask(B_POINTER_EVENTS, B_NO_POINTER_HISTORY);

			} else if (clicks > 1) {	// edit object
				if (buts & B_SECONDARY_MOUSE_BUTTON) {
				} else {
				}
			} else {
			}
		}
	}
}


void
EnvelopePanel::MouseMoved(BPoint pt, uint32 wh, const BMessage *msg)
{
	if (tracking == MOUSE_TRACK_NOT)
		return;
	if (tracking == MOUSE_TRACK_MOVE_ENV_SEG) {
		if (selectedPt >= 0) {
			Time	tTo = PixelToTime(pt.x);
			float	valTo = PixelToVal(pt.y);
			if (envelope->MoveableSegment(selectedPt, tTo, valTo)
					/* && editRegion.Contains(where) */	) {
				envelope->MoveSegment(selectedPt, tTo, valTo);
			}
		}
	}
}

void
EnvelopePanel::MouseUp(BPoint where)
{
	if (tracking == MOUSE_TRACK_NOT)
		return;
	if (tracking == MOUSE_TRACK_MOVE_ENV_SEG) {
		ConstrainClippingRegion(nullptr);
	}
	tracking = MOUSE_TRACK_NOT;
}


void
EnvelopePanel::Pulse()
{
	if (tPot->fValue != envelope->cueValue && !tPot->inMD) {
		tPot->SetFloatValue(envelope->cueValue);
	}
	
	if (displayMode == DISPLAY_POT) {
	} else if (displayMode == DISPLAY_ENV) {
	}
}
