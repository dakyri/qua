// MFCClipListView.cpp : implementation file
//

//#define _AFXDLL
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE
#include "stdafx.h"

#include "StdDefs.h"
#include "Colors.h"

#include "QuaMFC.h"
#include "MFCSimpleTypeView.h"
#include "MFCObjectView.h"

#include "Sym.h"
#include "Parse.h"

// MFCClipListView

IMPLEMENT_DYNCREATE(MFCSimpleTypeView, CListView)

MFCSimpleTypeView::MFCSimpleTypeView()
{
}

MFCSimpleTypeView::~MFCSimpleTypeView()
{
}

BEGIN_MESSAGE_MAP(MFCSimpleTypeView, CListView)
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
	ON_WM_LBUTTONDOWN( )
	ON_WM_LBUTTONDBLCLK( )
	ON_WM_MBUTTONUP()
	ON_WM_MBUTTONDOWN( )
	ON_WM_MBUTTONDBLCLK( )
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN( )
	ON_WM_RBUTTONDBLCLK( )
END_MESSAGE_MAP()


// MFCClipListView diagnostics

#ifdef _DEBUG
void MFCSimpleTypeView::AssertValid() const
{
	CListView::AssertValid();
}

void MFCSimpleTypeView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG


// MFCClipListView message handlers

int
MFCSimpleTypeView::OnCreate(LPCREATESTRUCT lpCreateStruct )
{
	long ret = CListView::OnCreate(lpCreateStruct);

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
	bm.LoadBitmap(IDB_SIMPLEVAR_IDX_IMG);
	images.Add(&bm, RGB(0, 0, 0));
	images.SetBkColor(GetSysColor(COLOR_WINDOW));
// don't want this ... we are not showing the little icons here, see custom draw
//	GetListCtrl().SetImageList(&images, LVSIL_SMALL);

	GetListCtrl().InsertColumn(0, _T("Name"), LVCFMT_LEFT, 0.45*bounds.right,-1);
	GetListCtrl().InsertColumn(1, _T("Type"), LVCFMT_LEFT, 0.25*bounds.right,-1);
	GetListCtrl().InsertColumn(2, _T("L"), LVCFMT_LEFT, 0.1*bounds.right,-1); // length
	GetListCtrl().InsertColumn(3, _T("C"), LVCFMT_LEFT, 0.1*bounds.right,-1); // controller mode
	GetListCtrl().InsertColumn(4, _T("E"), LVCFMT_LEFT, 0.1*bounds.right,-1); // envelope mode

	return ret;
}

/*
 * a brutal hack and slash from the CListViewEx DrawItem, which has
 * a few useful bits that might be useful in the long run.
 */
#define OFFSET_FIRST    2
#define OFFSET_OTHER    2
void
MFCSimpleTypeView::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	int			m_bClientWidthSel = TRUE;
	int			m_cxClient = 0;
	int			m_cxStateImageOffset = 0;
	COLORREF	m_clrText = ::GetSysColor(COLOR_WINDOWTEXT);
	COLORREF	m_clrTextBk = ::GetSysColor(COLOR_WINDOW);
	COLORREF	m_clrBkgnd = ::GetSysColor(COLOR_WINDOW);

	CListCtrl& listCtrl=GetListCtrl();
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rcItem(lpDrawItemStruct->rcItem);
	UINT uiFlags = ILD_TRANSPARENT;
	CImageList* pImageList;
	int nItem = lpDrawItemStruct->itemID;
	BOOL bFocus = (GetFocus() == this);
	COLORREF clrTextSave, clrBkSave;
	COLORREF clrImage = m_clrBkgnd;
	static _TCHAR szBuff[MAX_PATH];
	LPCTSTR pszText;

	StabEnt			*itemSym = SymForItem(nItem);
// get item data

	LV_ITEM lvi;
	lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
	lvi.iItem = nItem;
	lvi.iSubItem = 0;
	lvi.pszText = szBuff;
	lvi.cchTextMax = sizeof(szBuff);
	lvi.stateMask = 0xFFFF;     // get all state flags
	listCtrl.GetItem(&lvi);

	BOOL bSelected = (bFocus || (GetStyle() & LVS_SHOWSELALWAYS)) && lvi.state & LVIS_SELECTED;
	bSelected = bSelected || (lvi.state & LVIS_DROPHILITED);

