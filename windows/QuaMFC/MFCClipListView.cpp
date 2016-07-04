// MFCClipListView.cpp : implementation file
//
//#define _AFXDLL
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE
#include "stdafx.h"

#include "QuaMFC.h"
#include "MFCClipListView.h"
#include "MFCObjectView.h"
#include "MFCDataEditor.h"

#include "Sym.h"
#include "Clip.h"
#include "Voice.h"
#include "Sample.h"
#include "Metric.h"

#include <algorithm>
// MFCClipListView

IMPLEMENT_DYNCREATE(MFCClipListView, CTreeView)

#define QCI_SYMBOL_LPARAM	1000

MFCClipListView::MFCClipListView()
{
}

MFCClipListView::~MFCClipListView()
{
}

BEGIN_MESSAGE_MAP(MFCClipListView, CTreeView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()

	ON_COMMAND(ID_SAMPLETAKE_EDIT, OnPopupEditTake)
	ON_COMMAND(ID_SAMPLETAKE_DELETE, OnPopupDeleteTake)
	ON_COMMAND(ID_SAMPLETAKE_CREATE, OnPopupLoadSample)
	ON_COMMAND(ID_STREAMTAKE_EDIT, OnPopupEditTake)
	ON_COMMAND(ID_STREAMTAKE_DELETE, OnPopupDeleteTake)
	ON_COMMAND(ID_STREAMTAKE_CREATE, OnPopupCreateStream)
	ON_COMMAND(ID_CLIP_EDIT, OnPopupEditClip)
	ON_COMMAND(ID_CLIP_DELETE, OnPopupDeleteClip)
	ON_COMMAND(ID_CLIP_CREATE, OnPopupCreateClip)

{ WM_NOTIFY + WM_REFLECT_BASE, (WORD)(int)TVN_KEYDOWN, 0, 0, AfxSigNotify_v, (AFX_PMSG) (static_cast<void (AFX_MSG_CALL CCmdTarget::*)(NMHDR*, LRESULT*) > (OnKeyDown))},
{ WM_NOTIFY + WM_REFLECT_BASE, (WORD)(int)TVN_SELCHANGED, 0, 0, AfxSigNotify_v, (AFX_PMSG)(static_cast<void (AFX_MSG_CALL CCmdTarget::*)(NMHDR*, LRESULT*) > (OnNodeSelect)) },
{ WM_NOTIFY + WM_REFLECT_BASE, (WORD)(int)TVN_BEGINDRAG, 0, 0, AfxSigNotify_v, (AFX_PMSG)(static_cast<void (AFX_MSG_CALL CCmdTarget::*)(NMHDR*, LRESULT*) > (OnBeginDrag)) },
{ WM_NOTIFY + WM_REFLECT_BASE, (WORD)(int)TVN_ITEMEXPANDED, 0, 0, AfxSigNotify_v, (AFX_PMSG)(static_cast<void (AFX_MSG_CALL CCmdTarget::*)(NMHDR*, LRESULT*) > (OnNodeExpand)) },
{ WM_NOTIFY + WM_REFLECT_BASE, (WORD)(int)TVN_BEGINLABELEDIT, 0, 0, AfxSigNotify_v, (AFX_PMSG)(static_cast<void (AFX_MSG_CALL CCmdTarget::*)(NMHDR*, LRESULT*) > (OnBeginLabelEdit)) },
{ WM_NOTIFY + WM_REFLECT_BASE, (WORD)(int)TVN_ENDLABELEDIT, 0, 0, AfxSigNotify_v, (AFX_PMSG)(static_cast<void (AFX_MSG_CALL CCmdTarget::*)(NMHDR*, LRESULT*) > (OnEndLabelEdit)) },
{ WM_NOTIFY + WM_REFLECT_BASE, (WORD)(int)NM_RCLICK, 0, 0, AfxSigNotify_v, (AFX_PMSG)(static_cast<void (AFX_MSG_CALL CCmdTarget::*)(NMHDR*, LRESULT*) > (OnRightClick)) },

/*	ON_NOTIFY_REFLECT(TVN_KEYDOWN, OnKeyDown)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnNodeSelect)
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBeginDrag)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnNodeExpand)
	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, OnBeginLabelEdit)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndLabelEdit)
	ON_NOTIFY_REFLECT(NM_RCLICK,OnRightClick)*/
END_MESSAGE_MAP()


// MFCClipListView diagnostics

#ifdef _DEBUG
void MFCClipListView::AssertValid() const
{
	CTreeView::AssertValid();
}

void MFCClipListView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif //_DEBUG


// MFCClipListView message handlers

int
MFCClipListView::OnCreate(LPCREATESTRUCT lpCreateStruct )
{
	lpCreateStruct->style |= TVS_EDITLABELS|TVS_HASBUTTONS;
	long ret = CTreeView::OnCreate(lpCreateStruct);

	bounds.left = 0;
	bounds.top = 0;
	bounds.right = lpCreateStruct->cx;
	bounds.bottom = lpCreateStruct->cy;
	frame.left = lpCreateStruct->x;
	frame.top = lpCreateStruct->y;
	frame.right = frame.left+bounds.right; 
	frame.bottom = frame.top+bounds.bottom; 

	images.Create(
			16,
			15,
			ILC_COLOR8,
			11,
			1 
		);
	CBitmap bm;
	bm.LoadBitmap(IDB_CLIP_IDX_IMG);
	images.Add(&bm, RGB(0, 0, 0));
	images.SetBkColor(GetSysColor(COLOR_WINDOW));
	GetTreeCtrl().SetImageList(&images, TVSIL_NORMAL);

//	GetTreeCtrl().InsertColumn(0, _T("Clip"), LVCFMT_LEFT, bounds.right);

//	m_dropTarget.Register(this);

	return ret;
}


void
MFCClipListView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	bounds.right = cx;
	bounds.bottom = cy;
	frame.right = frame.left+cx;
	frame.bottom = frame.bottom+cy;
//	SetHVScroll();
}

