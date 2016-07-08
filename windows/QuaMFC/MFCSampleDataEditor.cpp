
//#define _AFXDLL
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE
#include "stdafx.h"
#include "qua_version.h"
#include "ShlObj.h"

#include "StdDefs.h"
#include "Colors.h"

#include "QuaMFC.h"
#include "QuaMFCDoc.h"
#include "MFCSampleDataEditor.h"
#include "MFCObjectView.h"

#include "Qua.h"
#include "Time.h"
#include "Sample.h"
#include "QuaDisplay.h"
#include "BaseVal.h"
#include "Stackable.h"
#include "Dictionary.h"

IMPLEMENT_DYNCREATE(MFCSampleDataEditor, MFCSequenceEditor)

BEGIN_MESSAGE_MAP(MFCSampleDataEditor, MFCSequenceEditor)
	//{{AFX_MSG_MAP(MFCSampleDataEditor)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_LBUTTONDOWN( )
	ON_WM_LBUTTONUP( )
	ON_WM_LBUTTONDBLCLK( )
	ON_WM_MBUTTONDOWN( )
	ON_WM_MBUTTONUP( )
	ON_WM_MBUTTONDBLCLK( )
	ON_WM_RBUTTONDOWN( )
	ON_WM_RBUTTONUP( )
	ON_WM_RBUTTONDBLCLK( )
	ON_WM_MOUSEMOVE( )
	ON_WM_MOUSEWHEEL( )
	ON_WM_KEYDOWN( )
	ON_WM_KEYUP( )
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
//	ON_COMMAND(ID_STREAMEDITORCONTEXT_ADD_CONTROLLER, OnAddController)
//	ON_COMMAND(ID_STREAMEDITORCONTEXT_ADD_SYSX, OnAddSysX)
//	ON_COMMAND(ID_STREAMEDITORCONTEXT_ADD_SYSC, OnAddSysC)
//	ON_COMMAND(ID_STREAMEDITORCONTEXT_ADD_VALUE, OnAddValur)
END_MESSAGE_MAP()

MFCSampleDataEditor::MFCSampleDataEditor(void)
{
	take = NULL;
#if QUA_V_SAMPLE_DRAW=='p'
	peakCache = NULL;
	peakBufferFrame = -1;
	currentPeakIdx = 0;
	peakingThread = NULL;
#endif
}

MFCSampleDataEditor::~MFCSampleDataEditor(void)
{
	fprintf(stderr, "deleting sample data editor %d\n", this);
#if QUA_V_SAMPLE_DRAW=='p'
	if (!CleanupPeakCache()) {
		fprintf(stderr, "something i hoped would never happen just happened, but maybe it could have happened at a worse time. ...\n");
	}
#endif
}

long
MFCSampleDataEditor::NHorizontalPix()
{
//	return 128;
	if (take == NULL || take->file == NULL || take->file->nChannels <= 0) {
		return 1;
	}
	return take->file->nChannels;
}

long
MFCSampleDataEditor::HorizontalPix(long n)
{
//	return Note2Pix(n);
	if (take == NULL || take->file == NULL || take->file->nChannels <= 0) {
		return (bounds.bottom-bounds.top)/2; // single line down middle
	}
	long cvh = (bounds.bottom-bounds.top)/take->file->nChannels;

	return n*cvh + (cvh/2);
}

// sample value between [-1, 1]
long
MFCSampleDataEditor::SampleVal2Pix(float sv, short chan)
{
	if (take == NULL || take->file == NULL || take->file->nChannels <= 0) {
		return (sv+1)*(bounds.bottom-bounds.top)/2; // single line down middle
	}
	long cvh = (bounds.bottom-bounds.top)/take->file->nChannels;
	return bounds.top+cvh*(chan+(sv+1)/2);
}

float
MFCSampleDataEditor::Pix2SampleVal(long pv, short &chan)
{
	if (take == NULL || take->file == NULL || take->file->nChannels <= 0) {
		return (2*(pv-bounds.top)/(bounds.bottom-bounds.top))-1;
	}
	long cvh = (bounds.bottom-bounds.top)/take->file->nChannels;
	chan = pv / cvh;
	return ((pv-bounds.top)/cvh)-chan-0.5;
}


void
MFCSampleDataEditor::SetHVScroll()
{
	long	atY=0;
	long	pixx = Time2Pix(lastScheduledEvent);
	if (pixx < bounds.right*2) {
		pixx = bounds.right;
	}
	SetScrollSizes(MM_TEXT, CSize(pixx, bounds.bottom));
}

bool
MFCSampleDataEditor::SetToTake(Take *t)
{
	fprintf(stderr, "MFCSampleDataEditor::SetToTake(%x)\n", t);
	if (t == NULL) {// watch this !!!!
		return true;
	} else if (t->type == Take::SAMPLE) {
		if (take != t) {
			fprintf(stderr, "about to clear all\n");
			ClearAllItemViews(false);
#if QUA_V_SAMPLE_DRAW=='p'
			fprintf(stderr, "about to do peaks\n");
			if (!SetupPeakCache((SampleTake*)t)) {
				fprintf(stderr, "something i hoped would never happen just happened ...\n");
			}
#else
			take = (SampleTake *)t;
#endif
			fprintf(stderr, "about to do add views\n");
			AddAllItemViews(true);
		}
		return true;
	}
	return false;
}

