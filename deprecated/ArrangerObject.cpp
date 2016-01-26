
#ifdef __INTEL__
#include <SupportDefs.h>
#include <StorageKit.h>
#include <InterfaceKit.h>
#endif
#include "StdDefs.h"
#include "include/ArrangerObject.h"
#include "include/Qua.h"
#include "Colors.h"
#include "include/Expression.h"
#include "include/Application.h"
#include "include/Method.h"
#include "include/SequencerWindow.h"
#include "include/Pool.h"
#include "include/Pool.h"
#include "include/QuaObject.h"
#include "include/Sym.h"
#include "include/Block.h"
#include "include/messid.h"
#include "include/Sample.h"
#include "include/ControllerBridge.h"
#include "Controller.h"
#include "include/Envelope.h"
#include "KeyVal.h"
#include "NumCtrl.h"

ArrangerObject::ArrangerObject(char *lbl,
						class Schedulable *s,
						Instance *i,
						TimeArrangeView *tv,
						rgb_color c):
	ObjectView(tv->ObjectRect(i->startTime, i->duration, i->channel),
			lbl, tv, "ArrangerObject", nullptr, B_WILL_DRAW|B_FRAME_EVENTS),
	EnvelopeDrawer(this, c, Inverse(c), red, green)
{
	BRect		rect;
	
	type = s->sym->type;
	tracking = MOUSE_TRACK_NOT;

	next = Arranger()->arrangerObjects;
	Arranger()->arrangerObjects = this;
	
	color = c;
	instance = i;
	instance->arrangerObject = this;
	displayedControlVariable = nullptr;
	SetEnvelope(nullptr);
}

ArrangerObject::ArrangerObject(char *lbl,
						class Schedulable *s,
						short chan,
						Time startt,
						Time dur,
						TimeArrangeView *tv, rgb_color c):
	ObjectView(tv->ObjectRect(startt, dur, chan), lbl, tv,
			"ArrangerObject", nullptr, B_WILL_DRAW|B_FRAME_EVENTS),
	EnvelopeDrawer(this, c, Inverse(c), red, green)
{
	BRect		rect;
	
	type = s->sym->type;
	tracking = MOUSE_TRACK_NOT;

	if (Arranger()) {
		next = Arranger()->arrangerObjects;
		Arranger()->arrangerObjects = this;
	} else {
		next = nullptr;
	}
	color = c;

	SetEnvelope(nullptr);

	displayedControlVariable = nullptr;
	
	switch (type) {
	
	case S_SAMPLE: {
		Sample *sample = (Sample *) s;
		instance = sample->AddInstance(startt,dur,chan);
		break;
	} 
	
	case S_POOL: {
		Pool *pool = (Pool *) s;
		instance = pool->AddInstance(startt,dur,chan);
		break;
	} 
	
	default: {
		instance = s->AddInstance(startt,dur,chan);
	}}
	instance->arrangerObject = this;
	
//	fprintf(stderr, "Created object\n");
}

ArrangerObject::~ArrangerObject()
{
}

ArrangerObject *
ArrangerObject::Clone(short chan, Time t)
{
	return new ArrangerObject(label, instance->schedulable, chan,
				t, instance->duration, Arranger(), color);
}

void
ArrangerObject::MouseDown(BPoint where)
{
	ulong		mods = modifiers(); // Key mods???
	ulong		buts;
	GetMouse(&where, &buts);
	if (  !instance->schedulable->ExtraneousMouse(this, where, buts, mods) &&
		  !MouseDownControlVariable(where, buts, mods)) {
		  
		BMessage	*msg;
		long		clicks;
		msg = Window()->CurrentMessage();
		clicks=msg->FindInt32("clicks");
		
		if (buts & B_PRIMARY_MOUSE_BUTTON){
			if (mods & B_SHIFT_KEY) {
				((ObjectViewContainer *)Parent())->AddSelection(this);
			} else {
				((ObjectViewContainer *)Parent())->Select(this);
			}
			
			if (mods & B_CONTROL_KEY) {
				msg = new BMessage(COPY_OBJECT);
			} else {
				msg = new BMessage(MOVE_OBJECT);
			}
	 		msg->AddPointer("arranger_object", this);
			DragMessage(msg, Bounds());
		} else if (buts & B_SECONDARY_MOUSE_BUTTON){
			if (mods & B_COMMAND_KEY) {
				track_bnd = Bounds();
				tracking = MOUSE_TRACK_RESIZE;
				SetMouseEventMask(B_POINTER_EVENTS, B_NO_POINTER_HISTORY);
				
				BRect oldBnd = track_bnd;
				if (where.x > 0 && track_bnd.right != where.x) {
					track_bnd.right = where.x;
					if (where.x <oldBnd.right) {
						ResizeTo(track_bnd.right-track_bnd.left,
								 track_bnd.bottom-track_bnd.top);
//						Arranger()->Draw(
//							Arranger()->ConvertFromScreen(
//										ConvertToScreen(oldBnd)));
					} else {
						Arranger()->Draw(
							Arranger()->ConvertFromScreen(
										ConvertToScreen(oldBnd)));
						Arranger()->StrokeRect(
								Arranger()->ConvertFromScreen(
										ConvertToScreen(track_bnd)));
					}						
				}
			} else {
				MouseDownControlVariableMenu(where);
			}
		}
	}
}