void
MFCClipListView::OnSizing( UINT which, LPRECT r)
{
	CView::OnSizing(which, r);
}


void
MFCClipListView::OnBeginLabelEdit(NMHDR *pNotifyStruct, LRESULT *result)
{
	*result = 1;
	NMTVDISPINFO	*tvp = (NMTVDISPINFO *)pNotifyStruct;
	HTREEITEM		it = tvp->item.hItem;
	
	selectedItem = it;
	GetTreeCtrl().Select(selectedItem, TVGN_CARET);
	DWORD_PTR		selectedData = GetTreeCtrl().GetItemData(selectedItem);
	if (selectedData > QCI_SYMBOL_LPARAM) {
		StabEnt		*sym = (StabEnt *)selectedData;
		switch (sym->type) {
		case TypedValue::S_CLIP:
				*result = 0;
				break;
			case TypedValue::S_TAKE:
				*result = 0;
				break;
			default:
				*result = 1;
				break;
		}
	} else {
        *result = 1;
	}
}

void
MFCClipListView::OnEndLabelEdit(NMHDR *pNotifyStruct,LRESULT *result)
{
	NMTVDISPINFO	*tvp = (NMTVDISPINFO *)pNotifyStruct;
	HTREEITEM		it = tvp->item.hItem;

	fprintf(stderr, "label edit end %s\n", tvp->item.pszText?tvp->item.pszText:"");
	DWORD_PTR		selectedData = GetTreeCtrl().GetItemData(selectedItem);
	if (selectedData > QCI_SYMBOL_LPARAM && SymTab::ValidSymbolName(tvp->item.pszText)) {
		StabEnt		*sym = (StabEnt *)selectedData;
		switch (sym->type) {
			case TypedValue::S_CLIP:
				glob.rename(sym, tvp->item.pszText);
				*result = 1;
				break;
			case TypedValue::S_TAKE:
				glob.rename(sym, tvp->item.pszText);
				*result = 1;
				break;
			default:
				*result = 0;
				break;
		}
	} else {
        *result = 0;
	}
}

void
MFCClipListView::OnMouseMove(UINT nFlags, CPoint point)
{
	/*
	if (draggingNow)
	{
		CTreeCtrl& tree = GetTreeCtrl();
		dragImageList->DragEnter(&tree, point);
		dragImageList->DragMove(point);
	}*/

	CTreeView::OnMouseMove(nFlags, point);
}

void
MFCClipListView::OnLButtonUp(UINT nFlags, CPoint point)
{
	/*
	if (draggingNow)
	{
		ReleaseCapture();

		draggingNow = FALSE;
		dragImageList->EndDrag();
		delete dragImageList;
		dragImageList = NULL;

		CTreeCtrl& tree = GetTreeCtrl();
		UINT flags;
		HTREEITEM hTargetItem = tree.HitTest(point, &flags);
//		if (hTargetItem != NULL && IsItemCanBeDroppedOn(m_hDraggedItem, hTargetItem))
//		{
//			HTREEITEM hNewItem = tree.InsertItem("Untitled", hTargetItem);
//			CopyItemProperties(hNewItem, m_hDraggedItem);
//			if (nFlags != MK_CONTROL)
//				tree.DeleteItem(m_hDraggedItem);
//		}

		draggedItem = NULL;
	}
*/
	CTreeView::OnLButtonUp(nFlags, point);
}


