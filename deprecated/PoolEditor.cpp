
#ifdef __INTEL__ 
#include <SupportKit.h>
#include <StorageKit.h>
#include <InterfaceKit.h>
#endif

#include "PoolEditor.h"
#include "Pool.h"
#include "Qua.h"
#include "QuaTypes.h"
#include "ToggleButton.h"
#include "InstancePanel.h"
#include "QuasiStack.h"
#include "Sample.h"
#include "Qua.h"
#include "SymEditPanel.h"
#include "SymEditBlockView.h"
#include "Block.h"
#include "Colors.h"
#include "TimeCtrl.h"
#include "NumCtrl.h"
#include "BlockDrawBits.h"
#include "Method.h"
#include "MyCheckBox.h"
#include "Metric.h"
#include "TakeView.h"


#define PB_WIDTH	40
#define BB_HEIGHT	10

class CtrlBit: public BBitmap
{
public:
	CtrlBit(BRect rect, short ct):
		BBitmap(BRect(
					0,0,
					rect.right-rect.left,rect.bottom-rect.top),
				B_CMAP8,
				true)
	{
		offView = new BView(Bounds(), "offscreen", (ulong)nullptr, (ulong)nullptr);
		offView->SetViewColor(B_TRANSPARENT_32_BIT);
		frame = rect;
		AddChild(offView);
		ConstructBitmap(nullptr);
		controller = ct;
	}
					
	void
	ConstructBitmap(Stream *s)
	{
		Lock();
		uchar	*p = (uchar *)Bits();
		for (int i=0; i<BitsLength(); i++) {
			*p++ = B_TRANSPARENT_8_BIT;
		}
		offView->StrokeRect(Bounds());
		offView->Sync();
		Unlock();
	}
	
	BRect			frame;
	BView			*offView;
	short			controller;
};

class BarBituate: public BView
{
public:
	Metric			*metric;
	short			pixelsPerBeat;
	
	BarBituate(BRect r, Metric *m, int pixy):
		BView(r, "kb", B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW)
	{
		SetFontSize(8);
		SetViewColor(mdGray);
		pixelsPerBeat = pixy;
		metric = m;
	}
	
	virtual void
	Draw(BRect r)
	{
		int	leftBeat = r.left / pixelsPerBeat;
		int	rightBeat = r.right / pixelsPerBeat + 1;
		for (int i=leftBeat; i<rightBeat; i++) {
			char	buf[30];
			sprintf(buf, "%d", i);
			DrawString(buf, BPoint(i*pixelsPerBeat,7));
		}
	}
};

class PianoBituate: public BView
{
public:
	short		yPsPerMidiNoteUnit;
	PianoBituate(BRect r, short yp) :
		BView(r, "kb", B_FOLLOW_NONE, B_WILL_DRAW)
	{
		SetFontSize(8);
		SetViewColor(mdGray);
		yPsPerMidiNoteUnit = yp;
	}
	
	virtual void
	Draw(BRect r)
	{
		int	leftBeat = r.top / yPsPerMidiNoteUnit;
		int	rightBeat = r.bottom / yPsPerMidiNoteUnit + 2;
		
		SetHighColor(black);
		SetLowColor(white);
		for (int i=leftBeat; i<rightBeat; i++) {
			short pitch = (128-i) % 12;
			BRect		keyRect(0,(i-1)*yPsPerMidiNoteUnit,
								20,i*yPsPerMidiNoteUnit);
			switch (pitch) {
			case 0:
			case 2:
			case 4:
			case 5:
			case 7:
			case 9:
			case 11:
				FillRect(keyRect, B_SOLID_LOW);
				StrokeRect(keyRect);
				break;
			case 1:
			case 3:
			case 6:
			case 8:
			case 10:
				FillRect(keyRect, B_SOLID_HIGH);
				break;
			}
			
			char	buf[30];
			sprintf(buf, "%d", 128-i);
			DrawString(buf, BPoint(21, i*yPsPerMidiNoteUnit));
		}
	}
};

//char		*fileClipPath = "/boot/tmp/quip";
//
inline float
StreamView::TickToXPix(long fr)
{
	return fr * (((float)pixelsPerBeat)/((float)metric->granularity));
}

inline float
StreamView::ValueToYPix(short v)
{
	return (128-v)*yPsPerMidiNoteUnit;
}

inline long
StreamView::XPixToTick(float x)
{
	return x * metric->granularity / pixelsPerBeat;
}

inline short
StreamView::YPixToValue(float x)
{
	return 128 - (x / yPsPerMidiNoteUnit);
}

StreamView::StreamView(BRect r, Stream *s, Metric *m, short pb, short pv):
	BView(r, "sample", (ulong)nullptr, (ulong)B_WILL_DRAW|B_FRAME_EVENTS)
{
	tracking = MOUSE_TRACK_NOT;
	
	metric = m;
	width = r.right - r.left;
	height = r.bottom - r.top;
	myStream = s;
	
	selected = nullptr;
	
//	yPsPerChannel = (r.bottom - r.top)/myStream->nChannels;
	yPsPerMidiNoteUnit = pv;
	pixelsPerBeat = pb;

//	leftFrame = 0;
//	currentPlayFrame = 0;
//	rightFrame = XPixToFrame(r.right - r.left);
//	leftDRF = leftDrawFrame = leftFrame;
//	rightDrawFrame = rightFrame;
//	drawReq = false;
	
	doLoop = false;
	
	leftSelectedTime = rightSelectedTime = INFINITE_TICKS;
	
//	currentDrawBuffer = nullptr;
//	currentPlayBuffer = nullptr;
//
//	minPix = new float[myStream->nChannels];
//	maxPix = new float[myStream->nChannels];
//	
//	drawThread = spawn_thread(DrawerWrapper, myStream->sym->name,
//						B_NORMAL_PRIORITY, this);
//	resume_thread(drawThread);
	BuildOtherBits();
}