void
ArrangerObject::MouseMoved(BPoint pt, uint32 wh, const BMessage *msg)
{
	if (tracking == MOUSE_TRACK_NOT)
		return;
	if (tracking == MOUSE_TRACK_RESIZE) {
		BRect oldBnd = track_bnd;
		if (pt.x > 0 && track_bnd.right != pt.x) {
			track_bnd.right = pt.x;
			if (pt.x <oldBnd.right) {
				ResizeTo(track_bnd.right-track_bnd.left,
						 track_bnd.bottom-track_bnd.top);
//							Arranger()->Draw(
//								Arranger()->ConvertFromScreen(
//											ConvertToScreen(oldBnd)));
			} else {
				Arranger()->Draw(
					Arranger()->ConvertFromScreen(
								ConvertToScreen(oldBnd)));
				Arranger()->StrokeRect(
						Arranger()->ConvertFromScreen(
								ConvertToScreen(track_bnd)));
			}						
		}
	} else if (tracking == MOUSE_TRACK_MOVE_ENV_SEG) {
		if (selectedPt >= 0 && track_envelope != nullptr) {
			Time	tTo = PixelToTime(pt.x);
			float	valTo = PixelToVal(pt.y);
			if (track_envelope->MoveableSegment(selectedPt, tTo, valTo)
					/* && editRegion.Contains(where) */	) {
				track_envelope->MoveSegment(selectedPt, tTo, valTo);
			}
		}
	} else if (tracking == MOUSE_TRACK_MOVE_LOGENTRY) {
		if (track_lastp.x != pt.x) {
			Time		tag_t =
				Arranger()->PixelToTime(pt.x-track_deltax);

			instance->logSem.Lock();
			instance->log.ModifyItemTime(track_streamitem, &tag_t);
			instance->logSem.Unlock();

			BRect lastr=track_rect;
			track_rect.OffsetTo(pt.x-track_deltax,track_rect.top);
			track_msg->ReplaceRect("_Qua_refrect", track_rect);
			BRect	updr(Min(track_rect.left,lastr.left),
						track_rect.top,
						Max(track_rect.right,lastr.right),
						track_rect.bottom);
			BRegion	cr;
			cr.Include(updr);
			ConstrainClippingRegion(&cr);
			Draw(updr);
			ConstrainClippingRegion(nullptr);
			
			track_lastp = pt;
		}
	}
}

void
ArrangerObject::MouseUp(BPoint where)
{
	if (tracking == MOUSE_TRACK_NOT)
		return;
	if (tracking == MOUSE_TRACK_RESIZE) {
		ResizeTo(track_bnd.right-track_bnd.left, track_bnd.bottom-track_bnd.top);
		ReDraw();
	} else if (tracking == MOUSE_TRACK_MOVE_ENV_SEG) {
		ConstrainClippingRegion(nullptr);		// ??? probs if we bail early?
	} else if (tracking == MOUSE_TRACK_MOVE_ENV_SEG) {
		// if dragged off view, delete the item and its image
		if (where.y < 0 || where.y > ObjectHeight()) {
			instance->logSem.Lock();
			instance->log.DeleteItem(track_streamitem);
			instance->logSem.Unlock();

			BRegion	cr;
			cr.Include(track_rect);
			ConstrainClippingRegion(&cr);
			Draw(track_rect);
			ConstrainClippingRegion(nullptr);
		}
	}
	tracking = MOUSE_TRACK_NOT;
}

bool
ArrangerObject::MouseDownControlVariable(BPoint where, ulong buts, ulong mods)
{
	if (	 (mods & B_COMMAND_KEY)
		  && (buts & B_PRIMARY_MOUSE_BUTTON)) {
		MouseDownEnvelopePoint(where, true);
		return true;
	} else if (MouseDownEnvelopePoint(where, false)) {
		return true;
	}
	return false;
}