void
MFCClipListView::OnRButtonUp(UINT nFlags, CPoint point)
{
	/*
	if (draggingNow)
	{
		ReleaseCapture();

		draggingNow = FALSE;
		dragImageList->EndDrag();
		delete dragImageList;
		dragImageList = NULL;

		CTreeCtrl& tree = GetTreeCtrl();
		UINT flags;
		HTREEITEM hTargetItem = tree.HitTest(point, &flags);
//		if (hTargetItem != NULL && IsItemCanBeDroppedOn(m_hDraggedItem, hTargetItem))
//		{
//			HTREEITEM hNewItem = tree.InsertItem("Untitled", hTargetItem);
//			CopyItemProperties(hNewItem, m_hDraggedItem);
//			if (nFlags != MK_CONTROL)
//				tree.DeleteItem(m_hDraggedItem);
//		}

		draggedItem = NULL;
	}
*/
	CTreeView::OnRButtonUp(nFlags, point);
}
void
MFCClipListView::OnKeyDown(NMHDR *pNotifyStruct,LRESULT *result)
{
	TV_KEYDOWN* pKeyDown = (TV_KEYDOWN *) pNotifyStruct;

	if (pKeyDown->wVKey == VK_DELETE) {
		fprintf(stderr, "delete key");
		/*
		BOOL bShowWarnings = ((CDaoViewApp *)AfxGetApp())->m_bShowWarnings;
		int retCode = IDYES;
		if (bShowWarnings)
		{
			retCode = MessageBox(
				_T("Are you sure you want to delete this item ?"),
				_T("DaoView - Warning"),MB_YESNO);
		}
		if (retCode == IDYES)
			DeleteItem(m_ItemSel);
			*/
	}
	*result = 0;
}


void
MFCClipListView::OnNodeSelect(NMHDR *pNotifyStruct,LRESULT *result)
{
	*result = 0;
	NMTREEVIEW	*tvp = (NMTREEVIEW *)pNotifyStruct;
	fprintf(stderr, "clip list node select %x %x\n", tvp->itemOld.lParam, tvp->itemNew.lParam);


	if (tvp->itemNew.lParam > QCI_SYMBOL_LPARAM) {
		StabEnt *s = (StabEnt *)tvp->itemNew.lParam;
		switch (s->type) {
			case TypedValue::S_CLIP: {
				if (editor) {
					;
				}
				break;
			}
			case TypedValue::S_TAKE: {
	fprintf(stderr, "take node select %x %x\n", tvp->itemOld.lParam, tvp->itemNew.lParam);
				if (editor) {
	fprintf(stderr, "editor node select %x %x\n", tvp->itemOld.lParam, tvp->itemNew.lParam);
					Take	*t = s->TakeValue();
					if (t != NULL) {
	fprintf(stderr, "set to  %x %x\n", tvp->itemOld.lParam, tvp->itemNew.lParam);
						editor->SetToTake(t);
					}
				}
				break;
			}
		}
	}
}