StreamView::~StreamView()
{
//	status_t	stat;
//	stillHere = false;
//	wait_for_thread(drawThread, &stat);
}

void
StreamView::BuildOtherBits()
{
	for (StreamItem	*p = myStream->head; p!=nullptr; p=p->next) {
		if (  p->type == S_CTRL &&
			  BitForController(((StreamCtrl*)p)->ctrl.controller) == nullptr) {
			CtrlBit	*poo = new CtrlBit(
									BRect(0,0,width,20),
									((StreamCtrl*)p)->ctrl.controller);
			otherBits.AddItem(poo);
		}
	}
}

CtrlBit*
StreamView::BitForController(short ct)
{
	for (short i=0; i<otherBits.CountItems(); i++) {
		CtrlBit	*poo = (CtrlBit *)otherBits.ItemAt(i);
		if (poo->controller == ct)
			return poo;
	}
	return nullptr;
}

CtrlBit*
StreamView::BitAtPoint(BPoint p)
{
	for (short i=0; i<otherBits.CountItems(); i++) {
		CtrlBit	*poo = (CtrlBit *)otherBits.ItemAt(i);
		if (poo->frame.Contains(p))
			return poo;
	}
	return nullptr;
}


void
StreamView::Draw(BRect r)
{
	SetLowColor(ViewColor());
	FillRect(r, B_SOLID_LOW);
	SetDrawingMode(B_OP_COPY);
	short	pixelsPerTick = pixelsPerBeat/metric->granularity;
	DrawGrid(this,
		r,
		pixelsPerTick,
		yPsPerMidiNoteUnit,
		r.left/pixelsPerTick,
			(r.right-r.left)/pixelsPerTick + 2,
		0,
			128,
		ltGray,
		mdGray, metric->granularity, 
		dkGray, metric->beatsPerBar, 
		mdGray,
		mdGray, 0, 
		mdGray, 0
	);


	SetDrawingMode(B_OP_OVER);
	for (int i=0; i<otherBits.CountItems(); i++) {
		CtrlBit	*poo = (CtrlBit *)otherBits.ItemAt(i);
		DrawBitmap(poo, poo->frame.LeftTop());
	}

	SetDrawingMode(B_OP_COPY);
	if (myStream) {
		SetHighColor(black);
		for (StreamItem	*p = myStream->head; p!=nullptr; p=p->next) {
			if (p->type == S_NOTE) {
				BRect	rect = StreamItemRect(p);
				if (rect.Intersects(r)) {
					DrawNote(p, rect);
				}
			}
		}
	}
}

BRect
StreamView::StreamItemRect(StreamItem *p)
{
	switch (p->type) {
	
	case S_NOTE: {
		StreamNote		*nip = (StreamNote *)p;
		float			y = ValueToYPix(((StreamNote *)p)->note.pitch);
		float			tx = TickToXPix(p->time.ticks);
		float			xl = 0;

		if (nip->note.duration == INFINITE_TICKS) {
			switch(nip->note.cmd) {
			
			case CMD_MIDI_KEY_PRESS: {
				StreamNote	*nop = nullptr;
				nop = nip->Subsequent(CMD_MIDI_NOTE_OFF, nip->note.pitch);
				if (nop) {
					xl = TickToXPix(nop->time.ticks - nip->time.ticks);
				} else {
					xl = TickToXPix(nip->note.duration);
				}
				return BRect(tx, y-yPsPerMidiNoteUnit+1, tx+xl, y-1);
			}
			
			case CMD_MIDI_NOTE_ON: {
				StreamNote	*nop = nullptr;
				nop = nip->Subsequent(CMD_MIDI_NOTE_OFF, nip->note.pitch);
				if (nop) {
					xl = TickToXPix(nop->time.ticks - nip->time.ticks);
				} else {
					xl = TickToXPix(nip->note.duration);
				}
				return BRect(tx, y-yPsPerMidiNoteUnit, tx+xl, y);
			}
			
			case CMD_MIDI_NOTE_OFF: {
				return BRect(0,y-yPsPerMidiNoteUnit,0,y);
			}
			
			}
		} else {
			xl = TickToXPix(nip->note.duration);
			return BRect(tx, y-yPsPerMidiNoteUnit, tx+xl, y);
		}
	}
	}
	return BRect(0,0,0,0);
}

