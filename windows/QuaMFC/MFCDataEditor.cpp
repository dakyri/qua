
// MFCDataEditor.cpp : implementation file
//
//#define _AFXDLL
#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include "qua_version.h"


#include "ShlObj.h"

#include "StdDefs.h"
#include "Colors.h"

#include "QuaMFC.h"
#include "QuaMFCDoc.h"
#include "MFCDataEditor.h"
#include "MFCObjectView.h"

#include "Qua.h"
#include "Time.h"
#include "Clip.h"

#include "MemDC.h"

#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef QUA_V_GDI_PLUS
#endif

CFont		MFCDataEditorTScale::displayFont;
/////////////////////////////////////////////////////////////////////////////
// MFCDataEditor

IMPLEMENT_DYNCREATE(MFCSequenceEditor, CScrollView)

MFCSequenceEditor::MFCSequenceEditor()
{
	displayMetric = &Metric::std;
	ticksPerNotch = 1;
	pixPerNotch = 2;
	frame = CRect(0, 0, 0, 0);
	bounds = CRect(0, 0, 0, 0);
//	channeler = NULL;
	lastScheduledEvent.Set("120:0.0 bbq");
	currentTool = ID_ARTOOL_POINT;
//	mouse_instance = NULL;
	mouse_action = QUA_MOUSE_ACTION_NONE;
	mouse_captured = false;
	xscale = NULL;
	yscale = NULL;
}

MFCSequenceEditor::~MFCSequenceEditor()
{
	fprintf(stderr, "deleting data editor %x\n", ((unsigned)this));
}

void
MFCSequenceEditor::PostNcDestroy()
{
// highly problematic stuff
	delete this;
}

BEGIN_MESSAGE_MAP(MFCSequenceEditor, CScrollView)
	//{{AFX_MSG_MAP(MFCDataEditor)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SIZING()
//	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
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
// MFCDataEditor drawing




// ticksPerNotch give the no of ticks per grid point. this allows a pixel width to separate
// grid markings on the display at the highest grid resolution, which is 1 tick per grid
// marking or 2 pixels per tick. ie highest resolution <=> ticksPerP2X == 1, and any higher
// value is a lower resolution
long
MFCSequenceEditor::Time2Pix(Time &t)
{
	if (t.metric != displayMetric) {
		;
	}
	return pixPerNotch*t.ticks/ticksPerNotch;
}


void
MFCSequenceEditor::Pix2Time(long px, Time &t)
{
	if (t.metric != displayMetric) {
		;
	}
	t.ticks = ticksPerNotch*px/pixPerNotch;
}



MFCEditorItemView *
MFCSequenceEditor::ItemViewFor(void *item)
{
	for (short i=0; i<NItemR(); i++) {
		MFCEditorItemView *iv = ItemR(i);
		if (iv->Represents(item)) {
			return iv;
		}
	}
	return NULL;
}

void
MFCSequenceEditor::DeselectAll()
{
	for (short i=0; i<NItemR(); i++) {
		MFCEditorItemView *iv = ItemR(i);
		iv->Select(false);
	}
}

void
MFCSequenceEditor::ChangeSelection(BRect &r)
{
//	for (short i=0; i<NIR(); i++) {
//		MFCInstanceView *iv = (MFCInstanceView *)IR(i);
//		if (iv->bounds.Intersects(r)) {
//			iv->Select(true);
//		} else {
//			iv->Select(false);
//		}
//	}
}


#ifdef QUA_V_GDI_PLUS
void
MFCSequenceEditor::DrawGridGraphics(Graphics &pdc, CRect &cbp)
{
	Metric *dm = (displayMetric?displayMetric:NULL);
//	pdc->SetBkColor(rgb_red);
	Pen	ltGrayPen(Color(255, 200, 200, 200), 1);
	Pen	mdGrayPen(Color(255, 140, 140, 140), 1);
	Pen	dkGrayPen(Color(255, 80, 80, 80), 1);

	long startTick = cbp.left/pixPerNotch;
	long endTick = cbp.right/pixPerNotch;
	short notchInc = 1;
	long notchPx = startTick*pixPerNotch;
	long tickCnt = 0;
	short tickPerNotch = 1;

//	MFCChannelView *fv =(MFCChannelView *)channeler->CR(0);
//	MFCChannelView *ev =(MFCChannelView *)channeler->CR(channeler->NCR()-1);

	for (tickCnt=startTick; tickCnt<=endTick; tickCnt+=tickPerNotch, notchPx += pixPerNotch) {
		Pen		*gridPen;
		if (tickCnt % (dm->granularity*dm->beatsPerBar) == 0) {
			gridPen = &dkGrayPen;
		} else if (tickCnt % dm->granularity == 0) {
			gridPen = &mdGrayPen;
		} else {
			gridPen = &ltGrayPen;
		}
		pdc.DrawLine(gridPen, notchPx, cbp.top, notchPx, cbp.bottom);
	}
	for (short i=0; i<NHorizontalPix(); i++) {
		pdc.DrawLine(&dkGrayPen, cbp.left, HorizontalPix(i), cbp.right, HorizontalPix(i));
	}
}
#endif

void
MFCSequenceEditor::DrawGrid(CDC *pdc, CRect &cbp)
{
	Metric *dm = (displayMetric?displayMetric:NULL);
//	pdc->SetBkColor(rgb_red);
	pdc->SelectObject(GetStockObject(DC_BRUSH));
	pdc->SelectObject(GetStockObject(DC_PEN));
	pdc->SetDCPenColor(rgb_ltGray);
	long startTick = cbp.left/pixPerNotch;
	long endTick = cbp.right/pixPerNotch;
	short notchInc = 1;
	long notchPx = startTick*pixPerNotch;
	long tickCnt = 0;
	short tickPerNotch = 1;

//	MFCChannelView *fv =(MFCChannelView *)channeler->CR(0);
//	MFCChannelView *ev =(MFCChannelView *)channeler->CR(channeler->NCR()-1);
	for (tickCnt=startTick; tickCnt<endTick; tickCnt+=tickPerNotch, notchPx += pixPerNotch) {
		if (tickCnt % (dm->granularity*dm->beatsPerBar) == 0) {
			pdc->SetDCPenColor(rgb_dkGray);
		} else if (tickCnt % dm->granularity == 0) {
			pdc->SetDCPenColor(rgb_mdGray);
		} else {
			pdc->SetDCPenColor(rgb_ltGray);
		}
		pdc->MoveTo(notchPx, cbp.top);
		pdc->LineTo(notchPx, cbp.bottom);
	}
	pdc->SetDCPenColor(rgb_dkGray);
//	for (short i=0; i<quaLink->NChannel(); i++) {
//		pdc->MoveTo(cbp->left, Channel2Pix(i));
//		pdc->LineTo(cbp->right, Channel2Pix(i));
//	}
}