// FrameMoved()
// Sets up stuff on the mixer window, if necessary.
// change symtab -->(check wierdness)-->
//				MoveTo()-->FrameMoved()-->update mixer
// update mixer window -->change symtab -->...
// update time window --> MessageReceived() --> MoveArrangerObject()
//				-->change symtab --> ...
// all roads lead here. this shouldn't start any.
// set display value just updates mixer window, if necessary.
// make sure it doesn't matter that we don't get a double bunger
// flicker effect, or a loop!!!
void
ArrangerObject::FrameMoved(BPoint parentPoint)
{
	BRect f = Frame();	// seem to get vals in window co-ords!

	if (instance->controlPanel) {
		QuaControllerBridge *cv = (instance&&instance->controlVariables)?
				instance->controlVariables->ControlVariableFor(
					instance->schedulable->starttSym):0;
		if (cv) {
			instance->controlPanel->Window()->Lock();
			cv->SetDisplayValue();
			instance->controlPanel->Window()->Unlock();
		}
		cv = (instance&&instance->controlVariables)?
				instance->controlVariables->ControlVariableFor(
					instance->schedulable->chanSym):0;
		if (cv) {
			instance->controlPanel->Window()->Lock();
			cv->SetDisplayValue();
			instance->controlPanel->Window()->Unlock();
		}
	}
	ObjectView::FrameMoved(parentPoint);
}

void
ArrangerObject::FrameResized(float width, float height)
{
//	fprintf(stderr, "resize %s of %x %x\n", instance && instance->sym? instance->sym->name:"<>", Arranger(), instance->controlPanel);
	instance->duration = Arranger()->PixelToTime(width);
	
	if (instance->controlPanel) {
		QuaControllerBridge *cv = (instance && instance->controlVariables)?
			instance->controlVariables->ControlVariableFor(
					instance->schedulable->durSym):0;
		BWindow	*win = instance->controlPanel->Window();
		if (win && cv) {
			win->Lock();
			cv->SetDisplayValue();
			win->Unlock();
		}
	}
	
}


void
ArrangerObject::MessageReceived(BMessage *inMsg)
{
//	return;
//	fprintf(stderr, "mr %x %x\n", instance, instance->schedulable);
	if (!instance->schedulable->ExtraneousMessage(this, inMsg))
		ObjectView::MessageReceived(inMsg);
}

void
ArrangerObject::Draw(BRect region)
{
	ObjectView::Draw(region);
	if (container && displayedControlVariable) {
		BRect	cb = container->Bounds();
		int	d = (cb.right-cb.left-30);
		int		llbl;
		for (	llbl = labelPoint.x + ((((int32)region.left) / d)) * d;
				llbl < region.right;
				llbl += d) {
			DrawString(
				displayedControlVariable->stabEnt->PreciseName(),
				BPoint(llbl+5,labelPoint.y+10));
		}
	}
	if (instance && instance->schedulable)
		instance->schedulable->DrawExtraneous(this, region);
}

void
ArrangerObject::DrawDisplayedControlVariable(BRect r)
{
	Envelope	*e = displayedControlVariable->envelope;
	if (e) {
		BRegion	regi;
		regi.Include(r);
		ConstrainClippingRegion(&regi);
		
		for (short i=0; i<e->nSeg; i++) {
			DrawEnvSegment(e, i);
		}
		ConstrainClippingRegion(&regi);
		for (short i=0; i<e->nSeg; i++) {
			DrawEnvSegmentPt(e, i);
		}
		ConstrainClippingRegion(nullptr);
	}
}

short
ArrangerObject::ObjectHeight()
{
	return DFLT_OBJECT_HEIGHT;
}

float
ArrangerObject::TimeToPixel(float x)
{
	return x * (Arranger()->fPixelsPerBeat/Arranger()->metric->granularity);
}

float
ArrangerObject::ValToPixel(float y)
{
	return ObjectHeight()*(1.0 - ((y-baseY) / scaleY));
}

float
ArrangerObject::PixelToTime(float x)
{
	return x*(Arranger()->metric->granularity/Arranger()->fPixelsPerBeat);
}

float
ArrangerObject::PixelToVal(float y)
{
	return baseY + ((ObjectHeight()-y)*scaleY/ObjectHeight());
}

void
ArrangerObject::SetEnvelope(Envelope *e)
{
	if (e) {
		float min = e->sym->minVal.FloatValue(nullptr);
		float max = e->sym->maxVal.FloatValue(nullptr);
	
		baseY = min;
		scaleY = (max-min);
		if (scaleY == 0)
			scaleY = 1;
	} else {
		baseY = 0;
		scaleY = 1;
	}
}