void
MFCClipListView::OnNodeExpand(NMHDR *pNotifyStruct,LRESULT *result)
{
	*result = 0;
	fprintf(stderr, "node expand\n");

	NMTREEVIEW	*tvp = (NMTREEVIEW *)pNotifyStruct;
	if (tvp->itemNew.lParam > QCI_SYMBOL_LPARAM) {
		StabEnt *s = (StabEnt *)tvp->itemNew.lParam;
		switch (s->type) {
			case TypedValue::S_CLIP: {
				Clip	*c = s->ClipValue(NULL);
				short img = 10;
				if (c->media != NULL) {
					if (c->media->type == Take::SAMPLE) {
						img = 8;
					} else if (c->media->type == Take::STREAM) {
						img = 9;
					}
				}
				if (tvp->itemNew.state & TVIS_EXPANDED) {
					GetTreeCtrl().SetItemImage(tvp->itemNew.hItem, 9, 9);
				} else {
					GetTreeCtrl().SetItemImage(tvp->itemNew.hItem, 9, 9);
				}
				break;
			}
			case TypedValue::S_TAKE: {
				if (tvp->itemNew.cChildren == 0) {
					Take	*t = s->TakeValue();
					short img = 13;
					if (t->type == Take::SAMPLE) {
						img = 11;
					} else if (t->type == Take::STREAM) {
						img = 12;
					}
					GetTreeCtrl().SetItemImage(tvp->itemNew.hItem, img, img);
				} else if (tvp->itemNew.state & TVIS_EXPANDED) {
					Take	*t = s->TakeValue();
					short img = 7;
					if (t->type == Take::SAMPLE) {
						img = 5;
					} else if (t->type == Take::STREAM) {
						img = 6;
					}
					GetTreeCtrl().SetItemImage(tvp->itemNew.hItem, img, img);
				} else {
					Take	*t = s->TakeValue();
					short img = 4;
					if (t->type == Take::SAMPLE) {
						img = 2;
					} else if (t->type == Take::STREAM) {
						img = 3;
					}
					GetTreeCtrl().SetItemImage(tvp->itemNew.hItem, img, img);
				}
				break;
			}
		}
	} else { // top level category
		if (tvp->itemNew.state & TVIS_EXPANDED) {
			GetTreeCtrl().SetItemImage(tvp->itemNew.hItem, 1, 1);
		} else {
			GetTreeCtrl().SetItemImage(tvp->itemNew.hItem, 0, 0);
		}
	}

	/*
	if (m_bNoNotifications)
		return;

	CTreeCtrlEx& ctlTree = (CTreeCtrlEx&) GetTreeCtrl();

	m_ItemSel = ctlTree.GetSelectedItem();

	UINT nImageID = m_ItemSel.GetImageID();
	GetDocument()->m_pListView->SetRedraw(FALSE);
	switch (nImageID)
	{
		case IID_DATABASE:
			GetDocument()->m_pListView->ShowDatabase();
			break;
		case IID_TABLES:
			GetDocument()->m_pListView->ShowTableSchema();
			break;
		case IID_TABLE:
			GetDocument()->m_pListView->ShowTableSchema(m_ItemSel.GetText());
			break;
		case IID_FIELDS:
			GetDocument()->m_pListView->ShowFields(m_ItemSel.GetParent().GetText());
			break;
		case IID_FIELD:
			GetDocument()->m_pListView->ShowFields(m_ItemSel.GetParent().GetParent().GetText(),m_ItemSel.GetText());
			break;
		case IID_QUERYDEFS:
			GetDocument()->m_pListView->ShowQuerySchema();
			break;
		case IID_QUERYDEF:
			GetDocument()->m_pListView->ShowQuerySchema(m_ItemSel.GetText());
			break;
		case IID_RELATIONS:
			GetDocument()->m_pListView->ShowRelations();
			break;
		case IID_RELATION:
			GetDocument()->m_pListView->ShowRelations(m_ItemSel.GetText());
			break;
		case IID_INDEXES:
			GetDocument()->m_pListView->ShowIndexes(m_ItemSel.GetParent().GetText());
			break;
		case IID_INDEX:
			GetDocument()->m_pListView->ShowIndexes(m_ItemSel.GetParent().GetParent().GetText(),m_ItemSel.GetText());
			break;
	}
	GetDocument()->m_pListView->SetRedraw(TRUE);
	*/
}

void
MFCClipListView::OnRightClick(NMHDR *pNotifyStruct,LRESULT *result)
{
	CTreeCtrl& ctl = (CTreeCtrl&) GetTreeCtrl();

	UINT nFlags;
	CPoint curPoint;
	GetCursorPos(&curPoint);
	ScreenToClient(&curPoint);
	selectedItem = ctl.HitTest(curPoint, &nFlags);
	if (selectedItem == NULL) {
		if (parent != NULL && parent->symbol != NULL && parent->symbol->type == TypedValue::S_SAMPLE) {
			DoPopupMenu(IDR_SAMPLETAKENULL_RCLICK, false);
		} else {
			DoPopupMenu(IDR_STREAMTAKENULL_RCLICK, false);
		}
		return;
	}
	ctl.Select(selectedItem, TVGN_CARET);

	DWORD_PTR selectedData = ctl.GetItemData(selectedItem);
	if (selectedData > QCI_SYMBOL_LPARAM) {
		StabEnt	*sym = (StabEnt *) selectedData;
		switch (sym->type) {
			case TypedValue::S_CLIP:
				DoPopupMenu(IDR_CLIP_RCLICK, false);
				break;
			case TypedValue::S_TAKE:
				if (sym->context && sym->context->type == TypedValue::S_SAMPLE) {
					DoPopupMenu(IDR_SAMPLETAKE_RCLICK, false);
				} else {
					DoPopupMenu(IDR_STREAMTAKE_RCLICK, false);
				}
				break;
		}
	}

	*result = 0;
}