#ifdef QUA_V_GDI_PLUS

void
MFCSequenceEditor::DrawCursor(Graphics &pdc, CRect &clipBox)
{
	;
}

#else

void
MFCSequenceEditor::DrawCursor(CDC *pdc, CRect *clipBox)
{
	;
}

#endif

// To this code
BOOL
MFCSequenceEditor::OnEraseBkgnd(CDC* pDC)
{
	//	cerr << "on erase back" << endl;
	return FALSE;
	//	return CScrollView::OnEraseBkgnd(pDC);
}

void
MFCSequenceEditor::OnPaint()
{
	// standard paint routine
	CPaintDC dc(this);
	OnPrepareDC(&dc);
	OnDraw(&dc);
}

void
MFCSequenceEditor::OnDraw(CDC* pdc)
{
	CRect	clipBox;
	int		cbType;
	cbType = pdc->GetClipBox(&clipBox);
//	fprintf(stderr, "OnDraw() clip (%d %d %d %d) type %s\n", clipBox.left, clipBox.top, clipBox.right, clipBox.bottom, cbType==COMPLEXREGION?"complex":cbType==SIMPLEREGION?"simple":cbType==NULLREGION?"null":"error");
//	fprintf(stderr, "lt %d\n", lastScheduledEvent.ticks);
	CDocument* pDoc = GetDocument();
#ifdef QUA_V_GDI_PLUS
	Graphics	graphics(pdc->m_hDC);
	DrawGridGraphics(graphics, clipBox);

// test the interoperation of GDI and GDI+
//	Color	p = Color::MakeARGB(100, 255, 20, 255);
//	Pen	semip(p, 3);
//	graphics.DrawLine(&semip, 0,0, 200, 200);
//	for (short i=0; i<NIR(); i++) {
//		MFCInstanceView *ir = (MFCInstanceView *)IR(i);
//		if (ir->bounds.Intersects(clipBox)) {
//			ir->Draw(&graphics, &clipBox);
//		}
//	}
	DrawCursor(graphics, clipBox);
#else
	DrawGrid(pdc, &clipBox);
	DrawCursor(pdc, &clipBox);
#endif
//	fprintf(stderr, "OnDraw() finito\n");
}

/////////////////////////////////////////////////////////////////////////////
// MFCDataEditor diagnostics

#ifdef _DEBUG
void MFCSequenceEditor::AssertValid() const
{
	CScrollView::AssertValid();
}

void MFCSequenceEditor::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// MFCDataEditor message handlers
/////////////////////////////////////////////////////////////////////////////

void
MFCSequenceEditor::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);
	bounds.right = cx;
	bounds.bottom = cy;
	frame.right = frame.left+cx;
	frame.bottom = frame.bottom+cy;
	if (xscale) {
		fprintf(stderr, "set on size sequence editor %d %d %d %d\n", bounds.left, bounds.top, bounds.right, bounds.bottom);
		if (!xscale->SetWindowPos(
				&wndTop,
				0, bounds.bottom-xscale->bounds.bottom, 0, 0,
				SWP_NOSIZE)) {
			fprintf(stderr, "no sizing\n");
		}
	}

	SetHVScroll();
}

void
MFCSequenceEditor::OnSizing( UINT which, LPRECT r)
{
	CScrollView::OnSizing(which, r);
}


int
MFCSequenceEditor::OnCreate(LPCREATESTRUCT lpCreateStruct )
{
	EnableScrollBarCtrl(SB_VERT);
	EnableScrollBarCtrl(SB_HORZ);
	bounds.left = 0;
	bounds.top = 0;
	bounds.right = lpCreateStruct->cx;
	bounds.bottom = lpCreateStruct->cy;
	frame.left = lpCreateStruct->x;
	frame.top = lpCreateStruct->y;
	frame.right = frame.left+bounds.right; 
	frame.bottom = frame.top+bounds.bottom; 
	SetHVScroll();
	target.Register(this);
	return CScrollView::OnCreate(lpCreateStruct);
}


void
MFCSequenceEditor::OnInitialUpdate()
{
	CQuaMFCDoc	*qdoc = (CQuaMFCDoc *)GetDocument();
	if (qdoc == NULL) {
		reportError("SequenceEditor: initial update of sequence editor finds a null sequencer document");
	} else if (qdoc->qua == NULL) {
		reportError("SequenceEditor: initial update finds a null sequencer");
	} else {	// set qua up with our hooks
//		quaLink = qdoc->qua->bridge.display;
//		quaLink->AddArranger(this);
//		SetScrollSizes(MM_TEXT, CSize(bounds.right, bounds.bottom));
//		reportError("%d %d", bounds.right, bounds.bottom);
		SetHVScroll();
//		display->AddChannelRepresentations(this);
	}
}


/////////////////////////////////////////////////////////////////////////////
// MFCDataEditor dragndrop handlers (virtuals)
/////////////////////////////////////////////////////////////////////////////

DROPEFFECT
MFCSequenceEditor::OnDragEnter(
			COleDataObject* object,
			DWORD keyState,
			CPoint point 
		)
{
	fprintf(stderr, "MFCDataEditor::on drag enter\n");
	dragon.SetTo(object, keyState);
	return dragon.dropEffect != DROPEFFECT_NONE? dragon.dropEffect:DROPEFFECT_MOVE;
}

