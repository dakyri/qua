
#ifdef __INTEL__ 
#include <SupportKit.h>
#include <StorageKit.h>
#include <InterfaceKit.h>
#endif

#include "SampleEditor.h"
#include "ScrawleBar.h"
#include "SymObject.h"
#include "QuaTypes.h"
#include "ToggleButton.h"
#include "InstancePanel.h"
#include "QuasiStack.h"
#include "Sample.h"
#include "Qua.h"
#include "SymEditPanel.h"
#include "SymEditBlockView.h"
#include "Block.h"
#include "Shortlist.h"
#include "Colors.h"
#include "Sampler.h"
#include "QuaFX.h"
#include "TimeCtrl.h"
#include "NumCtrl.h"
#include "Method.h"
#include "MyCheckBox.h"
#include "Metric.h"
#include "SampleFile.h"
#include "TakeView.h"
#include "BlockDrawBits.h"
#include "RenderStatusView.h"

char		*fileClipPath = "/boot/tmp/quip";

SampleView::SampleView(BRect r, Sample *s):
	ScrawleView(r, "sample", (ulong)nullptr, (ulong)B_WILL_DRAW|B_FRAME_EVENTS)
{
	tracking = MOUSE_TRACK_NOT;
	
	width = r.right - r.left;
	height = r.bottom - r.top;
	scaleX = 4;
	mySample = s;
	editTake = s->selectedTake;
	sampleDuration = mySample->selectedNFrames;
	framesPerPixel = mySample->selectedNFrames/(scaleX*width);
	if (framesPerPixel == 0)
		framesPerPixel = 1;
	yPsPerChannel = (r.bottom - r.top)/(editTake?editTake->file->nChannels:2);
	
	editTake = s->selectedTake;

	leftFrame = 0;
	currentPlayFrame = 0;
	rightFrame = XPixToFrame(r.right - r.left);
	leftDRF = leftDrawFrame = leftFrame;
	rightDrawFrame = rightFrame;
	drawReq = false;
	
	doLoop = false;
	
	leftSelectedFrame = rightSelectedFrame = -1;
	
	currentDrawBuffer = nullptr;
	currentPlayBuffer = nullptr;

	minPix = new float[4];//editTake->file->nChannels];
	maxPix = new float[4];//editTake->file->nChannels];
	
	drawThread = spawn_thread(DrawerWrapper, mySample->sym->name,
						B_NORMAL_PRIORITY, this);
	resume_thread(drawThread);
}

SampleView::~SampleView()
{
	status_t	stat;
	stillHere = false;
	wait_for_thread(drawThread, &stat);
}

long
SampleView::DrawerWrapper(void *data)
{
	return ((SampleView *)data)->DrawSampleBits();
}


void
SampleView::Draw(BRect r)
{
	leftDRF = XPixToFrame(r.left);
	drawReq = true;
}

void
SampleView::SetupDisplay()
{
	drawLock.Lock();
	framesPerPixel = mySample->selectedNFrames/(scaleX*width);
	if (framesPerPixel == 0)
		framesPerPixel = 1;
	leftDrawFrame = leftFrame = 0;
	rightDrawFrame = rightFrame = XPixToFrame(width);
	drawLock.Unlock();
}

void
SampleView::Refresh()
{
	leftDRF = leftDrawFrame;
	drawReq = true;
}

long
SampleView::DrawSampleBits()
{
	stillHere = true;
	while (stillHere) {
		if (drawReq && leftDRF < leftDrawFrame) {
			leftDrawFrame = leftDRF;
			drawReq = false;
		}
		if (leftDrawFrame < leftFrame) {
			leftDrawFrame = leftFrame;
		}
		DrawCurrentPixel();
	}
	return B_NO_ERROR;
}

void
SampleView::KeyDown(const char *bt, int32 nb)
{
	ulong	mods = modifiers();
	BView::KeyDown(bt,nb);
}

void
SampleView::MessageReceived(BMessage *inMsg)
{
	BView::MessageReceived(inMsg);
}

void
SampleView::MouseDown(BPoint p)
{
	track_first_frame = XPixToFrame(p.x);
	
	Select(track_first_frame, track_first_frame);

	ulong buts;
	track_oldpr = Window()->PulseRate();
	Window()->SetPulseRate(0);

	tracking = MOUSE_TRACK_SELECT_REGION;
	SetMouseEventMask(B_POINTER_EVENTS, B_NO_POINTER_HISTORY);
}


void
SampleView::MouseMoved(BPoint pt, uint32 wh, const BMessage *msg)
{
	if (tracking == MOUSE_TRACK_NOT)
		return;
	if (tracking == MOUSE_TRACK_SELECT_REGION) {
		long fr = XPixToFrame(pt.x);
		if (fr < track_first_frame) {
			if (fr < leftFrame) {
				leftFrame = fr;
				ScrollTo(BPoint(FrameToX(leftFrame), 0));
			}
			Select(fr, track_first_frame);
		} else if (fr > track_first_frame) {
			if (fr > rightFrame) {
				leftFrame = fr-framesPerPixel*width;
				ScrollTo(BPoint(FrameToX(leftFrame), 0));
			}
			Select(track_first_frame, fr);
		} else {
			leftSelectedFrame = rightSelectedFrame = fr;
		}
	}
}

void
SampleView::MouseUp(BPoint where)
{
	if (tracking == MOUSE_TRACK_NOT)
		return;
	if (tracking == MOUSE_TRACK_SELECT_REGION) {
		Window()->SetPulseRate(track_oldpr);
	}
	tracking = MOUSE_TRACK_NOT;
}

void
SampleView::Select(long lfr, long rfr)
{
	if (lfr < 0) lfr = 0; else if (lfr >= editTake->file->nFrames) lfr = editTake->file->nFrames-1;
	if (rfr < 0) rfr = 0; else if (rfr >= editTake->file->nFrames) rfr = editTake->file->nFrames-1;
	drawLock.Lock();
	SetDrawingMode(B_OP_INVERT);

	bool	redraw = true;
	
	if (leftSelectedFrame < rightSelectedFrame) {
		if ((rfr > rightDrawFrame && rightSelectedFrame > rightDrawFrame)
		   &&(lfr < leftDrawFrame && leftSelectedFrame < leftDrawFrame)) {
			redraw = false;
		}
	}
		
	if (redraw) {
		FillRect(BRect(	FrameToX(leftSelectedFrame), 0,
						FrameToX(rightSelectedFrame), height));

		leftSelectedFrame = lfr;
		rightSelectedFrame = rfr;

		FillRect(BRect(	FrameToX(leftSelectedFrame), 0,
						FrameToX(rightSelectedFrame), height));
	} else {
		leftSelectedFrame = lfr;
		rightSelectedFrame = rfr;
	}
	
	SampleEditor	*par = (SampleEditor*)Parent();
	par->lSelView->SetValue(lfr);
	par->rSelView->SetValue(rfr);
	drawLock.Unlock();
}


