
// MFCArrangeView.cpp : implementation file
//
//#define _AFXDLL
#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include "qua_version.h"

#include "StdDefs.h"

#include "QuaMFC.h"
#include "QuaMFCDoc.h"
#include "MFCArrangeView.h"
#include "MFCChannelView.h"
#include "MFCChannelMountView.h"
#include "ChildFrm.h"

#include "ShlObj.h"

#include "Qua.h"
#include "Time.h"
#include "Instance.h"
#include "Schedulable.h"
#include "Envelope.h"
#include "BaseVal.h"

#include "Colors.h"

#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MFCArrangeView

IMPLEMENT_DYNCREATE(MFCArrangeView, CScrollView)

MFCArrangeView::MFCArrangeView()
{
	displayMetric = &Metric::std;
	ticksPerNotch = 1;
	pixPerNotch = 2;
	frame = CRect(0, 0, 0, 0);
	bounds = CRect(0, 0, 0, 0);
	channeler = NULL;
	lastScheduledEvent.Set("120:0.0 bbq");
	currentTool = ID_ARTOOL_POINT;
	mouse_instance = NULL;
	mouse_action = QUA_MOUSE_ACTION_NONE;
	mouse_captured = false;
	originalCursor = NULL;
}

MFCArrangeView::~MFCArrangeView()
{
	for (short i=0; i<NIR(); i++) {
		delete (MFCInstanceView *)IR(i);
	}
}


BEGIN_MESSAGE_MAP(MFCArrangeView, MFCSequenceEditor)
	//{{AFX_MSG_MAP(MFCArrangeView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_WM_CREATE()
	ON_WM_DESTROY()
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
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MFCArrangeView drawing


void
MFCArrangeView::displayArrangementTitle(const char *nm)
{
	SetWindowText(nm);
}

void
MFCArrangeView::DisplayChannelMetric(Channel *c, Metric *m, bool async)
{
	if (async) {
		;
	} else {
		;
	}
}

void
MFCArrangeView::DisplayMetric(Metric *m, bool async)
{
	if (async) {
		;
	} else {
		;
	}
}

void
MFCArrangeView::DisplayTempo(float t, bool async)
{
	if (async) {
		;
	} else {
		;
	}
}

void
MFCArrangeView::DisplayCurrentTime(Time &t, bool async)
{
	if (async) {
		SetCursor(t);
	} else {
		SetCursor(t);
	}
}

void
MFCArrangeView::SetCursor(Time &t)
{
	long	oldCursorPx = cursorPx;
	cursorTime = t;
	cursorPx = Time2Pix(t);
	if (oldCursorPx != cursorPx) {
		BRect r;
		r.Set(cursorPx, 0, cursorPx+1,bounds.bottom);
		InvalidateRect(&r);
		r.Set(oldCursorPx, 0, oldCursorPx+1,bounds.bottom);
		InvalidateRect(&r);
		UpdateWindow();
	}
}

// height of channel c, or 0 if this is out of bounds
long
MFCArrangeView::ChannelHeight(int c)
{
	if (channeler == NULL) {
		return 0;
	}
	MFCChannelView	*cv = (MFCChannelView *)channeler->CR(c);
	return cv?cv->bounds.bottom:0;
}

// y pixel value of the top of channel c, in the arrangers
// frame of reference
long
MFCArrangeView::Channel2Pix(int c)
{
	if (channeler == NULL) {
		return 0;
	}
	MFCChannelView	*cv = (MFCChannelView *)channeler->CR(c);
	if (cv == NULL) {
		cv = (MFCChannelView *) channeler->CR(channeler->NCR());
		return cv?cv->frame.bottom:0;
	}

	return cv->frame.top;
}

#define MAXIMUM_ARRANGER_PIXEL 1000000
// ticksPerNotch give the no of ticks per grid point. this allows a pixel width to separate
// grid markings on the display at the highest grid resolution, which is 1 tick per grid
// marking or 2 pixels per tick. ie highest resolution <=> ticksPerP2X == 1, and any higher
// value is a lower resolution
long
MFCArrangeView::Time2Pix(Time &t)
{
	if (t.ticks == INFINITE_TICKS) {
		return MAXIMUM_ARRANGER_PIXEL;
	}
	if (t.metric != displayMetric) {
		;
	}
	return pixPerNotch*t.ticks/ticksPerNotch;
}


void
MFCArrangeView::Pix2Time(long px, Time &t)
{
	if (t.metric != displayMetric) {
		;
	}
	t.ticks = ticksPerNotch*px/pixPerNotch;
}


short
MFCArrangeView::Pix2Channel(long py)
{
	if (channeler == NULL) {
		return 0;
	}
	for (short i=0; i<channeler->NCR(); i++) {
		MFCChannelView	*cv = (MFCChannelView *)channeler->CR(i);
		if (cv->frame.top <= py && py < cv->frame.bottom) {
			return i;
		}
	}
	return 0;
}


MFCInstanceView*
MFCArrangeView::InstanceViewAtPoint(CPoint &p)
{
	for (short i=0; i<NIR(); i++) {
		MFCInstanceView *iv = (MFCInstanceView *)IR(i);
		if (iv->bounds.Contains(p)) {
			return iv;
		}
	}
	return NULL;
}

void
MFCArrangeView::DeselectAll()
{
	for (short i=0; i<NIR(); i++) {
		MFCInstanceView *iv = (MFCInstanceView *)IR(i);
		iv->Select(false);
	}
	MFCSequenceEditor::DeselectAll();
}

void
MFCArrangeView::ChangeSelection(BRect &r)
{
	for (short i=0; i<NIR(); i++) {
		MFCInstanceView *iv = (MFCInstanceView *)IR(i);
		if (iv->bounds.Intersects(r)) {
			iv->Select(true);
		} else {
			iv->Select(false);
		}
	}
}