bool
ArrangerObject::MouseDownEnvelopePoint(BPoint where, bool addPoint)
{
	Envelope	*e;
	if (!displayedControlVariable || !(e=displayedControlVariable->envelope))
		return false;
		
	Time tTo;
	tTo = PixelToTime(where.x);
	float valTo = PixelToVal(where.y);
	int lastSel = selectedPt;
	
	BRect		editRegion = Bounds();

	editRegion.left++; editRegion.right--;
	editRegion.top++; editRegion.bottom--;
	
	BRegion	regi;regi.Include(editRegion);
	ConstrainClippingRegion(&regi);
		
	if (addPoint) {
		// add point
		selectedPt = e->AddSegment(tTo, valTo, ENV_SEG_TYPE_LINEAR);
		// scrub old
		if (selectedPt != -1) {
//			if (lastSel >= selectedPt)
//				lastSel++;
//	
//			if (lastSel != selectedPt) // undo marking 
				DrawEnvSegmentPt(e, lastSel);
		} else {
			reportError("cannot add identical points to envelope");
			return false;
		}
	} else {
		// select point
		if (!SelectEnvSegmentPoint(e, where)) {
			return false;
		}
		DrawEnvSegmentPt(e, lastSel);
		DrawEnvSegmentPt(e, selectedPt);
	}

	fprintf(stderr, "point %d\n", selectedPt);
//	DisplaySelected();
	
	tracking = MOUSE_TRACK_MOVE_ENV_SEG;
	SetMouseEventMask(B_POINTER_EVENTS, B_NO_POINTER_HISTORY);
	track_envelope = e;
	
	return true;
}


void
ArrangerObject::ZotParameterMenu(BMenu *q, StabEnt *s)
{
	for (short i=0;i<instance->envelopes.CountItems(); i++) {
		Envelope	*e = (Envelope *)instance->envelopes.ItemAt(i);
				
		BMessage	*msg = new BMessage(SET_ENVELOPE);
		msg->AddPointer("control var", e->controlVar);
		
		BMenuItem	*item = new BMenuItem(e->sym->PreciseName(), msg);
		q->AddItem(item);
		item->SetTarget(this);
	}
}

bool
ArrangerObject::MouseDownControlVariableMenu(BPoint where)
{
	BPopUpMenu	*qMenu = new BPopUpMenu("env sel", true, FALSE);
	
	BPoint			orig = where;
	ConvertToScreen(&where);
	
	ZotParameterMenu(qMenu, instance->schedulable->sym);

	qMenu->SetAsyncAutoDestruct(true);
	qMenu->Go(where, true, false, true);
	
	return false;
}


void
ArrangerObject::DrawNewEnvSegment(Envelope *e, short pt)
{
	if (Window()) Window()->Lock();
	DrawEnvSegment(e, pt-1,
			pt==e->nSeg-1?
				-2:pt+1, true);
				
	// new segs
	DrawEnvSegment(e, pt-1);
	DrawEnvSegment(e, pt);
	
	DrawEnvSegmentPt(e, pt-1);
	DrawEnvSegmentPt(e, pt);
	if (Window()) Window()->Unlock();
}

			
void
ArrangerObject::DrawNewEnvSegPosition(Envelope *e, short pt, bool scrub)
{
	if (Window()) Window()->Lock();
	BRect		editRegion = Bounds();

	editRegion.left++; editRegion.right--;
	editRegion.top++; editRegion.bottom--;
	if (pt > 0) {
		float leftPt = TimeToPixel(e->segment[pt-1].time.ticks)-4;
		if (leftPt > editRegion.left)
			editRegion.left = leftPt;
	}
	if (pt < e->nSeg-1) {
		float rightPt = TimeToPixel(e->segment[pt+1].time.ticks)+4;
		if (rightPt < editRegion.right) {
			editRegion.right = rightPt;
		}
	}
	
	BRegion	regi;regi.Include(editRegion);
	ConstrainClippingRegion(&regi);
		
	if (scrub) {
		DrawEnvSegment(e, pt-1, -1, true);
		DrawEnvSegment(e, pt, -1, true);
		DrawEnvSegmentPt(e, pt, true);
	} else {
		// redraw everything
		
		Draw(editRegion);
		ConstrainClippingRegion(&regi);
		
		DrawEnvSegment(e, pt-1);
		DrawEnvSegment(e, pt);
		DrawEnvSegmentPt(e, pt);
	}
	ConstrainClippingRegion(nullptr);
	if (Window()) Window()->Unlock();
}

void
ArrangerObject::Select(bool sel)
{
	if (sel) {
		bgColor = black;
	} else {
		bgColor = color;
	}
	ObjectView::Select(sel);
}