status_t
SampleView::DrawCurrentPixel()
{
//	fprintf(stderr, "pix lf %d ldf %d rdf %d x %g", leftFrame, leftDrawFrame, rightDrawFrame, FrameToX(leftDrawFrame));

	if (Window() == nullptr) {
		return B_NO_ERROR;
	}
	
	if (editTake == nullptr) {
		return B_NO_ERROR;
	}
	
	if (leftDrawFrame > rightDrawFrame) {
		snooze(1000);
		return B_NO_ERROR;
	}
	if(leftDrawFrame+framesPerPixel > editTake->file->nFrames) {
		BRect	r(0,0,0,height);
		r.left = FrameToX(editTake->file->nFrames);
		r.right = r.left + width;
		if (Window()) Window()->Lock();
		SetHighColor(white);
		FillRect(r);
		if (Window()) Window()->Unlock();
		leftDrawFrame = editTake->file->nFrames;
		snooze(1000);
		return B_NO_ERROR;
	}
	
	for (short i=0; i<editTake->file->nChannels; i++) {
		minPix[i] = maxPix[i] = 0;
	}
	
	for (int i=0; i<framesPerPixel; i++) {
		long	curFrame = leftDrawFrame+i;
		if (!currentDrawBuffer || !currentDrawBuffer->HasFrame(editTake->file, curFrame)) {
			currentDrawBuffer = mySample->BufferForFrame(editTake->file, curFrame);
			if (!currentDrawBuffer) {
				snooze(1000);
//				fprintf(stderr, "skipping\n");
				return B_NO_ERROR;
			}
		}

		for (short j=0; j<editTake->file->nChannels; j++) {
			float	sam = currentDrawBuffer->data[
								((curFrame - currentDrawBuffer->fromFrame)*
										editTake->file->nChannels)
								 + j];
			if (sam < minPix[j])
				minPix[j] = sam;
			if (sam > maxPix[j])
				maxPix[j] = sam;
		}
	}
//	fprintf(stderr, "min %g %g max %g %g\n", minPix[0], minPix[1], maxPix[0], maxPix[1]);
	if (Window()) Window()->Lock();
	drawLock.Lock();
	if (leftDrawFrame >= leftSelectedFrame && leftDrawFrame <= rightSelectedFrame) {
		SetHighColor(Inverse(mySample->symObject->color));
		SetLowColor(black);
	} else {	
		SetHighColor(mySample->symObject->color);
		SetLowColor(white);
	}
	SetDrawingMode(B_OP_COPY);
	StrokeLine(
		BPoint(FrameToX(leftDrawFrame), 0),
		BPoint(FrameToX(leftDrawFrame), height), B_SOLID_LOW);
		
	SetDrawingMode(B_OP_BLEND);
	for (short i=0; i<editTake->file->nChannels; i++) {
		StrokeLine(
			BPoint(FrameToX(leftDrawFrame), SampleToY(i,maxPix[i])),
			BPoint(FrameToX(leftDrawFrame), SampleToY(i,minPix[i])));
	}
	leftDrawFrame+=framesPerPixel;
	
	drawLock.Unlock();
	if (Window()) Window()->Unlock();
	return B_NO_ERROR;
}


void
SampleView::FrameResized(float x, float y)
{
	if (width != x) {
		width = x;
		framesPerPixel = mySample->selectedNFrames/(scaleX*width);
		if (framesPerPixel == 0)
			framesPerPixel = 1;
		SampleEditor *e = (SampleEditor *)Parent();
		e->sampleScroll->SetRange(0,
		    		FrameToX(e->editTake->file->nFrames)-width);
		e->sampleScroll->SetValue(FrameToX(leftFrame));
		
		
		leftDrawFrame = leftFrame;
		
//		leftDRF = leftFrame;
//		drawReq = true;
	}
	rightFrame = leftFrame + framesPerPixel*x;
	rightDrawFrame = rightFrame;
	if (height != y) {
		height = y;
		yPsPerChannel = y/editTake->file->nChannels;
		leftDrawFrame = leftFrame;
//		leftDRF = leftFrame;
//		drawReq = true;
	}
}

void
SampleView::FrameMoved(BPoint pt)
{
	BView::FrameMoved(pt);
}

void
SampleView::ScrollTo(BPoint pt)
{
	leftFrame = pt.x*framesPerPixel;
	rightFrame = leftFrame + framesPerPixel*width;
	rightDrawFrame = rightFrame;
	leftDrawFrame = leftFrame;
	BView::ScrollTo(pt);
}

void
SampleView::ScaleX(float scalex)
{
	scaleX = scalex;
	drawLock.Lock();
	framesPerPixel = mySample->selectedNFrames/(scaleX*width);
	if (framesPerPixel == 0)
		framesPerPixel = 1;
		
	SampleEditor *e = (SampleEditor *)Parent();
	leftDrawFrame = leftFrame = XPixToFrame(e->sampleScroll->Value());
	rightDrawFrame = rightFrame = leftFrame + XPixToFrame(width);
	e->sampleScroll->max = FrameToX(e->editTake->file->nFrames)-width;

	drawLock.Unlock();
}

void
SampleView::ScaleY(float scaley)
{
	scaleY = scaley;
}

size_t
SampleView::Generate(float *outSig, long nFramesReqd, short nChannels)
{
	mySample->flock.RdLock();	// must!!  be a reader lock else buffers clag
	
	if (editTake == nullptr) {
		mySample->flock.RdUnlock();	// must!!  be a reader lock else buffers clag
		return B_ERROR;
	}
	
	long		outFrame = 0;
	long		loopStartFrame = 0;
	long		loopEndFrame = sampleDuration-1;
	if (leftSelectedFrame < rightSelectedFrame) {
		loopStartFrame = leftSelectedFrame;
		loopEndFrame = rightSelectedFrame;
	}
	
	while (outFrame < nFramesReqd) {
		if (currentPlayFrame < loopStartFrame) {
			currentPlayFrame = loopStartFrame;
		} else if (currentPlayFrame > loopEndFrame) {
			if (!doLoop)
				break;
			currentPlayFrame = loopStartFrame;
				// other loop startish things ...
//			LoopEnvelopes();
		}

		if (  currentPlayFrame < 0 ||
			  currentPlayFrame >= editTake->file->nFrames) {	// a frame of nothing
			currentPlayFrame += nFramesReqd;
			break;
		} else {
			if (!currentPlayBuffer || !currentPlayBuffer->HasFrame(editTake->file, currentPlayFrame)) {
				currentPlayBuffer = mySample->BufferForFrame(editTake->file, currentPlayFrame);
				if (currentPlayBuffer == nullptr) {
//					fprintf(stderr, "cant find buf for frame %d\n", currentPlayFrame);
					break;
				}				
			}

//			get crap from current frame

			int bufFrame = (currentPlayFrame - currentPlayBuffer->fromFrame);
			size_t	nFramesAvail = min32(nFramesReqd-outFrame,
										currentPlayBuffer->nFrames-bufFrame);
			nFramesAvail = min32(nFramesAvail,
								loopEndFrame-currentPlayFrame+1);

			memcpy(	&outSig[outFrame*editTake->file->nChannels],
					&currentPlayBuffer->data[bufFrame*editTake->file->nChannels],
					nFramesAvail*editTake->file->nChannels*sizeof(float));
			outFrame += nFramesAvail;
			currentPlayFrame += nFramesAvail;
		}
	}
	
	mySample->flock.RdUnlock();
	return outFrame;
}