BOOL
MFCClipListView::DoPopupMenu(UINT nMenuID, bool return_cmd)
{
	CMenu popMenu;

	if (!popMenu.LoadMenu(nMenuID)) {
		fprintf(stderr, "load menu %d fails\n", nMenuID);
		return FALSE;
	}

	CPoint posMouse;
	GetCursorPos(&posMouse);

	CMenu	*sub = popMenu.GetSubMenu(0);
	if (sub == NULL) {
		fprintf(stderr, "load menu %d has no popup\n", nMenuID);
		return FALSE;
	}

	BOOL ret = FALSE;
	if (return_cmd) {
		ret = sub->TrackPopupMenuEx(
						TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RETURNCMD,
						posMouse.x, posMouse.y, this, NULL);
	} else {
		sub->TrackPopupMenu(0,posMouse.x,posMouse.y,this);
		ret = TRUE;
	}
	return ret;
}

// Members
int
MFCClipListView::CreateList(CRect &r, MFCObjectView *v, QuaObjectRepresentation *vor, MFCDataEditor *e, UINT id)
{
	int ret = Create(NULL, "clip list",
				WS_CHILD|WS_VISIBLE|TVS_EDITLABELS|TVS_SHOWSELALWAYS,
				r, v, id, NULL);
	parent = vor;
	editor = e;
	return ret;
}

//HTREEITEM
//MFCClipListView::AddClipItem(char * nm, LPARAM lp, int im)
//{
//	HTREEITEM ind = GetTreeCtrl().InsertItem(0, nm, im);
//	if (ind >= 0) {
//		long ret = GetTreeCtrl().SetItemData(ind, lp);
//		if (!ret) {
//			fprintf(stderr, "can't set clip symbol\n");
//		}
//	}
//	return ind;
//}


HTREEITEM
MFCClipListView::ItemFor(StabEnt *s, HTREEITEM parent)
{
	HTREEITEM hCurrent=NULL;
	hCurrent = GetTreeCtrl().GetNextItem(parent, TVGN_CHILD);
	while (hCurrent != NULL) {
		TVITEM item;
		item.hItem = hCurrent;
		item.mask = TVIF_PARAM | TVIF_HANDLE;

		BOOL bWorked = GetTreeCtrl().GetItem(&item);
		if (item.lParam == (LPARAM) s) {
			return hCurrent;
		}
	// Try to get the next item
		hCurrent = GetTreeCtrl().GetNextItem(hCurrent, TVGN_NEXT);

	}
	return NULL;
}


HTREEITEM
MFCClipListView::ItemFor(StabEnt *s)
{
	if (s!= NULL) {
		switch (s->type) {
			case TypedValue::S_TAKE: {
				return ItemFor(s, TVI_ROOT);
			}
			case TypedValue::S_CLIP: {
				Clip	*c = s->ClipValue(NULL);
				if (c->media == NULL || c->media->sym == NULL) {
					return ItemFor(s, TVI_ROOT);
				}
				HTREEITEM	ipt = ItemFor(c->media->sym, TVI_ROOT);
				return ItemFor(s, ipt);
			}
		}
		return ItemFor(s, TVI_ROOT);
	}
	return NULL;
}

//HTREEITEM
//MFCClipListView::ItemFor(StabEnt *sym)
//{
//	LVFINDINFO	lvf;
//	lvf.lParam = (LPARAM) sym;
//	lvf.flags = LVFI_PARAM;
//	long ind = GetTreeCtrl().FindItem(&lvf);
//	return ind;
//}

//HTREEITEM
//MFCClipListView::ItemAtPoint(CPoint &p)
//{
//	LVFINDINFO	lvf;
//	lvf.pt = p;
//	lvf.vkDirection = VK_UP;
//	lvf.flags = LVFI_NEARESTXY;
//	HTREEITEM ind = GetTreeCtrl().FindItem(&lvf);
//	return ind;
//}


HTREEITEM
MFCClipListView::AddItem(const char *s, LPARAM type, HTREEITEM parent, int img)
{
	fprintf(stderr, "ai %s %x %x %d\n", s, type, parent, img);
	HTREEITEM ht = GetTreeCtrl().InsertItem(
				TVIF_TEXT|TVIF_PARAM|TVIF_STATE|TVIF_IMAGE|TVIF_SELECTEDIMAGE, /* nMask */
				s,
				img, /*int nImage */
				img, /* int nSelectedImage*/
				0 , /* UINT nState */
				0, /* UINT nStateMask */
				(LPARAM) type,
				parent,
				0 /*HTREEITEM hInsertAfter*/ 
			);
	return ht;
}

