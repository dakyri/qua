

//#define _AFXDLL
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE
#include "stdafx.h"
#include "qua_version.h"
#include "ShlObj.h"

#include "StdDefs.h"
#include "QuaMFC.h"
#include "QuaMFCDoc.h"
#include "MFCStreamDataEditor.h"
#include "MFCObjectView.h"
#include "Qua.h"
#include "QuaMidi.h"
#include "Time.h"
#include "Voice.h"

#include "MemDC.h"

#include <iostream>

CFont		MFCStreamEditorYScale::displayFont;

IMPLEMENT_DYNCREATE(MFCStreamDataEditor, MFCSequenceEditor)

BEGIN_MESSAGE_MAP(MFCStreamDataEditor, MFCSequenceEditor)
	//{{AFX_MSG_MAP(MFCDataEditor)
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

MFCStreamDataEditor::MFCStreamDataEditor(void)
{
	stream = nullptr;
	take = nullptr;
}

MFCStreamDataEditor::~MFCStreamDataEditor(void)
{
}


long
MFCStreamDataEditor::NHorizontalPix()
{
	return 128;
}

long
MFCStreamDataEditor::HorizontalPix(long n)
{
	return Note2Pix(n);
}

void
MFCStreamDataEditor::SetHVScroll()
{
	long	atY=0;
	long	pixx = Time2Pix(lastScheduledEvent);
	if (pixx < bounds.right*2) {
		pixx = bounds.right;
	}
	SetScrollSizes(MM_TEXT, CSize(pixx, Note2Pix(0)));
//	if (channeler) {
//		MFCChannelView *ev =(MFCChannelView *)channeler->CR(channeler->NCR()-1);
//		if (ev) {
//			SetScrollRange(SB_VERT, 0, ev->frame.bottom-bounds.bottom);
//			reportError("%d %d", bounds.right, ev->frame.bottom-bounds.bottom);
//			if (ev->frame.bottom > 0) {
//			}
//		}
//	}
//	SetScrollRange(SB_HORZ, 0, 10);
}

bool
MFCStreamDataEditor::SetToTake(Take *t)
{
	if (t == nullptr) {// watch this !!!!
		return true;
	} else if (t->type == Take::STREAM) {
		if (take != t) {
			ClearAllItemViews(false);
			take = (StreamTake *)t;
			stream = &take->stream;
			AddAllItemViews(true);
		}
		return true;
	}
	return false;
}