DROPEFFECT
MFCSequenceEditor::OnDragOver(
			COleDataObject* object,
			DWORD dwKeyState,
			CPoint point 
		)
{
//	fprintf(stderr, "MFCDataEditor::on drag over\n");
	return dragon.dropEffect != DROPEFFECT_NONE? dragon.dropEffect:DROPEFFECT_MOVE;
}


void
MFCSequenceEditor::OnDragLeave( )
{
	fprintf(stderr, "MFCDataEditor::on drag leave\n");
	dragon.Clear();
}

BOOL
MFCSequenceEditor::OnDrop(
			COleDataObject* object,
			DROPEFFECT dropEffect,
			CPoint point 
		)
{
	fprintf(stderr, "MFCDataEditor::on drop\n");
	point += GetScrollPosition();
	switch (dragon.type) {
		case QuaDrop::FILES:
		case QuaDrop::AUDIOFILES:
		case QuaDrop::MIDIFILES: {
			Time	at_time;
			short	at_channel;
			Pix2Time(point.x, at_time);
//			at_channel = Pix2Channel(point.y);
//			int		bar, barbeat, beattick;
//			at_time.GetBBQValue(bar, barbeat, beattick);
//			fprintf(stderr, "drag files is dropped: on point %d %d, channel %d, ticks %d t %d:%d.%d\n", point.x, point.y, at_channel, at_time.ticks, bar, barbeat, beattick);
			short	i;
			bool	drop_sample_file=false;
			bool	drop_midi_file=false;
			bool	drop_qua_file=false;
			for (i=0; i<dragon.count; i++) {
				string	mime_t = Qua::identifyFile(dragon.data.filePathList->at(i));
				if (mime_t.size() > 0) {
					fprintf(stderr, "%s (%s)\n", dragon.data.filePathList->at(i).c_str(), mime_t.c_str());
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
//				quaLink->CreateSample(&dragon.data.filePathList[i], dragon.count-i, at_channel, &at_time, NULL);
			} else if (drop_midi_file) {
				;
			} else if (drop_qua_file) {
				;
			} else {
				return FALSE;
			}
			return TRUE;
		}

		case QuaDrop::VOICE: 
		case QuaDrop::SAMPLE: 
		case QuaDrop::SCHEDULABLE: {
			Time	at_time;
			short	at_channel;
			Pix2Time(point.x, at_time);
//			at_channel = Pix2Channel(point.y);
//			int		bar, barbeat, beattick;
//			at_time.GetBBQValue(bar, barbeat, beattick);
//			fprintf(stderr, "drop schedulable %s: on point %d %d, channel %d, ticks %d t %d:%d.%d\n", dragon.data.symbol->name, point.x, point.y, at_channel, at_time.ticks, bar, barbeat, beattick);

//			quaLink->CreateInstance(dragon.data.symbol, at_channel, &at_time, NULL);
			return TRUE;
		}
			
		case QuaDrop::INSTANCE: {
			Time	at_time;
			short	at_channel;
			Pix2Time(point.x, at_time);
//			at_channel = Pix2Channel(point.y);
//			int		bar, barbeat, beattick;
//			at_time.GetBBQValue(bar, barbeat, beattick);
//			fprintf(stderr, "drop instance %s: on point %d %d, channel %d, ticks %d t %d:%d.%d\n", dragon.data.symbol->name, point.x, point.y, at_channel, at_time.ticks, bar, barbeat, beattick);

//			quaLink->MoveInstance(dragon.data.symbol, at_channel, &at_time, NULL);
			return TRUE;
		}
		case QuaDrop::APPLICATION:
		case QuaDrop::PORT:
		case QuaDrop::VSTPLUGIN:
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
MFCSequenceEditor::OnDropEx(
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
// MFCDataEditor mouse and keyboard handlers
/////////////////////////////////////////////////////////////////////////////

afx_msg void
MFCSequenceEditor::OnLButtonDown(
			UINT nFlags,
			CPoint point 
		)
{
	point += GetScrollPosition();
	last_mouse_point = mouse_click = point;

	short	at_channel;
//	at_channel = Pix2Channel(point.y);
	Time	at_time;
	Pix2Time(point.x, at_time);

//	int		bar, barbeat, beattick;
//	at_time.GetBBQValue(bar, barbeat, beattick);
//	fprintf(stderr, "left button down: on point %d %d, channel %d, ticks %d t %d:%d.%d\n", point.x, point.y, at_channel, at_time.ticks, bar, barbeat, beattick);

	switch (currentTool) {
		case ID_ARTOOL_POINT: {
			DeselectAll();
//			mouse_instance = InstanceViewAtPoint(mouse_click);
//			if (mouse_instance) {
//				mouse_action = QUA_MOUSE_ACTION_MOVE_INSTANCE;
//				mouse_move_inst_offset.x = mouse_click.x - mouse_instance->bounds.left;
//				mouse_move_inst_offset.y = mouse_click.y;
//				mouse_instance->Select(true);
//			}
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
//			mouse_instance = InstanceViewAtPoint(mouse_click);
//			if (mouse_instance) {
//				mouse_action = QUA_MOUSE_ACTION_SIZE_INSTANCE;
//				mouse_move_inst_offset.x = mouse_instance->bounds.left;
//				mouse_move_inst_offset.y = mouse_click.y;
//				Instance	*inst = mouse_instance->instance;
//				if (inst) {
//					Pix2Time(point.x-mouse_move_inst_offset.x, at_time);
//					quaLink->MoveInstance(inst->sym, -1, NULL, &at_time);
//				}
//			}
			break;
		}
	}
}

afx_msg void
MFCSequenceEditor::OnLButtonUp(
			UINT nFlags,
			CPoint point 
		)
{
	point += GetScrollPosition();
//	mouse_instance = NULL;
	mouse_action = QUA_MOUSE_ACTION_NONE;
}


afx_msg void
MFCSequenceEditor::OnLButtonDblClk(
			UINT nFlags,
			CPoint point 
		)
{
	point += GetScrollPosition();

	Time	at_time;
//	short	at_channel;
	Pix2Time(point.x, at_time);
//	at_channel = Pix2Channel(point.y);

	int		bar, barbeat, beattick;
	at_time.GetBBQValue(bar, barbeat, beattick);
//	fprintf(stderr, "left button double click: on point %d %d, channel %d, ticks %d t %d:%d.%d\n", point.x, point.y, at_channel, at_time.ticks, bar, barbeat, beattick);
}

afx_msg void
MFCSequenceEditor::OnMButtonDown(
			UINT nFlags,
			CPoint point 
		)
{
	point += GetScrollPosition();

	Time	at_time;
//	short	at_channel;
	Pix2Time(point.x, at_time);
//	at_channel = Pix2Channel(point.y);
//	int		bar, barbeat, beattick;
//	at_time.GetBBQValue(bar, barbeat, beattick);
//	fprintf(stderr, "middle button down: on point %d %d, channel %d, ticks %d t %d:%d.%d\n", point.x, point.y, at_channel, at_time.ticks, bar, barbeat, beattick);
}

afx_msg void
MFCSequenceEditor::OnMButtonUp(
			UINT nFlags,
			CPoint point 
		)
{
	point += GetScrollPosition();
}

afx_msg void
MFCSequenceEditor::OnMButtonDblClk(
			UINT nFlags,
			CPoint point 
		)
{
	point += GetScrollPosition();

	Time	at_time;
	short	at_channel;
	Pix2Time(point.x, at_time);
//	at_channel = Pix2Channel(point.y);
//	int		bar, barbeat, beattick;
//	at_time.GetBBQValue(bar, barbeat, beattick);
//	fprintf(stderr, "middle button double click: on point %d %d, channel %d, ticks %d t %d:%d.%d\n", point.x, point.y, at_channel, at_time.ticks, bar, barbeat, beattick);
}


afx_msg void
MFCSequenceEditor::OnRButtonDown(
			UINT nFlags,
			CPoint point 
		)
{
	EditorContextMenu(point, nFlags);
}

afx_msg void
MFCSequenceEditor::OnRButtonUp(
			UINT nFlags,
			CPoint point 
		)
{
	point += GetScrollPosition();
}

afx_msg void
MFCSequenceEditor::OnRButtonDblClk(
			UINT nFlags,
			CPoint point 
		)
{
	point += GetScrollPosition();

	Time	at_time;
	short	at_channel;
	Pix2Time(point.x, at_time);
//	at_channel = Pix2Channel(point.y);
//	int		bar, barbeat, beattick;
//	at_time.GetBBQValue(bar, barbeat, beattick);
//	fprintf(stderr, "right button double click: on point %d %d, channel %d, ticks %d t %d:%d.%d\n", point.x, point.y, at_channel, at_time.ticks, bar, barbeat, beattick);
}

void
MFCSequenceEditor::EditorContextMenu(CPoint &, UINT)
{
	;
}

void
MFCSequenceEditor::TrackPopupMenu(CPoint &pos, UINT nMenuID)
{
	CMenu popMenu;

	if (!popMenu.LoadMenu(nMenuID)) {
		fprintf(stderr, "load menu %d fails\n", nMenuID);
		return;
	}

	CMenu	*sub = popMenu.GetSubMenu(0);
	if (sub == NULL) {
		fprintf(stderr, "load menu %d has no popup\n", nMenuID);
		return;
	}
	sub->TrackPopupMenu(0,pos.x,pos.y,this);
}


afx_msg void
MFCSequenceEditor::OnMouseMove(
			UINT nFlags,
			CPoint point 
		)
{
	point += GetScrollPosition();

	Time	at_time;
	short	at_channel;
//	at_channel = Pix2Channel(point.y);

	if (bounds.PtInRect(point)) {
		if (!mouse_captured) {
//			SetCapture();
			mouse_captured = true;
		}
		// check cursor matches current tool
	} else if (bounds.PtInRect(point)) {
		if (mouse_captured) {
//			ReleaseCapture();
			mouse_captured = false;
		}
		// revert cursor to standard arrow
	}

	switch (mouse_action) {
		case QUA_MOUSE_ACTION_MOVE_INSTANCE: {
//			if (mouse_instance) {
//				Instance	*inst = mouse_instance->instance;
//				if (inst) {
//					Pix2Time(point.x-mouse_move_inst_offset.x, at_time);
//					Time	last_time;
//					Pix2Time(last_mouse_point.x-mouse_move_inst_offset.x, last_time);
//					short	last_channel = Pix2Channel(last_mouse_point.y);
//					if (last_time != at_time || last_channel != at_channel) {
//						quaLink->MoveInstance(inst->sym, at_channel, &at_time, NULL);
//					}
//				}
//			}
			break;
		}
		case QUA_MOUSE_ACTION_REGION_SELECT: {
			BRect	r(min(mouse_click.x,point.x),min(mouse_click.y,point.y),max(mouse_click.x,point.x),max(mouse_click.y,point.y));
			ChangeSelection(r);
			break;
		}
		case QUA_MOUSE_ACTION_SIZE_INSTANCE: {
//			if (mouse_instance) {
//				Instance	*inst = mouse_instance->instance;
//				if (inst) {
//					Pix2Time(point.x-mouse_move_inst_offset.x, at_time);
//					Time	last_time;
//					Pix2Time(last_mouse_point.x-mouse_move_inst_offset.x, last_time);
//					if (last_time != at_time) {
//						quaLink->MoveInstance(inst->sym, -1, NULL, &at_time);
//					}
//				}
//			}
			break;
		}
	}

	last_mouse_point = point;
}

afx_msg BOOL
MFCSequenceEditor::OnMouseWheel(
			UINT nFlags,
			short zDelta,
			CPoint point 
		)
{
//	point += GetScrollPosition();
	return CScrollView::OnMouseWheel(nFlags, zDelta, point);
}


afx_msg void
MFCSequenceEditor::OnKeyDown(
			UINT nChar,
			UINT nRepCnt,
			UINT nFlags 
		)
{
	switch (nChar) {
		case VK_DELETE: {
			short i=0;
//			while (i<NIR()) {
//				MFCInstanceView *iv = (MFCInstanceView *)IR(i);
//				if (iv == NULL) {
//					break;
//				}
//				if (iv->selected && iv->instance) {
//					quaLink->DeleteInstance(iv->instance->sym);
//				} else {
//					i++;
//				}
//			}
			break;
		}
		default: {
            CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
			break;
		}
	}
}

afx_msg void
MFCSequenceEditor::OnKeyUp(
			UINT nChar,
			UINT nRepCnt,
			UINT nFlags 
		)
{
	CScrollView::OnKeyUp(nChar, nRepCnt, nFlags);
}

afx_msg void
MFCSequenceEditor::OnVScroll(UINT nSBCode, UINT unPos, CScrollBar* pScrollBar)
{
	CScrollView::OnVScroll(nSBCode, unPos, pScrollBar);
}

BOOL
MFCSequenceEditor::OnScroll(
			UINT nScrollCode,
			UINT unPos,
			BOOL bDoScroll 
		)
{
	int nPos = (int) unPos;
//	fprintf(stderr, "on scroll %d/%d %d %d\n", nScrollCode&0xff, (nScrollCode&0xff00)>>8, nPos, bDoScroll);
	switch ((nScrollCode&0xff00)>>8) {
		case SB_BOTTOM: { //   Scroll to bottom. 
//			if (channeler && bDoScroll) {
//				long cPos = GetScrollPos(SB_VERT);
//				int		scrollMapMode;
//				CSize	totalSize;
//				CSize	pageSize;
//				CSize	lineSize;
//				GetDeviceScrollSizes(scrollMapMode, totalSize, pageSize, lineSize);
//				fprintf(stderr, "V Scrolling channels to bottom %d\n", pageSize.cy);
//				if (cPos < totalSize.cy) {
//					channeler->ScrollWindow(0, totalSize.cy-cPos, NULL, NULL);
//				}
//			}
			break;
		}
		case SB_ENDSCROLL: { //   End scroll. 
//			if (channeler) {
//				fprintf(stderr, "End of scroll\n");
//			}
			break;
		}
		case SB_LINEDOWN: { //   Scroll one line down. 
//			if (channeler && bDoScroll) {
//				long cPos = GetScrollPos(SB_VERT);
//				int		scrollMapMode;
//				CSize	totalSize;
//				CSize	pageSize;
//				CSize	lineSize;
//				GetDeviceScrollSizes(scrollMapMode, totalSize, pageSize, lineSize);
//				fprintf(stderr, "V Scrolling channels linedown %d %d %d\n", lineSize.cy, cPos, totalSize.cy);
//				if (cPos < totalSize.cy-bounds.bottom) {
//					int scrollAmt = 
//						(cPos+lineSize.cy > totalSize.cy-bounds.bottom)?cPos - totalSize.cy+bounds.bottom:-lineSize.cy;
//					channeler->ScrollWindow(0, scrollAmt, NULL, NULL);
//				}
//			}
			break;
		}
		case SB_LINEUP: { //   Scroll one line up. 
//			if (channeler && bDoScroll) {
//				long cPos = GetScrollPos(SB_VERT);
//				long chPos = channeler->GetScrollPos(SB_VERT);
//				int		scrollMapMode;
//				CSize	totalSize;
//				CSize	pageSize;
//				CSize	lineSize;
//				GetDeviceScrollSizes(scrollMapMode, totalSize, pageSize, lineSize);
//				if (cPos > 0) {
//					int scrollAmt = 
//						(cPos-lineSize.cy < 0)?cPos:lineSize.cy;
//					fprintf(stderr, "V Scrolling channels lineup %d %d %d scroll amt %d %d\n", lineSize.cy, cPos, totalSize.cy, scrollAmt, chPos);
//					channeler->ScrollWindow(0, scrollAmt, NULL, NULL);
//				}
//			}
			break;
		}
		case SB_PAGEDOWN: { //   Scroll one page down. 
//			if (channeler && bDoScroll) {
//				long cPos = GetScrollPos(SB_VERT);
//				int		scrollMapMode;
//				CSize	totalSize;
//				CSize	pageSize;
//				CSize	lineSize;
//				GetDeviceScrollSizes(scrollMapMode, totalSize, pageSize, lineSize);
//				fprintf(stderr, "V Scrolling channels pagedown %d\n", pageSize.cy);
//				if (cPos < totalSize.cy-bounds.bottom) {
//					int scrollAmt = 
//						(cPos+pageSize.cy > totalSize.cy-bounds.bottom)?cPos - totalSize.cy+bounds.bottom:-pageSize.cy;
//					channeler->ScrollWindow(0, scrollAmt, NULL, NULL);
//				}
//			}
			break;
		}
		case SB_PAGEUP: { //   Scroll one page up. 
//			if (channeler && bDoScroll) {
//				long cPos = GetScrollPos(SB_VERT);
//				int		scrollMapMode;
//				CSize	totalSize;
//				CSize	pageSize;
//				CSize	lineSize;
//				GetDeviceScrollSizes(scrollMapMode, totalSize, pageSize, lineSize);
//				fprintf(stderr, "V Scrolling channels pageup %d\n", pageSize.cy);
//				if (cPos > 0) {
//					int scrollAmt = 
//						(cPos-pageSize.cy < 0)?cPos:pageSize.cy;
//					channeler->ScrollWindow(0, scrollAmt, NULL, NULL);
//				}
//			}
			break;
		}
		case SB_THUMBPOSITION:  //   Scroll to the absolute position. The current position is provided in nPos. 
		case SB_THUMBTRACK: { //   Drag scroll box to specified position. The current position is provided in nPos. 
//			if (yscale && bDoScroll) {
//				long cPos = GetScrollPos(SB_VERT);
//				fprintf(stderr, "V Scrolling channels absolute in parallel %x %d %d\n", nScrollCode, nPos, cPos);
//				yscale->ScrollWindow(0,cPos-nPos,NULL,NULL);
//			}
			break;
		}
		case SB_TOP: { //   Scroll to top. 
//			if (channeler && bDoScroll) {
//				long cPos = GetScrollPos(SB_VERT);
//				fprintf(stderr, "V Scrolling to top (by %d)\n", -cPos);
//				if (cPos > 0) {
//					channeler->ScrollWindow(0,-cPos,NULL,NULL);
//				}
//			}
			break;
		}
	}
	return CScrollView::OnScroll(nScrollCode,unPos,bDoScroll);
}

BOOL
MFCSequenceEditor::OnScrollBy( CSize sizeScroll, BOOL bDoScroll  )
{
	bool scr = CScrollView::OnScrollBy(sizeScroll,bDoScroll);
	cerr << "MFCSequenceEditor onscrollby cx "<< sizeScroll.cx << ", cy "<< sizeScroll.cy << ", do " << bDoScroll << ", scrollview::onscrolly()  " << scr << endl;
	if (scr) {
		cerr << "MFCSequenceEditor scroll ok xscale " << xscale << endl;
		if (xscale) {
			if (bDoScroll) {
				cerr << "MFCSequenceEditor x scroll " << -sizeScroll.cx << endl;
				CRect	r;
//				xscale->GetWindowRect(&r);
				xscale->SetWindowPos(&wndTop,0,bounds.bottom-xscale->bounds.bottom,0,0,SWP_NOSIZE);
//				xscale->Invalidate();
			}
		}
		if (yscale) {
			if (bDoScroll) {
				yscale->ScrollWindow(0, -sizeScroll.cy);
			}
		}
	}
	return scr;
}

/////////////////////////////////////////////////////////////////////////////
// MFCDataEditor methods
/////////////////////////////////////////////////////////////////////////////

MFCEditorItemView*
MFCSequenceEditor::ItemViewAtPoint(CPoint &p, UINT flags, short &hitType, void *&clkit)
{
	for (short i=0; i<NItemR(); i++) {
		MFCEditorItemView *iv = ItemR(i);
		if ((hitType=iv->HitTest(p, flags, clkit)) != 0) {
			return iv;
		}
	}
	return NULL;
}

bool
MFCSequenceEditor::RemoveItemView(MFCEditorItemView *iv)
{
	if (iv == NULL) {
		return true;
	}
	RemItemR(iv);
	CRect	updr = iv->BoundingBox();
	CPoint	scr = GetScrollPosition();
	updr = bounds - scr;
	updr.bottom ++;
	updr.right ++;
	RedrawWindow(&updr);
	delete iv;
	return true;
}
#include <algorithm>
bool
MFCSequenceEditor::RemoveClipsNotIn(vector<StabEnt*> &list)
{
	long	i=0;
	while (i<NItemR()) {
		MFCEditorItemView	*iv = ItemR(i);
		bool	del = false;
		if (iv->type == MFCEditorItemView::CLIP) {
			Clip	*c = ((MFCClipItemView*)iv)->item;
			auto it = find(list.begin(), list.end(), c->sym);
			if (it != list.end()) {
				del = true;
			}
		}
		if (del) {
			RemItemR(iv);
			BRect	b = iv->BoundingBox();
			b.left-=2;
			b.right+=2;
			InvalidateRect(&b);
		} else {
			i++;
		}
	}
	return true;
}

MFCEditorItemView *
MFCSequenceEditor::AddClipItemView(Clip *clip)
{
	
	MFCEditorItemView *nv;
	nv = ItemViewFor(clip);
	if (nv != NULL) {
		return nv;
	}
	nv = new MFCClipItemView(this, clip);
	AddItemR(nv);
	nv->Redraw();
	UpdateWindow();
	Time	endT = clip->start + clip->duration;
	if (endT > lastScheduledEvent) {
		lastScheduledEvent = endT;
	}
	return nv;
}

bool
MFCSequenceEditor::DelClipItemView(MFCEditorItemView *iv)
{
	return RemoveItemView(iv);
}

bool
MFCSequenceEditor::ClearAllItemViews(bool redraw)
{
	long	count = NItemR();
	for (long i=0; i<count; i++) {
		MFCEditorItemView	*iv = ItemR(0);
		RemItemR(iv);
		delete iv;
	}
	if (redraw) {
		InvalidateRect(&bounds);
	}
	lastScheduledEvent = Time::zero;
	return true;
}

bool
MFCDataEditor::CreateEditor(CRect &r, MFCObjectView *v, UINT id)
{
	long ret = Create(NULL, "sequence editor", WS_CHILD|WS_VISIBLE, r, v, id, NULL);
	parent = v;
	return ret != 0;
}


/////////////////////////////////////////////////////////////////////////////
// MFCStreamItemView
/////////////////////////////////////////////////////////////////////////////
MFCEditorItemView::MFCEditorItemView(MFCSequenceEditor *a, short typ)
{
	editor = a;
	type = typ;
	selected = false;
//	fprintf(stderr, "%s -> ", i->startTime.StringValue());
//	fprintf(stderr, "%s\n", arrTime.StringValue());
//	fprintf(stderr, "%s -> ", i->duration.StringValue());
//	fprintf(stderr, "%s\n", arrDur.StringValue());
	CalculateBounds();
//	Redraw();
}

MFCEditorItemView::~MFCEditorItemView()
{
	;
}

void
MFCEditorItemView::CalculateBounds()
{
	;
}

#ifdef QUA_V_GDI_PLUS
void
MFCEditorItemView::Draw(Graphics &dc, CRect &clipBox)
{
// !!!??? need to clip properly for short instances with long names
	Pen			blackPen(Color(250, 0,0,0), 1);
	Pen			redPen(Color(250, 160, 10, 10), 1);
	SolidBrush	blueBrush(Color(100, 10, 10, 160));
	SolidBrush	blackBrush(Color(100, 0,0,0));

//	fprintf(stderr, "drawing instance view %d\n", bounds.right-bounds.left);
	dc.FillRectangle(&blueBrush, bounds.left, bounds.top, bounds.right-bounds.left, bounds.bottom-bounds.top);
	dc.DrawRectangle(selected?&redPen:&blackPen, bounds.left, bounds.top, bounds.right-bounds.left, bounds.bottom-bounds.top);
}

#else
void
MFCEditorItemView::Draw(CDC *dc, CRect *clipBox)
{
	;
}
#endif

void
MFCEditorItemView::Redraw()
{
	if (editor) {
		CRect	updr = bounds;
		CPoint	scr = editor->GetScrollPosition();
		updr = bounds - scr;
		updr.bottom ++;
		updr.right ++;
		editor->InvalidateRect(&updr);
	}
}
void
MFCEditorItemView::DrawMove()
{
	if (editor) {
		CPoint	scr = editor->GetScrollPosition();
		CRect	oldr = bounds - scr;
		oldr.right++;
		oldr.bottom++;
		CalculateBounds();
		editor->InvalidateRect(&oldr);
		Redraw();
	}
}

void
MFCEditorItemView::Select(bool sel)
{
	if (sel == selected) {
		return;
	}
	selected = sel;
	Redraw();
}

/////////////////////////////////////////////////////////////////////////////
// MFCClipItemView
/////////////////////////////////////////////////////////////////////////////
MFCClipItemView::MFCClipItemView(MFCSequenceEditor *ed, Clip *c):
	MFCEditorItemView(ed, CLIP)
{
	item = c;
	CalculateBounds();
}

MFCClipItemView::~MFCClipItemView()
{
}

void
MFCClipItemView::CalculateBounds()
{
	if (item && editor) {
		Time	sTime = item->start&editor->displayMetric;
		long	pixl = editor->Time2Pix(sTime);
		long	pixd;

		Time	arrDur = item->duration&editor->displayMetric;
		if (arrDur.ticks <= 0) {
			pixd = 10;
		} else {
			pixd = editor->Time2Pix(arrDur);
			if (pixd < 2) {
				pixd = 2;
			}
		}
		CSize sz = editor->GetTotalSize();
		fprintf(stderr, "calculate bounds %x %x %d %d %d\n", item, editor, pixl, pixd, arrDur.ticks);
		bounds.Set(pixl, 0, pixl+pixd, sz.cy);
	}
}

#ifdef QUA_V_GDI_PLUS
void
MFCClipItemView::Draw(Graphics *dc, CRect *clipBox)
{
// !!!??? need to clip properly for short instances with long names
	Pen			blackPen(Color(250, 0, 0, 0), 1);
	Pen			redPen(Color(250, 238, 100, 100), 1);
	Pen			orangePen(Color(200, 250, 150, 10), 1);
	SolidBrush	blueBrush(Color(100, 100, 100, 238));
	SolidBrush	blackBrush(Color(200, 0, 0, 0));
	SolidBrush	orangeBrush(Color(190, 250, 150, 10));
	dc->DrawLine(
		&orangePen,
		bounds.left, 0,
		bounds.left, bounds.bottom);
	bool	isMarker = false;
	if (item != NULL && item->duration.ticks <= 0) {
		isMarker = true;
	}
	if (!isMarker) {
		dc->DrawLine(
			&orangePen,
			bounds.right, 0,
			bounds.right, bounds.bottom);
	}

	PointF	tri[3];

	Font	labelFont(L"Arial", 8.0, FontStyleRegular, UnitPoint, NULL);
	wstring nm;
	const char *cp = item->sym->uniqueName();
	while (*cp) {
		nm.push_back(*cp++);
	}
	PointF	p;
	UINT py = 0;
	do {
		if (!isMarker) {
			// a triangle bit
			tri[0].X = bounds.left; tri[0].Y = py;
			tri[1].X = bounds.left+6; tri[1].Y = py+3;
			tri[2].X = bounds.left; tri[2].Y = py+6;
			dc->FillPolygon(&orangeBrush, tri, 3);
		}

		p.X = bounds.left-1;
		p.Y = py+5;
		RectF	box;
		StringFormat	sff = StringFormatFlagsDirectionVertical;
		dc->MeasureString(nm.c_str(), -1, &labelFont, p, &sff, &box);
		dc->DrawString(nm.c_str(), -1, &labelFont, box,	&sff, &blackBrush);

		if (!isMarker) {
			// a nother triangle bit
			tri[0].X = bounds.right;
			tri[1].X = bounds.right-6;
			tri[2].X = bounds.right;
			dc->FillPolygon(&orangeBrush, tri, 3);

			p.X = bounds.right-10;
			p.Y = py+5;
			dc->MeasureString(nm.c_str(), -1, &labelFont, p, &sff, &box);
			dc->DrawString(nm.c_str(), -1, &labelFont, box,	&sff, &blackBrush);
		}
		py += editor->bounds.bottom;
	} while (py < bounds.bottom);

}

#else
void
MFCClipItemView::Draw(CDC *dc, CRect *clipBox)
{
	;
}
#endif


bool
MFCClipItemView::Represents(void *it)
{
	if (it == NULL)
		return false;
	if (item == (Clip *)it) {
		return true;
	}
	return false;
}

short
MFCClipItemView::HitTest(CPoint &p, UINT flgs, void *&clkit)
{
//	fprintf(stderr, "hit test %d %d\n", p.y, bounds.bottom, bounds.top);
	clkit = NULL;
	bool	look_interior = ((flgs & MK_CONTROL) != 0);
	short rng = 2;
	if (look_interior) {
		rng = 4;
	} else {
		rng = 2;
	}
	if (Abs(p.x - bounds.left) < 2) {
		return 1;
	}
	if (Abs(p.x - bounds.right) < 2) {
		return 2;
	}
	return 0;
}

void
MFCClipItemView::SetClipStartTime(Time &st)
{
	if (item) {
		item->start = st;
		CRect	olb = bounds;olb.right = olb.left + 13;olb.left-=1;
		CRect	orb = bounds;orb.left = orb.right - 10;orb.right+=3;
		CalculateBounds();
		CRect	nrb = bounds;nrb.left = nrb.right - 10;nrb.right+=3;
		CRect	nlb = bounds;nlb.right = nlb.left + 13;nlb.left-=1;
		editor->InvalidateRect(&olb);
		editor->InvalidateRect(&orb);
		editor->InvalidateRect(&nrb);
		editor->InvalidateRect(&nlb);

	}
}


void
MFCClipItemView::SetClipEndTime(Time &et)
{
	if (item) {
		if (et >item->start) {
			item->duration = et-item->start;
//			CRect	olb = bounds;olb.right = olb.left + 13;olb.left-=1;
			CRect	orb = bounds;orb.left = orb.right - 10;orb.right+=3;
			CalculateBounds();
			CRect	nrb = bounds;nrb.left = nrb.right - 10;nrb.right+=3;
//			CRect	nlb = bounds;nlb.right = nlb.left + 13;nlb.left-=1;
//			editor->InvalidateRect(&olb);
			editor->InvalidateRect(&orb);
			editor->InvalidateRect(&nrb);
//			editor->InvalidateRect(&nlb);
		}

	}
}


IMPLEMENT_DYNCREATE(MFCDataEditorTScale, CWnd)

BEGIN_MESSAGE_MAP(MFCDataEditorTScale, CWnd)
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_SIZING()
	ON_WM_CREATE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_PAINT()
END_MESSAGE_MAP()

MFCDataEditorTScale::MFCDataEditorTScale()
{
	editor = NULL;
}

MFCDataEditorTScale::~MFCDataEditorTScale()
{
	;
}

BOOL
MFCDataEditorTScale::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE; // don't draw background
}


void
MFCDataEditorTScale::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	dc.SetDCBrushColor(rgb_blue);
	dc.FillSolidRect(bounds, rgb_blue);
	if (editor && editor->displayMetric) {
		CBrush	black(RGB(0,0,0)); 
		CPoint	point(0,0);
		point = editor->GetScrollPosition();

		Time	t=Time::zero;
		long	atx = point.x;
		editor->Pix2Time(atx,t);
		t = ~t;
		long	tsx = 0;
		while (tsx < bounds.right) {
			Time	markT = t&editor->displayMetric;
			char	buf[32];
			long xp=editor->Time2Pix(markT)-point.x;
			strcpy(buf, markT.StringValue());
			long	sx=0;
			long	sy=0;
			StringExtent(buf, &displayFont, this, sx, sy);
			CRect	tr(xp,0,xp+sx,sy);
			dc.SetTextColor(rgb_black);
			tr.top = tr.bottom-9;
			dc.SelectObject(&displayFont);
//			dc.SetBkMode(TRANSPARENT);
			dc.SetBkColor(rgb_blue);

			dc.DrawText(buf, -1, &tr, DT_LEFT|DT_BOTTOM);
			t += editor->displayMetric->granularity;
			tsx = xp+sx-point.x;
		}
	}

	/*
	CBrush	black(RGB(0,0,0)); 
	CBrush	grey(RGB(200,200,200));
	CBrush	white(RGB(255,255,255)); 
	CPen	greyPen(PS_SOLID, 1, RGB(200,200,200));
	CPen	bluePen(PS_SOLID, 1, RGB(140,140,230));
	dc.SetBkMode(TRANSPARENT);
	dc.SelectObject(&displayFont);
	for (short i=127; i>=0; i--) {
		short pitch = i%12;
		CRect	tr(0,i,10, 2*i);

		if (editor) {
			tr.right = keylen;
//			tr.bottom = editor->Note2Pix(i);
//			tr.top = editor->Note2Pix(i+1);
		}

		if (pitch==1 || pitch== 3 || pitch==6 || pitch == 8 || pitch== 10) {
			tr.bottom++;
			dc.FillRect(&tr, &black);
		} else {
			dc.FillRect(&tr, &white);
			if (pitch == 11 || pitch == 4) {
				dc.SelectObject(&greyPen);
				dc.MoveTo(tr.left, tr.top);
				dc.LineTo(tr.right, tr.top);
			}
		}
		if (pitch == 0) {
			char	buf[32];
			sprintf(buf, "C%d", i/12) ;
			dc.SetTextColor(RGB(80,80,240));
			tr.top = tr.bottom-9;
			dc.DrawText(buf, -1, &tr, DT_LEFT|DT_BOTTOM);
		}
	}
	*/
	return;
}

void
MFCDataEditorTScale::OnHScroll(
				UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	cerr << "on h scroll" << endl;
}

bool
MFCDataEditorTScale::CreateTScale(
		CRect &r, CWnd *w,
		UINT id, MFCSequenceEditor *ed)
{
	editor = ed;
	if (!Create(_T("STATIC"), "timeline",
				WS_CHILD|WS_VISIBLE
				/*|WS_CLIPSIBLINGS*/
				/*|TBS_TOOLTIPS*/
						, r, w, id)) {
		return false;
	}
	ed->xscale = this;
	SetDlgCtrlID(id);
//	CToolTipCtrl	*t=GetToolTips();

	return true;
}


void
MFCDataEditorTScale::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	bounds.right = cx;
	bounds.bottom = cy;
	fprintf(stderr, "tscale size %d %d\n", cx, cy);
}

void
MFCDataEditorTScale::OnSizing( UINT which, LPRECT r)
{
	CWnd::OnSizing(which, r);
}

afx_msg void
MFCDataEditorTScale::OnMove(int x, int y)
{
	CWnd::OnMove(x, y);
	fprintf(stderr, "tscale move %d %d\n", x, y);
}

int
MFCDataEditorTScale::OnCreate(LPCREATESTRUCT lpCreateStruct )
{
	bounds.left = 0;
	bounds.top = 0;
	bounds.right = lpCreateStruct->cx;
	bounds.bottom = lpCreateStruct->cy;
	return CWnd::OnCreate(lpCreateStruct);
}