HTREEITEM
MFCClipListView::AddItem(StabEnt *s)
{
	HTREEITEM	it = NULL;
	if (s!= NULL) {
		switch (s->type) {
			case TypedValue::S_TAKE: {
				it = ItemFor(s, TVI_ROOT);
				Take	*t = s->TakeValue();
				if (it == NULL) {
					it = AddItem(s->uniqueName(), (LPARAM)s, TVI_ROOT,
								(t->type==Take::STREAM)?3:
								(t->type==Take::SAMPLE)?2:4);
				}
				break;
			}
			case TypedValue::S_CLIP: {
				Clip	*c = s->ClipValue(NULL);
				if (c->media == NULL) { // an unattached clip object
					it = ItemFor(s, TVI_ROOT);
					if (it == NULL) {
						it = AddItem(s->uniqueName(), (LPARAM)s, TVI_ROOT,
									10);
					}
					return it;
				}
				StabEnt	*ts = c->media->sym;
				if (ts == NULL) { // an unattached clip object
					it = ItemFor(s, TVI_ROOT);
					if (it == NULL) {
						it = AddItem(s->uniqueName(), (LPARAM)s, TVI_ROOT,
									(c->media->type==Take::STREAM)?9:
									(c->media->type==Take::SAMPLE)?8:10);
					}
					return it;
				} else {
					HTREEITEM	itp = ItemFor(ts, TVI_ROOT);
					it = ItemFor(s, itp);
					if (it == NULL) {
						it = AddItem(s->uniqueName(), (LPARAM)s, itp,
									(c->media->type==Take::STREAM)?9:
									(c->media->type==Take::SAMPLE)?8:10);
					}
				}
				break;
			}
		}
	}
//	HTREEITEM ind = AddClipItem(s->uniqueName(), (LPARAM)s,
//					c->media==NULL?2:
//					c->media->type==Take::STREAM?1:
//					c->media->type==Take::SAMPLE?0:2);
	return it;
}

bool
MFCClipListView::RemoveItem(StabEnt *s)
{
	HTREEITEM ind = ItemFor(s);
	if (ind < 0)
		return false;
	if (GetTreeCtrl().DeleteItem(ind)) {
		return true;
	}
	return false;
}

bool
MFCClipListView::SelectClip(StabEnt *s, bool sel)
{
	return false;
}

void
MFCClipListView::OnPopupDeleteTake()
{
	DWORD_PTR selectedData = GetTreeCtrl().GetItemData(selectedItem);
	if (selectedData > QCI_SYMBOL_LPARAM) {
		StabEnt	*sym = (StabEnt *) selectedData;
		StabEnt	*pSym = parent->symbol;
		Take	*t = sym->TakeValue();
		switch (pSym->type) {
			case TypedValue::S_VOICE: {
				StreamTake	*c = NULL;
				if (t->type == Take::STREAM) {
					c = ((StreamTake *)t);
				}
				Voice	*v = NULL;
				if ((v = pSym->VoiceValue()) != NULL && c != NULL) {
					v->DeleteTake(c, true);
				}
				break;
			}
			case TypedValue::S_SAMPLE: {
				SampleTake	*c = NULL;
				if (t->type == Take::SAMPLE) {
					c = ((SampleTake *)t);
				}
				Sample	*v = NULL;
				if ((v = pSym->SampleValue()) != NULL && c != NULL) {
					v->DeleteTake(c, true);
				}
				break;
			}
		}
	}
}


void
MFCClipListView::OnPopupCreateClip()
{
	DWORD_PTR selectedData = GetTreeCtrl().GetItemData(selectedItem);
	if (selectedData > QCI_SYMBOL_LPARAM) {
		StabEnt	*sym = (StabEnt *) selectedData;
		fprintf(stderr, "Popup create clip %s\n", sym->uniqueName());
		Time	dur_time;
		Time	at_time;
		switch (sym->type) {
			case TypedValue::S_TAKE: {
				StabEnt	*chSym = parent->symbol;
				if (chSym) {
					switch (chSym->type) {
						case TypedValue::S_VOICE: {
							at_time.Set(1,0,0,&Metric::std);
							dur_time.Set(1,0,0,&Metric::std);
							Voice	*v;
							StreamTake	*take=sym->StreamTakeValue();
							string	nmbuf = glob.makeUniqueName(chSym, "clip", 1);
							if (take != NULL && (v = chSym->VoiceValue()) != NULL) {
								Clip	*c = v->AddClip(nmbuf, take, at_time, dur_time, true);
//								MFCEditorItemView	*added_item = AddClipItemView(c);
							}
							break;
						}
						case TypedValue::S_SAMPLE: {
							Sample	*v;
							at_time.Set(0,0,0,&Metric::std);
							SampleTake	*take=sym->SampleTakeValue();
							dur_time = take->Duration();
							string 	nmbuf = glob.makeUniqueName(chSym, "clip", 1);
							if (take != NULL && (v = chSym->SampleValue()) != NULL) {
								Clip	*c = v->AddClip(nmbuf, take, at_time, dur_time, true);
//								MFCEditorItemView	*added_item = AddClipItemView(c);
							}
							break;
						}
					}
				}
				break;
			}
			case TypedValue::S_CLIP:
//				quaLink->DeleteObject(sym);
				break;
		}
	}
}