SampleEditor::SampleEditor(BRect rr, float maxw, Sample *sa):
	DataEditor(rr,maxw,sa,&sampleRateMetric,sa->selectedNFrames)
{
	BRect		wrect;
	fprintf(stderr, "sample editor (%g %x\n", maxw, sa);
	mySample = sa;
	editTake = sa->selectedTake;
	isPreviewing = false;
	isRendering = false;
	
	BRect r = previewButton->Frame();
	BRect ebf = editBar->Frame();
	BRect	t = filterEdit->Frame();
	
	wrect.Set(MIXER_MARGIN+10, ebf.bottom+MIXER_MARGIN,
			 30, ebf.bottom+MIXER_MARGIN+10);
	statusView = new RenderStatusView(wrect, "status");
	statusView->SetResizingMode(B_FOLLOW_TOP|B_FOLLOW_LEFT);

	statusView->SetStatusValue(101);
	AddChild(statusView);
	BRect	svf = statusView->Frame();
	
	wrect.Set(ebf.right+MIXER_MARGIN, r.bottom+MIXER_MARGIN, ebf.right+40, r.bottom+MIXER_MARGIN+18);
	takeView = new SampleTakeView(wrect, mySample);
	takeView->SetFont(be_bold_font);
	takeView->SetFontSize(12);
	takeView->SetResizingMode(B_FOLLOW_TOP|B_FOLLOW_LEFT);
	takeView->SetViewColor(mdGray);
	AddChild(takeView);
	BRect tf = takeView->Frame();
	wrect.Set(MIXER_MARGIN,
			Max(svf.bottom, t.bottom)+MIXER_MARGIN,
			rr.right-rr.left-MIXER_MARGIN,
			rr.bottom-rr.top-MIXER_MARGIN - B_H_SCROLL_BAR_HEIGHT - 1);
	sampleView = new SampleView(wrect, mySample);
	sampleView->SetResizingMode(B_FOLLOW_TOP|B_FOLLOW_LEFT);
	AddChild(sampleView);

	wrect.Set( MIXER_MARGIN,
				rr.bottom-rr.top-MIXER_MARGIN - B_H_SCROLL_BAR_HEIGHT,
				rr.right-rr.left-MIXER_MARGIN,
				rr.bottom-rr.top-MIXER_MARGIN);
    sampleScroll = new ScrawleBar(wrect, "scroll", sampleView, sampleView,
               0, 30000, sampleView->scaleX, B_HORIZONTAL);
    sampleScroll->SetResizingMode(B_FOLLOW_LEFT_RIGHT);
    sampleScroll->SetRange(0,editTake?sampleView->FrameToX(editTake->file->nFrames)-sampleView->width:1000);
    AddChild(sampleScroll);
	
	ArrangeChildren();
}


void
SampleEditor::Draw(BRect r)
{
	DataEditor::Draw(r);
	LoweredBox(this, statusView->Frame(), ViewColor(), false);
}

SampleEditor::~SampleEditor()
{
	if (isRendering) {
		Render(false);
		status_t	err;
		wait_for_thread(renderThread, &err);
	}
}

void
SampleEditor::ArrangeChildren()
{
	origH = myHeight;
	origW = myWidth;
	
	BRect	sv = statusView->Frame();
	BRect	tv = takeView->Frame();
	filterEdit->MoveTo(tv.right+MIXER_MARGIN, tv.top);
	BRect	t = filterEdit->Frame();
	BRect	nvr = filterControls->Frame();
	filterControls->maxWidth = myWidth-2*MIXER_MARGIN-t.right;
	filterControls->MoveTo(t.right + MIXER_MARGIN, t.top);
	float h = Max(tv.bottom, t.bottom);
	h = Max(h, sv.bottom);
	h = Max(h, nvr.bottom) + MIXER_MARGIN;
	BRect	svr = sampleView->Frame();
	if (h != svr.top) {
		Window()->ResizeBy(0,h-svr.top);
	}
	sampleView->MoveTo(MIXER_MARGIN, h);
	BRect	s = sampleView->Frame();
	sampleScroll->MoveTo(MIXER_MARGIN, s.bottom+1);
	
	myHeight = s.bottom +1+ MIXER_MARGIN + B_H_SCROLL_BAR_HEIGHT;
	ResizeTo(myWidth, myHeight);
}

void
SampleEditor::AttachedToWindow()
{
	previewButton->SetTarget(this);
	loopButton->SetTarget(this);
	durView->SetTarget(this);
	lSelView->SetTarget(this);
	rSelView->SetTarget(this);
	editMenu->SetTargetForItems(this);
}

void
SampleEditor::FrameResized(float x, float y)
{
	myHeight = y;
	myWidth = x;
	BView *v;
	if ((v=Parent()) == nullptr) {
	} else if (strcmp(v->Name(),"back") == 0) {
		if (myHeight == origH) {
			BRect s = sampleView->Frame();
			sampleView->ResizeTo(
							myWidth-2*MIXER_MARGIN,
							myHeight-MIXER_MARGIN-s.top-B_H_SCROLL_BAR_HEIGHT-1);
//		    sampleScroll->SetRange(0,
//		    					sampleView->FrameToX(editTake->file->nFrames)-
//		    						(myWidth-2*MIXER_MARGIN));
 			s = sampleView->Frame();
 			sampleScroll->MoveTo(MIXER_MARGIN, s.bottom+1);
 			s = filterEdit->Frame();
			filterControls->maxWidth = myWidth-2*MIXER_MARGIN-s.right;
		} else {
		}
	}
	Invalidate();
}