bool
MFCSampleDataEditor::AddAllItemViews(bool redraw)
{
	if (NItemR() > 0) {
		ClearAllItemViews(false);
	}

	StabEnt	*pars = parent->Symbol();
	if (pars) {
		StabEnt	*sibp = pars->children;
		while (sibp != NULL) {
			if (sibp->type == TypedValue::S_CLIP) {
				Clip	*c = sibp->ClipValue(NULL);
				if (c->media != NULL) {
					StabEnt		*ts = c->media->sym;
					Take		*tk = ts->TakeValue();
					if (take == tk) {
						AddClipItemView(c);
					}
				}
			}
			sibp = sibp->sibling;
		}
	}

	SetHVScroll();
	if (redraw) {
		InvalidateRect(&bounds);
	}
	return true;
}

void
MFCSampleDataEditor::OnInitialUpdate()
{
	CQuaMFCDoc	*qdoc = (CQuaMFCDoc *)GetDocument();
	if (qdoc == NULL) {
//		reportError("SampleDataEditor: initial update of channel mount finds a null sequencer document");
	} else if (qdoc->qua == NULL) {
//		reportError("SampleDataEditor: initial update finds a null sequencer");
	} else {	// set qua up with our hooks
	}
	SetHVScroll();
}

#if QUA_V_SAMPLE_DRAW=='p'

bool
MFCSampleDataEditor::CleanupPeakCache()
{
	if (peakingThread != NULL) {
		status_t	err;
		stillPeaking = false;
		if ((err=WaitForSingleObject(peakingThread, 1000)) != WAIT_OBJECT_0) {
			fprintf(stderr, "Bad clock wait:%s\n", sem_error_string(err));
		}
	}
	return true;
}

bool
MFCSampleDataEditor::SchedulePeakRedraw(long from, long to)
{
	if (scheduledDraw) {
		if (to > lastUndrawnTick) {
			lastUndrawnTick = to;
		}
		if (from < firstUndrawnTick) {
			firstUndrawnTick = to;
		}
	} else {
		scheduledDraw = true;
		firstUndrawnTick = from;
		lastUndrawnTick = to;
	}
	return true;
}

bool
MFCSampleDataEditor::SignalPeakRedraw(long from, long to)
{
	scheduledDraw = false;
	CRect			updr = bounds;
	updr.left = from * pixPerNotch;
	updr.right = to * pixPerNotch;
	InvalidateRect(&updr);
//	fprintf(stderr, "signalling %d %d\n", from, to);
	return true;
}

bool
MFCSampleDataEditor::SetupPeakCache(SampleTake *t)
{
	if (t != take) {
		peakingMutx.Lock();

		take = t;
		if (take != NULL) {
			long	ntf = take->file->nFrames;
			Time	srt(ntf, &Metric::sampleRate);
			Time	sat = srt & displayMetric; // sat.ticks should now be size of cache
			if (peakCacheNChannel != take->file->nChannels || peakCacheLength != sat.ticks) {
				long i=0;
				long j=0;
				if (peakCache != NULL) {
					for (i=0; i<peakCacheNChannel; i++) {
						if (peakCache[i] != NULL) {
							delete [] peakCache[i];
						}
					}
					delete [] peakCache;
				}
				peakCacheNChannel = take->file->nChannels;
				peakCacheLength = sat.ticks;
				peakCache = new peak_data*[peakCacheNChannel];
				for (i=0; i<peakCacheNChannel; i++) {
					peakCache[i] = new peak_data[peakCacheLength];
					for (j=0; j<peakCacheLength; j++) {
						peakCache[i][j].hi = 0;
						peakCache[i][j].lo = 0;
						peakCache[i][j].valid = false;
					}
				}
			}
			lastScheduledEvent = sat;
			currentPeakIdx = 0;
			peakBufferFrame = -1;
			scheduledDraw = false;
		}
		if (peakingThread == NULL) {
			stillPeaking = true;
			peakingThread = spawn_thread(PeakeratorWrapper, "peakerator",
								B_DISPLAY_PRIORITY, this);
		}

		peakingMutx.Unlock();
	}
	return true;
}

int32
MFCSampleDataEditor::PeakeratorWrapper(void *data)
{
	int ret = ((MFCSampleDataEditor *)data)->Peakerator();
	return ret;
}