// set colors if item is selected

	CRect rcAllLabels;
	listCtrl.GetItemRect(nItem, rcAllLabels, LVIR_BOUNDS);

	CRect rcLabel;
	listCtrl.GetItemRect(nItem, rcLabel, LVIR_BOUNDS);
//	listCtrl.GetItemRect(nItem, rcLabel, LVIR_LABEL);

	rcAllLabels.left = rcLabel.left;
	if (m_bClientWidthSel && rcAllLabels.right<m_cxClient)
		rcAllLabels.right = m_cxClient;

	if (bSelected)	{
		clrTextSave = pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		clrBkSave = pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));

		pDC->FillRect(rcAllLabels, &CBrush(::GetSysColor(COLOR_HIGHLIGHT)));
	}
	else
		pDC->FillRect(rcAllLabels, &CBrush(m_clrTextBk));

// set color and mask for the icon

	if (lvi.state & LVIS_CUT)	{
		clrImage = m_clrBkgnd;
		uiFlags |= ILD_BLEND50;
	}	else if (bSelected) {
		clrImage = ::GetSysColor(COLOR_HIGHLIGHT);
		uiFlags |= ILD_BLEND50;
	}

// draw state icon

	UINT nStateImageMask = lvi.state & LVIS_STATEIMAGEMASK;
	if (nStateImageMask)	{
		int nImage = (nStateImageMask>>12) - 1;
		pImageList = listCtrl.GetImageList(LVSIL_STATE);
		if (pImageList)		{
			pImageList->Draw(pDC, nImage,
				CPoint(rcItem.left, rcItem.top), ILD_TRANSPARENT);
		}
	}

// draw normal and overlay icon

/*
	CRect rcIcon;
	ListCtrl.GetItemRect(nItem, rcIcon, LVIR_ICON);

	pImageList = listCtrl.GetImageList(LVSIL_SMALL);
	if (pImageList)	{
		UINT nOvlImageMask=lvi.state & LVIS_OVERLAYMASK;
		if (rcItem.left<rcItem.right-1)		{
			ImageList_DrawEx(pImageList->m_hImageList, lvi.iImage,
					pDC->m_hDC,rcIcon.left,rcIcon.top, 16, 16,
					m_clrBkgnd, clrImage, uiFlags | nOvlImageMask);
		}
	}
*/

// draw item label

//	listCtrl.GetItemRect(nItem, rcItem, LVIR_LABEL);
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH;

	listCtrl.GetItemRect(nItem, rcItem, LVIR_BOUNDS);
	listCtrl.GetColumn(0, &lvc); 

	rcItem.right = lvc.cx;

//	pszText = MakeShortString(pDC, szBuff,
//				rcItem.right-rcItem.left, 2*OFFSET_FIRST);
	pszText = szBuff;

	rcLabel = rcItem;
	rcLabel.left += OFFSET_FIRST;
	rcLabel.right -= OFFSET_FIRST;


	pDC->SetTextColor(rgb_black);
	pDC->DrawText(pszText,-1,rcLabel,DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);

// draw labels for extra columns
	if (itemSym) {
		UINT nOvlImageMask=lvi.state & LVIS_OVERLAYMASK;
// type
		listCtrl.GetColumn(1, &lvc); 
		rcItem.left = rcItem.right;
		rcItem.right += lvc.cx;

		rcLabel = rcItem;
		rcLabel.left += OFFSET_OTHER;
		rcLabel.right -= OFFSET_OTHER;

		string tnm = qut::unfind(typeIndex, (int)itemSym->type);
		if (tnm.size()) {
			pDC->SetTextColor(rgb_blue);
			long h = pDC->DrawText(tnm.c_str() , -1, rcLabel, DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);
		}
// length
		listCtrl.GetColumn(2, &lvc); 
		rcItem.left = rcItem.right;
		rcItem.right += lvc.cx;
//		fprintf(stderr, "%d %d %d\n", pszText, itemSym->type, lvc.cx);

		rcLabel = rcItem;
		rcLabel.left += OFFSET_OTHER;
		rcLabel.right -= OFFSET_OTHER;

		if (itemSym->indirection > 0) {
			char	buf[120];
			buf[0] = 0;
			for (short i=1; i<itemSym->indirection; i++) {
				char	nbuf[10];
				sprintf(nbuf, "%s%d", i>1?",":"",itemSym->size[i]);
				strcat(buf, nbuf);
			}
			pDC->SetTextColor(rgb_blue);
			pDC->DrawText(buf, -1, rcLabel,
				DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);
		}
// controller
		listCtrl.GetColumn(3, &lvc); 
		rcItem.left = rcItem.right;
		rcItem.right += lvc.cx;
		if (itemSym->controlMode != StabEnt::DISPLAY_NOT) {
			ImageList_DrawEx(images,
					itemSym->controlMode == StabEnt::DISPLAY_CTL ? 0 :
						itemSym->controlMode == StabEnt::DISPLAY_DISP? 2 : 0,
					pDC->m_hDC,rcItem.left,rcItem.top, 16, 16,
					m_clrBkgnd, clrImage, uiFlags | nOvlImageMask);
		}
// envelope
		listCtrl.GetColumn(4, &lvc); 
		rcItem.left = rcItem.right;
		rcItem.right += lvc.cx;
		if (itemSym->isEnveloped) {
			ImageList_DrawEx(images, 1 /* which one */,
					pDC->m_hDC,rcItem.left,rcItem.top, 16, 16,
					m_clrBkgnd, clrImage, uiFlags | nOvlImageMask);
		}
	}
