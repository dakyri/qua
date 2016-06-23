// MFCTakeListView.cpp : implementation file
//
#define _AFXDLL
#include "stdafx.h"

#include "StdDefs.h"

#include "QuaMFC.h"
#include "MFCTakeListView.h"
#include "MFCObjectView.h"
#include "MFCDataEditor.h"

#include "Sym.h"
#include "Clip.h"


// MFCTakeListView

IMPLEMENT_DYNCREATE(MFCTakeListView, CListView)

MFCTakeListView::MFCTakeListView()
{
}

MFCTakeListView::~MFCTakeListView()
{
}

BEGIN_MESSAGE_MAP(MFCTakeListView, CListView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginLabelEdit)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndLabelEdit)
	ON_NOTIFY_REFLECT(LVN_KEYDOWN,OnKeyDown)
//	ON_NOTIFY_REFLECT(LVN_SELCHANGED,OnSelect)
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG,OnBeginDrag)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()


// MFCTakeListView diagnostics

#ifdef _DEBUG
void MFCTakeListView::AssertValid() const
{
	CListView::AssertValid();
}

void MFCTakeListView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG


// MFCTakeListView message handlers

int
MFCTakeListView::OnCreate(LPCREATESTRUCT lpCreateStruct )
{
	long	ret = CListView::OnCreate(lpCreateStruct);

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
			16,
			ILC_COLOR8,
			3,
			1 
		);

	CBitmap bm;
	bm.LoadBitmap(IDB_TAKE_IDX_IMG);
	images.Add(&bm, RGB(0, 0, 0));
	images.SetBkColor(GetSysColor(COLOR_WINDOW));
	GetListCtrl().SetImageList(&images, LVSIL_SMALL);

	GetListCtrl().InsertColumn(0, _T("Take"), LVCFMT_LEFT, bounds.right);

	return ret;
}


void
MFCTakeListView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	bounds.right = cx;
	bounds.bottom = cy;
	frame.right = frame.left+cx;
	frame.bottom = frame.bottom+cy;
//	SetHVScroll();
}

void
MFCTakeListView::OnSizing( UINT which, LPRECT r)
{
	CView::OnSizing(which, r);
}


void
MFCTakeListView::OnBeginLabelEdit(LPNMHDR pnmhdr, LRESULT *pLResult)
{
	LV_DISPINFO  *plvDispInfo = (LV_DISPINFO *)pnmhdr;
	LV_ITEM      *plvItem = &plvDispInfo->item;

//	if (plvItem->pszText != NULL)
//		SetItemText(plvItem->iItem, plvItem->iSubItem, plvItem->pszText);
}

void
MFCTakeListView::OnEndLabelEdit(LPNMHDR pnmhdr, LRESULT *pLResult)
{
	LV_DISPINFO  *plvDispInfo = (LV_DISPINFO *)pnmhdr;
	LV_ITEM      *plvItem = &plvDispInfo->item;

//	if (plvItem->pszText != NULL)
//		SetItemText(plvItem->iItem, plvItem->iSubItem, plvItem->pszText);
}

void
MFCTakeListView::OnMouseMove(UINT nFlags, CPoint point)
{/*
	if (draggingNow)
	{
		CTreeCtrl& tree = GetTreeCtrl();
		dragImageList->DragEnter(&tree, point);
		dragImageList->DragMove(point);
	}*/

	CListView::OnMouseMove(nFlags, point);
}

void
MFCTakeListView::OnLButtonUp(UINT nFlags, CPoint point)
{/*
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
	CListView::OnLButtonUp(nFlags, point);
}


void
MFCTakeListView::OnRButtonUp(UINT nFlags, CPoint point)
{/*
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
	CListView::OnRButtonUp(nFlags, point);
}

afx_msg void
MFCTakeListView::OnBeginDrag(NMHDR *pnmh, LRESULT* bHandled)
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

void
MFCTakeListView::OnKeyDown(NMHDR *pNotifyStruct,LRESULT *result)
{
	TV_KEYDOWN* pKeyDown = (TV_KEYDOWN *) pNotifyStruct;

	if (pKeyDown->wVKey == VK_DELETE)
	{
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


DROPEFFECT
MFCTakeListView::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	fprintf(stderr, "drag enter\n");
	/*
	if (m_pDB)
		if (pDataObject->IsDataAvailable(m_nIDClipFormat))
			return DROPEFFECT_COPY;
*/
	return CListView::OnDragEnter(pDataObject, dwKeyState, point);
}

DROPEFFECT
MFCTakeListView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	/*
	if (m_pDB)
		if (pDataObject->IsDataAvailable(m_nIDClipFormat))
			return DROPEFFECT_COPY;
*/
	return CListView::OnDragOver(pDataObject, dwKeyState, point);
}

BOOL
MFCTakeListView::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
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
	}*/

	return FALSE;
}


void
MFCTakeListView::OnSelect(NMHDR *pNotifyStruct,LRESULT *result)
{
	*result = 0;
	NMLISTVIEW	*tvp = (NMLISTVIEW *)pNotifyStruct;

//	fprintf(stderr, "node select %x %x\n", tvp->itemOld.lParam, tvp->itemNew.lParam);

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

// Members
int
MFCTakeListView::CreateList(CRect &r, MFCObjectView *v, MFCDataEditor *e, UINT id)
{
	int ret = Create(NULL, "take list",
					WS_CHILD|WS_VISIBLE|LVS_REPORT|LVS_EDITLABELS|LVS_SHOWSELALWAYS,
					r, v, id, NULL);
	parent = v;
	editor = e;
	return ret;
}

long
MFCTakeListView::AddTakeItem(char * nm, LPARAM lp, int im)
{
	long ind = GetListCtrl().InsertItem(0, nm, im);
	if (ind >= 0) {
		long ret = GetListCtrl().SetItemData(ind, lp);
		if (!ret) {
			fprintf(stderr, "can't set take symbol\n");
		}
	}
	return ind;
}


long
MFCTakeListView::AddTake(StabEnt *s)
{
	if (s->type != TypedValue::S_TAKE) {
		return -1;
	}
	Take	*c = s->TakeValue();
	if (c == NULL) {
		return -1;
	}
	long ind = AddTakeItem(s->UniqueName(), (LPARAM)s,
					c->type==Take::STREAM?1:
					c->type==Take::SAMPLE?0:2);
	return ind;
}

bool
MFCTakeListView::RemoveTake(StabEnt *s)
{
	long ind = ItemForTake(s);
	if (ind < 0)
		return false;
	if (GetListCtrl().DeleteItem(ind)) {
		return true;
	}
	return false;
}

bool
MFCTakeListView::SelectTake(StabEnt *s, bool sel)
{
	return false;
}

long
MFCTakeListView::ItemForTake(StabEnt *sym)
{
	LVFINDINFO	lvf;
	lvf.lParam = (LPARAM) sym;
	lvf.flags = LVFI_PARAM;
	long ind = GetListCtrl().FindItem(&lvf);
	return ind;
}

long
MFCTakeListView::ItemAtPoint(CPoint &p)
{
	LVFINDINFO	lvf;
	lvf.pt = p;
	lvf.vkDirection = VK_UP;
	lvf.flags = LVFI_NEARESTXY;
	long ind = GetListCtrl().FindItem(&lvf);
	return ind;
}