void
StreamView::DrawNote(StreamItem *p, BRect r)
{
	switch (p->type) {
	case S_NOTE:
		StreamNote		*nip = (StreamNote *)p;
		
		switch (nip->note.cmd) {
		case CMD_MIDI_NOTE_OFF: {
//			float	pt = TickToXPix(p->time.ticks);
//			rgb_color	col = red;
//			col.green = col.blue = 180 - nip->note.dynamic;
//			SetHighColor(col);
//			StrokeLine(BPoint(pt, r.top+1), BPoint(pt, r.bottom-1));
//			SetHighColor(Darker(col,30));
//			StrokeLine(BPoint(pt+1, r.top+1), BPoint(pt+1, r.bottom-1));
//			break;
		}
		case CMD_MIDI_KEY_PRESS: {	
			rgb_color	col = red;
			col.green = col.blue = 180 - nip->note.dynamic;
			SetHighColor(col);
			FillRect(r);
			SetHighColor(Darker(col,20));
			StrokeLine(BPoint(r.left, r.top), BPoint(r.right, r.top));
			SetHighColor(Darker(col,15));
			StrokeLine(BPoint(r.left, r.top), BPoint(r.left, r.bottom));
			SetHighColor(Lighter(col,25));
			StrokeLine(BPoint(r.left, r.bottom), BPoint(r.right-1, r.bottom));
			SetHighColor(Lighter(col,25));
			StrokeLine(BPoint(r.right-1, r.top), BPoint(r.right-1, r.bottom));
			break;
		}
		case CMD_MIDI_NOTE_ON:
		default: {	
			rgb_color	col = red;
			col.green = col.blue = 180 - nip->note.dynamic;
			SetHighColor(col);
			FillRect(r);
			SetHighColor(Lighter(col,30));
			StrokeLine(BPoint(r.left, r.top), BPoint(r.right, r.top));
			StrokeLine(BPoint(r.left, r.top), BPoint(r.left, r.bottom));
			SetHighColor(Darker(col));
			StrokeLine(BPoint(r.left, r.bottom), BPoint(r.right, r.bottom));
			StrokeLine(BPoint(r.right, r.top), BPoint(r.right, r.bottom));
			break;
		}
		}
		break;
	}
}

void
StreamView::SetStream(Stream *str)
{
	myStream = str;
	Invalidate();
}


void
StreamView::KeyDown(const char *bt, int32 nb)
{
	ulong	mods = modifiers();
	BView::KeyDown(bt,nb);
}

void
StreamView::MessageReceived(BMessage *inMsg)
{
	BView::MessageReceived(inMsg);
}

void
StreamView::MouseDown(BPoint p)
{
	Time	t(XPixToTick(p.x));
	short	pitch = YPixToValue(p.y);
	ulong	buts;
	
	GetMouse(&p, &buts);
	if (myStream) {
		track_noteon = myStream->FindNearestNote(
										t,
										CMD_MIDI_NOTE_ON,
										pitch);
		track_keypress = myStream->FindNearestNote(
										t,
										CMD_MIDI_KEY_PRESS,
										pitch);
		if (track_keypress && track_keypress->time == t) {
			selected = track_keypress;
			
			track_lastr = StreamItemRect(selected);
			track_lastt = selected->time;

			tracking = MOUSE_TRACK_MOVE_KEYPRESS;
			SetMouseEventMask(B_POINTER_EVENTS, B_NO_POINTER_HISTORY);
		} else 	if (track_noteon) {
			selected = track_noteon;
			if (buts & B_SECONDARY_MOUSE_BUTTON) {
// resize note
				track_lastr = StreamItemRect(selected);
				track_delta = track_lastr.LeftTop();
				track_lastdur = -1;
				track_lastpitch = ((StreamNote*)selected)->note.pitch;
				
				myStream->Agglomerate(selected, agglomerate);

				tracking = MOUSE_TRACK_RESIZE_NOTE;
				SetMouseEventMask(B_POINTER_EVENTS, B_NO_POINTER_HISTORY);
				
			} else {
// move note around
				track_lastr = StreamItemRect(selected);
				track_delta = p - track_lastr.LeftTop();
				track_lastt = selected->time;
				track_lastpitch = ((StreamNote*)selected)->note.pitch;
				myStream->Agglomerate(selected, agglomerate);
				
				tracking = MOUSE_TRACK_MOVE_NOTE;
				SetMouseEventMask(B_POINTER_EVENTS, B_NO_POINTER_HISTORY);

			}
		} else if (track_ctrl = BitAtPoint(p)) {
			track_lastr = track_ctrl->frame;
			track_delta = p - track_lastr.LeftTop();

			tracking = MOUSE_TRACK_MOVE_NOTE;
			SetMouseEventMask(B_POINTER_EVENTS, B_NO_POINTER_HISTORY);

				
		} else {
//	long	firstSelectedFrame = XPixToFrame(p.x);
//	
//	Select(firstSelectedFrame, firstSelectedFrame);
//
//	ulong buts;
//
//	long last = firstSelectedFrame;
//				if (fr != last) {
//					last = fr;
//					if (fr < firstSelectedFrame) {
//						if (fr < leftFrame) {
//							leftFrame = fr;
//							ScrollTo(BPoint(FrameToX(leftFrame), 0));
//						}
//						Select(fr, firstSelectedFrame);
//					} else if (fr > firstSelectedFrame) {
//						if (fr > rightFrame) {
//							leftFrame = fr-framesPerPixel*width;
//							ScrollTo(BPoint(FrameToX(leftFrame), 0));
//						}
//						Select(firstSelectedFrame, fr);
//					} else {
//						leftSelectedFrame = rightSelectedFrame = fr;
//					}
//				}
		}
	}
}
			
			