/*
	for(int nColumn = 1; listCtrl.GetColumn(nColumn, &lvc); nColumn++)	{
		rcItem.left = rcItem.right;
		rcItem.right += lvc.cx;

		int nRetLen = listCtrl.GetItemText(nItem, nColumn,	szBuff, sizeof(szBuff));
		if (nRetLen == 0)
			continue;

//		pszText = MakeShortString(pDC, szBuff,
//			rcItem.right - rcItem.left, 2*OFFSET_OTHER);

		pszText = szBuff;

		UINT nJustify = DT_LEFT;

		if(pszText == szBuff) {
			switch(lvc.fmt & LVCFMT_JUSTIFYMASK) {
			case LVCFMT_RIGHT:
				nJustify = DT_RIGHT;
				break;
			case LVCFMT_CENTER:
				nJustify = DT_CENTER;
				break;
			default:
				break;
			}
		}

		rcLabel = rcItem;
		rcLabel.left += OFFSET_OTHER;
		rcLabel.right -= OFFSET_OTHER;

		pDC->DrawText(pszText, -1, rcLabel,
			nJustify | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);
	}*/

// draw focus rectangle if item has focus

	if (lvi.state & LVIS_FOCUSED && bFocus)
		pDC->DrawFocusRect(rcAllLabels);

// set original colors if item was selected

	if (bSelected) {
		pDC->SetTextColor(clrTextSave);
		pDC->SetBkColor(clrBkSave);
	}
}

/*
LPCTSTR CListViewEx::MakeShortString(CDC* pDC, LPCTSTR lpszLong, int nColumnLen, int nOffset)
{
	static const _TCHAR szThreeDots[] = _T("...");

	int nStringLen = lstrlen(lpszLong);

	if(nStringLen == 0 ||
		(pDC->GetTextExtent(lpszLong, nStringLen).cx + nOffset) <= nColumnLen)
	{
		return(lpszLong);
	}

	static _TCHAR szShort[MAX_PATH];

	lstrcpy(szShort,lpszLong);
	int nAddLen = pDC->GetTextExtent(szThreeDots,sizeof(szThreeDots)).cx;

	for(int i = nStringLen-1; i > 0; i--)
	{
		szShort[i] = 0;
		if((pDC->GetTextExtent(szShort, i).cx + nOffset + nAddLen)
			<= nColumnLen)
		{
			break;
		}
	}

	lstrcat(szShort, szThreeDots);
	return(szShort);
}*/

void
MFCSimpleTypeView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	bounds.right = cx;
	bounds.bottom = cy;
	frame.right = frame.left+cx;
	frame.bottom = frame.bottom+cy;
//	SetHVScroll();
}

void
MFCSimpleTypeView::OnSizing( UINT which, LPRECT r)
{
	CView::OnSizing(which, r);
}


void
MFCSimpleTypeView::OnBeginLabelEdit(LPNMHDR pnmhdr, LRESULT *result)
{
	LV_DISPINFO  *plvDispInfo = (LV_DISPINFO *)pnmhdr;
	LV_ITEM      *plvItem = &plvDispInfo->item;

	*result = 1; // non edit ... unless set to 0 below
	if (plvItem->iSubItem == 0) {
		StabEnt	*sym = SymForItem(plvItem->iItem);
		if (sym != NULL) {
			switch (sym->type) {
				default:
					*result = 0;
					break;
			}
		}
	}
}