int32
MFCSampleDataEditor::Peakerator()
{
	stillPeaking = true;
	while (stillPeaking) {
		peakingMutx.Lock();
// check cache is ok
		if (take != NULL && take->file != NULL) {
			SampleFile	*file = take->file;
			if (currentPeakIdx < peakCacheLength) {		// get frames for this index
				Time	startT(currentPeakIdx, displayMetric);
				Time	startF = startT & Metric::sampleRate;
				Time	endT(currentPeakIdx+1, displayMetric);
				Time	endF = endT & Metric::sampleRate;

				long frame = startF.ticks;
				while (frame < endF.ticks) {
					if (frame >= file->nFrames) {
						break;
					}
					long	nBufferFrames = PEAKER_RAW_BUFSIZE/(file->nChannels*file->sampleSize);
					if (peakBufferFrame < 0 ||
							frame < peakBufferFrame ||
							frame >= peakBufferFrame+peakBufferNFrames) {
						long	frameChunk = frame/nBufferFrames;
						long	chunkStartFrame = frameChunk*nBufferFrames;

						file->SeekToFrame(chunkStartFrame);
						long nr = file->Read(
										peakRawBuffer,
										PEAKER_RAW_BUFSIZE);
						if (nr <= 0) {
							return false;
						}
						peakBufferNFrames = file->NormalizeInputCpy(
									peakSampleBuffer, peakRawBuffer, nr);
						peakBufferFrame = chunkStartFrame;
					}
					long cgo = (frame%nBufferFrames)*file->nChannels;
					for (short i=0; i<file->nChannels; i++) {
						if (peakSampleBuffer[cgo + i] > peakCache[i][currentPeakIdx].hi) {
							peakCache[i][currentPeakIdx].hi = peakSampleBuffer[cgo + i];
						}
						if (peakSampleBuffer[cgo + i] < peakCache[i][currentPeakIdx].lo) {
							peakCache[i][currentPeakIdx].lo = peakSampleBuffer[cgo + i];
						}
					}
					frame++;
				}
				for (short i=0; i<file->nChannels; i++) {
					peakCache[i][currentPeakIdx].valid = true;
				}
				
				if (scheduledDraw && endT.ticks >= lastUndrawnTick) {
					SignalPeakRedraw(firstUndrawnTick, lastUndrawnTick);
				}

				currentPeakIdx++;
			}
		}

		peakingMutx.Unlock();
	}
	return B_OK;
}

bool
MFCSampleDataEditor::PeakData(long idx, short channel, float &sample_hi, float &sample_lo)
{
	bool	done = true;
	sample_hi = 0;
	sample_lo = 0;
	peakingMutx.Lock();
	if (idx < peakCacheLength && channel < peakCacheNChannel) {
		peak_data *p = &peakCache[channel][idx];
		if (p->valid) {
			sample_hi = p->hi;
			sample_lo = p->lo;
		} else {
			done = false;
		}
	} else {
		// not sure if this should give false or true
	}
	peakingMutx.Unlock();
	return done;
}