void
StreamView::MouseMoved(BPoint p, uint32 wh, const BMessage *msg)
{
	if (tracking == MOUSE_TRACK_NOT)
		return;
	if (tracking == MOUSE_TRACK_MOVE_KEYPRESS) {
		Time	t = XPixToTick(p.x);

		if (t != track_lastt) {
			myStream->ModifyItemTime(selected,&t);
			BRect r = StreamItemRect(selected);
			r.right = Min(r.right, Bounds().right);
			r.right++;
			BRegion		clip;
			clip.Include(track_lastr);
			clip.Include(r);
			ConstrainClippingRegion(&clip);
			BRect	a = r|track_lastr;
			Draw(a);
			
			ConstrainClippingRegion(nullptr);
			track_lastr = r;
			track_lastt = t;
		}
	} else if (tracking == MOUSE_TRACK_RESIZE_NOTE) {
		p.x = p.x-track_delta.x;
		float dur=XPixToTick(p.x);
		if (track_lastdur != dur && dur > 0) {
			myStream->SetNoteDuration(
								((StreamNote*)selected),
								dur);

			BRect r = StreamItemRect(selected);
			r.right++;
			BRegion		clip;
			clip.Include(track_lastr);
			clip.Include(r);
			ConstrainClippingRegion(&clip);
			BRect	a = r|track_lastr;
			
			Draw(a);

			ConstrainClippingRegion(nullptr);
			track_lastdur = dur;
			track_lastr = r;
		}
	} else if (tracking == MOUSE_TRACK_MOVE_NOTE) {
		p.x = p.x - track_delta.x;	// take into account touch point of note

		Time t = XPixToTick(p.x);
		pitch_t pitch = YPixToValue(p.y);

		if (t != track_lastt || pitch != track_lastpitch) {
			myStream->MoveNoteAgglomerate(
						agglomerate,t,pitch);
			BRect r = StreamItemRect(selected);
			r.right++;
			BRegion		clip;
			clip.Include(track_lastr);
			clip.Include(r);
			ConstrainClippingRegion(&clip);
			BRect	a = r|track_lastr;

			Draw(a);
			
			ConstrainClippingRegion(nullptr);
			track_lastr = r;
			track_lastt = t;
			track_lastpitch = pitch;
		}
	} else if (tracking == MOUSE_TRACK_MOVE_CTRL) {
		p.y = p.y-track_delta.y;
		track_ctrl->frame.OffsetTo(0,p.y);
		BRect	r = track_ctrl->frame;
		if (track_lastr != r) {
			BRegion		clip;
			clip.Include(track_lastr);
			clip.Include(r);
			ConstrainClippingRegion(&clip);
			BRect	a = r|track_lastr;
			
			Draw(a);
			
			ConstrainClippingRegion(nullptr);
			track_lastr = r;
		}
	}
}


void
StreamView::MouseUp(BPoint where)
{
	if (tracking == MOUSE_TRACK_NOT)
		return;
	if (tracking == MOUSE_TRACK_MOVE_KEYPRESS) {
		if (	track_noteon &&
				(track_keypress->time <track_noteon->time ||
				track_keypress->time > ((StreamNote*)track_noteon)->Duration())) {
			BRect	a = StreamItemRect(track_keypress);
			a.right = Min(a.right+1, Bounds().right);
			myStream->DeleteItem(track_keypress);
			BRegion		clip;
			clip.Include(a);
			ConstrainClippingRegion(&clip);
			
			Draw(a);

			ConstrainClippingRegion(nullptr);
		}
	} else if (tracking == MOUSE_TRACK_RESIZE_NOTE) {
		bool	clearup = false;
		for (int i=0; i<agglomerate.CountItems(); i++) {
			StreamItem	*ap = (StreamItem*)agglomerate.ItemAt(i);
			if (ap->time > selected->time+track_lastdur) {
				BRect	a = StreamItemRect(ap);
				a.right = Min(a.right+1, Bounds().right);
				myStream->DeleteItem(ap);
				BRegion		clip;
				clip.Include(a);
				ConstrainClippingRegion(&clip);
				
				Draw(a);

				ConstrainClippingRegion(nullptr);
			}
		}
	} else if (tracking == MOUSE_TRACK_MOVE_NOTE) {
	} else if (tracking == MOUSE_TRACK_MOVE_CTRL) {
	}
	tracking = MOUSE_TRACK_NOT;
}

void
StreamView::Select(long lfr, long rfr)
{
//	if (lfr < 0) lfr = 0; else if (lfr >= myPool->nFrames) lfr = myStream->nFrames-1;
//	if (rfr < 0) rfr = 0; else if (rfr >= myPool->nFrames) rfr = myStream->nFrames-1;
//	drawLock.Lock();
	SetDrawingMode(B_OP_INVERT);
	FillRect(BRect(	TickToXPix(leftSelectedTime.ticks), 0,
					TickToXPix(rightSelectedTime.ticks), height));

	leftSelectedTime = lfr;
	rightSelectedTime = rfr;

	FillRect(BRect(	TickToXPix(leftSelectedTime.ticks), 0,
					TickToXPix(rightSelectedTime.ticks), height));
	PoolEditor	*par = (PoolEditor*)Parent();
	par->lSelView->SetValue(lfr);
	par->rSelView->SetValue(rfr);
//	drawLock.Unlock();
}