void
MFCSimpleTypeView::OnEndLabelEdit(LPNMHDR pnmhdr, LRESULT *result)
{
	LV_DISPINFO  *plvDispInfo = (LV_DISPINFO *)pnmhdr;
	LV_ITEM      *plvItem = &plvDispInfo->item;

	*result = 0; // default action is to reject the edit 
	if (plvItem->iSubItem == 0 && plvItem->pszText != NULL && SymTab::ValidSymbolName(plvItem->pszText)) {
		*result = 0; // default action is to reject the edit 
		StabEnt	*sym = SymForItem(plvItem->iItem);
		if (sym != NULL) {
			StabEnt	*fnd = glob.findContextSymbol(plvItem->pszText, sym->context);
			if (fnd == NULL) {
				GetListCtrl().SetItemText(plvItem->iItem, plvItem->iSubItem, plvItem->pszText); // is this necessary????
				QuaPerceptualSet	*quaLink=parent->QuaLink();
				switch (sym->type) {
					default:
						if (quaLink) {
							quaLink->Rename(sym, plvItem->pszText);
						} else {
							glob.rename(sym, plvItem->pszText);
						}
						*result = 1;
						break;
				}
			}
		}
	}
}

void
MFCSimpleTypeView::OnMouseMove(UINT nFlags, CPoint point)
{/*
	if (draggingNow)
	{
		CTreeCtrl& tree = GetTreeCtrl();
		dragImageList->DragEnter(&tree, point);
		dragImageList->DragMove(point);
	}*/

	CListView::OnMouseMove(nFlags, point);
}

afx_msg BOOL
MFCSimpleTypeView::OnMouseWheel(
			UINT nFlags,
			short zDelta,
			CPoint point 
		)
{
	return CListView::OnMouseWheel(nFlags, zDelta, point);
}

void
MFCSimpleTypeView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CListView::OnLButtonDown(nFlags, point);
}

void
MFCSimpleTypeView::OnMButtonDown(UINT nFlags, CPoint point)
{
	CListView::OnMButtonDown(nFlags, point);
}

void
MFCSimpleTypeView::OnRButtonDown(UINT nFlags, CPoint point)
{
	CListView::OnRButtonDown(nFlags, point);
	ContextMenu(nFlags, point);
}

void
MFCSimpleTypeView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CListView::OnLButtonDblClk(nFlags, point);
}

void
MFCSimpleTypeView::OnMButtonDblClk(UINT nFlags, CPoint point)
{
	CListView::OnMButtonDblClk(nFlags, point);
}

void
MFCSimpleTypeView::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	CListView::OnRButtonDblClk(nFlags, point);
}

void
MFCSimpleTypeView::OnLButtonUp(UINT nFlags, CPoint point)
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
	CListView::OnLButtonUp(nFlags, point);
}


void
MFCSimpleTypeView::OnMButtonUp(UINT nFlags, CPoint point)
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
	CListView::OnMButtonUp(nFlags, point);
}

void
MFCSimpleTypeView::OnRButtonUp(UINT nFlags, CPoint point)
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
MFCSimpleTypeView::OnBeginDrag(NMHDR *pnmh, LRESULT* bHandled)
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
MFCSimpleTypeView::OnKeyDown(NMHDR *pNotifyStruct,LRESULT *result)
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


DROPEFFECT
MFCSimpleTypeView::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
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
MFCSimpleTypeView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	/*
	if (m_pDB)
		if (pDataObject->IsDataAvailable(m_nIDClipFormat))
			return DROPEFFECT_COPY;
*/
	return CListView::OnDragOver(pDataObject, dwKeyState, point);
}

BOOL
MFCSimpleTypeView::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
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
MFCSimpleTypeView::OnSelect(NMHDR *pNotifyStruct,LRESULT *result)
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
MFCSimpleTypeView::CreateList(CRect &r, MFCObjectView *v, UINT id)
{
	int ret = Create(NULL, "simple list",
				WS_CHILD|WS_VISIBLE|
				LVS_OWNERDRAWFIXED|LVS_REPORT|LVS_EDITLABELS|LVS_SHOWSELALWAYS,
				r, v, id, NULL);
	parent = v;
	return ret;
}