#endif
//FILE *ddbg = fopen("ddbg", "w");
void
MFCSampleDataEditor::OnDraw(CDC* pdc)
{
	Pen			blackPen(Color(250, 0, 0, 0), 1);
	Pen			redPen(Color(250, 160, 10, 10), 1);
	Pen			bluePen(Color(250, 10, 10, 160), 1);
	Pen			greenPen(Color(250, 10,100,10), 1);
	SolidBrush	blueBrush(Color(100, 10, 10, 160));
	SolidBrush	greenBrush(Color(200, 10,100,10));
	SolidBrush	softgreenBrush(Color(50, 60,160,60));
	SolidBrush	blackBrush(Color(200, 0, 0, 0));
	SolidBrush	hardblackBrush(Color(250, 0, 0, 0));

	CRect	clipBox;
	int		cbType;
	cbType = pdc->GetClipBox(&clipBox);
	CDocument* pDoc = GetDocument();
#ifdef QUA_V_GDI_PLUS
	Graphics	graphics(pdc->m_hDC);
	DrawGridGraphics(&graphics, &clipBox);

	if (take != NULL && take->file != NULL) {
#if QUA_V_SAMPLE_DRAW=='p'
		short channel;
		long startTick = clipBox.left/pixPerNotch;
		long endTick = clipBox.right/pixPerNotch;
		short notchInc = 1;
		long tickCnt = 0;
		short tickPerNotch = 1;

		Point	*point;
		long	nticks = (endTick-startTick+1)/tickPerNotch;
		long	npoints = nticks*2;
		point = new Point[npoints];

		for (channel=0; channel<take->file->nChannels; channel++) {
			long	pixx;
			float	sample_hi = 0;
			float	sample_lo = 0;
			long	pxx_hi=0;
			long	pxx_lo=0;

			for (int i=0; i<npoints; i++) {
				point[i].X = 0;
				point[i].Y = 0;
			}
			long	pointCount = 0;
			long	firstFrameThisTick = 0;
			long	lastFrameThisTick = 0;
			long	notchPx = startTick*pixPerNotch;

			Time	startT(startTick, displayMetric);
			Time	startF = startT & Metric::sampleRate;

			lastFrameThisTick = startF.ticks - 1;
			for (tickCnt=startTick; tickCnt<=endTick; tickCnt+=tickPerNotch, notchPx += pixPerNotch) {
				firstFrameThisTick = lastFrameThisTick+1;
				startT.ticks = tickCnt + tickPerNotch;
				startF = startT & Metric::sampleRate;
				lastFrameThisTick = startF.ticks-1;

				bool have_peak_data=PeakData(tickCnt, channel, sample_hi, sample_lo);

				if (have_peak_data) {
					pxx_hi = SampleVal2Pix(sample_hi, channel);
					pxx_lo = SampleVal2Pix(sample_lo, channel);
					point[pointCount].X = notchPx;
					point[pointCount].Y = pxx_hi;
					point[npoints-1-pointCount].X = notchPx;
					point[npoints-1-pointCount].Y = pxx_lo;
				} else {
					// should print a 'being completed' message
					SchedulePeakRedraw(tickCnt, endTick);
					break;
				}
				pointCount++;
			}
			graphics.FillPolygon(&blueBrush, point, npoints);
			graphics.DrawPolygon(&bluePen, point, npoints);
		}
		delete [] point;
#else
// clunky sample draw that reads from the file in the drawing loop
// ugly but effective. 
		short channel;
		long startTick = clipBox.left/pixPerNotch;
		long endTick = clipBox.right/pixPerNotch;
		short notchInc = 1;
		long tickCnt = 0;
		short tickPerNotch = 1;

		Point	*point;
		long	nticks = (endTick-startTick+1)/tickPerNotch;
		long	npoints = nticks*2;
		point = new Point[npoints];
		for (channel=0; channel<take->file->nChannels; channel++) {
			long notchPx = startTick*pixPerNotch;
			long	pixx;
			float	sample_hi = 0;
			float	sample_lo = 0;
			long	pxx_hi=0;
			long	pxx_lo=0;

			for (int i=0; i<npoints; i++) {
				point[i].X = 0;
				point[i].Y = 0;
			}
			long	pointCount = 0;
			long	firstFrameThisTick = 0;
			long	lastFrameThisTick = 0;

			Time	startT(startTick, displayMetric);
			Time	startF = startT & Metric::sampleRate;

			lastFrameThisTick = startF.ticks - 1;
			for (tickCnt=startTick; tickCnt<=endTick; tickCnt+=tickPerNotch, notchPx += pixPerNotch) {
//				get the peak data for this time slot, convert to pix, and add to points
				firstFrameThisTick = lastFrameThisTick+1;
				startT.ticks = tickCnt + tickPerNotch;
				startF = startT & Metric::sampleRate;
				lastFrameThisTick = startF.ticks-1;

				bool done=!take->PeakData(firstFrameThisTick, lastFrameThisTick, channel, sample_hi, sample_lo);
				pxx_hi = SampleVal2Pix(sample_hi, channel);
				pxx_lo = SampleVal2Pix(sample_lo, channel);
				point[pointCount].X = notchPx;
				point[pointCount].Y = pxx_hi;
				point[npoints-1-pointCount].X = notchPx;
				point[npoints-1-pointCount].Y = pxx_lo;
				if (done) {
					break;
				}
				pointCount++;
			}
			graphics.FillPolygon(&blueBrush, point, npoints);
		}
		delete [] point;
	}
#endif

	for (short i=0; i<NItemR(); i++) {
		MFCEditorItemView *ir = ItemR(i);
		if (ir->type != MFCEditorItemView::DISCRETE && ir->BoundingBox().Intersects(clipBox)) {
			ir->Draw(&graphics, &clipBox);
		}
	}
	for (short i=0; i<NItemR(); i++) {
		MFCEditorItemView *ir = ItemR(i);
		if (ir->type == MFCEditorItemView::DISCRETE && ir->BoundingBox().Intersects(clipBox)) {
			ir->Draw(&graphics, &clipBox);
		}
	}
	DrawCursor(&graphics, &clipBox);
#else
	DrawGrid(pdc, &clipBox);
	DrawCursor(pdc);
#endif
}


/////////////////////////////////////////////////////////////////////////////
// MFCDataEditor assorted wrappers
/////////////////////////////////////////////////////////////////////////////
/*
MFCEditorItemView*
MFCSampleDataEditor::AddStreamItemView(StreamItem *i, CPoint *pt, bool redraw)
{
	if (i == NULL) {
		return NULL;
	}
	Time	evT = i->time + i->Duration();
	if (evT > lastScheduledEvent) {
		lastScheduledEvent = evT;
	}
	MFCEditorItemView *nv = NULL;
	switch (i->type) {
		case S_BEND:
		case S_CTRL:
		case S_PROG: {
			uchar	d1=0;
			if (i->type == S_CTRL) {
				d1 = ((StreamCtrl*)i)->ctrl.controller;
			}
			MFCStreamItemListView *nlv = NULL;
			nlv = ItemListViewFor(i);
			if (nlv == NULL) {
				nlv = new MFCStreamItemListView(this, pt, i->type, d1);
				AddI(nlv);
			}
			if (redraw) {
				nlv->Redraw(true);
			}
			nv = nlv;
			break;
		}
		case S_NOTE: {
			MFCStreamItemView	*siv = new MFCStreamItemView(this, i);
			AddI(siv);
			if (redraw) {
				siv->Redraw(true);
			}
			nv = siv;
			break;
		}
	}
	return nv;
}*/