#ifdef QUA_V_GDI_PLUS
void
MFCArrangeView::DrawGridGraphics(Graphics *pdc, CRect *cbp)
{
	Metric *dm = (displayMetric?displayMetric:quaLink->QMetric());
//	pdc->SetBkColor(rgb_red);
	Pen	ltGrayPen(AlphaColor(255, rgb_ltGray), 1);
	Pen	mdGrayPen(AlphaColor(255, rgb_mdGray), 1);
	Pen	dkGrayPen(AlphaColor(255, rgb_dkGray), 1);

	long startTick = cbp->left/pixPerNotch;
	long endTick = cbp->right/pixPerNotch;
	short notchInc = 1;
	long notchPx = startTick*pixPerNotch;
	long tickCnt = 0;
	short tickPerNotch = 1;

	MFCChannelView *fv =(MFCChannelView *)channeler->CR(0);
	MFCChannelView *ev =(MFCChannelView *)channeler->CR(channeler->NCR()-1);

	for (tickCnt=startTick; tickCnt<=endTick; tickCnt+=tickPerNotch, notchPx += pixPerNotch) {
		Pen		*gridPen;
		if (tickCnt % (dm->granularity*dm->beatsPerBar) == 0) {
			gridPen = &dkGrayPen;
		} else if (tickCnt % dm->granularity == 0) {
			gridPen = &mdGrayPen;
		} else {
			gridPen = &ltGrayPen;
		}
		pdc->DrawLine(gridPen, notchPx, cbp->top, notchPx, cbp->bottom);
	}
	for (short i=0; i<quaLink->NChannel(); i++) {
		pdc->DrawLine(&dkGrayPen, cbp->left, Channel2Pix(i), cbp->right, Channel2Pix(i));
	}
}
#endif

void
MFCArrangeView::DrawGrid(CDC *pdc, CRect *cbp)
{
	Metric *dm = (displayMetric?displayMetric:quaLink->QMetric());
//	pdc->SetBkColor(rgb_red);
	pdc->SelectObject(GetStockObject(DC_BRUSH));
	pdc->SelectObject(GetStockObject(DC_PEN));
	pdc->SetDCPenColor(rgb_ltGray);
	long startTick = cbp->left/pixPerNotch;
	long endTick = cbp->right/pixPerNotch;
	short notchInc = 1;
	long notchPx = startTick*pixPerNotch;
	long tickCnt = 0;
	short tickPerNotch = 1;

	MFCChannelView *fv =(MFCChannelView *)channeler->CR(0);
	MFCChannelView *ev =(MFCChannelView *)channeler->CR(channeler->NCR()-1);
	for (tickCnt=startTick; tickCnt<endTick; tickCnt+=tickPerNotch, notchPx += pixPerNotch) {
		if (tickCnt % (dm->granularity*dm->beatsPerBar) == 0) {
			pdc->SetDCPenColor(rgb_dkGray);
		} else if (tickCnt % dm->granularity == 0) {
			pdc->SetDCPenColor(rgb_mdGray);
		} else {
			pdc->SetDCPenColor(rgb_ltGray);
		}
		pdc->MoveTo(notchPx, cbp->top);
		pdc->LineTo(notchPx, cbp->bottom);
	}
	pdc->SetDCPenColor(rgb_dkGray);
	for (short i=0; i<quaLink->NChannel(); i++) {
		pdc->MoveTo(cbp->left, Channel2Pix(i));
		pdc->LineTo(cbp->right, Channel2Pix(i));
	}
}

#ifdef QUA_V_GDI_PLUS

void
MFCArrangeView::DrawCursor(Graphics *pdc, CRect *clipBox)
{
	bool	doDraw = true;
	if (clipBox != NULL) {
		if (clipBox->left > cursorPx || cursorPx > clipBox->right) {
			doDraw = false;
		}
	}
	if (doDraw) {
		Pen	cursorPen(AlphaColor(255, rgb_purple), 1);
		pdc->DrawLine(&cursorPen,
						cursorPx, clipBox->top,
						cursorPx, clipBox->bottom);
	}
}

#else

void
MFCArrangeView::DrawCursor(CDC *pdc, CRect *clipBox)
{
	;
}

#endif

void
MFCArrangeView::OnDraw(CDC* pdc)
{
	CRect	clipBox;
	int		cbType;
	cbType = pdc->GetClipBox(&clipBox);
//	fprintf(stderr, "OnDraw() clip (%d %d %d %d) type %s\n", clipBox.left, clipBox.top, clipBox.right, clipBox.bottom, cbType==COMPLEXREGION?"complex":cbType==SIMPLEREGION?"simple":cbType==NULLREGION?"null":"error");
//	fprintf(stderr, "lt %d\n", lastScheduledEvent.ticks);
	CDocument* pDoc = GetDocument();
#ifdef QUA_V_GDI_PLUS
	Graphics	graphics(pdc->m_hDC);
	DrawGridGraphics(&graphics, &clipBox);
// straight GDI seems a lot faster out of the box at drawing, ... perhaps there are ways of
// optimizing GDI+, but for the big line array in the grid, it slows the scrolling
// oth there's an occaisional glitch where the drawing code loops, and
// very little doco on gdi - gdi+ hybrids
//	DrawGrid(pdc, &clipBox);

// test the interoperation of GDI and GDI+
//	Color	p = Color::MakeARGB(100, 255, 20, 255);
//	Pen	semip(p, 3);
//	graphics.DrawLine(&semip, 0,0, 200, 200);
	for (short i=0; i<NIR(); i++) {
		MFCInstanceView *ir = (MFCInstanceView *)IR(i);
		if (ir->bounds.Intersects(clipBox)) {
			ir->Draw(&graphics, &clipBox);
		}
	}
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
	// draw envelopes

	// draw cursor (erase old one?)
	DrawCursor(&graphics, &clipBox);
#else
	DrawGrid(pdc, &clipBox);
	for (short i=0; i<NIR(); i++) {
		MFCInstanceView *ir = (MFCInstanceView *)IR(i);
		ir->Draw(pdc, &clipBox);
	}
	// draw envelopes

	// draw cursor (erase old one?)
	DrawCursor(pdc, &clipBox);
#endif
//	fprintf(stderr, "OnDraw() finito\n");
}