bool
MFCStreamDataEditor::AddAllItemViews(bool redraw)
{
	if (NItemR() > 0) {
		ClearAllItemViews(false);
	}
	if (stream == nullptr) {
		if (redraw) {
			InvalidateRect(&bounds);
		}
		return false;
	}

	StreamItem	*p = stream->head;
	CPoint		pt;
	pt.x = 0;
	pt.y = 0;
	while (p != nullptr) {
		MFCEditorItemView	*iv = AddStreamItemView(p, &pt, false);
		if (iv->type == MFCEditorItemView::LIST) {
			pt.y = iv->BoundingBox().bottom + 5;
		}
		p = p->next;
	}

	StabEnt	*pars = parent->Symbol();
	if (pars) {
		StabEnt	*sibp = pars->children;
		while (sibp != nullptr) {
			if (sibp->type == TypedValue::S_CLIP) {
				Clip	*c = sibp->ClipValue(nullptr);
				if (c->media != nullptr) {
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
MFCStreamDataEditor::OnInitialUpdate()
{
	CQuaMFCDoc	*qdoc = (CQuaMFCDoc *)GetDocument();
	if (qdoc == nullptr) {
//		reportError("initial update of stream data editor finds a null sequencer document");
	} else if (qdoc->qua == nullptr) {
//		reportError("initial update of stream data editor finds a null sequencer");
	} else {	// set qua up with our hooks
//		quaLink = qdoc->qua->bridge.display;
//		quaLink->AddArranger(this);
//		SetScrollSizes(MM_TEXT, CSize(bounds.right, bounds.bottom));
//		reportError("%d %d", bounds.right, bounds.bottom);
//		display->AddChannelRepresentations(this);
	}
	SetHVScroll();
}

// To this code
BOOL
MFCStreamDataEditor::OnEraseBkgnd(CDC* pDC)
{
	//	cerr << "on erase back" << endl;
	return FALSE;
	//	return CScrollView::OnEraseBkgnd(pDC);
}

void
MFCStreamDataEditor::OnPaint()
{
	// standard paint routine
	CPaintDC dc(this);
	OnPrepareDC(&dc);
	OnDraw(&dc);
}

void
MFCStreamDataEditor::OnDraw(CDC* pdc)
{
	CRect	clipBox;
	int		cbType;
	cbType = pdc->GetClipBox(&clipBox);
//	fprintf(stderr, "OnDraw() clip (%d %d %d %d) type %s\n", clipBox.left, clipBox.top, clipBox.right, clipBox.bottom, cbType==COMPLEXREGION?"complex":cbType==SIMPLEREGION?"simple":cbType==nullptrREGION?"null":"error");
//	fprintf(stderr, "lt %d\n", lastScheduledEvent.ticks);
	CDocument* pDoc = GetDocument();
#ifdef QUA_V_GDI_PLUS
	CMemDC memdc(pdc);
	Graphics	graphics(memdc);
	DrawGridGraphics(graphics, clipBox);

// test the interoperation of GDI and GDI+
//	Color	p = Color::MakeARGB(100, 255, 20, 255);
//	Pen	semip(p, 3);
//	graphics.DrawLine(&semip, 0,0, 200, 200);

	for (short i=0; i<NItemR(); i++) {
		MFCEditorItemView *ir = ItemR(i);
		if (ir->type != MFCEditorItemView::DISCRETE && ir->BoundingBox().Intersects(clipBox)) {
			ir->Draw(graphics, clipBox);
		}
	}
	for (short i=0; i<NItemR(); i++) {
		MFCEditorItemView *ir = ItemR(i);
		if (ir->type == MFCEditorItemView::DISCRETE && ir->BoundingBox().Intersects(clipBox)) {
			ir->Draw(graphics, clipBox);
		}
	}
	DrawCursor(graphics, clipBox);
#else
	DrawGrid(pdc, &clipBox);
	DrawCursor(pdc);
#endif
//	fprintf(stderr, "OnDraw() finito\n");
}

/////////////////////////////////////////////////////////////////////////////
// MFCDataEditor assorted wrappers
/////////////////////////////////////////////////////////////////////////////
MFCEditorItemView*
MFCStreamDataEditor::AddStreamItemView(StreamItem *i, CPoint *pt, bool redraw)
{
	if (i == nullptr) {
		return nullptr;
	}
	Time	evT = i->time + i->Duration();
	if (evT > lastScheduledEvent) {
		lastScheduledEvent = evT;
	}
	MFCEditorItemView *nv = nullptr;
	switch (i->type) {
		case TypedValue::S_BEND:
		case TypedValue::S_CTRL:
		case TypedValue::S_PROG: {
			uchar	d1=0;
			if (i->type == TypedValue::S_CTRL) {
				d1 = ((StreamCtrl*)i)->ctrl.controller;
			}
			MFCStreamItemListView *nlv = nullptr;
			nlv = ItemListViewFor(i);
			if (nlv == nullptr) {
				nlv = new MFCStreamItemListView(this, pt, i->type, d1);
				AddItemR(nlv);
			}
			if (redraw) {
				nlv->Redraw();
				UpdateWindow();
			}
			nv = nlv;
			break;
		}
		case TypedValue::S_NOTE: {
			MFCStreamItemView	*siv = new MFCStreamItemView(this, i);
			AddItemR(siv);
			if (redraw) {
				siv->Redraw();
				UpdateWindow();
			}
			nv = siv;
			break;
		}
	}
	return nv;
}


MFCStreamItemListView *
MFCStreamDataEditor::ItemListViewFor(StreamItem *item)
{
	for (short i=0; i<NItemR(); i++) {
		MFCEditorItemView *iv = ItemR(i);
		if (iv->Represents(item)) {
			if (iv->type == MFCEditorItemView::LIST) {
				return (MFCStreamItemListView*)iv;
			} else {	// this would be a bit pear shaped
				fprintf(stderr, "shape of pear: bad item type deep in ItemListViewFor()\n");
				return nullptr;
			}
		} else if (iv->type == MFCEditorItemView::LIST) {
			MFCStreamItemListView *slv = (MFCStreamItemListView *)iv;
			if (slv->listitemType == TypedValue::S_CTRL) {
				if (slv->listitemType == item->type && slv->listitemData1 == ((StreamCtrl*)item)->ctrl.controller) {
					return slv;
				}
			} else if (slv->listitemType == TypedValue::S_BEND || slv->listitemType == TypedValue::S_PROG) {
				if (slv->listitemType == item->type) {
					return slv;
				}
			} else {
				// this is also a bit pear shaped but we can live with it
			}
		}
	}
	return nullptr;
}

void
MFCStreamDataEditor::MoveItemViewFor(void *i)
{
	MFCEditorItemView *nv = ItemViewFor(i);
	nv->DrawMove();
}

void
MFCStreamDataEditor::RemoveItemViewFor(void *i)
{
	MFCEditorItemView *qir = ItemViewFor(i);
	RemoveItemView(qir);
}

void
MFCStreamDataEditor::SetItemDuration(MFCEditorItemView *iv, dur_t dur)
{
	if (dur > 0) {
		if (iv->type == MFCEditorItemView::DISCRETE) {
			MFCStreamItemView *itv = (MFCStreamItemView *)iv;
			if (itv->item && stream) {
				stream->SetDuration(itv->item, dur);
				itv->DrawMove();
			}
		}
	}
}

void
MFCStreamDataEditor::SetStartTimeMidiParams(MFCEditorItemView *iv, Time &at, int8 pitch)
{
	if (iv->type == MFCEditorItemView::DISCRETE) {
		MFCStreamItemView *itv = (MFCStreamItemView *)iv;
		if (itv->item && stream) {
			stream->ModifyItemTime(itv->item, at);
			itv->item->SetMidiParams(-1, pitch, -1, true);
			itv->DrawMove();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// MFCDataEditor mouse and keyboard handlers
/////////////////////////////////////////////////////////////////////////////

afx_msg void
MFCStreamDataEditor::OnLButtonDown( UINT nFlags, CPoint point  )
{
	point += GetScrollPosition();
	last_mouse_point = mouse_click = point;

	mouse_action = QUA_MOUSE_ACTION_NONE;

	short	at_note;
	at_note = Pix2Note(point.y);
	Time	at_time;
	Time	dur_time;
	Pix2Time(point.x, at_time);
	dur_time.metric = at_time.metric;
	dur_time.ticks = 1;

	cerr << "got note time " << at_time.metric <<", "<< at_time.ticks << endl;

//	int		bar, barbeat, beattick;
//	at_time.GetBBQValue(bar, barbeat, beattick);
//	fprintf(stderr, "left button down: on point %d %d, channel %d, ticks %d t %d:%d.%d\n", point.x, point.y, at_channel, at_time.ticks, bar, barbeat, beattick);

	if (stream == nullptr) {
		return;
	}
	mouse_sub_item = nullptr;
	switch (currentTool) {
		case ID_ARTOOL_POINT: {
			DeselectAll();
			short	hitType;
			void	*hitIt=nullptr;
			mouse_item = ItemViewAtPoint(mouse_click, nFlags, hitType, hitIt);
			if (mouse_item) {
				if (hitIt) {
					mouse_sub_item = (StreamItem *)hitIt;
				}
				switch (mouse_item->type) {
					case MFCEditorItemView::DISCRETE: {
						MFCStreamItemView	*siv = (MFCStreamItemView *)mouse_item;
						StreamItem	*item = siv->item;
						if (item) {
							if (item->type == TypedValue::S_NOTE) {
								mouse_action = QUA_MOUSE_ACTION_MOVE_EVENT;
								mouse_move_item_offset.x = mouse_click.x - mouse_item->BoundingBox().left;
								mouse_move_item_offset.y = mouse_click.y;
								mouse_item->Select(true);
							}
						}
						break;
					}
					case MFCEditorItemView::LIST: {
						MFCStreamItemListView	*siv = (MFCStreamItemListView *)mouse_item;
						if (hitType == 1) { // top edge
							mouse_action = QUA_MOUSE_ACTION_MOVE_INSTANCE;
							mouse_move_item_offset.x = mouse_click.x;
							mouse_move_item_offset.y = mouse_click.y;
							mouse_item->Select(true);
						} else if (hitType == 2) { // bottom edge
							mouse_action = QUA_MOUSE_ACTION_SIZE_INSTANCE;
							mouse_move_item_offset.x = mouse_click.x;
							mouse_move_item_offset.y = mouse_click.y;
							mouse_item->Select(true);
						} else if (hitType == 3) { 	// top edge of an interior item point
							mouse_action = QUA_MOUSE_ACTION_MOVE_EVENT;
							mouse_move_item_offset.x = mouse_click.x;
							mouse_move_item_offset.y = mouse_click.y;
							mouse_item->Select(true);
						} else if (hitType == 4) { // side edge of an interior item point
							mouse_action = QUA_MOUSE_ACTION_SIZE_EVENT;
							mouse_move_item_offset.x = mouse_click.x;
							mouse_move_item_offset.y = mouse_click.y;
							mouse_item->Select(true);
						}
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
			void	*hitIt=nullptr;
			mouse_item = ItemViewAtPoint(mouse_click, nFlags, hitType, hitIt);
			cerr << "draw note time " << at_time.metric << ", " << at_time.ticks << endl;

			if (mouse_item == nullptr) {
				StreamItem		*iv= new StreamNote(at_time, 0, at_note, 80, 1);
				cerr << iv->time.metric << " got this " << endl;
				stream->InsertItem(iv);
				mouse_item = AddStreamItemView(iv, nullptr, true);
			}
			if (mouse_item) {
				if (hitIt) {
					mouse_sub_item = (StreamItem *)hitIt;
				}
				switch (mouse_item->type) {
					case MFCEditorItemView::DISCRETE: {
						MFCStreamItemView	*siv = (MFCStreamItemView *)mouse_item;
						StreamItem	*item = siv->item;
						if (item) {
							if (item->type == TypedValue::S_NOTE) {
								mouse_action = QUA_MOUSE_ACTION_SIZE_EVENT;
								mouse_move_item_offset.x = siv->BoundingBox().left;
								mouse_move_item_offset.y = mouse_click.y;
								Pix2Time(point.x-mouse_move_item_offset.x, at_time);
								SetItemDuration(siv, at_time.ticks);
							}
						}
						break;
					}
					case MFCEditorItemView::LIST: {
						MFCStreamItemListView	*siv = (MFCStreamItemListView *)mouse_item;
						Pix2Time(point.x, at_time);
						StreamItem	*it = nullptr;
						float		val = siv->Pix2Val(point.y);
						last_controller_value = val;
						switch (siv->listitemType) {
							case TypedValue::S_CTRL: {
								if (stream) {
									mouse_action = QUA_MOUSE_ACTION_DRAW_EVENT;
									if ((it=stream->FindItemAtTime(0, at_time, TypedValue::S_CTRL, -1, siv->listitemData1)) == nullptr) {
										it = new StreamCtrl(at_time, 0, siv->listitemData1, 0);
										stream->InsertItem(it);
									}
									siv->SetSubItemValue(it, val);
								}
								break;
							}
							case TypedValue::S_BEND: {
								if (stream) {
									mouse_action = QUA_MOUSE_ACTION_DRAW_EVENT;
									if ((it=stream->FindItemAtTime(0, at_time, TypedValue::S_BEND, -1, siv->listitemData1))==nullptr) {
										it = new StreamBend(at_time, 0, 0);
										stream->InsertItem(it);
									}
									siv->SetSubItemValue(it, val);
								}
								break;
							}
						}
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
MFCStreamDataEditor::OnLButtonUp(
			UINT nFlags,
			CPoint point 
		)
{
	point += GetScrollPosition();
	mouse_item = nullptr;
	mouse_action = QUA_MOUSE_ACTION_NONE;
}


afx_msg void
MFCStreamDataEditor::OnLButtonDblClk(
			UINT nFlags,
			CPoint point 
		)
{
	point += GetScrollPosition();

	Time	at_time;
	short	at_note;
	Pix2Time(point.x, at_time);
	at_note = Pix2Note(point.y);

//	int		bar, barbeat, beattick;
//	at_time.GetBBQValue(bar, barbeat, beattick);
//	fprintf(stderr, "left button double click: on point %d %d, channel %d, ticks %d t %d:%d.%d\n", point.x, point.y, at_channel, at_time.ticks, bar, barbeat, beattick);
}

afx_msg void
MFCStreamDataEditor::OnMButtonDown(
			UINT nFlags,
			CPoint point 
		)
{
	point += GetScrollPosition();

	Time	at_time;
	short	at_note;
	Pix2Time(point.x, at_time);
	at_note = Pix2Note(point.y);
//	int		bar, barbeat, beattick;
//	at_time.GetBBQValue(bar, barbeat, beattick);
//	fprintf(stderr, "middle button down: on point %d %d, channel %d, ticks %d t %d:%d.%d\n", point.x, point.y, at_channel, at_time.ticks, bar, barbeat, beattick);
}

afx_msg void
MFCStreamDataEditor::OnMButtonUp(
			UINT nFlags,
			CPoint point 
		)
{
	point += GetScrollPosition();
	mouse_action = QUA_MOUSE_ACTION_NONE;
}

afx_msg void
MFCStreamDataEditor::OnMButtonDblClk(
			UINT nFlags,
			CPoint point 
		)
{
	point += GetScrollPosition();

	Time	at_time;
	short	at_channel;
	Pix2Time(point.x, at_time);
	at_channel = Pix2Note(point.y);

//	int		bar, barbeat, beattick;
//	at_time.GetBBQValue(bar, barbeat, beattick);
//	fprintf(stderr, "middle button double click: on point %d %d, channel %d, ticks %d t %d:%d.%d\n", point.x, point.y, at_channel, at_time.ticks, bar, barbeat, beattick);
}

void
MFCStreamDataEditor::EditorContextMenu(CPoint &point, UINT nFlags)
{
	Time	at_time;
	short	at_note;
	Pix2Time(point.x, at_time);
	at_note = Pix2Note(point.y);

	CPoint		popPt = point;
	ClientToScreen(&popPt);

	CMenu		*ctxtMenu = new CMenu;
	ctxtMenu->CreatePopupMenu();
	int i=0;
	char buf[128];
	short hitType;
	void *hitIt=nullptr;
	CPoint itPt = point;
	itPt += GetScrollPosition();
	MFCEditorItemView	* iv = ItemViewAtPoint(itPt, nFlags|MK_CONTROL, hitType, hitIt);
	bool menu_built = false;
	if (iv != nullptr) {
		if (iv->type == MFCEditorItemView::LIST) {
			MFCStreamItemListView	*siv = (MFCStreamItemListView *)iv;
			switch (siv->listitemType) {
				case TypedValue::S_BEND: {
					menu_built = true;
					ctxtMenu->AppendMenu(MF_POPUP, ID_STREAMEDITORCONTEXT_DEL_BEND, "delete bend message");
					break;
				}
				case TypedValue::S_CTRL: {
					menu_built = true;
					ctxtMenu->AppendMenu(MF_POPUP, ID_STREAMEDITORCONTEXT_DEL_CTRL, "delete control message");
					CMenu	*controllerMenu = new CMenu;
					controllerMenu->CreatePopupMenu();
					for (i=0; i<128; i++) {
						if (i != siv->listitemData1) {
							string cnm = qut::unfind(dfltMidiCtrlLabelIndex, i);
							if (cnm.size()) {
								sprintf(buf, "control %d", i);
								controllerMenu->AppendMenu(MF_STRING, ID_STREAMEDITORCONTEXT_DUP_CONTROLLER+i, buf);
							} else {
								controllerMenu->AppendMenu(MF_STRING, ID_STREAMEDITORCONTEXT_DUP_CONTROLLER+i, cnm.c_str());
							}
						}
					}
					break;
				}
				case TypedValue::S_PROG: {
					menu_built = true;
					CMenu	*setprogMenu = new CMenu;
					CMenu	*insprogMenu = new CMenu;
					CMenu	*bankMenu = new CMenu;
					CMenu	*subankMenu = new CMenu;
					setprogMenu->CreatePopupMenu();
					insprogMenu->CreatePopupMenu();
					bankMenu->CreatePopupMenu();
					subankMenu->CreatePopupMenu();

					for (i=0; i<128; i++) {
						sprintf(buf, "program %d", i);
						insprogMenu->AppendMenu(MF_STRING, ID_STREAMEDITORCONTEXT_ADD_PROGRAM+i, buf);
					}
					for (i=0; i<128; i++) {
						sprintf(buf, "program %d", i);
						setprogMenu->AppendMenu(MF_STRING, ID_STREAMEDITORCONTEXT_SET_PROGRAM+i, buf);
					}
					for (i=0; i<128; i++) {
						sprintf(buf, "bank %d", i);
						bankMenu->AppendMenu(MF_STRING, ID_STREAMEDITORCONTEXT_SET_BANK+i, buf);
					}
					for (i=0; i<128; i++) {
						sprintf(buf, "sub-bank %d", i);
						subankMenu->AppendMenu(MF_STRING, ID_STREAMEDITORCONTEXT_SET_SUBBANK+i, buf);
					}
					ctxtMenu->AppendMenu(MF_POPUP, (UINT) setprogMenu->m_hMenu, "set program item");
					ctxtMenu->AppendMenu(MF_POPUP, (UINT) bankMenu->m_hMenu, "set program bank");
					ctxtMenu->AppendMenu(MF_POPUP, (UINT) subankMenu->m_hMenu, "set program sub-bank");
					ctxtMenu->AppendMenu(MF_POPUP, (UINT) insprogMenu->m_hMenu, "insert program item");
					ctxtMenu->AppendMenu(MF_POPUP, ID_STREAMEDITORCONTEXT_DEL_PROGRAM, "delete program");

					break;
				}
			}
		} else if (iv->type == MFCEditorItemView::DISCRETE) {
			MFCStreamItemView	*siv = (MFCStreamItemView *)iv;
			if (siv->item) {
				switch (siv->item->type) {
					case TypedValue::S_NOTE: {
						menu_built = true;
						ctxtMenu->AppendMenu(MF_POPUP, ID_STREAMEDITORCONTEXT_DEL_NOTE, "delete note event");
						break;
					}
				}
			}
		} else if (iv->type == MFCEditorItemView::CLIP) {
			menu_built = true;
			ctxtMenu->AppendMenu(MF_POPUP, ID_EDITORCONTEXT_DEL_CLIP, "delete clip");
		} else if (iv->type == MFCEditorItemView::ENV) {
		} else {
		}
	}

	if (!menu_built) { // run with the default menu
		CMenu	*controllerMenu = new CMenu;
		controllerMenu->CreatePopupMenu();
		for (i=0; i<128; i++) {
			string cnm = qut::unfind(dfltMidiCtrlLabelIndex, (int) i);
			if (cnm.size()) {
				sprintf(buf, "control %d", i);
				controllerMenu->AppendMenu(MF_STRING, ID_STREAMEDITORCONTEXT_ADD_CONTROLLER+i, buf);
			} else {
				controllerMenu->AppendMenu(MF_STRING, ID_STREAMEDITORCONTEXT_ADD_CONTROLLER+i, cnm.c_str());
			}
		}
		CMenu	*progMenu = new CMenu;
		progMenu->CreatePopupMenu();
		for (i=0; i<128; i++) {
			sprintf(buf, "program %d", i);
			progMenu->AppendMenu(MF_STRING, ID_STREAMEDITORCONTEXT_ADD_PROGRAM+i, buf);
		}
		ctxtMenu->AppendMenu(MF_POPUP, (UINT) controllerMenu->m_hMenu, "Add controller");
		ctxtMenu->AppendMenu(MF_POPUP, (UINT) progMenu->m_hMenu, "Add program item");
		ctxtMenu->AppendMenu(MF_POPUP, ID_STREAMEDITORCONTEXT_ADD_BEND, "Add bend controller");
		ctxtMenu->AppendMenu(MF_POPUP, ID_STREAMEDITORCONTEXT_ADD_VALUE, "Add value item");
		ctxtMenu->AppendMenu(MF_POPUP, ID_STREAMEDITORCONTEXT_ADD_SYSC, "Add sysc item");
		ctxtMenu->AppendMenu(MF_POPUP, ID_STREAMEDITORCONTEXT_ADD_SYSX, "Add sysx item");
		ctxtMenu->AppendMenu(MF_POPUP, ID_EDITORCONTEXT_ADD_CLIP, "Add Clip");
	}

	short ret = ctxtMenu->TrackPopupMenuEx(
						TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RETURNCMD,
						popPt.x, popPt.y, this, nullptr);
	delete ctxtMenu;
	if (ret == 0) {	// cancel
		return;
	}

	if (ret >= ID_STREAMEDITORCONTEXT_ADD_CONTROLLER && ret <= ID_STREAMEDITORCONTEXT_ADD_CONTROLLER+127) {
		short ctl = ret-ID_STREAMEDITORCONTEXT_ADD_CONTROLLER;
		if (stream != nullptr) {
			StreamItem		*iv=nullptr;
			iv = new StreamCtrl(at_time, 0, ctl, 80);
			stream->InsertItem(iv);
			MFCEditorItemView	*added_item = AddStreamItemView(iv, &itPt, true);
		}
	} else if (ret >= ID_STREAMEDITORCONTEXT_DUP_CONTROLLER && ret <= ID_STREAMEDITORCONTEXT_DUP_CONTROLLER+127) {
		short ctl_to = ret-ID_STREAMEDITORCONTEXT_DUP_CONTROLLER;
		if (stream != nullptr && iv != nullptr) {
			MFCStreamItemListView	*siv = (MFCStreamItemListView *)iv;
			if (siv->listitemType == TypedValue::S_CTRL) {
				stream->DuplicateController(siv->listitemData1,ctl_to);
				CRect	sb = siv->BoundingBox();
				MFCEditorItemView	*added_item = RefreshControllerView(ctl_to, sb.bottom+5);
			}
		}
	} else if (ret >= ID_STREAMEDITORCONTEXT_ADD_PROGRAM && ret <= ID_STREAMEDITORCONTEXT_ADD_PROGRAM+127) {
		short prg = ret-ID_STREAMEDITORCONTEXT_ADD_PROGRAM;
		if (stream != nullptr) {
			StreamItem		*iv=nullptr;
			iv = new StreamProg(at_time, 0, prg, NON_PROG, NON_PROG);
			stream->InsertItem(iv);
			MFCEditorItemView	*added_item = AddStreamItemView(iv, &itPt, true);
		}
	} else if (ret >= ID_STREAMEDITORCONTEXT_SET_PROGRAM && ret <= ID_STREAMEDITORCONTEXT_SET_PROGRAM+127) {
		short prg = ret-ID_STREAMEDITORCONTEXT_SET_PROGRAM;
		if (stream != nullptr) {
			MFCStreamItemListView	*siv = (MFCStreamItemListView *)iv;
			StreamItem		*si= stream->FindItemAtTime(-1, at_time, TypedValue::S_PROG, -1, -1);
			StreamProg		*sip = (StreamProg *)si;
			if (si != nullptr) {
				siv->SetSubItemValue(si, prg, sip->prog.bank, sip->prog.subbank);
			}
		}
	} else if (ret >= ID_STREAMEDITORCONTEXT_SET_BANK && ret <= ID_STREAMEDITORCONTEXT_SET_BANK+127) {
		short prg = ret-ID_STREAMEDITORCONTEXT_SET_BANK;
		if (stream != nullptr) {
			MFCStreamItemListView	*siv = (MFCStreamItemListView *)iv;
			StreamItem		*si= stream->FindItemAtTime(-1, at_time, TypedValue::S_PROG, -1, -1);
			StreamProg		*sip = (StreamProg *)si;
			if (siv != nullptr && si != nullptr) {
				siv->SetSubItemValue(si, sip->prog.program, prg, sip->prog.subbank);
			}
		}
	} else if (ret >= ID_STREAMEDITORCONTEXT_SET_SUBBANK && ret <= ID_STREAMEDITORCONTEXT_SET_SUBBANK+127) {
		short prg = ret-ID_STREAMEDITORCONTEXT_SET_SUBBANK;
		if (stream != nullptr) {
			MFCStreamItemListView	*siv = (MFCStreamItemListView *)iv;
			StreamItem		*si= stream->FindItemAtTime(-1, at_time, TypedValue::S_PROG, -1, -1);
			StreamProg		*sip = (StreamProg *)si;
			if (siv != nullptr && si != nullptr) {
				siv->SetSubItemValue(si, sip->prog.program, sip->prog.bank, prg);
			}
		}

	} else if (ret == ID_STREAMEDITORCONTEXT_DEL_PROGRAM) {
		if (stream != nullptr) {
			MFCStreamItemListView	*siv = (MFCStreamItemListView *)iv;
			StreamItem		*si= stream->FindItemAtTime(-1, at_time, TypedValue::S_PROG, -1, -1);
			if (siv != nullptr && si != nullptr) {
				siv->DelSubItem(si);
			}
		}
	} else if (ret == ID_STREAMEDITORCONTEXT_DEL_BEND) {
		if (stream != nullptr) {
			MFCStreamItemListView	*siv = (MFCStreamItemListView *)iv;
			StreamItem		*si= stream->FindItemAtTime(-1, at_time, TypedValue::S_BEND, -1, -1);
			if (siv != nullptr && si != nullptr) {
				siv->DelSubItem(si);
			}
		}
	} else if (ret == ID_STREAMEDITORCONTEXT_DEL_CTRL) {
		if (stream != nullptr) {
			MFCStreamItemListView	*siv = (MFCStreamItemListView *)iv;
			StreamItem		*si= stream->FindItemAtTime(-1, at_time, TypedValue::S_CTRL, -1, siv->listitemData1);
			if (siv != nullptr && si != nullptr) {
				siv->DelSubItem(si);
			}
		}
	} else if (ret == ID_STREAMEDITORCONTEXT_DEL_VALUE) {
		;
	} else if (ret == ID_STREAMEDITORCONTEXT_DEL_SYSX) {
		;
	} else if (ret == ID_STREAMEDITORCONTEXT_DEL_SYSC) {
		;
	} else if (ret == ID_STREAMEDITORCONTEXT_ADD_VALUE) {
		;
	} else if (ret == ID_STREAMEDITORCONTEXT_ADD_SYSX) {
		;
	} else if (ret == ID_STREAMEDITORCONTEXT_ADD_SYSC) {
		;
	} else if (ret == ID_STREAMEDITORCONTEXT_ADD_BEND) {
		if (stream != nullptr) {
			StreamItem		*si=nullptr;
			si = new StreamBend(at_time, 0, 0);
			stream->InsertItem(si);
			MFCEditorItemView	*added_item = AddStreamItemView(si, &itPt, true);
		}
	} else if (ret == ID_STREAMEDITORCONTEXT_DEL_NOTE) {
		if (stream != nullptr && iv != nullptr) {
			DelItemView(iv);
		}
	} else if (ret == ID_STREAMEDITORCONTEXT_CLEAR_CONTROLLER) {
		if (stream != nullptr && iv != nullptr) {
			DelItemView(iv);
		}
	} else if (ret == ID_EDITORCONTEXT_ADD_CLIP) {
		if (take != nullptr) {
			StabEnt	*chSym = parent->Symbol();
			if (chSym) {
				Voice	*v;
				if ((v = chSym->VoiceValue()) != nullptr) {
					Time	dur_time;
					dur_time.Set(1,0,0,at_time.metric);
					string nmbuf = glob.makeUniqueName(chSym, "clip", 1);
					Clip	*c = v->AddClip(nmbuf, take, at_time, dur_time, true);
//					MFCEditorItemView	*added_item = AddClipItemView(c);
				}
			}
		}
	} else if (ret == ID_EDITORCONTEXT_DEL_CLIP) {
		StabEnt	*chSym = parent->Symbol();
		if (iv != nullptr && chSym) {
			Voice	*v;
			MFCClipItemView	*civ = (MFCClipItemView*)iv;
			if ((v = chSym->VoiceValue()) != nullptr && civ->item != nullptr) {
				Clip	*c = civ->item;
				fprintf(stderr, "remove clip %x\n", c);
//				DelClipItemView(iv);
				v->RemoveClip(c, true);
			}
		}
	}
}


bool
MFCStreamDataEditor::CursorCheck(CPoint point, bool isCtl)
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
			if (originalCursor != nullptr) {
				::SetCursor(originalCursor);
				originalCursor = nullptr;
			}
		}
	}
	return true;
}

afx_msg void
MFCStreamDataEditor::OnMouseMove(
			UINT nFlags,
			CPoint point 
		)
{
	point += GetScrollPosition();

	Time	at_time;
	short	at_note;
	at_note = Pix2Note(point.y);

	CursorCheck(point, (nFlags&MK_CONTROL)!=0);

	switch (mouse_action) {
		case QUA_MOUSE_ACTION_MOVE_INSTANCE: {
			if (mouse_item) {
				switch (mouse_item->type) {
					case MFCEditorItemView::DISCRETE: {
						break;
					}
					case MFCEditorItemView::LIST: {
						MFCStreamItemListView	*siv = (MFCStreamItemListView *)mouse_item;
						siv->MoveToY(point.y);
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
						MFCStreamItemView	*siv = (MFCStreamItemView *)mouse_item;
						StreamItem	*item = siv->item;
						if (item) {
							Pix2Time(point.x-mouse_move_item_offset.x, at_time);
							Time	last_time;
							Pix2Time(last_mouse_point.x-mouse_move_item_offset.x, last_time);
							short	last_note = Pix2Note(last_mouse_point.y);
							if (last_time != at_time || last_note != at_note) {
								SetStartTimeMidiParams(mouse_item, at_time, at_note);
							}
						}
						break;
					}
					case MFCEditorItemView::LIST: {
						MFCStreamItemListView	*siv = (MFCStreamItemListView *)mouse_item;
						if (mouse_sub_item) {
							float v = siv->Pix2Val(point.y);
							siv->SetSubItemValue(mouse_sub_item, v);
						}
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
						MFCStreamItemListView	*siv = (MFCStreamItemListView *)mouse_item;
						siv->ResizeToY(point.y - siv->BoundingBox().top);
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
						MFCStreamItemView	*siv = (MFCStreamItemView *)mouse_item;
						StreamItem	*item = siv->item;
						if (item) {
							Pix2Time(point.x-mouse_move_item_offset.x, at_time);
							Time	last_time;
							Pix2Time(last_mouse_point.x-mouse_move_item_offset.x, last_time);
							if (last_time != at_time) {
								SetItemDuration(mouse_item, at_time.ticks);
							}
						}
						break;
					}
					case MFCEditorItemView::LIST: {
						MFCStreamItemListView	*siv = (MFCStreamItemListView *)mouse_item;
						Time	to_time;
						Pix2Time(point.x, to_time);
						siv->SetSubItemTime(mouse_sub_item, to_time);
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
						MFCStreamItemView	*siv = (MFCStreamItemView *)mouse_item;
						StreamItem	*item = siv->item;
						break;
					}
					case MFCEditorItemView::LIST: {
						MFCStreamItemListView	*siv = (MFCStreamItemListView *)mouse_item;
						Pix2Time(point.x, at_time);
						StreamItem	*it = nullptr;	// item strictly on pixel time
						StreamItem	*pt = nullptr; // item strictly before pixel time
						float		val = siv->Pix2Val(point.y);
						switch (siv->listitemType) {
							case TypedValue::S_CTRL: {
								if (stream) {
									short	actualval = 127*val;
									pt=stream->FindItemAtTime(-2,
														at_time,
										TypedValue::S_CTRL,
														-1,
														siv->listitemData1);
									it=stream->FindItemAtTime(0,
														at_time,
										TypedValue::S_CTRL,
														-1,
														siv->listitemData1,
														pt);
									if (pt==nullptr || ((StreamCtrl*)pt)->ctrl.amount != actualval) {
										if (it==nullptr) {
											StreamItem	*ft = nullptr;
											if (pt != nullptr) {
												ft = pt->Subsequent(TypedValue::S_CTRL, -1, siv->listitemData1);
											}
											if (ft != nullptr && ((StreamCtrl *)ft)->ctrl.amount == actualval) {
												siv->SetSubItemTime(ft, at_time);
											} else {
												it = new StreamCtrl(at_time, 0, siv->listitemData1, 0);
												stream->InsertItem(it);
												siv->SetSubItemValue(it, val);
											}
										} else {
											siv->SetSubItemValue(it, val);
										}
									} else { // pt != nullptr && pt->amount == the pixelval
										if (it != nullptr) {
											siv->DelSubItem(it);
											it = nullptr;
										}
									}
								}
								break;
							}
							case TypedValue::S_BEND: {
								if (stream) {
									short	actualval = 127*val;
									pt=stream->FindItemAtTime(-2, at_time, TypedValue::S_BEND, -1, siv->listitemData1);
									it=stream->FindItemAtTime(0, at_time, TypedValue::S_BEND, -1, siv->listitemData1, pt);
									if (pt==nullptr || ((StreamCtrl*)pt)->ctrl.amount != actualval) {
										if (it==nullptr) {
											StreamItem	*ft = nullptr;
											if (pt != nullptr) {
												ft = pt->Subsequent(TypedValue::S_CTRL, -1, siv->listitemData1);
											}
											if (ft != nullptr && ((StreamBend *)ft)->bend.bend == actualval) {
												siv->SetSubItemTime(ft, at_time);
											} else {
												it = new StreamBend(at_time, 0, 0);
												stream->InsertItem(it);
												siv->SetSubItemValue(it, val);
											}
										} else {
											siv->SetSubItemValue(it, val);
										}
									} else { // pt != nullptr && pt->amount == the pixelval
										if (it != nullptr) {
											siv->DelSubItem(it);
											it = nullptr;
										}
									}
								}
								break;
							}
						}
					}
				}
			}
			break;
		}
	}

	last_mouse_point = point;
}

afx_msg BOOL
MFCStreamDataEditor::OnMouseWheel(
			UINT nFlags,
			short zDelta,
			CPoint point 
		)
{
	point += GetScrollPosition();
	return MFCDataEditor::OnMouseWheel(nFlags, zDelta, point);
}


afx_msg void
MFCStreamDataEditor::OnKeyDown(
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
				if (iv == nullptr) {
					break;
				}
				if (iv->selected) {
					switch (iv->type) {
						case MFCEditorItemView::DISCRETE: {
							MFCStreamItemView	*siv = (MFCStreamItemView *)iv;
							DelItemView(iv);
							break;
						}
						case MFCEditorItemView::LIST: {
							MFCStreamItemListView	*siv = (MFCStreamItemListView *)iv;
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
MFCStreamDataEditor::OnKeyUp(
			UINT nChar,
			UINT nRepCnt,
			UINT nFlags 
		)
{
	CursorCheck(CPoint(bounds.left+1,bounds.top+1), false);
	MFCDataEditor::OnKeyUp(nChar, nRepCnt, nFlags);
}

long
MFCStreamDataEditor::NoteHeight()
{
	return 4;
}

long
MFCStreamDataEditor::Note2Pix(long n)
{
	return (128-n)*NoteHeight();
}

long
MFCStreamDataEditor::Pix2Note(long px)
{
	return 128-(px/NoteHeight());
}

// this gets called from the context menu, and the key down routines
// i think we're well behaved round here, but we should just be really
// bloody careful about complications this might cause with drawing routines....
bool
MFCStreamDataEditor::DelItemView(MFCEditorItemView *iv)
{
	switch (iv->type) {
		case MFCEditorItemView::DISCRETE: {
			MFCStreamItemView	*siv = (MFCStreamItemView *)iv;
			if (stream) {
				stream->DeleteItem(siv->item);
			}
			break;
		}
		case MFCEditorItemView::LIST: {
			MFCStreamItemListView	*siv = (MFCStreamItemListView *)iv;
			if (stream) {
				stream->DeleteItemsMatching(siv->listitemType, -1, siv->listitemData1);
			}
			break;
		}
	}
	CRect	updr = bounds;
	RemItemR(iv);
	delete iv;
	InvalidateRect(&bounds);
	return true;
}

MFCEditorItemView *
MFCStreamDataEditor::RefreshControllerView(ctrl_t ct, UINT py)
{
	for (short i=0; i<NItemR(); i++) {
		MFCEditorItemView *iv = ItemR(i);
		if (iv->type == MFCEditorItemView::LIST) {
			MFCStreamItemListView *siv = (MFCStreamItemListView*)iv;
			if (siv->listitemType == TypedValue::S_CTRL && siv->listitemData1 == ct) {
				CRect	udr = iv->BoundingBox();
				InvalidateRect(&udr);
                return iv;
			}
		}
	}
	CPoint pt;
	pt.x = 0;
	pt.y = py;
	MFCEditorItemView *iv = new MFCStreamItemListView(this, &pt, TypedValue::S_CTRL, ct);
	if (iv) {
		AddItemR(iv);
		iv->Redraw();
		UpdateWindow();
	}
	return iv;
}



/////////////////////////////////////////////
// Stream Item (discrete item, like notes)
/////////////////////////////////////////////
MFCStreamItemView::MFCStreamItemView(MFCStreamDataEditor *ed, StreamItem *it):
	MFCEditorItemView(ed, DISCRETE)
{
	item = it;
	CalculateBounds();
}

MFCStreamItemView::~MFCStreamItemView()
{
}

void
MFCStreamItemView::CalculateBounds()
{
	if (item && editor) {
		MFCStreamDataEditor	*sed = (MFCStreamDataEditor *)editor;

		Time	arrTime = item->time&editor->displayMetric;
		long	pixl = editor->Time2Pix(arrTime);
		long	pixd;

		Time	arrDur = item->Duration();
		pixd = editor->Time2Pix(arrDur);
		if (pixd < 2) {
			pixd = 2;
		}
		long	pixn = 0;
		if (item->type == TypedValue::S_NOTE) {
			StreamNote	*n = (StreamNote *)item;
			pixn = sed->Note2Pix(n->note.pitch);
		}
		fprintf(stderr, "calculate bounds %x %x %d %d %d\n", item, editor, pixl, pixd, arrDur.ticks);
		bounds.Set(pixl, pixn, pixl+pixd, pixn+sed->NoteHeight());
	}
}

#ifdef QUA_V_GDI_PLUS
void
MFCStreamItemView::Draw(Graphics *dc, CRect *clipBox)
{
// !!!??? need to clip properly for short instances with long names
	Pen			blackPen(Color(250, 0, 0, 0), 1);
	Pen			redPen(Color(250, 160, 10, 10), 1);
	SolidBrush	blueBrush(Color(100, 10, 10, 160));
	SolidBrush	blackBrush(Color(100, 0, 0, 0));

//	fprintf(stderr, "drawing instance view %d\n", bounds.right-bounds.left);
	dc->FillRectangle(&blueBrush, bounds.left, bounds.top, bounds.right-bounds.left, bounds.bottom-bounds.top);
	dc->DrawRectangle(selected?&redPen:&blackPen, bounds.left, bounds.top, bounds.right-bounds.left, bounds.bottom-bounds.top);
}

#else
void
MFCStreamItemView::Draw(CDC *dc, CRect *clipBox)
{
	;
}
#endif


bool
MFCStreamItemView::Represents(void *it)
{
	if (it == nullptr)
		return false;
	if (item == (StreamItem *)it) {
		return true;
	}
	return false;
}

short
MFCStreamItemView::HitTest(CPoint &p, UINT flgs, void *&clkit)
{
	clkit = nullptr;
	return bounds.Contains(p)?1:0;
}

/////////////////////////////////////////////
// A stream list item. a single object that
// does drawing and hot spots for many events
// of a particular kind e.g. controllers....
/////////////////////////////////////////////

MFCStreamItemListView::MFCStreamItemListView(MFCStreamDataEditor *ed, CPoint *p, short i_type, short d1):
	MFCEditorItemView(ed, LIST)
{
	listitemType = i_type;
	listitemData1 = d1;
	CalculateBounds();
	if (p) {
		bounds.MoveToY(p->y);
	}
}

MFCStreamItemListView::~MFCStreamItemListView()
{
	;
}

bool
MFCStreamItemListView::Represents(void *it)
{
	StreamItem	*sit = (StreamItem *)it;
	if (sit->type == TypedValue::S_BEND || sit->type == TypedValue::S_PROG) {
		if (sit->type == listitemType) {
			return true;
		}
	} else if (sit->type == TypedValue::S_CTRL) {
		if (listitemType == TypedValue::S_CTRL && ((StreamCtrl *)it)->ctrl.controller == listitemData1) {
			return true;
		}
	}
	return false;
}


short
MFCStreamItemListView::HitTest(CPoint &p, UINT flgs, void *&clkit)
{
//	fprintf(stderr, "hit test %d %d\n", p.y, bounds.bottom, bounds.top);
	clkit = nullptr;
	bool	look_interior = ((flgs & MK_CONTROL) != 0);
	if (!look_interior) {
		if (Abs(p.y - bounds.top) < 2) {
			return 1;
		}
		if (Abs(p.y - bounds.bottom) < 2) {
			return 2;
		}
	}
	MFCStreamDataEditor	*sed = (MFCStreamDataEditor *)editor;
	if (bounds.Contains(p)) {
		if (sed->stream) {	// may need to be careful here with deleting, perhaps lock???
			StreamItem	*si = sed->stream->head;
			StreamItem	*last = nullptr;
			while (si != nullptr) {
				if (listitemType == TypedValue::S_PROG) {
					long ppxt = editor->Time2Pix(si->time);
					if (Abs(ppxt-p.x) < 2) {
						clkit = si;
						return 4;
					}
					last = si;
				} else if (listitemType == TypedValue::S_BEND) {
					long ppxt = editor->Time2Pix(si->time);
					long ppyt = Val2Pix((StreamBend *)si);
					if (Abs(ppxt-p.x) < 2) {
						clkit = si;
						return 4;
					}
					if (last) {
						long pplstxt = editor->Time2Pix(last->time);
						long pplstyt = Val2Pix((StreamBend *)last);
						if (p.x >= pplstxt && p.x < ppxt && Abs(pplstyt-p.y) < 2) {
							clkit = last;
							return 3;
						}
					}
					last = si;
				} else if (listitemType == TypedValue::S_CTRL) {
					StreamCtrl	*sc = (StreamCtrl *)si;
					if (sc->ctrl.controller == listitemData1) {
						long ppxt = editor->Time2Pix(si->time);
						long ppyt = Val2Pix(sc);
						if (Abs(ppxt-p.x) < 2) {
							clkit = si;
							return 4;
						}
						if (last) {
							long pplstxt = editor->Time2Pix(last->time);
							long pplstyt = Val2Pix((StreamCtrl *)last);
							if (p.x >= pplstxt && p.x < ppxt && Abs(pplstyt-p.y) < 2) {
								clkit = last;
								return 3;
							}
						}
						last = si;
					}
				}
				si = si->next;
			}
			if (last) {
				if (listitemType == TypedValue::S_CTRL) {
					long pplstxt = editor->Time2Pix(last->time);
					long pplstyt = Val2Pix((StreamCtrl *)last);
					if (p.x >= pplstxt && Abs(pplstyt-p.y) < 2) {
						clkit = last;
						return 3;
					}
				} else if (listitemType == TypedValue::S_BEND) {
					long pplstxt = editor->Time2Pix(last->time);
					long pplstyt = Val2Pix((StreamCtrl *)last);
					if (p.x >= pplstxt && Abs(pplstyt-p.y) < 2) {
						clkit = last;
						return 3;
					}
				}
			}
		}
		if (look_interior) {
			return 5;
		}
	}
	return 0;
}

void
MFCStreamItemListView::MoveToY(long newy)
{
	if (newy == bounds.top) {
		return;
	}
	if (editor) {
		CPoint	scr = editor->GetScrollPosition();
		CRect	oldr = bounds - scr;
		oldr.right++;
		oldr.bottom++;
		bounds.MoveToY(newy);
		editor->InvalidateRect(&oldr);
// redraw
		CRect	updr = bounds;
		updr = bounds - scr;
		updr.bottom ++;
		updr.right ++;
//		editor->RedrawWindow(&updr);
		editor->InvalidateRect(&updr);
	}
}

void
MFCStreamItemListView::ResizeToY(long newh)
{
	if (newh == (bounds.bottom-bounds.top)) {
		return;
	}
	if (editor) {
		CPoint	scr = editor->GetScrollPosition();
		CRect	oldr = bounds - scr;
		oldr.right++;
		oldr.bottom++;
		bounds.bottom = bounds.top+newh;
		editor->InvalidateRect(&oldr);
// redraw
		CRect	updr = bounds;
		updr = bounds - scr;
		updr.bottom ++;
		updr.right ++;
//		editor->RedrawWindow(&updr);
		editor->InvalidateRect(&updr);
	}
}

bool
MFCStreamItemListView::SetSubItemTime(StreamItem *si, Time &t)
{
	if (si == nullptr) {
		return false;
	}
	if (t > editor->lastScheduledEvent) {
		editor->lastScheduledEvent = t;
	}
	long	d1 = -1;
	Time	tot = t;
	if (si->type != listitemType) {
		return false;
	}
	if (si->type == TypedValue::S_CTRL) {
		d1 = ((StreamCtrl *)si)->ctrl.controller;
		if (d1 != listitemData1) {
			return false;
		}
	}
	if (si->time == t) {
		return true;
	}
// dunno that we need to worry about this sort of bounds checking
//	StreamItem	*sxi = si->Subsequent(si->type, -1, d1);
//	if (sxi != nullptr) {
//		if (tot > sxi->time) {
//			tot = sxi->time;
//			tot.ticks--;
//		}
//	}
	MFCStreamDataEditor	*sed = (MFCStreamDataEditor *)editor;
	if (sed->stream) {
		long	pix1 = sed->Time2Pix(si->time);
		long	pix2 = sed->Time2Pix(tot);
		CRect	updr = bounds;
		updr.left = min(pix1, pix2)-1;
		updr.right = max(pix1, pix2)+1;
		updr.bottom++;
		updr.top--;
		if (si->type == TypedValue::S_PROG) {
			updr.right += 40; // !! by rights the pixel width of the label + 1
		}
		sed->stream->ModifyItemTime(si, tot);
		editor->InvalidateRect(&updr);
	}
	return true;
}

bool
MFCStreamItemListView::SetSubItemValue(StreamItem *si, float v)
{
	if (si->type != listitemType) {
		return false;
	}
	MFCStreamDataEditor	*sed = (MFCStreamDataEditor *)editor;
	if (si->type == TypedValue::S_CTRL) {
		StreamCtrl	*sic = ((StreamCtrl *)si);
		if (sic->ctrl.controller != listitemData1) {
			return false;
		}
		CRect	updr = bounds;
		long pixo = Val2Pix(sic);
		if (v > 1) v = 1;
		else if (v < 0) v = 0;
		sic->ctrl.amount = 127*v;
		long pixn = Val2Pix(sic);
		if (pixo == pixn) {
			return true;
		}
		StreamItem *sx = si->Subsequent(TypedValue::S_CTRL, -1, listitemData1);
		if (sx) {
			updr.right = editor->Time2Pix(sx->time)+1;
		}
		updr.left = editor->Time2Pix(si->time)-1;
//		updr.top = min(pixo, pixn)-2;
//		updr.bottom = max(pixo, pixn)+2;
		CPoint	scr = editor->GetScrollPosition();
		updr = updr - scr;
		editor->InvalidateRect(&updr);
		return true;
	} else if (si->type == TypedValue::S_BEND) {
		StreamBend	*sib = ((StreamBend *)si);
		CRect	updr = bounds;
		long pixo = Val2Pix(sib);
		if (v > 1) v = 1;
		else if (v < 0) v = 0;
		sib->bend.bend = ((1<<14)-1)*v;
		long pixn = Val2Pix(sib);
		if (pixo == pixn) {
			return true;
		}
		StreamItem *sx = si->Subsequent(TypedValue::S_BEND, -1, listitemData1);
		if (sx) {
			updr.right = editor->Time2Pix(sx->time);
		}
		updr.left = editor->Time2Pix(si->time);
//		updr.top = min(pixo, pixn)-2;
//		updr.bottom = max(pixo, pixn)+2;
		CPoint	scr = editor->GetScrollPosition();
		updr = updr - scr;
		editor->InvalidateRect(&updr);
		return true;
	} else if (si->type == TypedValue::S_PROG) {
		StreamProg	*sip = ((StreamProg *)si);
	}
	return false;
}

bool
MFCStreamItemListView::SetSubItemValue(StreamItem *si, int8 d1, int8 d2, int8 d3)
{
	if (si->type != listitemType) {
		return false;
	}
	MFCStreamDataEditor	*sed = (MFCStreamDataEditor *)editor;
	if (si->type == TypedValue::S_CTRL) {
		StreamCtrl	*sic = ((StreamCtrl *)si);
		if (sic->ctrl.controller != listitemData1) {
			return false;
		}
		return true;
	} else if (si->type == TypedValue::S_BEND) {
		StreamBend	*sib = ((StreamBend *)si);
	} else if (si->type == TypedValue::S_PROG) {
		StreamProg	*sip = ((StreamProg *)si);
		long pixl = sed->Time2Pix(si->time);
		CRect	updr = bounds;
		updr.left = pixl;
		updr.right = pixl+1;
		if (d1 != NON_PROG) {// should be string length in pixels!!!!
			updr.right += 20; 
		}
		if (d2 != NON_PROG) {// should be string length in pixels!!!!
			updr.right += 20; 
		}
		if (d3 != NON_PROG) {// should be string length in pixels!!!!
			updr.right += 20; 
		}
		sip->prog.program = d1;
		sip->prog.bank = d2;
		sip->prog.subbank = d3;
		editor->InvalidateRect(&updr);
		return true;
	}
	return false;
}

bool
MFCStreamItemListView::DelSubItem(StreamItem *si)
{
	if (si == nullptr) {
		return false;
	}
	if (si->type != listitemType) {
		return false;
	}
	MFCStreamDataEditor	*sed = (MFCStreamDataEditor *)editor;
	if (si->type == TypedValue::S_CTRL) {
		StreamCtrl	*sic = ((StreamCtrl *)si);
		if (sic->ctrl.controller != listitemData1) {
			return false;
		}
		if (sed->stream) {
			CRect	updr = bounds;
			long pixl = sed->Time2Pix(si->time);
			updr.left = pixl;
			StreamItem	*subi = si->Subsequent(TypedValue::S_CTRL, -1, sic->ctrl.controller);
			if (subi) {
//				updr.right = sed->Time2Pix(subi->time) + 1;
			}
			sed->stream->DeleteItem(si);
			editor->InvalidateRect(&updr);
			return true;
		}
	} else if (si->type == TypedValue::S_BEND) {
		StreamBend	*sib = ((StreamBend *)si);
		if (sed->stream) {
			CRect	updr = bounds;
			long pixl = sed->Time2Pix(si->time);
			updr.left = pixl;
			StreamItem	*subi = si->Subsequent(TypedValue::S_BEND, -1, -1);
			if (subi) {
//				updr.right = sed->Time2Pix(subi->time) + 1;
			}
			sed->stream->DeleteItem(si);
			editor->InvalidateRect(&updr);
			return true;
		}
	} else if (si->type == TypedValue::S_PROG) {
		CRect	updr = bounds;
		StreamProg	*sip = ((StreamProg *)si);
		long pixl = sed->Time2Pix(si->time);
		updr.left = pixl;
		updr.right = pixl + 1;
		if (sip->prog.bank != NON_PROG) {// should be string length in pixels!!!!
			updr.right += 20; 
		}
		if (sip->prog.subbank != NON_PROG) {// should be string length in pixels!!!!
			updr.right += 20; 
		}
		if (sip->prog.program != NON_PROG) {// should be string length in pixels!!!!
			updr.right += 20; 
		}
		if (sed->stream) {
			sed->stream->DeleteItem(si);
			editor->InvalidateRect(&updr);
		}
	}
	return false;
}

long
MFCStreamItemListView::Val2Pix(StreamBend *sb)
{
	long pxv = (bounds.bottom - bounds.top) * ((float)(sb->bend.bend) / (1<<14));
	return bounds.bottom - pxv;
}

long
MFCStreamItemListView::Val2Pix(StreamCtrl *sc)
{
	long pxv = (bounds.bottom - bounds.top) * ((float)(sc->ctrl.amount) / 128);
	return bounds.bottom - pxv;
}

float
MFCStreamItemListView::Pix2Val(long px)
{
	if (px >= bounds.bottom)
		return 0.0;
	if (px <= bounds.top)
		return 1.0;
	return ((float)(bounds.bottom - px))/(bounds.bottom - bounds.top);
}


#ifdef QUA_V_GDI_PLUS
void
MFCStreamItemListView::Draw(Graphics *dc, CRect *clipBox)
{
// !!!??? need to clip properly for short instances with long names
	Pen			blackPen(Color(250, 0, 0, 0), 1);
	Pen			redPen(Color(250, 160, 10, 10), 1);
	Pen			bluePen(Color(250, 10, 10, 160), 1);
	Pen			greenPen(Color(250, 10,100,10), 1);
	SolidBrush	blueBrush(Color(100, 10, 10, 160));
	SolidBrush	greenBrush(Color(200, 10,100,10));
	SolidBrush	softgreenBrush(Color(50, 60,160,60));
	SolidBrush	blackBrush(Color(200, 0, 0, 0));
	SolidBrush	hardblackBrush(Color(250, 0, 0, 0));

//	fprintf(stderr, "drawing instance view %d\n", bounds.right-bounds.left);
	dc->DrawRectangle(selected?&redPen:&bluePen, bounds.left, bounds.top, bounds.right-bounds.left, bounds.bottom-bounds.top);
	Font	labelFont(L"Arial", 8.0, FontStyleRegular, UnitPoint, nullptr);

	MFCStreamDataEditor	*sed = (MFCStreamDataEditor *)editor;
	wstring	nm;
	if (listitemType == TypedValue::S_PROG) {
		nm = L"program";
	} else if (listitemType == TypedValue::S_BEND) {
		nm = L"bend";
	} else if (listitemType == TypedValue::S_CTRL) {
		nm = L"controller " + to_wstring(listitemData1); 
	} else {
		nm = L"shape of pear";
	}

	PointF	p(bounds.left+1, bounds.top);
	do {
		dc->DrawString(nm.c_str(), -1, &labelFont, p, &blackBrush);
		p.X += editor->bounds.right;
	} while (p.X < bounds.right - 100);

	if (sed->stream) {	// may need to be careful here with deleting, perhaps lock???
		StreamItem	*p = sed->stream->head;
		StreamItem	*last = nullptr;
		while (p != nullptr) {
			if (p->type == listitemType) {
				if (listitemType == TypedValue::S_CTRL) {
					StreamCtrl	*pc = (StreamCtrl *)p;
					if (pc->ctrl.controller == listitemData1) {
						long	pixy = Val2Pix(pc);
						long	pixx = editor->Time2Pix(p->time);
						if (last != nullptr) {
							long	pixly = Val2Pix((StreamCtrl *)last);
							long	pixlst = editor->Time2Pix(last->time);
							dc->FillRectangle(&softgreenBrush,
									pixlst+1, pixly,
									pixx-pixlst, bounds.bottom-pixly);
							dc->DrawLine(
									&greenPen,
									pixlst, pixly,
									pixx, pixly);
							if (pixly < pixy) {
								pixy = pixly;
							}
						}
						dc->DrawLine(
								&greenPen,
								pixx, pixy,
								pixx, bounds.bottom);
						last = p;
					}
				} else if (listitemType == TypedValue::S_BEND) {
					long	pixx = editor->Time2Pix(p->time);
					long	pixy = Val2Pix((StreamBend *)p);
					if (last != nullptr) {
						long	pixly = Val2Pix((StreamBend *)last);
						long	pixlst = editor->Time2Pix(last->time);
						dc->FillRectangle(&softgreenBrush,
								pixlst+1, pixly,
								pixx-pixlst, bounds.bottom-pixly);
						dc->DrawLine(
								&greenPen,
								pixlst, pixly,
								pixx, pixly);
						if (pixly < pixy) {
							pixy = pixly;
						}
					}
					dc->DrawLine(
							&greenPen,
							pixx, pixy,
							pixx, bounds.bottom);
					last = p;
				} else if (listitemType == TypedValue::S_PROG) {
					StreamProg	*progp = (StreamProg *)p;
					long	pixx = editor->Time2Pix(p->time);
					if (last != nullptr) {
						long	pixlst = editor->Time2Pix(last->time);
						dc->FillRectangle(&softgreenBrush,
								pixlst+1, bounds.top,
								pixx-pixlst, bounds.bottom-bounds.top);
					}
					dc->DrawLine(
							&greenPen,
							pixx, bounds.top,
							pixx, bounds.bottom);
					PointF	txtpt(pixx+1, bounds.bottom-12);
					if (progp->prog.bank == NON_PROG && progp->prog.subbank == NON_PROG) {
						nm = to_wstring(progp->prog.program);
					} else if (progp->prog.bank == NON_PROG) {
						nm = to_wstring(progp->prog.subbank) + L"/"+to_wstring(progp->prog.program);
					} else if (progp->prog.subbank == NON_PROG) {
						nm = to_wstring(progp->prog.bank) + L"/" + to_wstring(progp->prog.program);
					} else {
						nm =  to_wstring(progp->prog.bank) + L"/"+ to_wstring(progp->prog.subbank)+ L"/" + to_wstring(progp->prog.program);
					}
					dc->DrawString(nm.c_str(), -1, &labelFont, txtpt, &hardblackBrush);
					last = p;
				}
			}
			p = p->next;
		}
		if (last != nullptr) {
			if (listitemType == TypedValue::S_PROG) {
				long	pixlst = editor->Time2Pix(last->time);
				dc->FillRectangle(&softgreenBrush,
						pixlst+1, bounds.top,
						bounds.right-pixlst-1, bounds.bottom-bounds.top);
			} else if (listitemType == TypedValue::S_CTRL) {
				long	pixy = Val2Pix((StreamCtrl *)last);
				long	pixlst = editor->Time2Pix(last->time);
				dc->FillRectangle(&softgreenBrush,
						pixlst+1, pixy,
						bounds.right-pixlst, bounds.bottom-pixy);
				dc->DrawLine(
						&greenPen,
						pixlst, pixy,
						bounds.right, pixy);
			} else if (listitemType == TypedValue::S_BEND) {
				long	pixy = Val2Pix((StreamBend *)last);
				long	pixlst = editor->Time2Pix(last->time);
				dc->FillRectangle(&softgreenBrush,
						pixlst+1, pixy,
						bounds.right-pixlst, bounds.bottom-pixy);
				dc->DrawLine(
						&greenPen,
						pixlst, pixy,
						bounds.right, pixy);
			}
		}
	}
}

#else
void
MFCStreamItemListView::Draw(CDC *dc, CRect *clipBox)
{
	;
}
#endif

void
MFCStreamItemListView::CalculateBounds()
{
	if (editor) {
		long	pixh = 50;
		if (listitemType == TypedValue::S_PROG) {
			pixh = 25;
		} else {
			pixh = 50;
		}
		long	pixn = 0;
		bounds.Set(0, pixn, 1000000, pixn+pixh);
	}
}

/////////////////////////////////////////////
// A keyboard like y scale
/////////////////////////////////////////////

IMPLEMENT_DYNCREATE(MFCStreamEditorYScale, CWnd)

BEGIN_MESSAGE_MAP(MFCStreamEditorYScale, CWnd)
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_SIZING()
	ON_WM_CREATE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

MFCStreamEditorYScale::MFCStreamEditorYScale()
{
	editor = nullptr;
}

MFCStreamEditorYScale::~MFCStreamEditorYScale()
{
	;
}

BOOL
MFCStreamEditorYScale::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE; // don't draw background
}


void
MFCStreamEditorYScale::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
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
			tr.bottom = editor->Note2Pix(i);
			tr.top = editor->Note2Pix(i+1);
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

	return;
}


bool
MFCStreamEditorYScale::CreateYScale(CRect &r, CWnd *w, UINT id, MFCStreamDataEditor *ed)
{
	keylen = r.right-r.left;
	editor = ed;
	if (!Create(_T("STATIC"), "piano", WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS/*|TBS_TOOLTIPS*/, r, w, id)) {
		return false;
	}
	SetDlgCtrlID(id);
//	CToolTipCtrl	*t=GetToolTips();

	return true;
}


void
MFCStreamEditorYScale::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	bounds.right = cx;
	bounds.bottom = cy;
}

void
MFCStreamEditorYScale::OnSizing( UINT which, LPRECT r)
{
	CWnd::OnSizing(which, r);
}

afx_msg void
MFCStreamEditorYScale::OnMove(int x, int y)
{
	CWnd::OnMove(x, y);
}

int
MFCStreamEditorYScale::OnCreate(LPCREATESTRUCT lpCreateStruct )
{
	bounds.left = 0;
	bounds.top = 0;
	bounds.right = lpCreateStruct->cx;
	bounds.bottom = lpCreateStruct->cy;
	return CWnd::OnCreate(lpCreateStruct);
}