void
MFCSampleDataEditor::MoveItemViewFor(void *i)
{
	MFCEditorItemView *nv = ItemViewFor(i);
	nv->DrawMove();
}

void
MFCSampleDataEditor::RemoveItemViewFor(void *i)
{
	MFCEditorItemView *qir = ItemViewFor(i);
	RemoveItemView(qir);
}

/////////////////////////////////////////////////////////////////////////////
// MFCDataEditor mouse and keyboard handlers
/////////////////////////////////////////////////////////////////////////////

afx_msg void
MFCSampleDataEditor::OnLButtonDown(
			UINT nFlags,
			CPoint point 
		)
{
	point += GetScrollPosition();
	last_mouse_point = mouse_click = point;

	mouse_action = QUA_MOUSE_ACTION_NONE;

//	short	at_note = Pix2Note(point.y);
	Time	at_time;
	Time	dur_time;
	Pix2Time(point.x, at_time);
	dur_time.metric = at_time.metric;
	dur_time.ticks = 1;

//	int		bar, barbeat, beattick;
//	at_time.GetBBQValue(bar, barbeat, beattick);
//	fprintf(stderr, "left button down: on point %d %d, channel %d, ticks %d t %d:%d.%d\n", point.x, point.y, at_channel, at_time.ticks, bar, barbeat, beattick);

	if (take == NULL) {
		return;
	}
//	mouse_sub_item = NULL;
	switch (currentTool) {
		case ID_ARTOOL_POINT: {
			DeselectAll();
			short	hitType;
			void	*hitIt=NULL;
			mouse_item = ItemViewAtPoint(mouse_click, nFlags, hitType, hitIt);
			if (mouse_item) {
				if (hitIt) {
//					mouse_sub_item = (StreamItem *)hitIt;
				}
				switch (mouse_item->type) {
					case MFCEditorItemView::DISCRETE: {
//						MFCStreamItemView	*siv = (MFCStreamItemView *)mouse_item;
//						StreamItem	*item = siv->item;
//						if (item) {
//							if (item->type == S_NOTE) {
//								mouse_action = QUA_MOUSE_ACTION_MOVE_EVENT;
//								mouse_move_item_offset.x = mouse_click.x - mouse_item->BoundingBox().left;
//								mouse_move_item_offset.y = mouse_click.y;
//								mouse_item->Select(true);
//							}
//						}
						break;
					}
					case MFCEditorItemView::LIST: {
//						MFCStreamItemListView	*siv = (MFCStreamItemListView *)mouse_item;
//						if (hitType == 1) { // top edge
//							mouse_action = QUA_MOUSE_ACTION_MOVE_INSTANCE;
//							mouse_move_item_offset.x = mouse_click.x;
//							mouse_move_item_offset.y = mouse_click.y;
//							mouse_item->Select(true);
//						} else if (hitType == 2) { // bottom edge
//							mouse_action = QUA_MOUSE_ACTION_SIZE_INSTANCE;
//							mouse_move_item_offset.x = mouse_click.x;
//							mouse_move_item_offset.y = mouse_click.y;
//							mouse_item->Select(true);
//						} else if (hitType == 3) { 	// top edge of an interior item point
//							mouse_action = QUA_MOUSE_ACTION_MOVE_EVENT;
//							mouse_move_item_offset.x = mouse_click.x;
//							mouse_move_item_offset.y = mouse_click.y;
//							mouse_item->Select(true);
//						} else if (hitType == 4) { // side edge of an interior item point
//							mouse_action = QUA_MOUSE_ACTION_SIZE_EVENT;
//							mouse_move_item_offset.x = mouse_click.x;
//							mouse_move_item_offset.y = mouse_click.y;
//							mouse_item->Select(true);
//						}
						break;
					}

					case MFCEditorItemView::CLIP: {
						MFCClipItemView	*siv = (MFCClipItemView *)mouse_item;
						if (hitType == 1) { 	// top edge of an interior item point
							mouse_action = QUA_MOUSE_ACTION_MOVE_EVENT;
							mouse_move_item_offset.x = mouse_click.x;
							mouse_move_item_offset.y = mouse_click.y;
							mouse_item->Select(true);
						} else if (hitType == 2) { // side edge of an interior item point
							mouse_action = QUA_MOUSE_ACTION_SIZE_EVENT;
							mouse_move_item_offset.x = mouse_click.x;
							mouse_move_item_offset.y = mouse_click.y;
							mouse_item->Select(true);
						}
						break;
					}

				}
			}
		}
		case ID_ARTOOL_SLICE: {
			break;
		}
		case ID_ARTOOL_RGSELECT: {
			mouse_action = QUA_MOUSE_ACTION_REGION_SELECT;
			BRect	r(min(mouse_click.x,point.x),min(mouse_click.y,point.y),max(mouse_click.x,point.x),max(mouse_click.y,point.y));
			ChangeSelection(r);
			break;
		}
		case ID_ARTOOL_DRAW: {
			short	hitType;
			void	*hitIt=NULL;
			mouse_item = ItemViewAtPoint(mouse_click, nFlags, hitType, hitIt);
			if (mouse_item == NULL) {
//				StreamItem		*iv=NULL;
//				iv = new StreamNote(&at_time, 0, at_note, 80, 1);
//				stream->InsertItem(iv);
//				mouse_item = AddStreamItemView(iv, NULL, true);
			}
			if (mouse_item) {
				if (hitIt) {
//					mouse_sub_item = (StreamItem *)hitIt;
				}
				switch (mouse_item->type) {
					case MFCEditorItemView::DISCRETE: {
//						MFCStreamItemView	*siv = (MFCStreamItemView *)mouse_item;
//						StreamItem	*item = siv->item;
//						if (item) {
//							if (item->type == S_NOTE) {
//								mouse_action = QUA_MOUSE_ACTION_SIZE_EVENT;
//								mouse_move_item_offset.x = siv->BoundingBox().left;
//								mouse_move_item_offset.y = mouse_click.y;
//								Pix2Time(point.x-mouse_move_item_offset.x, at_time);
//								SetItemDuration(siv, at_time.ticks);
//							}
//						}
						break;
					}
					case MFCEditorItemView::LIST: {
						break;
					}
				}
			} else {
				;
			}
			break;
		}
	}
}