void
SampleEditor::MessageReceived(BMessage *inMsg)
{
	switch(inMsg->what) {
	case PREVIEW_LOOP: {
		sampleView->doLoop = loopButton->Value();
		break;
	}
	case PREVIEW_REGION: {
		Preview(inMsg->FindInt32("state") == STATUS_RUNNING);
		break;
	}
	
	case SELECT_ALL: {
		sampleView->Select(0, editTake->file->nFrames);
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
			sampleView->Select(lSelView->TickValue(), rSelView->TickValue());
		break;
	}
	
	case SAMPLE_DURATION: {
		sampleView->sampleDuration = durView->TickValue();
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

bool
SampleEditor::GetKthChunks(int k, int lastChunk, Shortlist *w)
{
	bool	added = true;
	if (k < 2) {
//	fprintf(stderr, "get kth... %x %x\n", mySample, sampleView);
		short chunk = mySample->ChunkForFrame(editTake->file, sampleView->leftDrawFrame)+k;
		if (chunk <= lastChunk)
			added = added && w->Add(chunk);
	}
	if (added && isPreviewing) {
		short	chunk = mySample->ChunkForFrame(editTake->file,
									sampleView->currentPlayFrame)+k;
		if (sampleView->rightSelectedFrame != sampleView->leftSelectedFrame) {
			short	endChunk = mySample->ChunkForFrame(editTake->file, sampleView->rightSelectedFrame);
			short	startChunk = mySample->ChunkForFrame(editTake->file, sampleView->leftSelectedFrame);
			while (chunk > endChunk && chunk > 0) {
				chunk -= (endChunk-startChunk+1);
			}
		}
		if (chunk >= 0 && chunk <= lastChunk)
			added = added && w->Add(chunk);
	}
	return added;
}

size_t
SampleEditor::Generate(float *outSig, long nFramesReqd, short nChannels)
{
	long		outFrame = sampleView->Generate(outSig, nFramesReqd, nChannels);

	for (short i=outFrame; i<nFramesReqd; i++) {
		outSig[2*i] = 0;
		outSig[2*i+1] = 0;
	}
	
	if (editTake->file->nChannels == 2) {
		for (short i=0; i<nFramesReqd; i++) {
			outSig[2*i] = outSig[2*i];
			outSig[2*i+1] = outSig[2*i+1];
		}
	} else if (editTake->file->nChannels == 1) {
		for (short i=nFramesReqd-1; i>=0; i--) {
			outSig[2*i] = outSig[i];
			outSig[2*i+1] = outSig[i];
		}
	}
	schlock.Lock();
	if (offlineFilter->mainBlock && filterStack->locusStatus == STATUS_RUNNING) {
		theTime = sampleView->currentPlayFrame;
		UpdateEnvelopes(theTime);
		Stream	s;
		bool	uac = UpdateActiveBlock(
						mySample->uberQua,
						&s,
						offlineFilter->mainBlock,
						1,
						&theTime,
						TRUE, this,
						filterStack);
		s.ClearStream();
		ApplyQuaFX(	filterStack,
					offlineFilter->mainBlock,
					outSig,
					nFramesReqd,
					2);
	}			
	schlock.Unlock();
	return nFramesReqd;
}

bool
SampleEditor::QuitRequested()
{
	Preview(false);
	status_t	stat;
	sampleView->stillHere = false;
	wait_for_thread(sampleView->drawThread, &stat);
	return true;
}

status_t
SampleEditor::Preview(bool start)
{
	if (start) {
		if (!isPreviewing) {
			sampleView->currentPlayFrame = 0;
			mySample->uberQua->sampler->StartPreview(this);
			isPreviewing = true;
		}
	} else {
		if (isPreviewing) {
			mySample->uberQua->sampler->StopPreview(this);
			isPreviewing = false;
		}
	}
	return B_NO_ERROR;
}


long
SampleEditor::RenderWrapper(void *data)
{
	return ((SampleEditor *)data)->Renderer();
}

long
SampleEditor::Renderer()
{
	long			currentRenderFrame = 0;
	short			percentComplete = 0;
	Time			rendTime(0, &sampleRateMetric);
	
	statusView->SetStatusValue(0);
	
	mySample->flock.RdLock();	// must!!  be a reader lock else buffers clag
	
	if (editTake == nullptr) {
		mySample->flock.RdUnlock();	// must!!  be a reader lock else buffers clag
		return B_ERROR;
	}
		
	SampleFile		inFile(editTake->file->fileType);
	inFile = *editTake->file;
	if (inFile.InitCheck() != B_NO_ERROR) {
		reportError("Renderer: can't recreate input file");
		return B_ERROR;
	}
	inFile.SeekToFrame(0);

	int				takeno;
	BPath			path;
	SampleFile		*outFile = mySample->NewTakeFile(takeno, path);
	if (outFile == nullptr) {
		return B_ERROR;
	}
	
#define framesPerBuffer	1024
	char	*fileBuffer = new char[editTake->file->sampleSize
									* editTake->file->nChannels
									* framesPerBuffer];
	float	*workBuffer = new float[framesPerBuffer
									* editTake->file->nChannels
									* sizeof(float)];								
	while (isRendering && currentRenderFrame < sampleView->sampleDuration) {
		// get next buffer
		long		framesAvail = min32(editTake->file->nFrames-currentRenderFrame,
										framesPerBuffer);
		long		bytesAvail = framesAvail*editTake->file->sampleSize*editTake->file->nChannels;
		
		long		nBytesRead = 0;
		
		nBytesRead = inFile.Read(fileBuffer, bytesAvail);
		
		if (nBytesRead == 0) {
			reportError("qua: attempt to read past end of file, at frame %d", currentRenderFrame);
			break;
		} else if (nBytesRead < 0) {
			reportError("qua: file error, at frame %d: %s", currentRenderFrame, ErrorStr(nBytesRead));
			break;
		} else if (nBytesRead != bytesAvail) {
			reportError("qua: wird read %d and not %d", nBytesRead, bytesAvail);
		}
		long		framesRead = nBytesRead / (editTake->file->sampleSize*editTake->file->nChannels);
		inFile.NormalizeInputCpy(workBuffer, fileBuffer, nBytesRead);
		long		framesToWrite = min32(	framesRead,
									 		sampleView->sampleDuration-
									 			currentRenderFrame);
		long		bytesToWrite =	framesToWrite*
									editTake->file->sampleSize*
									editTake->file->nChannels;
		for (int i=framesRead*editTake->file->nChannels;
				 i<framesToWrite*editTake->file->nChannels;
				 i++) {
			workBuffer[i] = 0;
		}
//		fprintf(stderr, "current render frame %d, read %d/%d, to write %d/%d\n",
//				currentRenderFrame, framesRead,nBytesRead, framesToWrite, bytesToWrite);
		// process
		schlock.Lock();
		if (offlineFilter->mainBlock && filterStack->locusStatus == STATUS_RUNNING) {
			rendTime = currentRenderFrame;
			UpdateEnvelopes(rendTime);
			Stream	s;
			bool	uac = UpdateActiveBlock(
							mySample->uberQua,
							&s,
							offlineFilter->mainBlock,
							1,
							&rendTime,
							TRUE, this,
							filterStack);
			s.ClearStream();
			ApplyQuaFX(	filterStack,
						offlineFilter->mainBlock,
						workBuffer,
						framesToWrite, editTake->file->nChannels);
		}			
		schlock.Unlock();
		// gack it out.
		outFile->NormalizeOutputCpy(fileBuffer, workBuffer, framesToWrite);
		outFile->Write(fileBuffer, bytesToWrite);
		
		currentRenderFrame += framesToWrite;
		
		if (isRendering && (currentRenderFrame*100)/sampleView->sampleDuration > percentComplete) {
			int	pc = (currentRenderFrame*100)/sampleView->sampleDuration;
			if (pc != percentComplete) {
				percentComplete = pc;
				statusView->SetStatusValue(percentComplete);
			}
		}
	}
	
	delete fileBuffer;
	delete workBuffer;
	
	outFile->Finalize();

	if (!isRendering) {
		BEntry		ent(path.Path(), true);
		ent.Remove();
		delete outFile;
	} else {
		char		takenm[MAX_QUA_NAME_LENGTH];
		sprintf(takenm, "Take %d", takeno);
		
		SampleTake	*newTake = mySample->AddSampleTake(
										outFile, takenm, (char *)path.Path());
			// safe? this is the only reader to mod this list
		statusView->Window()->Lock();
		takeView->AddTake(newTake);
		statusView->Window()->Unlock();
	}
	
	mySample->flock.RdUnlock();
	
	statusView->Window()->Lock();
	statusView->SetStatusValue(101);
	statusView->Window()->Unlock();

	return B_NO_ERROR;
}

status_t
SampleEditor::Render(bool doit)
{
	status_t err = B_NO_ERROR;
	if (doit) {
		if (!isRendering) {
			isRendering = true;
		    renderThread = spawn_thread(RenderWrapper, "rendererer",
								B_NORMAL_PRIORITY, this);
			if (renderThread < B_NO_ERROR)
				return renderThread;
			err = resume_thread(renderThread);
		}
	} else {
		isRendering = false;
//		wait_for_thread(renderThread, &err);
	}
	return err;
}

status_t
SampleEditor::Copy()
{
	status_t		err = B_NO_ERROR;
	if (  sampleView->leftSelectedFrame >= 0 &&
		   sampleView->rightSelectedFrame >= 0) {
		long			currentRenderFrame = 0;
		long			leftSel =  sampleView->leftSelectedFrame;
		long			rightSel =  sampleView->rightSelectedFrame;
		short			percentComplete = 0;
		
		statusView->SetStatusValue(0);
		
		mySample->flock.RdLock();	// must!!  be a reader lock else buffers clag
		
		if (editTake == nullptr) {
			mySample->flock.RdUnlock();	// must!!  be a reader lock else buffers clag
			return B_ERROR;
		}
		
		SampleFile		inFile(editTake->file->fileType);
		inFile = *editTake->file;
		if (inFile.InitCheck() != B_NO_ERROR) {
			reportError("Renderer: can't recreate input file");
			return B_ERROR;
		}
	
		SampleFile		*pasteFile = new SampleFile(editTake->file->fileType);
		pasteFile->SetTo(fileClipPath, B_READ_WRITE|B_CREATE_FILE);
		pasteFile->SetFormat(editTake->file->nChannels, editTake->file->sampleSize, mySample->sampleRate);
		
		char	*fileBuffer = new char[editTake->file->sampleSize
										* editTake->file->nChannels
										* framesPerBuffer];
		float	*workBuffer = new float[framesPerBuffer
										* editTake->file->nChannels
										* sizeof(float)];								
		inFile.SeekToFrame(leftSel);
		currentRenderFrame = 0;						
		while (currentRenderFrame < rightSel-leftSel+1) {
			// get next buffer
			long		framesAvail;
			framesAvail = min32(rightSel-leftSel+1-currentRenderFrame,
									framesPerBuffer);
			long		bytesAvail = framesAvail*editTake->file->sampleSize*editTake->file->nChannels;
			long		nBytesRead = inFile.Read(fileBuffer, bytesAvail);
			if (nBytesRead != bytesAvail) {
				reportError("qua: wird read %d and not %d", nBytesRead, bytesAvail);
				err = B_ERROR;
				break;
			}
			long		framesRead = nBytesRead /
								(editTake->file->sampleSize*editTake->file->nChannels);
			inFile.NormalizeInputCpy(workBuffer, fileBuffer, nBytesRead);

			long		framesToWrite = framesRead;	
			long		bytesToWrite =	framesToWrite*
										editTake->file->sampleSize*
										editTake->file->nChannels;
			// gack it out.
			pasteFile->NormalizeOutputCpy(fileBuffer, workBuffer, framesToWrite);
			pasteFile->Write(fileBuffer, bytesToWrite);

			if ((currentRenderFrame*100)/sampleView->sampleDuration > percentComplete) {
				percentComplete = (currentRenderFrame*100)/sampleView->sampleDuration;
				statusView->SetStatusValue(percentComplete);
			}
			currentRenderFrame += framesRead;
		}
		
		mySample->flock.RdUnlock();
		
		pasteFile->Finalize();
	
		delete fileBuffer;
		delete workBuffer;
		delete pasteFile;
		
		
		statusView->SetStatusValue(101);
	}
	return err;
}

status_t
SampleEditor::Cut()
{
	status_t		err = B_NO_ERROR;
	if (  sampleView->leftSelectedFrame >= 0 &&
		   sampleView->rightSelectedFrame >= 0) {
		long			currentRenderFrame = 0;
		long			leftSel =  sampleView->leftSelectedFrame;
		long			rightSel =  sampleView->rightSelectedFrame;
		short			percentComplete = 0;
		
		statusView->SetStatusValue(0);
		
		mySample->flock.RdLock();	// must!!  be a reader lock else buffers clag
		
		if (editTake == nullptr) {
			mySample->flock.RdUnlock();	// must!!  be a reader lock else buffers clag
			return B_ERROR;
		}
		
		SampleFile		inFile(editTake->file->fileType);
		inFile = *editTake->file;
		if (inFile.InitCheck() != B_NO_ERROR) {
			reportError("Renderer: can't recreate input file");
			return B_ERROR;
		}
	
		int				takeno;
		BPath			path;
		SampleFile		*outFile = mySample->NewTakeFile(takeno, path);
		if (outFile == nullptr) {
			return B_ERROR;
		}
		
		SampleFile		*pasteFile = new SampleFile(editTake->file->fileType);
		pasteFile->SetTo(fileClipPath, B_READ_WRITE|B_CREATE_FILE);
		pasteFile->SetFormat(editTake->file->nChannels, editTake->file->sampleSize, mySample->sampleRate);

		char	*fileBuffer = new char[editTake->file->sampleSize
										* editTake->file->nChannels
										* framesPerBuffer];
		float	*workBuffer = new float[framesPerBuffer
										* editTake->file->nChannels
										* sizeof(float)];
		inFile.SeekToFrame(0);					
		while (currentRenderFrame < editTake->file->nFrames) {
			// get next buffer
			long		framesAvail;
			if (currentRenderFrame < leftSel) {
				framesAvail = min32(leftSel-currentRenderFrame,
									framesPerBuffer);
			} else if (currentRenderFrame <= rightSel) {
				framesAvail = min32(rightSel-currentRenderFrame+1,
									framesPerBuffer);
			} else if (currentRenderFrame < editTake->file->nFrames) {
				framesAvail = min32(editTake->file->nFrames-currentRenderFrame,
									framesPerBuffer);
			}
			
			long		bytesAvail = framesAvail*editTake->file->sampleSize*editTake->file->nChannels;
			long		nBytesRead = inFile.Read(fileBuffer, bytesAvail);
			if (nBytesRead != bytesAvail) {
				reportError("qua: wird read %d and not %d", nBytesRead, bytesAvail);
				err = B_ERROR;
				break;
			}
			long		framesRead = nBytesRead /
								(editTake->file->sampleSize*editTake->file->nChannels);
			inFile.NormalizeInputCpy(workBuffer, fileBuffer, nBytesRead);

			long		framesToWrite = framesRead;	
			long		bytesToWrite =	framesToWrite*
										editTake->file->sampleSize*
										editTake->file->nChannels;
			// gack it out.
			if (currentRenderFrame >= leftSel && currentRenderFrame <= rightSel) {
				pasteFile->NormalizeOutputCpy(fileBuffer, workBuffer, framesToWrite);
				pasteFile->Write(fileBuffer, bytesToWrite);
			} else {
				outFile->NormalizeOutputCpy(fileBuffer, workBuffer, framesToWrite);
				outFile->Write(fileBuffer, bytesToWrite);
			}			
			if ((currentRenderFrame*100)/sampleView->sampleDuration > percentComplete) {
				percentComplete = (currentRenderFrame*100)/sampleView->sampleDuration;
				statusView->SetStatusValue(percentComplete);
			}
			currentRenderFrame += framesRead;
		}
		
		mySample->flock.RdUnlock();
		
		outFile->Finalize();
	
		delete fileBuffer;
		delete workBuffer;
		
		if (err != B_NO_ERROR) {
			BEntry		ent(path.Path(), true);
			ent.Remove();
			delete outFile;
		} else {
			char		takenm[MAX_QUA_NAME_LENGTH];
			sprintf(takenm, "Take %d", takeno);

			outFile->SetTo((char *)path.Path(), B_READ_ONLY);		
			
			SampleTake	*newTake = mySample->AddSampleTake(
											outFile, takenm, (char *)path.Path());
				// safe? this is the only reader to mod this list
			takeView->AddTake(newTake);
		}
		delete pasteFile;
		
		
		statusView->SetStatusValue(101);
	}
	return err;
}

status_t
SampleEditor::Paste()
{
	status_t		err = B_NO_ERROR;
	if (  sampleView->leftSelectedFrame >= 0 &&
		   sampleView->rightSelectedFrame >= 0) {
		long			currentRenderFrame = 0;
		long			leftSel =  sampleView->leftSelectedFrame;
		long			rightSel =  sampleView->rightSelectedFrame;
		short			percentComplete = 0;
		
		statusView->SetStatusValue(0);
		
		mySample->flock.RdLock();	// must!!  be a reader lock else buffers clag
		
		if (editTake == nullptr) {
			mySample->flock.RdUnlock();	// must!!  be a reader lock else buffers clag
			return B_ERROR;
		}
		
		SampleFile		inFile(editTake->file->fileType);
		inFile = *editTake->file;
		if (inFile.InitCheck() != B_NO_ERROR) {
			reportError("Renderer: can't recreate input file");
			return B_ERROR;
		}
	
		int				takeno;
		BPath			path;
		SampleFile		*outFile = mySample->NewTakeFile(takeno, path);
		if (outFile == nullptr) {
			return B_ERROR;
		}
		
		SampleFile		*pasteFile = new SampleFile;
		pasteFile->SetTo(fileClipPath, B_READ_ONLY);
		if (pasteFile->InitCheck() != B_NO_ERROR) {
			reportError("no clipping");
			delete pasteFile;
			return B_ERROR;
		}
		
		char	*fileBuffer = new char[editTake->file->sampleSize
										* editTake->file->nChannels
										* framesPerBuffer];
		float	*workBuffer = new float[framesPerBuffer
										* editTake->file->nChannels
										* sizeof(float)];
		inFile.SeekToFrame(0);							
		while (currentRenderFrame < editTake->file->nFrames +
					(pasteFile->nFrames-(rightSel-leftSel+1))) {
			// get next buffer
			long		framesAvail;
			if (currentRenderFrame < leftSel) {
				framesAvail = min32(leftSel-currentRenderFrame,
									framesPerBuffer);
			} else if (currentRenderFrame < leftSel+pasteFile->nFrames) {
				framesAvail = min32(leftSel+pasteFile->nFrames-currentRenderFrame,
									framesPerBuffer);
			} else if (currentRenderFrame < editTake->file->nFrames) {
				if (currentRenderFrame == leftSel+pasteFile->nFrames)
					inFile.SeekToFrame(rightSel);
				framesAvail = min32(editTake->file->nFrames
									  + (pasteFile->nFrames-(rightSel-leftSel+1))
									  - currentRenderFrame,
									framesPerBuffer);
			}
			
			long		bytesAvail = framesAvail*editTake->file->sampleSize*editTake->file->nChannels;
			long		nBytesRead;
			long		framesRead;

			if (currentRenderFrame >= leftSel && currentRenderFrame < leftSel+pasteFile->nFrames) {
				nBytesRead = pasteFile->Read(fileBuffer, bytesAvail);
				framesRead = pasteFile->NormalizeInputCpy(workBuffer, fileBuffer, nBytesRead);
				if (nBytesRead != bytesAvail) {
					reportError("qua quip: wird read %d and not %d", nBytesRead, bytesAvail);
					err = B_ERROR;
					break;
				}
			} else {
				nBytesRead = inFile.Read(fileBuffer, bytesAvail);
				framesRead = inFile.NormalizeInputCpy(workBuffer, fileBuffer, nBytesRead);
				if (nBytesRead != bytesAvail) {
					reportError("qua: wird read %d and not %d", nBytesRead, bytesAvail);
					err = B_ERROR;
					break;
				}
			}

			long		framesToWrite = framesRead;	
			long		bytesToWrite =	framesToWrite*
										editTake->file->sampleSize*
										editTake->file->nChannels;
			// gack it out.
			
			outFile->NormalizeOutputCpy(fileBuffer, workBuffer, framesToWrite);
			outFile->Write(fileBuffer, bytesToWrite);

			if ((currentRenderFrame*100)/sampleView->sampleDuration > percentComplete) {
				percentComplete = (currentRenderFrame*100)/sampleView->sampleDuration;
				statusView->SetStatusValue(percentComplete);
			}
			currentRenderFrame += framesRead;
		}
		
		mySample->flock.RdUnlock();
		
		outFile->Finalize();
	
		delete fileBuffer;
		delete workBuffer;
		
		if (err != B_NO_ERROR) {
			BEntry		ent(path.Path(), true);
			ent.Remove();
			delete outFile;
		} else {
			char		takenm[MAX_QUA_NAME_LENGTH];
			sprintf(takenm, "Take %d", takeno);
			
			outFile->SetTo((char *)path.Path(), B_READ_ONLY);		
			SampleTake	*newTake = mySample->AddSampleTake(
											outFile, takenm, (char *)path.Path());
				// safe? this is the only reader to mod this list
			takeView->AddTake(newTake);
		}
		delete pasteFile;
		
		statusView->SetStatusValue(101);
	}
	return B_NO_ERROR;
}

status_t
SampleEditor::Overwrite()
{
	status_t		err = B_NO_ERROR;
	if (  sampleView->leftSelectedFrame >= 0 &&
		   sampleView->rightSelectedFrame >= 0) {
		long			currentRenderFrame = 0;
		long			leftSel =  sampleView->leftSelectedFrame;
		long			rightSel =  sampleView->rightSelectedFrame;
		short			percentComplete = 0;
		
		statusView->SetStatusValue(0);
		
		mySample->flock.RdLock();	// must!!  be a reader lock else buffers clag
		
		if (editTake == nullptr) {
			mySample->flock.RdUnlock();	// must!!  be a reader lock else buffers clag
			return B_ERROR;
		}
		
		SampleFile		inFile(editTake->file->fileType);
		inFile = *editTake->file;
		if (inFile.InitCheck() != B_NO_ERROR) {
			reportError("Renderer: can't recreate input file");
			return B_ERROR;
		}
	
		int				takeno;
		BPath			path;
		SampleFile		*outFile = mySample->NewTakeFile(takeno, path);
		if (outFile == nullptr) {
			return B_ERROR;
		}
		
		SampleFile		*pasteFile = new SampleFile;
		pasteFile->SetTo(fileClipPath, B_READ_ONLY);
		if (pasteFile->InitCheck() != B_NO_ERROR) {
			reportError("no clipping");
			delete pasteFile;
			return B_ERROR;
		}
		
		char	*fileBuffer = new char[editTake->file->sampleSize
										* editTake->file->nChannels
										* framesPerBuffer];
		float	*workBuffer = new float[framesPerBuffer
										* editTake->file->nChannels
										* sizeof(float)];
		inFile.SeekToFrame(0);							
		while (currentRenderFrame < editTake->file->nFrames) {
			// get next buffer
			long		framesAvail;
			if (currentRenderFrame < leftSel) {
				framesAvail = min32(leftSel-currentRenderFrame,
									framesPerBuffer);
			} else if (currentRenderFrame < leftSel+pasteFile->nFrames) {
				framesAvail = min32(leftSel+pasteFile->nFrames - currentRenderFrame,
									framesPerBuffer);
			} else if (currentRenderFrame < editTake->file->nFrames) {
				if (currentRenderFrame == leftSel + pasteFile->nFrames)
					inFile.SeekToFrame(currentRenderFrame);
				framesAvail = min32(editTake->file->nFrames-currentRenderFrame,
									framesPerBuffer);
			}
			
			long		bytesAvail = framesAvail*editTake->file->sampleSize*editTake->file->nChannels;
			long		nBytesRead;
			long		framesRead;

			if (currentRenderFrame >= leftSel && currentRenderFrame < leftSel+pasteFile->nFrames) {
				nBytesRead = pasteFile->Read(fileBuffer, bytesAvail);
				framesRead = pasteFile->NormalizeInputCpy(workBuffer, fileBuffer, nBytesRead);
			} else {
				nBytesRead = inFile.Read(fileBuffer, bytesAvail);
				framesRead = inFile.NormalizeInputCpy(workBuffer, fileBuffer, nBytesRead);
			}
			if (nBytesRead != bytesAvail) {
				reportError("qua: wird read %d and not %d", nBytesRead, bytesAvail);
				err = B_ERROR;
				break;
			}

			long		framesToWrite = framesRead;	
			long		bytesToWrite =	framesToWrite*
										editTake->file->sampleSize*
										editTake->file->nChannels;
			// gack it out.
			
			outFile->NormalizeOutputCpy(fileBuffer, workBuffer, framesToWrite);
			outFile->Write(fileBuffer, bytesToWrite);

			if ((currentRenderFrame*100)/sampleView->sampleDuration > percentComplete) {
				percentComplete = (currentRenderFrame*100)/sampleView->sampleDuration;
				statusView->SetStatusValue(percentComplete);
			}
			currentRenderFrame += framesToWrite;
		}
		
		mySample->flock.RdUnlock();
		
		outFile->Finalize();
	
		delete fileBuffer;
		delete workBuffer;
		
		if (err != B_NO_ERROR) {
			BEntry		ent(path.Path(), true);
			ent.Remove();
			delete outFile;
		} else {
			char		takenm[MAX_QUA_NAME_LENGTH];
			sprintf(takenm, "Take %d", takeno);
			
			outFile->SetTo((char *)path.Path(), B_READ_ONLY);		
			SampleTake	*newTake = mySample->AddSampleTake(
											outFile, takenm, (char *)path.Path());
				// safe? this is the only reader to mod this list
			takeView->AddTake(newTake);
		}
		delete pasteFile;
		
		statusView->SetStatusValue(101);
	}
	return err;
}

status_t
SampleEditor::Clear()
{
	status_t		err = B_NO_ERROR;
	if (  sampleView->leftSelectedFrame >= 0 &&
		   sampleView->rightSelectedFrame >= 0) {
		long			currentRenderFrame = 0;
		long			leftSel =  sampleView->leftSelectedFrame;
		long			rightSel =  sampleView->rightSelectedFrame;
		short			percentComplete = 0;
		
		statusView->SetStatusValue(0);
		
		mySample->flock.RdLock();	// must!!  be a reader lock else buffers clag
		
		if (editTake == nullptr) {
			mySample->flock.RdUnlock();	// must!!  be a reader lock else buffers clag
			return B_ERROR;
		}
		
		SampleFile		inFile(editTake->file->fileType);
		inFile = *editTake->file;
		if (inFile.InitCheck() != B_NO_ERROR) {
			reportError("Renderer: can't recreate input file");
			return B_ERROR;
		}
	
		int				takeno;
		BPath			path;
		SampleFile		*outFile = mySample->NewTakeFile(takeno, path);
		if (outFile == nullptr) {
			return B_ERROR;
		}
		
		char	*fileBuffer = new char[editTake->file->sampleSize
										* editTake->file->nChannels
										* framesPerBuffer];
		float	*workBuffer = new float[framesPerBuffer
										* editTake->file->nChannels
										* sizeof(float)];
		inFile.SeekToFrame(0);						
		while (currentRenderFrame < editTake->file->nFrames) {
			// get next buffer
			long		framesAvail;
			if (currentRenderFrame < leftSel) {
				framesAvail = min32(leftSel-currentRenderFrame,
									framesPerBuffer);
			} else if (currentRenderFrame <= rightSel) {
				framesAvail = min32(rightSel-currentRenderFrame+1,
									framesPerBuffer);
			} else if (currentRenderFrame < editTake->file->nFrames) {
				framesAvail = min32(editTake->file->nFrames-currentRenderFrame,
									framesPerBuffer);
			}
			
			long		bytesAvail = framesAvail*editTake->file->sampleSize*editTake->file->nChannels;
			long		nBytesRead = inFile.Read(fileBuffer, bytesAvail);
			if (nBytesRead != bytesAvail) {
				reportError("qua: wird read %d and not %d", nBytesRead, bytesAvail);
				err = B_ERROR;
				break;
			}
			long		framesRead = nBytesRead /
								(editTake->file->sampleSize*editTake->file->nChannels);
			inFile.NormalizeInputCpy(workBuffer, fileBuffer, nBytesRead);

			long		framesToWrite = framesRead;	
			long		bytesToWrite =	framesToWrite*
										editTake->file->sampleSize*
										editTake->file->nChannels;
			// gack it out.
			
			if (currentRenderFrame >= leftSel && currentRenderFrame <= rightSel) {
				for (short i=0; i<framesToWrite*editTake->file->nChannels; i++)
					workBuffer[i] = 0;
			}			
			outFile->NormalizeOutputCpy(fileBuffer, workBuffer, framesToWrite);
			outFile->Write(fileBuffer, bytesToWrite);
			
			if ((currentRenderFrame*100)/sampleView->sampleDuration > percentComplete) {
				percentComplete = (currentRenderFrame*100)/sampleView->sampleDuration;
				statusView->SetStatusValue(percentComplete);
			}
			currentRenderFrame += framesRead;
		}
		
		mySample->flock.RdUnlock();
		
		outFile->Finalize();
	
		delete fileBuffer;
		delete workBuffer;
		
		if (err != B_NO_ERROR) {
			BEntry		ent(path.Path(), true);
			ent.Remove();
			delete outFile;
		} else {
			char		takenm[MAX_QUA_NAME_LENGTH];
			sprintf(takenm, "Take %d", takeno);
			
			outFile->SetTo((char *)path.Path(), B_READ_ONLY);		

			SampleTake	*newTake = mySample->AddSampleTake(
											outFile, takenm, (char *)path.Path());
				// safe? this is the only reader to mod this list
			takeView->AddTake(newTake);
		}

		statusView->SetStatusValue(101);
	}
	return err;
}

status_t
SampleEditor::Delete()
{
	status_t		err = B_NO_ERROR;
	if (  sampleView->leftSelectedFrame >= 0 &&
		   sampleView->rightSelectedFrame >= 0) {
		long			currentRenderFrame = 0;
		long			leftSel =  sampleView->leftSelectedFrame;
		long			rightSel =  sampleView->rightSelectedFrame;
		short			percentComplete = 0;
				
		statusView->SetStatusValue(0);
		
		mySample->flock.RdLock();	// must!!  be a reader lock else buffers clag
		
		if (editTake == nullptr) {
			mySample->flock.RdUnlock();	// must!!  be a reader lock else buffers clag
			return B_ERROR;
		}
		
		SampleFile		inFile(editTake->file->fileType);
		inFile = *editTake->file;
		if (inFile.InitCheck() != B_NO_ERROR) {
			reportError("Renderer: can't recreate input file");
			return B_ERROR;
		}
	
		int				takeno;
		BPath			path;
		SampleFile		*outFile = mySample->NewTakeFile(takeno, path);
		if (outFile == nullptr) {
			return B_ERROR;
		}
		
		char	*fileBuffer = new char[editTake->file->sampleSize
										* editTake->file->nChannels
										* framesPerBuffer];
		float	*workBuffer = new float[framesPerBuffer
										* editTake->file->nChannels
										* sizeof(float)];
		inFile.SeekToFrame(0);	
		while (currentRenderFrame < editTake->file->nFrames) {
			// get next buffer
			long		framesAvail=0;
			if (currentRenderFrame < leftSel) {
				framesAvail = min32(leftSel-currentRenderFrame,
									framesPerBuffer);
			} else if (currentRenderFrame < editTake->file->nFrames) {
				if (currentRenderFrame <= rightSel) {
					currentRenderFrame = rightSel+1;
					inFile.SeekToFrame(currentRenderFrame);
				}
				framesAvail = min32(editTake->file->nFrames-currentRenderFrame,
									framesPerBuffer);
			} else {
				break;
			}
			
			long		bytesAvail = framesAvail*editTake->file->sampleSize*editTake->file->nChannels;
			long		nBytesRead = inFile.Read(fileBuffer, bytesAvail);
			if (nBytesRead != bytesAvail) {
				reportError("qua: wird read %d and not %d", nBytesRead, bytesAvail);
				err = B_ERROR;
				break;
			}
			long		framesRead = nBytesRead /
								(editTake->file->sampleSize*editTake->file->nChannels);
			inFile.NormalizeInputCpy(workBuffer, fileBuffer, nBytesRead);

			long		framesToWrite = framesRead;	
			long		bytesToWrite =	framesToWrite*
										editTake->file->sampleSize*
										editTake->file->nChannels;
//			fprintf(stderr, "read %d frames @ fr %d%d nf %d: %d fravail %d %d l %d r %d: to write %d %d\n",
//				  framesRead, editTake->file->nFrames, currentRenderFrame,
//				  framesAvail, nBytesRead, bytesAvail, 
//				  leftSel, rightSel, framesToWrite, bytesToWrite);
			// gack it out.
			if (currentRenderFrame < leftSel || currentRenderFrame > rightSel) {
				outFile->NormalizeOutputCpy(fileBuffer, workBuffer, framesToWrite);
				long wr = outFile->Write(fileBuffer, bytesToWrite);
			}
			if ((currentRenderFrame*100)/sampleView->sampleDuration > percentComplete) {
				percentComplete = (currentRenderFrame*100)/sampleView->sampleDuration;
				statusView->SetStatusValue(percentComplete);
			}
			currentRenderFrame += framesRead;
		}
		
		mySample->flock.RdUnlock();
		
		outFile->Finalize();
	
		delete fileBuffer;
		delete workBuffer;
		
		if (err != B_NO_ERROR) {
			BEntry		ent(path.Path(), true);
			ent.Remove();
			delete outFile;
		} else {
			char		takenm[MAX_QUA_NAME_LENGTH];
			sprintf(takenm, "Take %d", takeno);

			outFile->SetTo((char *)path.Path(), B_READ_ONLY);		
			SampleTake	*newTake = mySample->AddSampleTake(
											outFile, takenm, (char *)path.Path());
				// safe? this is the only reader to mod this list
			takeView->AddTake(newTake);
		}
		
		statusView->SetStatusValue(101);
	}
	return err;
}

status_t
SampleEditor::Crop()
{
	status_t		err = B_NO_ERROR;
	if (  sampleView->leftSelectedFrame >= 0 &&
		   sampleView->rightSelectedFrame >= 0) {
		long			currentRenderFrame = 0;
		long			leftSel =  sampleView->leftSelectedFrame;
		long			rightSel =  sampleView->rightSelectedFrame;
		short			percentComplete = 0;
		
		statusView->SetStatusValue(0);
		
		mySample->flock.RdLock();	// must!!  be a reader lock else buffers clag
		
		if (editTake == nullptr) {
			mySample->flock.RdUnlock();	// must!!  be a reader lock else buffers clag
			return B_ERROR;
		}
		
		SampleFile		inFile(editTake->file->fileType);
		inFile = *editTake->file;
		if (inFile.InitCheck() != B_NO_ERROR) {
			reportError("Renderer: can't recreate input file");
			mySample->flock.RdUnlock();	// must!!  be a reader lock else buffers clag
			return B_ERROR;
		}
	
		int				takeno;
		BPath			path;
		SampleFile		*outFile = mySample->NewTakeFile(takeno, path);
		if (outFile == nullptr) {
			reportError("No new take");
			mySample->flock.RdUnlock();	// must!!  be a reader lock else buffers clag
			return B_ERROR;
		}
//		fprintf(stderr, "take %s\n", path.Path());
		
		char	*fileBuffer = new char[editTake->file->sampleSize
										* editTake->file->nChannels
										* framesPerBuffer];
		float	*workBuffer = new float[framesPerBuffer
										* editTake->file->nChannels
										* sizeof(float)];
		inFile.SeekToFrame(leftSel);
		currentRenderFrame = 0;						
		while (currentRenderFrame < rightSel-leftSel+1) {
//			fprintf(stderr, "Crop: frame %d\r", currentRenderFrame);
			// get next buffer
			long		framesAvail;
			framesAvail = min32(rightSel-leftSel+1-currentRenderFrame,
									framesPerBuffer);
			
			long		bytesAvail = framesAvail*editTake->file->sampleSize*editTake->file->nChannels;
			long		nBytesRead = inFile.Read(fileBuffer, bytesAvail);
			if (nBytesRead != bytesAvail) {
				reportError("qua: wird read %d and not %d", nBytesRead, bytesAvail);
				err = B_ERROR;
				break;
			}
			long		framesRead = nBytesRead /
								(editTake->file->sampleSize*editTake->file->nChannels);
			inFile.NormalizeInputCpy(workBuffer, fileBuffer, bytesAvail);

			long		framesToWrite = framesRead;	
			long		bytesToWrite =	framesToWrite*
										editTake->file->sampleSize*
										editTake->file->nChannels;
			// gack it out.
			outFile->NormalizeOutputCpy(fileBuffer, workBuffer, framesToWrite);
			outFile->Write(fileBuffer, bytesToWrite);

			if ((currentRenderFrame*100)/sampleView->sampleDuration > percentComplete) {
				percentComplete = (currentRenderFrame*100)/sampleView->sampleDuration;
				statusView->SetStatusValue(percentComplete);
			}
			
			currentRenderFrame += framesRead;
		}
		
		mySample->flock.RdUnlock();
		
		outFile->Finalize();
	
		delete fileBuffer;
		delete workBuffer;
		
		if (err != B_NO_ERROR) {
			BEntry		ent(path.Path(), true);
			ent.Remove();
			delete outFile;
		} else {
			char		takenm[MAX_QUA_NAME_LENGTH];
			sprintf(takenm, "Take %d", takeno);

			outFile->SetTo((char *)path.Path(), B_READ_ONLY);		
			
			SampleTake	*newTake = mySample->AddSampleTake(
											outFile, takenm, (char *)path.Path());
				// safe? this is the only reader to mod this list
			takeView->AddTake(newTake);
		}
		
		statusView->SetStatusValue(101);
	}
	return err;
}