long
MFCSimpleTypeView::AddSymItem(const char * nm, LPARAM lp, int im)
{
	long ind = GetListCtrl().InsertItem(0, nm, im);
	if (ind >= 0) {
		long ret = GetListCtrl().SetItemData(ind, lp);
		if (!ret) {
			fprintf(stderr, "can't set clip symbol\n");
		}
	}
	return ind;
}


long
MFCSimpleTypeView::ItemForSym(StabEnt *sym)
{
	LVFINDINFO	lvf;
	lvf.lParam = (LPARAM) sym;
	lvf.flags = LVFI_PARAM;
	long ind = GetListCtrl().FindItem(&lvf);
	return ind;
}


StabEnt *
MFCSimpleTypeView::SymForItem(long item)
{
	if (item < 0) {
		return NULL;
	}
	DWORD_PTR lparam = GetListCtrl().GetItemData(item);
	if (lparam == NULL) {
		return NULL;
	}
	return (StabEnt *)lparam;
}

bool
MFCSimpleTypeView::RemoveSymbolsNotIn(vector<StabEnt*> &present)
{
	int hVar=-1, hNextVar=-1;
	hVar = GetListCtrl().GetNextItem(-1, LVNI_ALL);
	while (hVar >= 0) {

		DWORD_PTR lparam = GetListCtrl().GetItemData(hVar);
		hNextVar = -1;
		if (lparam != 0) {
			StabEnt	*s = (StabEnt *)lparam;
			auto it = find(present.begin(), present.end(), s);
			if (it != present.end()) {
				hNextVar = hVar;
				GetListCtrl().DeleteItem(hVar);
			}
		}
		if (hNextVar < 0) {
			hNextVar = GetListCtrl().GetNextItem(hVar, LVNI_ALL);
		}
// Try to get the next item
		hVar = hNextVar;
	}
	return true;
}

long
MFCSimpleTypeView::ItemAtPoint(CPoint &p)
{
	UINT	pFlags=0;
	long ind = GetListCtrl().HitTest(p, &pFlags);
	return ind;
}

long
MFCSimpleTypeView::AddSym(StabEnt *s)
{
	if (s == NULL) {
		return -1;
	}
	long ind=-1;

// these are best kept hidden for the moment
	if (	(s->isClone) ||	(s->isHidden)) {
		return -1;
	}
// these will be internal datum that we don't want people to fuck with

	switch (s->type) {
	case TypedValue:: S_TIME:
		case TypedValue::S_CHANNEL:

		case TypedValue::S_BYTE:
		case TypedValue::S_SHORT:
		case TypedValue::S_INT:
		case TypedValue::S_LONG:
		case TypedValue::S_FLOAT:
		case TypedValue::S_BOOL:
			if (s->refType == TypedValue::REF_POINTER) {
				return -1;
			}
			if (s->refType == TypedValue::REF_INSTANCE) {
				return -1;
			}
			if (s->refType == TypedValue::REF_VALUE) {
				return -1;
			}
			ind = AddSymItem(s->uniqueName(), (LPARAM)s, 0);
			break;
		case TypedValue::S_LAMBDA:
			ind = AddSymItem(s->uniqueName(), (LPARAM)s, 0);
			break;
		default:
			return -1;
	}
	return ind;
}

bool
MFCSimpleTypeView::RemoveSym(StabEnt *s)
{
	long ind = ItemForSym(s);
	if (ind < 0)
		return false;
	if (GetListCtrl().DeleteItem(ind)) {
		return true;
	}
	return false;
}

bool
MFCSimpleTypeView::SelectSym(StabEnt *s, bool sel)
{
	return false;
}