afx_msg void
MFCSampleDataEditor::OnLButtonUp(
			UINT nFlags,
			CPoint point 
		)
{
	point += GetScrollPosition();
	mouse_item = NULL;
	mouse_action = QUA_MOUSE_ACTION_NONE;
}


afx_msg void
MFCSampleDataEditor::OnLButtonDblClk(
			UINT nFlags,
			CPoint point 
		)
{
	point += GetScrollPosition();

	Time	at_time;
	Pix2Time(point.x, at_time);
//	short	at_note = Pix2Note(point.y);

//	int		bar, barbeat, beattick;
//	at_time.GetBBQValue(bar, barbeat, beattick);
//	fprintf(stderr, "left button double click: on point %d %d, channel %d, ticks %d t %d:%d.%d\n", point.x, point.y, at_channel, at_time.ticks, bar, barbeat, beattick);
}

afx_msg void
MFCSampleDataEditor::OnMButtonDown(
			UINT nFlags,
			CPoint point 
		)
{
	point += GetScrollPosition();

	Time	at_time;
	Pix2Time(point.x, at_time);
//	short	at_note = Pix2Note(point.y);
//	int		bar, barbeat, beattick;
//	at_time.GetBBQValue(bar, barbeat, beattick);
//	fprintf(stderr, "middle button down: on point %d %d, channel %d, ticks %d t %d:%d.%d\n", point.x, point.y, at_channel, at_time.ticks, bar, barbeat, beattick);
}

afx_msg void
MFCSampleDataEditor::OnMButtonUp(
			UINT nFlags,
			CPoint point 
		)
{
	point += GetScrollPosition();
	mouse_action = QUA_MOUSE_ACTION_NONE;
}

afx_msg void
MFCSampleDataEditor::OnMButtonDblClk(
			UINT nFlags,
			CPoint point 
		)
{
	point += GetScrollPosition();

	Time	at_time;
//	short	at_note = Pix2Note(point.y);
	Pix2Time(point.x, at_time);

//	int		bar, barbeat, beattick;
//	at_time.GetBBQValue(bar, barbeat, beattick);
//	fprintf(stderr, "middle button double click: on point %d %d, channel %d, ticks %d t %d:%d.%d\n", point.x, point.y, at_channel, at_time.ticks, bar, barbeat, beattick);
}