/////////////////////////////////////////////////////////////////////////////
// MFCArrangeView diagnostics

#ifdef _DEBUG
void MFCArrangeView::AssertValid() const
{
	CView::AssertValid();
}

void MFCArrangeView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// MFCArrangeView message handlers
/////////////////////////////////////////////////////////////////////////////

void
MFCArrangeView::EditorContextMenu(CPoint &point, UINT nFlags)
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

	MFCInstanceView		*instv=NULL;
	Instance			*inst=NULL;
	MFCEditorItemView	*iv=NULL;
	long				nEnvelope=0;
	bool				add_dflt_items = true;
	bool				added_items = false;
	vector<Envelope *> envelopes;
	if ((instv= InstanceViewAtPoint(itPt))!=NULL) {
		add_dflt_items = true;
		added_items = true;
		inst = instv->instance;
		if (inst) {
			CMenu		*envMenu = new CMenu;
			envMenu->CreatePopupMenu();

			envelopes = quaLink->ListEnvelopesFor(inst->sym);
			nEnvelope = envelopes.size();
			char	buf[256];
			sprintf(buf, "Envelopes for '%s'", inst->sym->name.c_str());
			ctxtMenu->AppendMenu(MF_POPUP, (UINT) envMenu->m_hMenu, buf);
//			for each variable
//				envMenu->AppendMenu(MF_STRING, ID_EDIT_SHOW_ENVELOPE, variable name);
//					tick state
		}
	} else if ((iv  = ItemViewAtPoint(itPt, nFlags|MK_CONTROL, hitType, hitIt)) != NULL) {
		if (iv->type == MFCEditorItemView::LIST) {
		} else if (iv->type == MFCEditorItemView::DISCRETE) {
		} else if (iv->type == MFCEditorItemView::CLIP) {
			add_dflt_items = true;
			added_items = true;
			MFCClipItemView	*civ = (MFCClipItemView*)iv;
			Clip	*c = civ->item;
			bool	ismark = false;
			if (c && c->duration.ticks <= 0) {
				ismark = true;
			}
			ctxtMenu->AppendMenu(MF_STRING, ID_EDITORCONTEXT_DEL_CLIP,
				ismark? "Delete Marker":"Delete Region");
		} else if (iv->type == MFCEditorItemView::ENV) {
		} else {
		}
	}

	if (add_dflt_items) { // run with the default menu
		if (added_items) {
			ctxtMenu->AppendMenu(MF_SEPARATOR);
		}
		ctxtMenu->AppendMenu(MF_STRING, ID_EDIT_ADD_CLIP, "Add Region");
		ctxtMenu->AppendMenu(MF_STRING, ID_EDIT_ADD_MARKER, "Add Marker");
	}

	short ret = ctxtMenu->TrackPopupMenuEx(
						TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RETURNCMD,
						popPt.x, popPt.y, this, NULL);
	delete ctxtMenu;
	if (ret == 0) {	// cancel
		return;
	}

	CQuaMFCDoc	*qdoc = (CQuaMFCDoc *)GetDocument();
	if (qdoc == NULL) {
		fprintf(stderr, "menu item track finds null doc");
		return;
	} else if (qdoc->qua == NULL) {
		fprintf(stderr, "menu item track finds null sequencer");
	}
	if (ret == ID_EDIT_ADD_CLIP || ret == ID_EDIT_ADD_MARKER) {
		StabEnt	*qSym = qdoc->qua->sym;
		if (qSym) {
			Time	dur_time;
			string nm;
			if (ret == ID_EDIT_ADD_CLIP) {
				dur_time.Set(1,0,0,at_time.metric);
				nm = glob.makeUniqueName(qSym, "region", 1);
			} else {
				dur_time.ticks = 0;
				nm = glob.makeUniqueName(qSym, "marker", 1);
			}
			Clip	*c = qdoc->qua->addClip(nm, at_time, dur_time, true);
//			MFCEditorItemView	*added_item = AddClipItemView(c);
		}
	} else if (ret == ID_EDITORCONTEXT_DEL_CLIP || ret == ID_EDITORCONTEXT_DEL_MARKER) {
		StabEnt	*qSym = qdoc->qua->sym;
		if (iv != NULL && qSym) {
			Sample	*v;
			MFCClipItemView	*civ = (MFCClipItemView*)iv;
			if (civ->item != NULL) {
				Clip	*c = civ->item;
				fprintf(stderr, "remove clip %x\n", c);
//				DelClipItemView(iv);
				qdoc->qua->removeClip(c, true);
			}
		}
	} else if (ret >= ID_EDIT_SHOW_ENVELOPE && ret < ID_EDIT_SHOW_ENVELOPE+nEnvelope) {
		if (inst != NULL) {
			long		ind = ret-ID_EDIT_SHOW_ENVELOPE;
			StabEnt		*envSym = envelopes[ind]->sym;
			if (envSym) {
				long  nSegs = qdoc->qua->CountEnvelopeSegments(
													inst->sym, envSym);
				EnvelopeSegment		*segs = new EnvelopeSegment[nSegs];

				qdoc->qua->FetchEnvelopeSegments(inst->sym, envSym, nSegs, segs);

				delete segs;
			}
		}
	}
}



int
MFCArrangeView::OnCreate(LPCREATESTRUCT lpCreateStruct )
{
	int		ret = MFCSequenceEditor::OnCreate(lpCreateStruct);

	BRect		r;
	r.Set(0, bounds.bottom-60, 600, bounds.bottom-50);
	tScale.CreateTScale(r, this, 1111, this);
	return ret;
}

void
MFCArrangeView::OnDestroy()
{
	;
}