void
MFCClipListView::OnPopupDeleteClip()
{
	DWORD_PTR selectedData = GetTreeCtrl().GetItemData(selectedItem);
	if (selectedData > QCI_SYMBOL_LPARAM && parent && parent->symbol) {
		StabEnt	*sym = (StabEnt *) selectedData;
		Clip	*c = sym->ClipValue(NULL);
		if (c != NULL) {
			StabEnt	*pSym = parent->symbol;
			switch (pSym->type) {
				case TypedValue::S_VOICE: {
					Voice	*v;
					if ((v = pSym->VoiceValue()) != NULL && c != NULL) {
						v->RemoveClip(c, true);
					}
					break;
				}
				case TypedValue::S_SAMPLE: {
					Sample	*v;
					if ((v = pSym->SampleValue()) != NULL && c != NULL) {
						v->RemoveClip(c, true);
					}
					break;
				}
			}
		}
	}
}

void
MFCClipListView::OnPopupEditClip()
{
	DWORD_PTR selectedData = GetTreeCtrl().GetItemData(selectedItem);
	if (selectedData > QCI_SYMBOL_LPARAM) {
		StabEnt	*sym = (StabEnt *) selectedData;
		fprintf(stderr, "Popup delete %s\n", sym->uniqueName());
		switch (sym->type) {
			case TypedValue::S_CLIP:
//				quaLink->DeleteObject(sym);
				break;
			case TypedValue::S_TAKE:
//				quaLink->DeleteObject(sym);
				break;
		}
	}
}

void
MFCClipListView::OnPopupCreateStream()
{
	if (parent != NULL && parent->symbol != NULL && parent->symbol->type == TypedValue::S_VOICE) {
		parent->quaLink->CreateStreamTake(parent->symbol);
	}
}

void
MFCClipListView::OnPopupLoadSample()
{
	if (parent != NULL && parent->symbol != NULL && parent->symbol->type == TypedValue::S_SAMPLE) {
		CFileDialog		fileOpenDlg(
							true, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
							"Audio files (*.wav,*.aif)|*.wav;*.aif;*.aiff;*.wave||"
						);
		INT_PTR	ret = fileOpenDlg.DoModal();
		if (ret == IDOK) {
			CString pathName = fileOpenDlg.GetPathName();
			string	samplePath((LPCTSTR)pathName);
			parent->quaLink->LoadSampleTake(parent->symbol, samplePath);
		}
	}
}

void
MFCClipListView::OnPopupEditTake()
{
	DWORD_PTR selectedData = GetTreeCtrl().GetItemData(selectedItem);
	if (selectedData > QCI_SYMBOL_LPARAM) {
		StabEnt	*sym = (StabEnt *) selectedData;
		switch (sym->type) {
			case TypedValue::S_CLIP:
				break;
			case TypedValue::S_TAKE:
				break;
		}
	}
}



DROPEFFECT
MFCClipListView::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	fprintf(stderr, "drag enter\n");
/*
	if (m_pDB)
		if (pDataObject->IsDataAvailable(m_nIDClipFormat))
			return DROPEFFECT_COPY;
*/
	return CTreeView::OnDragEnter(pDataObject, dwKeyState, point);
}

DROPEFFECT
MFCClipListView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
/*
	if (m_pDB)
		if (pDataObject->IsDataAvailable(m_nIDClipFormat))
			return DROPEFFECT_COPY;
*/
	return CTreeView::OnDragOver(pDataObject, dwKeyState, point);
}

BOOL
MFCClipListView::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
/*
	if (!(m_pDB && pDataObject->IsDataAvailable(m_nIDClipFormat)))
		return CTreeView::OnDrop(pDataObject, dropEffect, point);

	HGLOBAL hGlob = pDataObject->GetGlobalData(m_nIDClipFormat);
	if (hGlob != NULL){
		CSharedFile globFile;
		globFile.SetHandle(hGlob,FALSE);
		CArchive ar(&globFile,CArchive::load);
		CDragItem dragItem;
		dragItem.Serialize(ar);
		ar.Close();
		dragItem.Transfer(this,m_pDB);
		return TRUE;
	}
*/

	return FALSE;
}