void
MFCSampleDataEditor::EditorContextMenu(CPoint &point, UINT nFlags)
{
	Time	at_time;
//	short	at_note = Pix2Note(point.y);
	Pix2Time(point.x, at_time);

	CPoint		popPt = point;
	ClientToScreen(&popPt);

	CMenu		*ctxtMenu = new CMenu;
	ctxtMenu->CreatePopupMenu();
	short	i=0;
	char	buf[128];
	short	hitType;
	void	*hitIt=NULL;
	CPoint	itPt = point;
	itPt += GetScrollPosition();
	MFCEditorItemView	* iv = ItemViewAtPoint(itPt, nFlags|MK_CONTROL, hitType, hitIt);
	bool	menu_built = false;
	if (iv != NULL) {
		if (iv->type == MFCEditorItemView::LIST) {
		} else if (iv->type == MFCEditorItemView::DISCRETE) {
		} else if (iv->type == MFCEditorItemView::CLIP) {
			menu_built = true;
			ctxtMenu->AppendMenu(MF_POPUP, ID_EDITORCONTEXT_DEL_CLIP, "Delete Clip");
		} else if (iv->type == MFCEditorItemView::ENV) {
		} else {
		}
	}

	if (!menu_built) { // run with the default menu
		ctxtMenu->AppendMenu(MF_POPUP, ID_EDITORCONTEXT_ADD_CLIP, "Add Clip");
	}

	short ret = ctxtMenu->TrackPopupMenuEx(
						TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RETURNCMD,
						popPt.x, popPt.y, this, NULL);
	delete ctxtMenu;
	if (ret == 0) {	// cancel
		return;
	}

	if (ret == ID_EDITORCONTEXT_ADD_CLIP) {
		if (take != NULL) {
			StabEnt	*chSym = parent->Symbol();
			if (chSym) {
				Sample	*v;
				if ((v = chSym->SampleValue()) != NULL) {
					Time	dur_time;
					dur_time.Set(1,0,0,at_time.metric);
					string	nmbuf = glob.makeUniqueName(chSym, "clip", 1);
					Clip	*c = v->AddClip(nmbuf, take, at_time, dur_time, true);
//					MFCEditorItemView	*added_item = AddClipItemView(c);
				}
			}
		}
	} else if (ret == ID_EDITORCONTEXT_DEL_CLIP) {
		StabEnt	*chSym = parent->Symbol();
		if (iv != NULL && chSym) {
			Sample	*v;
			MFCClipItemView	*civ = (MFCClipItemView*)iv;
			if ((v = chSym->SampleValue()) != NULL && civ->item != NULL) {
				Clip	*c = civ->item;
				fprintf(stderr, "remove clip %x\n", c);
//				DelClipItemView(iv);
				v->RemoveClip(c, true);
			}
		}
	}
}


bool
MFCSampleDataEditor::CursorCheck(CPoint point, bool isCtl)
{
	if (bounds.PtInRect(point)) {
		if (!mouse_captured) {
			SetCapture();
			mouse_captured = true;
			originalCursor = ::GetCursor();
		}
		switch (currentTool) {
			case ID_ARTOOL_POINT: {
				if (isCtl) {
					::SetCursor(pointCtlCursor);
				} else {
					::SetCursor(pointCursor);
				}
				break;
			}
			case ID_ARTOOL_DRAW: {
				if (isCtl) {
					::SetCursor(drawCtlCursor);
				} else {
					::SetCursor(drawCursor);
				}
				break;
			}
			case ID_ARTOOL_SLICE: {
				::SetCursor(sliceCursor);
				break;
			}
			case ID_ARTOOL_RGSELECT: {
				::SetCursor(regionCursor);
				break;
			}
		}
	} else if (!bounds.PtInRect(point)) {
		if (mouse_captured) {
			ReleaseCapture();
			mouse_captured = false;
			if (originalCursor != NULL) {
				::SetCursor(originalCursor);
				originalCursor = NULL;
			}
		}
	}
	return true;
}