//status_t
//StreamView::DrawCurrentPixel()
//{
////	fprintf(stderr, "pix %d %d %g\n", leftDrawFrame, rightDrawFrame, FrameToX(leftDrawFrame));
//
//	if (Window() == nullptr) {
//		return B_NO_ERROR;
//	}
//	
//	if (	leftDrawFrame > rightDrawFrame ||
//			leftDrawFrame+framesPerPixel > myStream->nFrames) {
//		snooze(1000);
//		return B_NO_ERROR;
//	}
//	
//	for (short i=0; i<myStream->nChannels; i++) {
//		minPix[i] = maxPix[i] = 0;
//	}
//	
//	for (int i=0; i<framesPerPixel; i++) {
//		long	curFrame = leftDrawFrame+i;
//		if (!currentDrawBuffer || !currentDrawBuffer->HasFrame(curFrame)) {
//			currentDrawBuffer = myStream->BufferForFrame(curFrame);
//			if (!currentDrawBuffer) {
//				snooze(1000);
////				fprintf(stderr, "skipping\n");
//				return B_NO_ERROR;
//			}
//		}
//		for (short j=0; j<myStream->nChannels; j++) {
//			float	sam = currentDrawBuffer->data[
//								((curFrame - currentDrawBuffer->fromFrame)*
//										myStream->nChannels)
//								 + i];
//			if (sam < minPix[j])
//				minPix[j] = sam;
//			if (sam > maxPix[j])
//				maxPix[j] = sam;
//		}
//	}
//		
//	if (Window()) Window()->Lock();
//	drawLock.Lock();
//	if (leftDrawFrame >= leftSelectedFrame && leftDrawFrame <= rightSelectedFrame) {
//		SetHighColor(white);
//		SetLowColor(black);
//	} else {	
//		SetHighColor(black);
//		SetLowColor(white);
//	}
//	SetDrawingMode(B_OP_COPY);
//	StrokeLine(
//		BPoint(FrameToX(leftDrawFrame), 0),
//		BPoint(FrameToX(leftDrawFrame), height), B_SOLID_LOW);
//	for (short i=0; i<myStream->nChannels; i++) {
//		StrokeLine(
//			BPoint(FrameToX(leftDrawFrame), StreamToY(i,maxPix[i])),
//			BPoint(FrameToX(leftDrawFrame), StreamToY(i,minPix[i])));
//	}
//	leftDrawFrame+=framesPerPixel;
//	
//	drawLock.Unlock();
//	if (Window()) Window()->Unlock();
//	return B_NO_ERROR;
//}


void
StreamView::FrameResized(float x, float y)
{
	width = x;
	height = y;
}

void
StreamView::FrameMoved(BPoint pt)
{
	BView::FrameMoved(pt);
}

void
StreamView::ScrollTo(BPoint pt)
{
	PoolEditor	*po = (PoolEditor *)Parent();
	po->barbituate->ScrollTo(pt.x, 0);
	po->pianobituate->ScrollTo(0, pt.y);
	BView::ScrollTo(pt);
}

//size_t
//StreamView::Generate(float *outSig, long nFramesReqd, short nChannels)
//{
//	long		outFrame = 0;
//	long		loopStartFrame = 0;
//	long		loopEndFrame = sampleDuration-1;
//	while (outFrame < nFramesReqd) {
//		if (currentPlayFrame < loopStartFrame) {
//			currentPlayFrame = loopStartFrame;
//		} else if (currentPlayFrame > loopEndFrame) {
//			if (!doLoop)
//				break;
//			currentPlayFrame = loopStartFrame;
//				// other loop startish things ...
////			LoopEnvelopes();
//		}
//
//		if (  currentPlayFrame < 0 ||
//			  currentPlayFrame >= myStream->nFrames) {	// a frame of nothing
//			break;
//		} else {
//			if (!currentPlayBuffer || !currentPlayBuffer->HasFrame(currentPlayFrame)) {
//				currentPlayBuffer = myStream->BufferForFrame(currentPlayFrame);
//				if (currentPlayBuffer == nullptr) {
//					fprintf(stderr, "break\n");
//					break;
//				}				
//			}
//
////			get crap from current frame
//
//			int bufFrame = (currentPlayFrame - currentPlayBuffer->fromFrame);
//			size_t	nFramesAvail = min32(nFramesReqd-outFrame,
//										currentPlayBuffer->nFrames-bufFrame);
//			nFramesAvail = min32(nFramesAvail,
//								loopEndFrame-currentPlayFrame+1);
//
//			memcpy(	&outSig[outFrame*myStream->nChannels],
//					&currentPlayBuffer->data[bufFrame*myStream->nChannels],
//					nFramesAvail*myStream->nChannels*sizeof(float));
//			outFrame += nFramesAvail;
//			currentPlayFrame += nFramesAvail;
//		}
//	}
//	return outFrame;
//}