afx_msg void
MFCClipListView::OnBeginDrag(NMHDR *pnmh, LRESULT* bHandled)
{
	fprintf(stderr, "MFCQuaSymbolIndexView begin drag\n");
	NMLISTVIEW* tvp = (NMLISTVIEW*)pnmh;

/*	if (!draggingNow) {
		if  (tvp->itemNew.lParam > QSI_SYMBOL_LPARAM ) {
			StabEnt	*s = (StabEnt *)tvp->itemNew.lParam;
			bool	draggable = false;
			UINT	dragFormat = 0;
			switch (s->type) {
				case S_INSTANCE:	// move an instance
					draggable = true;
					dragFormat = QuaDrop::instanceFormat;
					break;
				case S_SAMPLE:	// create a sample instance
					draggable = true;
					dragFormat = QuaDrop::sampleFormat;
					break;
				case S_VOICE:	// create a voice instance
					draggable = true;
					dragFormat = QuaDrop::voiceFormat;
					break;
			}
			if (!draggable) {
				*bHandled = TRUE;
				return;
			}

			COleDataSource srcItem;

			CTreeCtrl& tree = GetTreeCtrl();
			QuaDrop::SetSymbolSource(&srcItem, dragFormat, s);
			tree.Select(tvp->itemNew.hItem, TVGN_CARET);
			selectedItem = tvp->itemNew.hItem;
			srcItem.DoDragDrop();

		}
	}
*/
	*bHandled = TRUE;
}

bool
MFCClipListView::RemoveClipsNotIn(vector<StabEnt*> &present)
{
	HTREEITEM hItem=NULL;
	hItem = GetTreeCtrl().GetNextItem(TVI_ROOT, TVGN_CHILD);
	while (hItem != NULL) {
		TVITEM item;
		item.hItem = hItem;
		item.mask = TVIF_PARAM | TVIF_HANDLE;

		BOOL bWorked = GetTreeCtrl().GetItem(&item);
		if (item.lParam >= QCI_SYMBOL_LPARAM) {
			StabEnt	*s = (StabEnt *)item.lParam;
			if (s->type == TypedValue::S_CLIP) {
				if (find(present.begin(), present.end(), s) == present.end()) {
					GetTreeCtrl().DeleteItem(hItem);
				}
			} else if (s->type == TypedValue::S_TAKE) {
				HTREEITEM hTakeChild=NULL;
				hTakeChild = GetTreeCtrl().GetNextItem(hItem, TVGN_CHILD);
				while (hTakeChild != NULL) {
					TVITEM sitem;
					sitem.hItem = hTakeChild;
					sitem.mask = TVIF_PARAM | TVIF_HANDLE;
					bWorked = GetTreeCtrl().GetItem(&sitem);
					if (sitem.lParam >= QCI_SYMBOL_LPARAM) {
						StabEnt	*ss = (StabEnt *)sitem.lParam;
						if (ss->type == TypedValue::S_CLIP) {
							if (find(present.begin(), present.end(), ss) == present.end()) {
								GetTreeCtrl().DeleteItem(hTakeChild);
							}
						}
					}
					hTakeChild = GetTreeCtrl().GetNextItem(hTakeChild, TVGN_NEXT);
				}
			}
		}

// Try to get the next item
		hItem = GetTreeCtrl().GetNextItem(hItem, TVGN_NEXT);
	}
	return true;
}

bool
MFCClipListView::RemoveTakesNotIn(vector<StabEnt*> &present)
{
	HTREEITEM hTake=NULL, hNextTake=NULL;
	hTake = GetTreeCtrl().GetNextItem(TVI_ROOT, TVGN_CHILD);
	while (hTake != NULL) {
		hNextTake = GetTreeCtrl().GetNextItem(hTake, TVGN_NEXT);
		TVITEM item;
		item.hItem = hTake;
		item.mask = TVIF_PARAM | TVIF_HANDLE;

		BOOL bWorked = GetTreeCtrl().GetItem(&item);
		if (item.lParam >= QCI_SYMBOL_LPARAM) {
			StabEnt	*s = (StabEnt *)item.lParam;
			if (s->type == TypedValue::S_TAKE && find(present.begin(), present.end(), s) == present.end()) {
				GetTreeCtrl().DeleteItem(hTake);
			}
		}

// Try to get the next item
		hTake = hNextTake;
	}
	return true;
}