afx_msg void
MFCSampleDataEditor::OnMouseMove(
			UINT nFlags,
			CPoint point 
		)
{
	point += GetScrollPosition();

	Time	at_time;
//	short	at_note = Pix2Note(point.y);

	CursorCheck(point, (nFlags&MK_CONTROL)!=0);

	switch (mouse_action) {
		case QUA_MOUSE_ACTION_MOVE_INSTANCE: {
			if (mouse_item) {
				switch (mouse_item->type) {
					case MFCEditorItemView::DISCRETE: {
						break;
					}
					case MFCEditorItemView::LIST: {
//						MFCStreamItemListView	*siv = (MFCStreamItemListView *)mouse_item;
//						siv->MoveToY(point.y);
						break;
					}
				}
			}
			break;
		}
		case QUA_MOUSE_ACTION_MOVE_EVENT: {
			if (mouse_item) {
				switch (mouse_item->type) {
					case MFCEditorItemView::DISCRETE: {
						break;
					}
					case MFCEditorItemView::LIST: {
						break;
					}
					case MFCEditorItemView::CLIP: {
						MFCClipItemView	*siv = (MFCClipItemView *)mouse_item;
						Pix2Time(point.x, at_time);
						siv->SetClipStartTime(at_time);
						break;
					}
				}
			}
			break;
		}
		case QUA_MOUSE_ACTION_REGION_SELECT: {
			BRect	r(min(mouse_click.x,point.x),min(mouse_click.y,point.y),max(mouse_click.x,point.x),max(mouse_click.y,point.y));
			ChangeSelection(r);
			break;
		}
		case QUA_MOUSE_ACTION_SIZE_INSTANCE: {
			if (mouse_item) {
				switch (mouse_item->type) {
					case MFCEditorItemView::DISCRETE: {
						break;
					}
					case MFCEditorItemView::LIST: {
//						MFCStreamItemListView	*siv = (MFCStreamItemListView *)mouse_item;
//						siv->ResizeToY(point.y - siv->BoundingBox().top);
						break;
					}
				}
			}
			break;
		}
		case QUA_MOUSE_ACTION_SIZE_EVENT: {
			if (mouse_item) {
				switch (mouse_item->type) {
					case MFCEditorItemView::DISCRETE: {
						break;
					}
					case MFCEditorItemView::LIST: {
						break;
					}
					case MFCEditorItemView::CLIP: {
						MFCClipItemView	*siv = (MFCClipItemView *)mouse_item;
						Pix2Time(point.x, at_time);
						siv->SetClipEndTime(at_time);
						break;
					}
				}
			}
			break;
		}
		case QUA_MOUSE_ACTION_DRAW_EVENT: {
			if (mouse_item) {
				switch (mouse_item->type) {
					case MFCEditorItemView::DISCRETE: {
//						MFCStreamItemView	*siv = (MFCStreamItemView *)mouse_item;
//						StreamItem	*item = siv->item;
						break;
					}
					case MFCEditorItemView::LIST: {
//						MFCStreamItemListView	*siv = (MFCStreamItemListView *)mouse_item;
//						Pix2Time(point.x, at_time);
//						StreamItem	*it = NULL;	// item strictly on pixel time
//						StreamItem	*pt = NULL; // item strictly before pixel time
//						float		val = siv->Pix2Val(point.y);
//						switch (siv->listitemType) {
//							case S_CTRL: {
//								if (stream) {
//									short	actualval = 127*val;
//									pt=stream->FindItemAtTime(-2,
//														at_time,
//														S_CTRL,
//														-1,
//														siv->listitemData1);
//									it=stream->FindItemAtTime(0,
//														at_time,
//														S_CTRL,
//														-1,
//														siv->listitemData1,
//														pt);
//									if (pt==NULL || ((StreamCtrl*)pt)->ctrl.amount != actualval) {
//										if (it==NULL) {
//											StreamItem	*ft = NULL;
//											if (pt != NULL) {
//												ft = pt->Subsequent(S_CTRL, -1, siv->listitemData1);
//											}
//											if (ft != NULL && ((StreamCtrl *)ft)->ctrl.amount == actualval) {
//												siv->SetSubItemTime(ft, at_time);
//											} else {
//												it = new StreamCtrl(&at_time, 0, siv->listitemData1, 0);
//												stream->InsertItem(it);
//												siv->SetSubItemValue(it, val);
//											}
//										} else {
//											siv->SetSubItemValue(it, val);
//										}
//									} else { // pt != NULL && pt->amount == the pixelval
//										if (it != NULL) {
//											siv->DelSubItem(it);
//											it = NULL;
//										}
//									}
//								}
//								break;
//							}
//						}
					}
				}
			}
			break;
		}
	}

	last_mouse_point = point;
}

afx_msg BOOL
MFCSampleDataEditor::OnMouseWheel(
			UINT nFlags,
			short zDelta,
			CPoint point 
		)
{
	point += GetScrollPosition();
	return MFCDataEditor::OnMouseWheel(nFlags, zDelta, point);
}


afx_msg void
MFCSampleDataEditor::OnKeyDown(
			UINT nChar,
			UINT nRepCnt,
			UINT nFlags 
		)
{
	CursorCheck(CPoint(bounds.left+1,bounds.top+1), nChar==VK_CONTROL);
	switch (nChar) {
		case VK_DELETE: {
			short i=0;
			while (i<NItemR()) {
				MFCEditorItemView *iv = ItemR(i);
				if (iv == NULL) {
					break;
				}
				if (iv->selected) {
					switch (iv->type) {
						case MFCEditorItemView::DISCRETE: {
							break;
						}
						case MFCEditorItemView::LIST: {
							break;
						}
						case MFCEditorItemView::CLIP: {
							MFCClipItemView	*siv = (MFCClipItemView *)iv;
							break;
						}
					}
				} else {
					i++;
				}
			}
			break;
		}
		default: {
            MFCDataEditor::OnKeyDown(nChar, nRepCnt, nFlags);
			break;
		}
	}
}

afx_msg void
MFCSampleDataEditor::OnKeyUp(
			UINT nChar,
			UINT nRepCnt,
			UINT nFlags 
		)
{
	CursorCheck(CPoint(bounds.left+1,bounds.top+1), false);
	MFCDataEditor::OnKeyUp(nChar, nRepCnt, nFlags);
}


// this gets called from the context menu, and the key down routines
// i think we're well behaved round here, but we should just be really
// bloody careful about complications this might cause with drawing routines....
bool
MFCSampleDataEditor::DelItemView(MFCEditorItemView *iv)
{
	switch (iv->type) {
		case MFCEditorItemView::DISCRETE: {
//			MFCStreamItemView	*siv = (MFCStreamItemView *)iv;
//			if (stream) {
//				stream->DeleteItem(siv->item);
//			}
			break;
		}
		case MFCEditorItemView::LIST: {
//			MFCStreamItemListView	*siv = (MFCStreamItemListView *)iv;
//			if (stream) {
//				stream->DeleteItemsMatching(siv->listitemType, -1, siv->listitemData1);
//			}
			break;
		}
	}
	CRect	updr = bounds;
	RemItemR(iv);
	delete iv;
	InvalidateRect(&bounds);
	return true;
}