void
MFCSimpleTypeView::ContextMenu(UINT nFlags, CPoint point)
{
	long	caty[] = {
//				TypedValue::S_DOUBLE,
//				TypedValue::S_LAMBDA,
		TypedValue::S_FLOAT,
		TypedValue::S_INT,
		TypedValue::S_SHORT,
		TypedValue::S_BYTE,
		TypedValue::S_BOOL
			};
	CPoint		popPt = point;
	ClientToScreen(&popPt);

	CMenu		*ctxtMenu = new CMenu;
	ctxtMenu->CreatePopupMenu();
	CMenu		*addVarMenu = new CMenu;
	addVarMenu->CreatePopupMenu();

	char		buf[512];
	long	item = ItemAtPoint(point);
	for (short i=0; i<sizeof(caty)/sizeof(caty[0]); i++) {
		if (ID_VARIABLECONTEXT_ADD_VAR + caty[i] > ID_VARIABLECONTEXT_ADD_VAR_TYPE_RANGE) {
			reportError("Internal error. type index value exceeded in menu creator, is %d and shouldn't exceed %d",
					caty[i], ID_VARIABLECONTEXT_ADD_VAR_TYPE_RANGE-ID_VARIABLECONTEXT_ADD_VAR);
		} else {
			string tnm = qut::unfind(typeIndex, (int)caty[i]);
			if (tnm.size()) {
				sprintf(buf, "%s", tnm.c_str());
				addVarMenu->AppendMenu(MF_STRING, ID_VARIABLECONTEXT_ADD_VAR + caty[i], buf);
			}
		}
	}

	ctxtMenu->AppendMenu(MF_POPUP, (UINT) addVarMenu->m_hMenu, "add variable");
	ctxtMenu->AppendMenu(MF_STRING, ID_VARIABLECONTEXT_ADD_METHOD, "add method");
	StabEnt		*itemSym = NULL;
	if (item >= 0) {
// delete selected item
// set selected item envelope
// set selected item display type
// set selected item length ... to create an array
		itemSym = SymForItem(item);

		if (itemSym != NULL) {
			CMenu		*setVarTypeMenu = new CMenu;
			setVarTypeMenu->CreatePopupMenu();
			for (short i=0; i<sizeof(caty)/sizeof(caty[0]); i++) {
				if (ID_VARIABLECONTEXT_SET_VAR_TYPE + caty[i] > ID_VARIABLECONTEXT_SET_VAR_TYPE_RANGE) {
					reportError("Internal error. type index value exceeded in menu creator, is %d and shouldn't exceed %d",
							caty[i], ID_VARIABLECONTEXT_SET_VAR_TYPE_RANGE-ID_VARIABLECONTEXT_SET_VAR_TYPE);
				} else {
					string tnm = qut::unfind(typeIndex, (int)caty[i]);
					if (tnm.size() && itemSym->type != caty[i]) {
						sprintf(buf, "%s", tnm.c_str());
						setVarTypeMenu->AppendMenu(MF_STRING, ID_VARIABLECONTEXT_SET_VAR_TYPE + caty[i], buf);
					}
				}
			}

			sprintf(buf, "set '%s' type", itemSym->uniqueName());
			ctxtMenu->AppendMenu(MF_POPUP, (UINT) setVarTypeMenu->m_hMenu, buf);

			sprintf(buf, "delete '%s'", itemSym->uniqueName());
			ctxtMenu->AppendMenu(MF_STRING, ID_VARIABLECONTEXT_DELETE_SELECTED, buf);
			sprintf(buf, "set '%s' envelope", itemSym->uniqueName());
			ctxtMenu->AppendMenu(MF_STRING, ID_VARIABLECONTEXT_ENVELOPE_SELECTED, buf);

			CMenu		*dispVarMenu = new CMenu;
			dispVarMenu->CreatePopupMenu();
			dispVarMenu->AppendMenu(MF_STRING, ID_VARIABLECONTEXT_NODISPLAY_SELECTED, "none");
			dispVarMenu->AppendMenu(MF_STRING, ID_VARIABLECONTEXT_CONTROL_SELECTED, "control");
			dispVarMenu->AppendMenu(MF_STRING, ID_VARIABLECONTEXT_DISPLAY_SELECTED, "display");
			sprintf(buf, "set '%s' display", itemSym->uniqueName());
			ctxtMenu->AppendMenu(MF_POPUP, (UINT) dispVarMenu->m_hMenu, buf);

			ctxtMenu->AppendMenu(MF_STRING, ID_VARIABLECONTEXT_SET_LENGTH_SELECTED, "set length");
		}
	}

// add item
//    by type
// structures ????
	short ret = ctxtMenu->TrackPopupMenuEx(
						TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RETURNCMD,
						popPt.x, popPt.y, this, NULL);

	if (ret >= ID_VARIABLECONTEXT_ADD_VAR && ret <= ID_VARIABLECONTEXT_ADD_VAR_TYPE_RANGE) {
		long typ = ret - ID_VARIABLECONTEXT_ADD_VAR;
		if (parent && parent->Symbol()) {
			StabEnt	*pSym = parent->Symbol();
			Stackable	*stkbl = pSym->StackableValue();
			if (stkbl) {
				char	*vnm = "variable%d";
				switch(typ) {
				case TypedValue::S_BYTE: vnm = "bytevar"; break;
					case TypedValue::S_SHORT: vnm = "wordvar"; break;
					case TypedValue::S_INT: vnm = "intvar"; break;
					case TypedValue::S_LONG: vnm = "longvar"; break;
					case TypedValue::S_FLOAT: vnm = "floatvar"; break;
				}
				string	nmbuf = glob.makeUniqueName(pSym, vnm, 1);
				StabEnt	*nsym=DefineSymbol(nmbuf, typ, 0, 0, pSym, TypedValue::REF_STACK, false, false, StabEnt::DISPLAY_NOT);
				stkbl->ReAllocateChildren();
				AddSym(nsym);
			}
		}
	} else if (ret == ID_VARIABLECONTEXT_ADD_METHOD) {
		if (parent && parent->Symbol()) {
			StabEnt	*pSym = parent->Symbol();
			string nmbuf = glob.makeUniqueName(pSym, "action", 1);
			QuaPerceptualSet	*quaLink=parent->QuaLink();
			if (quaLink) {
                StabEnt *mSym = quaLink->CreateMethod(nmbuf, pSym);
				if (mSym) {
					AddSym(mSym);
					fprintf(stderr, "tried to add method element\n");
				}
			}
		}
	} else if (ret >= ID_VARIABLECONTEXT_SET_VAR_TYPE && ret <= ID_VARIABLECONTEXT_SET_VAR_TYPE_RANGE) {
		long typ = ret - ID_VARIABLECONTEXT_SET_VAR_TYPE;
		if (itemSym && parent && parent->Symbol()) {
			StabEnt	*pSym = parent->Symbol();
			Stackable	*stkbl = pSym->StackableValue();
			if (stkbl) {
				bool	validChange = false;
				switch(typ) {
				case TypedValue::S_BYTE: validChange = true; break;
					case TypedValue::S_SHORT: validChange = true; break;
					case TypedValue::S_INT: validChange = true; break;
					case TypedValue::S_LONG: validChange = true; break;
					case TypedValue::S_FLOAT: validChange = true; break;
				}
				if (validChange) {
					itemSym->type = typ;
					stkbl->ReAllocateChildren();
				}
			}
		}
	} else if (ret == ID_VARIABLECONTEXT_DELETE_SELECTED) {
		if (itemSym) {
			glob.DeleteSymbol(itemSym, true);
			RemoveSym(itemSym);
			if (parent && parent->Symbol()) {
				StabEnt	*pSym = parent->Symbol();
				Stackable	*stkbl = pSym->StackableValue();
				if (stkbl) {
					stkbl->ReAllocateChildren();
				}
			}
		}
	} else if (ret == ID_VARIABLECONTEXT_ENVELOPE_SELECTED) {
		if (itemSym) {
			itemSym->SetEnvelopeMode(true);
		}
	} else if (ret == ID_VARIABLECONTEXT_NODISPLAY_SELECTED) {
		if (itemSym) {
			itemSym->SetDisplayMode(StabEnt::DISPLAY_NOT);
// forces redisplay of this entry to show icon change
			GetListCtrl().SetItemText(item, 3, "N");
		}
	} else if (ret == ID_VARIABLECONTEXT_DISPLAY_SELECTED) {
		if (itemSym) {
			itemSym->SetDisplayMode(StabEnt::DISPLAY_DISP);
// forces redisplay of this entry
			GetListCtrl().SetItemText(item, 3, "D");
		}
	} else if (ret == ID_VARIABLECONTEXT_CONTROL_SELECTED) {
		if (itemSym) {
			itemSym->SetDisplayMode(StabEnt::DISPLAY_CTL);
// forces redisplay of this entry
			GetListCtrl().SetItemText(item, 3, "C");
		}
	} else if (ret == ID_VARIABLECONTEXT_SET_LENGTH_SELECTED) {
		reportError("Currently only undimensioned data are supported by this interface .... but they can be added manually by hand-editting the script");
	}
}