void
MFCArrangeView::OnInitialUpdate()
{
	CQuaMFCDoc	*qdoc = (CQuaMFCDoc *)GetDocument();
	if (qdoc == NULL) {
		reportError("ArrangeView: initial update of arrange view finds a null sequencer document");
	} else if (qdoc->qua == NULL) {
		reportError("ArrangeView: initial update finds a null sequencer");
	} else {	// set qua up with our hooks
		SetLinkage(&qdoc->display);
		quaLink->AddArranger(this);

		CFrameWnd	*fw = GetParentFrame();
		QuaChildFrame *qfr = (QuaChildFrame *)fw;
		if (qfr) {
			qfr->transportBar.SetLinkage(quaLink);
			quaLink->AddTransporter(&qfr->transportBar);
		}
		if (qdoc->qua->metric) {
			quaLink->DisplayTempo(qdoc->qua->metric->tempo, false);
		}

		for (auto ip = qdoc->qua->schedule.begin(); ip != qdoc->qua->schedule.end(); ++ip) {
			AddInstanceRepresentation(*ip);
		}
		AddAllItemViews(false);

		quaLink->DisplayGlobalTime(qdoc->qua->theTime, false);

		SetHVScroll();
	}
}


bool
MFCArrangeView::AddAllItemViews(bool redraw)
{
	if (NItemR() > 0) {
		ClearAllItemViews(false);
	}

	CQuaMFCDoc	*qdoc = (CQuaMFCDoc *)GetDocument();
	if (qdoc == NULL) {
		fprintf(stderr, "ArrangeView::AddAllItemViews() finds a null sequencer document");
		return false;
	}
	if (qdoc->qua == NULL) {
		fprintf(stderr, "ArrangeView::AddAllItemViews() finds a null sequencer");
		return false;
	}
	StabEnt	*pars = qdoc->qua->sym;
	if (pars) {
		StabEnt	*sibp = pars->children;
		while (sibp != NULL) {
			if (sibp->type == TypedValue::S_CLIP) {
				Clip	*c = sibp->ClipValue(NULL);
//				StabEnt		*ts = c->media->sym;
				AddClipItemView(c);
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


/////////////////////////////////////////////////////////////////////////////
// MFCArrangeView dragndrop handlers (virtuals)
/////////////////////////////////////////////////////////////////////////////

DROPEFFECT
MFCArrangeView::OnDragEnter(
			COleDataObject* object,
			DWORD keyState,
			CPoint point 
		)
{
	fprintf(stderr, "MFCArrangeView::on drag enter\n");
	dragon.SetTo(object, keyState);
	switch (dragon.type) {
		case QuaDrop::SAMPLE:
		case QuaDrop::VOICE:
		case QuaDrop::INSTANCE:
		case QuaDrop::FILES:
			break;
		default:
			dragon.dropEffect = DROPEFFECT_NONE;
			return DROPEFFECT_NONE;
	}
	return dragon.dropEffect != DROPEFFECT_NONE? dragon.dropEffect:DROPEFFECT_NONE;
}

DROPEFFECT
MFCArrangeView::OnDragOver(
			COleDataObject* object,
			DWORD dwKeyState,
			CPoint point 
		)
{
	return dragon.dropEffect != DROPEFFECT_NONE? dragon.dropEffect:DROPEFFECT_NONE;
}


void
MFCArrangeView::OnDragLeave( )
{
	dragon.Clear();
}

BOOL
MFCArrangeView::OnDrop(
			COleDataObject* object,
			DROPEFFECT dropEffect,
			CPoint point 
		)
{
	cout << "MFCArrangeView::on drop\n";
	point += GetScrollPosition();
	switch (dragon.type) {
		case QuaDrop::FILES:
		case QuaDrop::AUDIOFILES:
		case QuaDrop::MIDIFILES: {
			Time	at_time;
			short	at_channel;
			Pix2Time(point.x, at_time);
			at_channel = Pix2Channel(point.y);
//			int		bar, barbeat, beattick;
//			at_time.GetBBQValue(bar, barbeat, beattick);
//			fprintf(stderr, "drag files is dropped: on point %d %d, channel %d, ticks %d t %d:%d.%d\n", point.x, point.y, at_channel, at_time.ticks, bar, barbeat, beattick);
			short	i;
			bool	drop_sample_file=false;
			bool	drop_midi_file=false;
			bool	drop_qua_file=false;
			for (i=0; ((unsigned)i)<dragon.data.filePathList->size(); i++) {
				string	mime_t = Qua::identifyFile(dragon.data.filePathList->at(i));
				if (mime_t.size() > 0) {
					cout << "drop " << dragon.data.filePathList->at(i) << ", " << mime_t << endl;
					if (mime_t == "audio/x-midi") {
						drop_midi_file = true;
						break;
					} else if (mime_t == "audio/x-wav" ||
								mime_t == "audio/x-raw" ||
								mime_t == "audio/x-aiff") {
						drop_sample_file = true;
						break;
					} else if (mime_t == "audio/x-quascript") {
						drop_qua_file = true;
						break;
					}
				} else {
//					fprintf(stderr, "dragged file %d unidentifiable\n", i);
				}
			}
			if (i == dragon.count) { // didn't find something useable in the drop
				return FALSE;
			}
			if (drop_sample_file) {
				quaLink->CreateSample("", *dragon.data.filePathList, at_channel, &at_time, NULL);
			} else if (drop_midi_file) {
				;
			} else if (drop_qua_file) {
				;
			} else {
				return FALSE;
			}
			return TRUE;
		}

		case QuaDrop::VSTPLUGIN: 
		case QuaDrop::VOICE: 
		case QuaDrop::SAMPLE: 
		case QuaDrop::SCHEDULABLE: {
			Time	at_time;
			short	at_channel;
			Pix2Time(point.x, at_time);
			at_channel = Pix2Channel(point.y);
//			int		bar, barbeat, beattick;
//			at_time.GetBBQValue(bar, barbeat, beattick);
//			fprintf(stderr, "drop schedulable %s: on point %d %d, channel %d, ticks %d t %d:%d.%d\n", dragon.data.symbol->name, point.x, point.y, at_channel, at_time.ticks, bar, barbeat, beattick);

			quaLink->CreateInstance(dragon.data.symbol, at_channel, at_time, NULL);
			return TRUE;
		}
			
		case QuaDrop::INSTANCE: {
			Time	at_time;
			short	at_channel;
			Pix2Time(point.x, at_time);
			at_channel = Pix2Channel(point.y);
//			int		bar, barbeat, beattick;
//			at_time.GetBBQValue(bar, barbeat, beattick);
//			fprintf(stderr, "drop instance %s: on point %d %d, channel %d, ticks %d t %d:%d.%d\n", dragon.data.symbol->name, point.x, point.y, at_channel, at_time.ticks, bar, barbeat, beattick);
			Instance *i = dragon.data.symbol->InstanceValue();
			if (i != nullptr) {
				quaLink->MoveInstance(dragon.data.symbol, at_channel, at_time, i->duration);
			} else {
				cout << "oops bad drop ... expected an instance" << endl;
			}
			return TRUE;
		}
		case QuaDrop::APPLICATION:
		case QuaDrop::PORT:
		case QuaDrop::BUILTIN:
		case QuaDrop::TEMPLATE:
			switch (dropEffect) {
				case DROPEFFECT_COPY:
					fprintf(stderr, "left click drag: on drop copy %d %d\n", point.x, point.y);
					break;
				case DROPEFFECT_MOVE:
					fprintf(stderr, "left click drag: on drop move %d %d\n", point.x, point.y);
					break;
				case DROPEFFECT_LINK:
					fprintf(stderr, "left click drag: on drop link %d %d\n", point.x, point.y);
					break;
			}
			break;
	}
	return FALSE;
}

DROPEFFECT
MFCArrangeView::OnDropEx(
			COleDataObject* object,
			DROPEFFECT dropDefault,
			DROPEFFECT dropList,
			CPoint point 
		)
{
	if (dragon.keyState & MK_RBUTTON) {
		fprintf(stderr, "right click drag: on drop ex %d %d\n", point.x, point.y);

		return OnDrop(object, dropDefault, point)? dragon.dropEffect:DROPEFFECT_NONE;
	}
	return -1;
}



/////////////////////////////////////////////////////////////////////////////
// MFCArrangeView mouse and keyboard handlers
/////////////////////////////////////////////////////////////////////////////

afx_msg void
MFCArrangeView::OnLButtonDown(
			UINT nFlags,
			CPoint point 
		)
{
	point += GetScrollPosition();
	last_mouse_point = mouse_click = point;

	short	at_channel;
	at_channel = Pix2Channel(point.y);
	Time	at_time;
	Pix2Time(point.x, at_time);

//	int		bar, barbeat, beattick;
//	at_time.GetBBQValue(bar, barbeat, beattick);
//	fprintf(stderr, "left button down: on point %d %d, channel %d, ticks %d t %d:%d.%d\n", point.x, point.y, at_channel, at_time.ticks, bar, barbeat, beattick);

	switch (currentTool) {
		case ID_ARTOOL_POINT: {
			DeselectAll();
			short	hitType;
			void	*hitIt=NULL;
			;
			if ((mouse_instance = InstanceViewAtPoint(mouse_click))!=NULL) {
				mouse_action = QUA_MOUSE_ACTION_MOVE_INSTANCE;
				mouse_move_inst_offset.x = mouse_click.x - mouse_instance->bounds.left;
				mouse_move_inst_offset.y = mouse_click.y;
				mouse_instance->Select(true);
			}
			if ((mouse_item = ItemViewAtPoint(mouse_click, nFlags, hitType, hitIt))!=NULL) {
				if (hitIt) {
//					mouse_sub_item = (StreamItem *)hitIt;
				}
				switch (mouse_item->type) {
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
			break;
		}
		case ID_ARTOOL_SLICE: {
			break;
		}
		case ID_ARTOOL_RGSELECT: {
			fprintf(stderr, "region select\n");
			mouse_action = QUA_MOUSE_ACTION_REGION_SELECT;
			BRect	r(min(mouse_click.x,point.x),min(mouse_click.y,point.y),max(mouse_click.x,point.x),max(mouse_click.y,point.y));
			ChangeSelection(r);
			break;
		}
		case ID_ARTOOL_DRAW: {
			mouse_instance = InstanceViewAtPoint(mouse_click);
			if (mouse_instance) {
				mouse_action = QUA_MOUSE_ACTION_SIZE_INSTANCE;
				mouse_move_inst_offset.x = mouse_instance->bounds.left;
				mouse_move_inst_offset.y = mouse_click.y;
				Instance	*inst = mouse_instance->instance;
				if (inst) {
					Pix2Time(point.x-mouse_move_inst_offset.x, at_time);
					quaLink->MoveInstance(inst->sym, -1, inst->startTime, at_time);
				}
			}
			break;
		}
	}
}

afx_msg void
MFCArrangeView::OnLButtonUp(
			UINT nFlags,
			CPoint point 
		)
{
	point += GetScrollPosition();
	mouse_instance = NULL;
	mouse_action = QUA_MOUSE_ACTION_NONE;
}


afx_msg void
MFCArrangeView::OnLButtonDblClk(
			UINT nFlags,
			CPoint point 
		)
{
	point += GetScrollPosition();

	Time	at_time;
	short	at_channel;
	Pix2Time(point.x, at_time);
	at_channel = Pix2Channel(point.y);

	int		bar, barbeat, beattick;
	at_time.GetBBQValue(bar, barbeat, beattick);
	fprintf(stderr, "left button double click: on point %d %d, channel %d, ticks %d t %d:%d.%d\n", point.x, point.y, at_channel, at_time.ticks, bar, barbeat, beattick);
}

afx_msg void
MFCArrangeView::OnMButtonDown(
			UINT nFlags,
			CPoint point 
		)
{
	point += GetScrollPosition();

	Time	at_time;
	short	at_channel;
	Pix2Time(point.x, at_time);
	at_channel = Pix2Channel(point.y);
//	int		bar, barbeat, beattick;
//	at_time.GetBBQValue(bar, barbeat, beattick);
//	fprintf(stderr, "middle button down: on point %d %d, channel %d, ticks %d t %d:%d.%d\n", point.x, point.y, at_channel, at_time.ticks, bar, barbeat, beattick);
}

afx_msg void
MFCArrangeView::OnMButtonUp(
			UINT nFlags,
			CPoint point 
		)
{
	point += GetScrollPosition();
}

afx_msg void
MFCArrangeView::OnMButtonDblClk(
			UINT nFlags,
			CPoint point 
		)
{
	point += GetScrollPosition();

	Time	at_time;
	short	at_channel;
	Pix2Time(point.x, at_time);
	at_channel = Pix2Channel(point.y);
//	int		bar, barbeat, beattick;
//	at_time.GetBBQValue(bar, barbeat, beattick);
//	fprintf(stderr, "middle button double click: on point %d %d, channel %d, ticks %d t %d:%d.%d\n", point.x, point.y, at_channel, at_time.ticks, bar, barbeat, beattick);
}


afx_msg void
MFCArrangeView::OnMouseMove(
			UINT nFlags,
			CPoint point 
		)
{
	point += GetScrollPosition();

	Time	at_time;
	short	at_channel;
	at_channel = Pix2Channel(point.y);

	if (bounds.PtInRect(point)) {
		if (!mouse_captured) {
			SetCapture();
			mouse_captured = true;
			switch (currentTool) {
				case ID_ARTOOL_POINT: {
					originalCursor = ::SetCursor(pointCursor);
					break;
				}
				case ID_ARTOOL_SLICE: {
					originalCursor = ::SetCursor(sliceCursor);
					break;
				}
				case ID_ARTOOL_DRAW: {
					originalCursor = ::SetCursor(drawCursor);
					break;
				}
				case ID_ARTOOL_RGSELECT: {
					originalCursor = ::SetCursor(regionCursor);
					break;
				}
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

	switch (mouse_action) {
		case QUA_MOUSE_ACTION_MOVE_INSTANCE: {
			if (mouse_instance) {
				Instance	*inst = mouse_instance->instance;
				if (inst) {
					Pix2Time(point.x-mouse_move_inst_offset.x, at_time);
					Time	last_time;
					Pix2Time(last_mouse_point.x-mouse_move_inst_offset.x, last_time);
					short	last_channel = Pix2Channel(last_mouse_point.y);
					if (last_time != at_time || last_channel != at_channel) {
						quaLink->MoveInstance(inst->sym, at_channel, at_time, inst->duration);
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
			if (mouse_instance) {
				Instance	*inst = mouse_instance->instance;
				if (inst) {
					Pix2Time(point.x-mouse_move_inst_offset.x, at_time);
					Time	last_time;
					Pix2Time(last_mouse_point.x-mouse_move_inst_offset.x, last_time);
					if (last_time != at_time) {
						quaLink->MoveInstance(inst->sym, -1, inst->startTime, at_time);
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
	}

	last_mouse_point = point;
}

afx_msg BOOL
MFCArrangeView::OnMouseWheel(
			UINT nFlags,
			short zDelta,
			CPoint point 
		)
{
//	point += GetScrollPosition();
	return CScrollView::OnMouseWheel(nFlags, zDelta, point);
}


afx_msg void
MFCArrangeView::OnKeyDown(
			UINT nChar,
			UINT nRepCnt,
			UINT nFlags 
		)
{
	switch (nChar) {
		case VK_DELETE: {
			short i=0;
			while (i<NIR()) {
				MFCInstanceView *iv = (MFCInstanceView *)IR(i);
				if (iv == NULL) {
					break;
				}
				if (iv->selected && iv->instance) {
					quaLink->DeleteInstance(iv->instance->sym);
				} else {
					i++;
				}
			}
			break;
		}
		default: {
            CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
			break;
		}
	}
}

afx_msg void
MFCArrangeView::OnKeyUp(
			UINT nChar,
			UINT nRepCnt,
			UINT nFlags 
		)
{
	CScrollView::OnKeyUp(nChar, nRepCnt, nFlags);
}

afx_msg void
MFCArrangeView::OnVScroll(UINT nSBCode, UINT unPos, CScrollBar* pScrollBar)
{
	CScrollView::OnVScroll(nSBCode, unPos, pScrollBar);
}

BOOL
MFCArrangeView::OnScroll(
			UINT nScrollCode,
			UINT unPos,
			BOOL bDoScroll 
		)
{
	int nPos = (int) unPos;
//	fprintf(stderr, "on scroll %d/%d %d %d\n", nScrollCode&0xff, (nScrollCode&0xff00)>>8, nPos, bDoScroll);
	switch ((nScrollCode&0xff00)>>8) {
		case SB_BOTTOM: { //   Scroll to bottom. 
			if (channeler && bDoScroll) {
				long cPos = GetScrollPos(SB_VERT);
				int		scrollMapMode;
				CSize	totalSize;
				CSize	pageSize;
				CSize	lineSize;
				GetDeviceScrollSizes(scrollMapMode, totalSize, pageSize, lineSize);
//				fprintf(stderr, "V Scrolling channels to bottom %d\n", pageSize.cy);
				if (cPos < totalSize.cy) {
					channeler->ScrollWindow(0, totalSize.cy-cPos, NULL, NULL);
				}
			}
			break;
		}
		case SB_ENDSCROLL: { //   End scroll. 
			if (channeler) {
//				fprintf(stderr, "End of scroll\n");
			}
			break;
		}
		case SB_LINEDOWN: { //   Scroll one line down. 
			if (channeler && bDoScroll) {
				long cPos = GetScrollPos(SB_VERT);
				int		scrollMapMode;
				CSize	totalSize;
				CSize	pageSize;
				CSize	lineSize;
				GetDeviceScrollSizes(scrollMapMode, totalSize, pageSize, lineSize);
//				fprintf(stderr, "V Scrolling channels linedown %d %d %d\n", lineSize.cy, cPos, totalSize.cy);
				if (cPos < totalSize.cy-bounds.bottom) {
					int scrollAmt = 
						(cPos+lineSize.cy > totalSize.cy-bounds.bottom)?cPos - totalSize.cy+bounds.bottom:-lineSize.cy;
					channeler->ScrollWindow(0, scrollAmt, NULL, NULL);
				}
			}
			break;
		}
		case SB_LINEUP: { //   Scroll one line up. 
			if (channeler && bDoScroll) {
				long cPos = GetScrollPos(SB_VERT);
				long chPos = channeler->GetScrollPos(SB_VERT);
				int		scrollMapMode;
				CSize	totalSize;
				CSize	pageSize;
				CSize	lineSize;
				GetDeviceScrollSizes(scrollMapMode, totalSize, pageSize, lineSize);
				if (cPos > 0) {
					int scrollAmt = 
						(cPos-lineSize.cy < 0)?cPos:lineSize.cy;
//					fprintf(stderr, "V Scrolling channels lineup %d %d %d scroll amt %d %d\n", lineSize.cy, cPos, totalSize.cy, scrollAmt, chPos);
					channeler->ScrollWindow(0, scrollAmt, NULL, NULL);
				}
			}
			break;
		}
		case SB_PAGEDOWN: { //   Scroll one page down. 
			if (channeler && bDoScroll) {
				long cPos = GetScrollPos(SB_VERT);
				int		scrollMapMode;
				CSize	totalSize;
				CSize	pageSize;
				CSize	lineSize;
				GetDeviceScrollSizes(scrollMapMode, totalSize, pageSize, lineSize);
//				fprintf(stderr, "V Scrolling channels pagedown %d\n", pageSize.cy);
				if (cPos < totalSize.cy-bounds.bottom) {
					int scrollAmt = 
						(cPos+pageSize.cy > totalSize.cy-bounds.bottom)?cPos - totalSize.cy+bounds.bottom:-pageSize.cy;
					channeler->ScrollWindow(0, scrollAmt, NULL, NULL);
				}
			}
			break;
		}
		case SB_PAGEUP: { //   Scroll one page up. 
			if (channeler && bDoScroll) {
				long cPos = GetScrollPos(SB_VERT);
				int		scrollMapMode;
				CSize	totalSize;
				CSize	pageSize;
				CSize	lineSize;
				GetDeviceScrollSizes(scrollMapMode, totalSize, pageSize, lineSize);
//				fprintf(stderr, "V Scrolling channels pageup %d\n", pageSize.cy);
				if (cPos > 0) {
					int scrollAmt = 
						(cPos-pageSize.cy < 0)?cPos:pageSize.cy;
					channeler->ScrollWindow(0, scrollAmt, NULL, NULL);
				}
			}
			break;
		}
		case SB_THUMBPOSITION:  //   Scroll to the absolute position. The current position is provided in nPos. 
		case SB_THUMBTRACK: { //   Drag scroll box to specified position. The current position is provided in nPos. 
			if (channeler && bDoScroll) {
				long cPos = GetScrollPos(SB_VERT);
//				fprintf(stderr, "V Scrolling channels absolute in parallel %x %d %d\n", nScrollCode, nPos, cPos);
				channeler->ScrollWindow(0,cPos-nPos,NULL,NULL);
			}
			break;
		}
		case SB_TOP: { //   Scroll to top. 
			if (channeler && bDoScroll) {
				long cPos = GetScrollPos(SB_VERT);
//				fprintf(stderr, "V Scrolling to top (by %d)\n", -cPos);
				if (cPos > 0) {
					channeler->ScrollWindow(0,-cPos,NULL,NULL);
				}
			}
			break;
		}
	}
	return CScrollView::OnScroll(nScrollCode,unPos,bDoScroll);
}

BOOL
MFCArrangeView::OnScrollBy(
			CSize sizeScroll,
			BOOL bDoScroll 
		)
{
//	fprintf(stderr, "onscrollby %d %d %d\n", sizeScroll.cx, sizeScroll.cy, bDoScroll);
	return MFCSequenceEditor::OnScrollBy(sizeScroll,bDoScroll);
}

/////////////////////////////////////////////////////////////////////////////
// MFCArrangeView methods
/////////////////////////////////////////////////////////////////////////////

void
MFCArrangeView::SetHVScroll()
{
	fprintf(stderr, "set arranger scroll\n");
	long	atY=0;
	if (channeler) {
		MFCChannelView *ev =(MFCChannelView *)channeler->CR(channeler->NCR()-1);
		if (ev) {
//			SetScrollRange(SB_VERT, 0, ev->frame.bottom-bounds.bottom);
//			reportError("%d %d", bounds.right, ev->frame.bottom-bounds.bottom);
			if (ev->frame.bottom > 0) {
				SetScrollSizes(MM_TEXT, CSize(2*Time2Pix(lastScheduledEvent), ev->frame.bottom));
			}
		} else {
			SetScrollSizes(MM_TEXT, CSize(2*Time2Pix(lastScheduledEvent), 0));
		}
	}
//	SetScrollRange(SB_HORZ, 0, 10);
}

void
MFCArrangeView::AddInstanceRepresentation(Instance *i)
{
	MFCInstanceView *nv = new MFCInstanceView(i, this);
	if (!nv) return;
	AddI(nv);
	nv->Redraw();
}


void
MFCArrangeView::UpdateControllerDisplay(StabEnt *stacker, QuasiStack *qs, StabEnt*sym)
{
	Instance	*inst=stacker->InstanceValue();
	fprintf(stderr, "MFCArrangeView::Update controller display\n");
	if (inst) {
		if (sym == inst->schedulable.chanSym ||
				sym == inst->schedulable.durSym ||
				sym == inst->schedulable.starttSym) {
			MoveInstanceRepresentation(inst);
			return;
		}
	}
}

void
MFCArrangeView::MoveInstanceRepresentation(Instance *i)
{
	MFCInstanceView *nv = (MFCInstanceView *)InstanceRepresentationFor(i);
	if (!nv) return;
	nv->DrawMove();
}

void
MFCArrangeView::RemoveInstanceRepresentation(Instance *i)
{
	QuaInstanceRepresentation *qir = InstanceRepresentationFor(i);
	if (!qir) return;
	RemI(qir);
	MFCInstanceView	*iv = (MFCInstanceView	*)qir;
	CRect	updr = iv->bounds;
	CPoint	scr = GetScrollPosition();
	updr = bounds - scr;
	updr.bottom ++;
	updr.right ++;
	RedrawWindow(&updr);
	delete iv;
}

void
MFCArrangeView::updateClipIndexDisplay()
{
	vector<StabEnt*> presentClips;
	CQuaMFCDoc	*qdoc = (CQuaMFCDoc *)GetDocument();
	if (qdoc == NULL) {
		fprintf(stderr, "menu item track finds null doc");
		return;
	} else if (qdoc->qua == NULL) {
		fprintf(stderr, "menu item track finds null sequencer");
		return;
	}
	for (short i=0; i<qdoc->qua->nClip(); i++) {
		Clip	*c = qdoc->qua->regionClip(i);
		if (c) {
//			clipsView->AddItem(c->sym);
			presentClips.push_back(c->sym);
			AddClipItemView(c);
		}
	}
	RemoveClipsNotIn(presentClips);
//	clipsView->RemoveClipsNotIn(presentClips);
}
/////////////////////////////////////////////////////////////////////////////
// MFCInstanceView
/////////////////////////////////////////////////////////////////////////////
MFCInstanceView::MFCInstanceView(Instance *i, MFCArrangeView *a)
//	: MFCEditorItemView(NULL, INSTANCE)
{
	instance = i;
	arranger = a;
	selected = false;
	CalculateBounds();
//	Redraw();
}

MFCInstanceView::~MFCInstanceView()
{
	;
}

#ifdef QUA_V_GDI_PLUS
void
MFCInstanceView::Draw(Graphics *dc, CRect *clipBox)
{
// !!!??? need to clip properly for short instances with long names
	Pen			blackPen(AlphaColor(250, rgb_black), 1);
	Pen			redPen(AlphaColor(250, rgb_red), 1);
	SolidBrush	blueBrush(AlphaColor(100, rgb_blue));
	SolidBrush	blackBrush(AlphaColor(100, rgb_black));

	CRect		clipBounds = bounds;
	if (clipBox->left > bounds.left) clipBounds.left = clipBox->left-1;
	if (clipBox->right < bounds.right) clipBounds.right = clipBox->right+1;

//	fprintf(stderr, "drawing instance view %d\n", clipBounds.right-clipBounds.left);
	dc->FillRectangle(&blueBrush,
			bounds.left, bounds.top,
			bounds.right-bounds.left, bounds.bottom-bounds.top);
	dc->DrawRectangle(selected?&redPen:&blackPen,
			bounds.left, bounds.top,
			clipBounds.right-bounds.left, bounds.bottom-bounds.top);
	Font	labelFont(L"Arial", 8.0, FontStyleRegular, UnitPoint, NULL);
	wstring nm;
	const char *cp = instance->sym->uniqueName();
	while (*cp) { nm.push_back(*cp++); }
	float lbx = bounds.left+2;
#define LBLSEP 200
	if (clipBox->left > lbx) {
		int nld = clipBox->left - lbx;
		nld = nld/LBLSEP;
//		if (nld > 2) lbx += (nld-2)*LBLSEP;
	}
	PointF	p(lbx, clipBounds.top);
	do {
		dc->DrawString(nm.c_str(), -1, &labelFont, p, &blackBrush);
		p.X += LBLSEP;
	} while (p.X < clipBounds.right);
}

#else
void
MFCInstanceView::Draw(CDC *dc, CRect *clipBox)
{
	;
}
#endif

void
MFCInstanceView::Redraw(bool redraw)
{
	if (arranger) {
		CRect	updr = bounds;
		CPoint	scr = arranger->GetScrollPosition();
		updr = bounds - scr;
		updr.bottom ++;
		updr.right ++;
		if (redraw) {
			arranger->RedrawWindow(&updr);
		} else {
			arranger->InvalidateRect(&updr);
		}
	}
}

void
MFCInstanceView::CalculateBounds()
{
	if (instance) {
		MFCChannelView	*cv = (MFCChannelView *)arranger->channeler->ChannelRepresentationFor(instance->channel);
		if (cv) {
			Time	rt = instance->duration&(&Metric::mSec);
			Time	arrTime = instance->startTime&arranger->displayMetric;
			Time	arrDur = instance->duration&arranger->displayMetric;
			long	pixl = arranger->Time2Pix(arrTime);
			long	pixd = arranger->Time2Pix(arrDur);
			if (pixd < 2) {
				pixd = 2;
			}
			bounds.Set(pixl, cv->frame.top, pixl+pixd, cv->frame.bottom);
		}
	}
}

void
MFCInstanceView::DrawMove()
{
	if (arranger) {
		CPoint	scr = arranger->GetScrollPosition();
		CRect	oldr = bounds - scr;
		oldr.right++;
		oldr.bottom++;
		CalculateBounds();
		arranger->InvalidateRect(&oldr);
		Redraw();
	}
}

void
MFCInstanceView::Select(bool sel)
{
	if (sel == selected) {
		return;
	}
	selected = sel;
	Redraw();
}