PoolEditor::PoolEditor(BRect rr, float maxw, Pool *p):
	DataEditor(rr,maxw, p, p->uberQua->metric, p->duration.ticks)
{
	BRect		wrect;
	
	myPool = p;
	isPreviewing = false;
//	isRendering = false;
	
	BRect r = previewButton->Frame();
	BRect ebf = editBar->Frame();
	BRect	t = filterEdit->Frame();
	
//	wrect.Set(MIXER_MARGIN+10, ebf.bottom+MIXER_MARGIN,
//			 30, ebf.bottom+MIXER_MARGIN+10);
//	statusView = new RenderStatusView(wrect, "status");
//	statusView->SetStatusValue(101);
//	AddChild(statusView);
//	BRect	svf = statusView->Frame();
//	
	wrect.Set(ebf.right+MIXER_MARGIN, r.bottom+MIXER_MARGIN, ebf.right+40, r.bottom+MIXER_MARGIN+18);
	takeView = new StreamTakeView(wrect, myPool);
	takeView->SetResizingMode(B_FOLLOW_TOP|B_FOLLOW_LEFT);
	takeView->SetFont(be_bold_font);
	takeView->SetFontSize(12);
	takeView->SetViewColor(mdGray);
	AddChild(takeView);
	BRect tf = takeView->Frame();
	
	barbituate = new BarBituate(
						BRect(0,0,
							rr.right-rr.left-2*MIXER_MARGIN
								- B_V_SCROLL_BAR_WIDTH - 1 - PB_WIDTH,
							BB_HEIGHT),
					p->metric,
					24
					);
	barbituate->SetResizingMode(B_FOLLOW_TOP|B_FOLLOW_LEFT);
	barbituate->MoveTo(PB_WIDTH,Max(tf.bottom, t.bottom)+MIXER_MARGIN);
	AddChild(barbituate);
	BRect bbf = barbituate->Frame();
	
	pianobituate = new PianoBituate(
						BRect(0,0,
							PB_WIDTH,
							rr.bottom-rr.top-MARGIN - B_H_SCROLL_BAR_HEIGHT - 1 - bbf.bottom),
					10
					);
	pianobituate->SetResizingMode(B_FOLLOW_TOP|B_FOLLOW_LEFT);
	pianobituate->MoveTo(MIXER_MARGIN,bbf.bottom);
	AddChild(pianobituate);
	BRect pbf = pianobituate->Frame();
	
	wrect.Set(pbf.right,
			bbf.bottom,
			rr.right-rr.left-MIXER_MARGIN - B_V_SCROLL_BAR_WIDTH - 1,
			rr.bottom-rr.top-MARGIN - B_H_SCROLL_BAR_HEIGHT - 1);
	streamView = new StreamView(wrect, myPool->mainStream, myPool->metric, 24, 10);
	AddChild(streamView);

	wrect.Set( pbf.right,
				rr.bottom-rr.top-MIXER_MARGIN - B_H_SCROLL_BAR_HEIGHT,
				rr.right-rr.left-MIXER_MARGIN - B_V_SCROLL_BAR_WIDTH-1,
				rr.bottom-rr.top-MIXER_MARGIN);
    timeScroll = new BScrollBar(wrect, "scroll", streamView,
               0, 30000, B_HORIZONTAL);
    timeScroll->SetResizingMode(B_FOLLOW_NONE);
    timeScroll->SetRange(0,streamView->TickToXPix(myPool->duration.ticks)
    							- streamView->width);
    AddChild(timeScroll);
	
	wrect.Set(	rr.right-rr.left-MIXER_MARGIN - B_V_SCROLL_BAR_WIDTH,
				bbf.bottom,
				rr.right-rr.left-MIXER_MARGIN,
				rr.bottom-rr.top-MARGIN - B_H_SCROLL_BAR_HEIGHT-1);
    valueScroll = new BScrollBar(wrect, "scroll", streamView,
               0, 30000, B_VERTICAL);
    valueScroll->SetResizingMode(B_FOLLOW_NONE);
    valueScroll->SetRange(0,streamView->ValueToYPix(0)-streamView->height);
    AddChild(valueScroll);
	
	ArrangeChildren();
}

void
PoolEditor::Draw(BRect r)
{
	DataEditor::Draw(r);
//	LoweredBox(this, statusView->Frame(), ViewColor(), false);
}

PoolEditor::~PoolEditor()
{
}

void
PoolEditor::ArrangeChildren()
{
	origH = myHeight;
	origW = myWidth;
	
//	BRect	sv = statusView->Frame();
	BRect	tv = takeView->Frame();
	filterEdit->MoveTo(tv.right+MIXER_MARGIN, tv.top);
	BRect	t = filterEdit->Frame();
	BRect	nvr = filterControls->Frame();
	
	filterControls->maxWidth = myWidth-2*MIXER_MARGIN-t.right;
	filterControls->MoveTo(t.right + MIXER_MARGIN, t.top);
	float h = Max(tv.bottom, t.bottom);
//	h = Max(h, sv.bottom);
	h = Max(h, nvr.bottom) + MIXER_MARGIN;

	BRect	svr = barbituate->Frame();
	if (h != svr.top) {
		Window()->ResizeBy(0,h-svr.top);
	}
	
	BRect	pgh = pianobituate->Frame();
	barbituate->MoveTo(pgh.right, h);
	BRect	sgh = barbituate->Frame();
	pianobituate->MoveTo(MIXER_MARGIN, sgh.bottom);
	streamView->MoveTo(pgh.right, sgh.bottom);
	BRect	s = streamView->Frame();
	timeScroll->MoveTo(pgh.right, s.bottom+1);
	valueScroll->MoveTo(s.right+1, s.top);
	
	myHeight = s.bottom +1+ MIXER_MARGIN + B_H_SCROLL_BAR_HEIGHT;
	ResizeTo(myWidth, myHeight);
}

void
PoolEditor::AttachedToWindow()
{
	previewButton->SetTarget(this);
	loopButton->SetTarget(this);
	durView->SetTarget(this);
	lSelView->SetTarget(this);
	rSelView->SetTarget(this);
	editMenu->SetTargetForItems(this);
}

void
PoolEditor::FrameResized(float x, float y)
{
	myHeight = y;
	myWidth = x;
	BView *v;
	if ((v=Parent()) == nullptr) {
	} else if (strcmp(v->Name(),"back") == 0) {
		if (myHeight == origH) {
			BRect s = streamView->Frame();
			streamView->ResizeTo(
							myWidth-MIXER_MARGIN-s.left-B_V_SCROLL_BAR_WIDTH-1,
							myHeight-MIXER_MARGIN-s.top-B_H_SCROLL_BAR_HEIGHT-1);
			s = streamView->Frame();
			pianobituate->MoveTo(MIXER_MARGIN, s.top);
			pianobituate->ResizeTo(PB_WIDTH,s.bottom-s.top);
			timeScroll->ResizeTo(s.right-s.left,B_H_SCROLL_BAR_HEIGHT);
			valueScroll->ResizeTo(B_V_SCROLL_BAR_WIDTH,s.bottom-s.top);
		    timeScroll->SetRange(0,
    					streamView->TickToXPix(myPool->duration.ticks)
    						-(s.right-s.left));
		    valueScroll->SetRange(0,
    					streamView->ValueToYPix(0)
    						-(s.bottom-s.top));
 			s = streamView->Frame();
 			timeScroll->MoveTo(s.left, s.bottom+1);
 			valueScroll->MoveTo(s.right+1, s.top);
 			s = filterEdit->Frame();
			filterControls->maxWidth = myWidth-2*MIXER_MARGIN-s.right;
		} else {
		}
	}
	Invalidate();
}

void
PoolEditor::MessageReceived(BMessage *inMsg)
{
	switch(inMsg->what) {
	case PREVIEW_LOOP: {
		streamView->doLoop = loopButton->Value();
		break;
	}
	case PREVIEW_REGION: {
		Preview(inMsg->FindInt32("state") == STATUS_RUNNING);
		break;
	}
	
	case SELECT_ALL: {
		streamView->Select(0, myPool->duration.ticks);
		break;
	}
	
	case RENDER_REGION: {
		Render(true);
		break;
	}
	
	case HALT_RENDER: {
		Render(false);
		break;
	}
	
	case REGION_SELECT: {
		if (lSelView->TickValue() <= rSelView->TickValue())
			streamView->Select(lSelView->TickValue(), rSelView->TickValue());
		break;
	}
	
	case SAMPLE_DURATION: {
		myPool->selectedTake->duration = myPool->duration = durView->TickValue();
		break;
	}
	
	case SELECT_PREVIW_TAKE: {
		break;
	}
	
	case SELECT_MAIN_TAKE: {
		break;
	}
	
	case COPY_REGION: {
		Copy();
		break;
	}
	case CUT_REGION: {
		Cut();
		break;
	}
	case PASTE_REGION: {
		Paste();
		break;
	}
	case CLEAR_REGION: {
		Clear();
		break;
	}
	case DELETE_REGION: {
		Delete();
		break;
	}
	case CROP_REGION: {
		Crop();
		break;
	}
	case OVERWRITE_REGION: {
		Overwrite();
		break;
	}
	
	default:
		DataEditor::MessageReceived(inMsg);
	}
}


//size_t
//PoolEditor::Generate(float *outSig, long nFramesReqd, short nChannels)
//{
//	long		outFrame = sampleView->Generate(outSig, nFramesReqd, nChannels);
//
//	for (short i=outFrame; i<nFramesReqd; i++) {
//		outSig[2*i] = 0;
//		outSig[2*i+1] = 0;
//	}
//
//	if (mySample->nChannels == 2) {
//		for (short i=0; i<nFramesReqd; i++) {
//			outSig[2*i] = outSig[2*i];
//			outSig[2*i+1] = outSig[2*i+1];
//		}
//	} else if (mySample->nChannels == 1) {
//		for (short i=nFramesReqd-1; i>=0; i--) {
//			outSig[2*i] = outSig[i];
//			outSig[2*i+1] = outSig[i];
//		}
//	}
//	schlock.Lock();
//	if (offlineFilter->mainBlock && filterStack->locusStatus == STATUS_RUNNING) {
//		theTime = sampleView->currentPlayFrame;
//		UpdateEnvelopes(theTime);
//		Stream	s;
//		bool	uac = UpdateActiveBlock(
//						mySample->uberQua,
//						&s,
//						offlineFilter->mainBlock,
//						1,
//						&theTime,
//						TRUE, this,
//						filterStack);
//		s.ClearStream();
//		ApplyQuaFX(	filterStack,
//					offlineFilter->mainBlock,
//					outSig,
//					nFramesReqd,
//					2);
//	}			
//	schlock.Unlock();
//	return nFramesReqd;
//}

bool
PoolEditor::QuitRequested()
{
	Preview(false);
//	status_t	stat;
//	sampleView->stillHere = false;
//	wait_for_thread(sampleView->drawThread, &stat);
	return true;
}

status_t
PoolEditor::Preview(bool start)
{
	if (start) {
		if (!isPreviewing) {
			streamView->currentPlayTime = 0;
//			mySample->uberQua->sampler->StartPreview(this);
			isPreviewing = true;
		}
	} else {
		if (isPreviewing) {
//			mySample->uberQua->sampler->StopPreview(this);
			isPreviewing = false;
		}
	}
	return B_NO_ERROR;
}

//
//long
//PoolEditor::RenderWrapper(void *data)
//{
//	return ((PoolEditor *)data)->Renderer();
//}

//long
//PoolEditor::Renderer()
//{
//	long			currentRenderFrame = 0;
//	short			percentComplete = 0;
//	Time			rendTime(0, &SampleRateMetric);
//	
//	statusView->SetStatusValue(0);
//	
//	mySample->flock.RdLock();	// must!!  be a reader lock else buffers clag
//	
//	SampleFile		inFile(mySample->sampleFile->fileType);
//	inFile = *mySample->sampleFile;
//	if (inFile.InitCheck() != B_NO_ERROR) {
//		reportError("Renderer: can't recreate input file");
//		return B_ERROR;
//	}
//
//	int				takeno;
//	BPath			path;
//	SampleFile		*outFile = mySample->NewTakeFile(takeno, path);
//	if (outFile == nullptr) {
//		return B_ERROR;
//	}
//	
//#define framesPerBuffer	1024
//	char	*fileBuffer = new char[mySample->sampleSize
//									* mySample->nChannels
//									* framesPerBuffer];
//	float	*workBuffer = new float[framesPerBuffer
//									* mySample->nChannels
//									* sizeof(float)];								
//	while (isRendering && currentRenderFrame < sampleView->sampleDuration) {
//		// get next buffer
//		long		framesAvail = min32(mySample->nFrames-currentRenderFrame,
//										framesPerBuffer);
//		long		bytesAvail = framesAvail*mySample->sampleSize*mySample->nChannels;
//		long		nBytesRead = inFile.Read(fileBuffer, bytesAvail);
//		if (nBytesRead != bytesAvail) {
//			reportError("qua: wird read %d and not %d", nBytesRead, bytesAvail);
//		}
//		long		framesRead = nBytesRead / (mySample->sampleSize*mySample->nChannels);
//		inFile.NormalizeInputCpy(workBuffer, fileBuffer, framesAvail);
//		long		framesToWrite = min32(	framesRead,
//									 		sampleView->sampleDuration-
//									 			currentRenderFrame);
//		long		bytesToWrite =	framesToWrite*
//									mySample->sampleSize*
//									mySample->nChannels;
//		for (int i=framesRead*mySample->nChannels;
//				 i<framesToWrite*mySample->nChannels;
//				 i++) {
//			workBuffer[i] = 0;
//		}
//		// process
//		schlock.Lock();
//		if (offlineFilter->mainBlock && filterStack->locusStatus == STATUS_RUNNING) {
//			rendTime = currentRenderFrame;
//			UpdateEnvelopes(currentRenderFrame);
//			Stream	s;
//			bool	uac = UpdateActiveBlock(
//							mySample->uberQua,
//							&s,
//							offlineFilter->mainBlock,
//							1,
//							&rendTime,
//							TRUE, this,
//							filterStack);
//			s.ClearStream();
//			ApplyQuaFX(	filterStack,
//						offlineFilter->mainBlock,
//						workBuffer,
//						framesToWrite, mySample->nChannels);
//		}			
//		schlock.Unlock();
//		// gack it out.
//		outFile->NormalizeOutputCpy(fileBuffer, workBuffer, framesToWrite);
//		outFile->Write(fileBuffer, bytesToWrite);
//		
//		if ((currentRenderFrame*100)/sampleView->sampleDuration > percentComplete) {
//			percentComplete = (currentRenderFrame*100)/sampleView->sampleDuration;
//			statusView->SetStatusValue(percentComplete);
//		}
//	}
//	
//	delete fileBuffer;
//	delete workBuffer;
//	
//	if (!isRendering) {
//		BEntry		ent(path.Path(), true);
//		ent.Remove();
//		delete outFile;
//	} else {
//		char		takenm[MAX_QUA_NAME_LENGTH];
//		sprintf(takenm, "Take %d", takeno);
//		
//		SampleTake	*newTake = mySample->AddSampleTake(
//										outFile, takenm, path.Path());
//			// safe? this is the only reader to mod this list
//		takeView->AddTake(newTake);
//	}
//	
//	mySample->flock.RdLock();
//	
//	statusView->SetStatusValue(101);
//
//	return B_NO_ERROR;
//}

status_t
PoolEditor::Render(bool doit)
{
	status_t err = B_NO_ERROR;
//	if (doit) {
//		if (!isRendering) {
//			isRendering = true;
//		    renderThread = spawn_thread(RenderWrapper, "rendererer",
//								B_NORMAL_PRIORITY, this);
//			if (renderThread < B_NO_ERROR)
//				return renderThread;
//			err = resume_thread(renderThread);
//		}
//	} else {
//		isRendering = false;
//		wait_for_thread(renderThread, &err);
//	}
	return err;
}

status_t
PoolEditor::Copy()
{
	status_t		err = B_NO_ERROR;
	return err;
}

status_t
PoolEditor::Cut()
{
	status_t		err = B_NO_ERROR;
	return err;
}

status_t
PoolEditor::Paste()
{
	status_t		err = B_NO_ERROR;
	return B_NO_ERROR;
}

status_t
PoolEditor::Overwrite()
{
	status_t		err = B_NO_ERROR;
	return err;
}

status_t
PoolEditor::Clear()
{
	status_t		err = B_NO_ERROR;
	return err;
}

status_t
PoolEditor::Delete()
{
	status_t		err = B_NO_ERROR;
	return err;
}

status_t
PoolEditor::Crop()
{
	status_t		err = B_NO_ERROR;
	return err;
}
