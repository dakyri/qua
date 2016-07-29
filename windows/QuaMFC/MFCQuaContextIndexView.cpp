#include "qua_version.h"
// MFCArrangeView.cpp : implementation file
//
//#define _AFXDLL
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE
#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>

#include "QuaMFC.h"
#include "QuaContextMFCDoc.h"
#include "MFCQuaContextIndexView.h"
#include "QuaDrop.h"

#include "XBrowseForFolder.h"

#include "StdDefs.h"

#include "Qua.h"
#include "QuaEnvironment.h"
#include "Channel.h"
#include "Sample.h"
#include "Voice.h"
#include "Lambda.h"
#include "QuaPort.h"
#include "Sym.h"

#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define QCI_SYMBOL_LPARAM	1000

/////////////////////////////////////////////////////////////////////////////
// MFCQuaContextIndexView

IMPLEMENT_DYNCREATE(MFCQuaContextIndexView, CTreeView)

MFCQuaContextIndexView::MFCQuaContextIndexView():
	CTreeView()
{
	draggingNow		= FALSE;
	draggedItem		= NULL;
	dragImageList	= NULL;
	selectedItem	= NULL;
}

MFCQuaContextIndexView::~MFCQuaContextIndexView()
{
}


BEGIN_MESSAGE_MAP(MFCQuaContextIndexView, CTreeView)
	//{{AFX_MSG_MAP(MFCQuaContextIndexView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	ON_COMMAND(ID_VSTPLUGIN_VIEW, OnVstPluginView)
	ON_COMMAND(ID_VSTPLUGIN_DISABLE, OnVstPluginDisable)
	ON_COMMAND(ID_VSTPLUGIN_GLOBAL_SAVE, OnVstPluginSave)
	ON_COMMAND(ID_VSTPLUGIN_GLOBAL_RELOAD, OnVstPluginReload)
	ON_COMMAND(ID_VSTPLUGIN_GLOBAL_SET_DIRECTORY, OnVstPluginSetDirectory)
	ON_COMMAND(ID_VSTPLUGIN_GLOBAL_ADD_DIRECTORY, OnVstPluginSetDirectory)
	ON_COMMAND(ID_INSTANCE_DELETE, OnPopupDelete)
{ WM_NOTIFY + WM_REFLECT_BASE, (WORD)(int)TVN_SELCHANGED, 0, 0, AfxSigNotify_v, (AFX_PMSG)(static_cast<void (AFX_MSG_CALL CCmdTarget::*)(NMHDR*, LRESULT*) > (OnNodeSelect))},
{ WM_NOTIFY + WM_REFLECT_BASE, (WORD)(int)TVN_BEGINDRAG, 0, 0, AfxSigNotify_v, (AFX_PMSG)(static_cast<void (AFX_MSG_CALL CCmdTarget::*)(NMHDR*, LRESULT*) > (OnBeginDrag)) },
{ WM_NOTIFY + WM_REFLECT_BASE, (WORD)(int)TVN_ITEMEXPANDED, 0, 0, AfxSigNotify_v, (AFX_PMSG)(static_cast<void (AFX_MSG_CALL CCmdTarget::*)(NMHDR*, LRESULT*) > (OnNodeExpand)) },
{ WM_NOTIFY + WM_REFLECT_BASE, (WORD)(int)TVN_BEGINLABELEDIT, 0, 0, AfxSigNotify_v, (AFX_PMSG)(static_cast<void (AFX_MSG_CALL CCmdTarget::*)(NMHDR*, LRESULT*) > (OnBeginLabelEdit)) },
{ WM_NOTIFY + WM_REFLECT_BASE, (WORD)(int)TVN_ENDLABELEDIT, 0, 0, AfxSigNotify_v, (AFX_PMSG)(static_cast<void (AFX_MSG_CALL CCmdTarget::*)(NMHDR*, LRESULT*) > (OnEndLabelEdit)) },
{ WM_NOTIFY + WM_REFLECT_BASE, (WORD)(int)NM_RCLICK, 0, 0, AfxSigNotify_v, (AFX_PMSG)(static_cast<void (AFX_MSG_CALL CCmdTarget::*)(NMHDR*, LRESULT*) > (OnRightClick)) },
/*ON_NOTIFY_REFLECT(TVN_SELCHANGED,OnNodeSelect)
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG,OnBeginDrag)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED,OnNodeExpand)
	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT,OnBeginLabelEdit)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT,OnEndLabelEdit)
	ON_NOTIFY_REFLECT(NM_RCLICK,OnRightClick)*/
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MFCQuaContextIndexView drawing

void
MFCQuaContextIndexView::displayArrangementTitle(const char *nm)
{
	;
}

/////////////////////////////////////////////////////////////////////////////
// MFCQuaContextIndexView diagnostics

#ifdef _DEBUG
void MFCQuaContextIndexView::AssertValid() const
{
	CView::AssertValid();
}

void MFCQuaContextIndexView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// MFCQuaContextIndexView message handlers
/////////////////////////////////////////////////////////////////////////////
void
MFCQuaContextIndexView::OnInitialUpdate()
{
	CQuaContextMFCDoc	*qdoc = (CQuaContextMFCDoc *)GetDocument();
	if (qdoc == NULL) {
		reportError("QuaContextIndexView: initial update of context view mount finds a null context document");
//	} else if (qdoc->qua == NULL) {
//		reportError("QuaContextIndexView initial update finds a null sequencer");
	} else {	// set qua up with our hooks
//		quaLink = &qdoc->qua->display;
//		quaLink->AddIndexer(this);
//		top = AddTopLevelClass(qdoc->qua->sym->name, S_QUA, TVI_ROOT);
		quas = AddTopLevelClass("Arrangements", TypedValue::S_QUA, TVI_ROOT, 0);
		vstplugins = AddTopLevelClass("VST Plugins", TypedValue::S_VST_PLUGIN, TVI_ROOT, 0);
		builtins = AddTopLevelClass("Builtin", TypedValue::S_BUILTIN, TVI_ROOT, 0);
		methods = AddTopLevelClass("Methods", TypedValue::S_LAMBDA, TVI_ROOT, 0);
		templates = AddTopLevelClass("Templates", TypedValue::S_TEMPLATE, TVI_ROOT, 0);
		ports = AddTopLevelClass("Ports", TypedValue::S_PORT, TVI_ROOT, 0);

		AddAllIndexItems();
	}
}

/////////////////////////////////////////////////////////////////////////////
// MFCQuaContextIndexView members
/////////////////////////////////////////////////////////////////////////////
void
MFCQuaContextIndexView::addToSymbolIndex(StabEnt *s)
{
	if (s!= NULL) {
		switch (s->type) {
			case TypedValue::S_QUA: {
				HTREEITEM	it = IndexItemFor(s, quas);
				if (it == NULL) {
					it = AddIndexItem(s->uniqueName(), (LPARAM)s, quas, 2);
				}
				break;
			}
			case TypedValue::S_VST_PLUGIN: {
				HTREEITEM	it = IndexItemFor(s, vstplugins);
				if (it == NULL) {
#ifdef QUA_V_VST_HOST
					VstPlugin	*vp = s->VstValue();
					it = AddIndexItem(s->uniqueName(), (LPARAM)s, vstplugins,
							(vp->status==VST_PLUG_LOADED)?
								(vp->isSynth?11:3):
								(vp->isSynth?12:10));
					GetTreeCtrl().SortChildren(vstplugins);
#endif
				}
				break;
			}
			case TypedValue::S_BUILTIN: {
				HTREEITEM	it = IndexItemFor(s, builtins);
				if (it == NULL) {
					it = AddIndexItem(s->uniqueName(), (LPARAM)s, builtins, 5);
					GetTreeCtrl().SortChildren(builtins);
				}
				break;
			}
			case TypedValue::S_LAMBDA: {
				HTREEITEM	it = IndexItemFor(s, methods);
				if (it == NULL) {
					it = AddIndexItem(s->uniqueName(), (LPARAM)s, methods, 6);
					GetTreeCtrl().SortChildren(vstplugins);
				}
				break;
			}
			case TypedValue::S_TEMPLATE: {
				HTREEITEM	it = IndexItemFor(s, templates);
				if (it == NULL) {
					it = AddIndexItem(s->uniqueName(), (LPARAM)s, templates, 4);
				}
				break;
			}
			case TypedValue::S_PORT: {
				HTREEITEM	it = IndexItemFor(s, ports);
				if (it == NULL) {
					QuaPort	*p = s->PortValue();
					switch (p->deviceType) {
						case QuaPort::Device::JOYSTICK:
							it = AddIndexItem(s->uniqueName(), (LPARAM)s, ports, 9);
							break;
						case QuaPort::Device::AUDIO:
							it = AddIndexItem(s->uniqueName(), (LPARAM)s, ports, 8);
							break;
						default:
						case QuaPort::Device::MIDI:
							it = AddIndexItem(s->uniqueName(), (LPARAM)s, ports, 7);
							break;
					}
				}
				break;
			}
		}
	}
}

void
MFCQuaContextIndexView::removeFromSymbolIndex(StabEnt *s)
{
	HTREEITEM ht = IndexItemFor(s);
	if (ht != NULL) {
		GetTreeCtrl().DeleteItem(ht);
	}
}


void
MFCQuaContextIndexView::symbolNameChanged(StabEnt *s)
{
	HTREEITEM ht = IndexItemFor(s);
	if (ht != NULL) {
		GetTreeCtrl().SetItemText(ht, s->uniqueName());
	}
}

HTREEITEM
MFCQuaContextIndexView::AddTopLevelClass(char *s, LPARAM type, HTREEITEM parent, int img)
{
	HTREEITEM ht = GetTreeCtrl().InsertItem(
				TVIF_TEXT|TVIF_PARAM|TVIF_STATE|TVIF_IMAGE|TVIF_SELECTEDIMAGE, /* nMask */
				s,
				img, /*int nImage */
				img, /* int nSelectedImage*/
				0, /* UINT nState */
				0, /* UINT nStateMask */
				(LPARAM) type,
				parent,
				0 /*HTREEITEM hInsertAfter*/ 
			);
	return ht;
}


HTREEITEM
MFCQuaContextIndexView::AddIndexItem(const char *s, LPARAM type, HTREEITEM parent, int img)
{
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
MFCQuaContextIndexView::IndexItemFor(StabEnt *s, HTREEITEM parent)
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
MFCQuaContextIndexView::IndexItemFor(StabEnt *s)
{
	if (s!= NULL) {
		switch (s->type) {
			case TypedValue::S_QUA: {
				return IndexItemFor(s, quas);
				break;
			}
			case TypedValue::S_BUILTIN: {
				return IndexItemFor(s, builtins);
				break;
			}
			case TypedValue::S_VST_PLUGIN: {
				return IndexItemFor(s, vstplugins);
				break;
			}
			case TypedValue::S_LAMBDA: {
				return IndexItemFor(s, methods);
				break;
			}
			case TypedValue::S_TEMPLATE: {
				return IndexItemFor(s, templates);
				break;
			}
			case TypedValue::S_PORT: {
				return IndexItemFor(s, ports);
				break;
			}
		   /*
			case TypedValue::S_INSTANCE: {
				StabEnt	*p = s->context;
				fprintf(stderr, "adding instance item, parent %x %s\n", p, p->name);
				switch (p->type) {
					case S_VOICE: {
						HTREEITEM	ipt = IndexItemFor(p, voices);
						if (ipt != NULL) {
							return IndexItemFor(s, ipt);
						}
						break;
					}
					case S_SAMPLE: {
						HTREEITEM	ipt = IndexItemFor(p, samples);
						if (ipt != NULL) {
							return IndexItemFor(s, ipt);
						}
						break;
					}
					default: {
						break;
					}
				}
				break;
			}*/
		}
	}
	return NULL;
}

void
MFCQuaContextIndexView::AddAllIndexItems()
{
	for (int i=0; i<MAX_SYMBOLS; i++) {
		if (glob[i] != NULL && !glob[i]->isDeleted && glob[i]->context == NULL) {
			addToSymbolIndex(glob[i]);
		}
	}
}

afx_msg void
MFCQuaContextIndexView::OnBeginDrag(NMHDR *pnmh, LRESULT* bHandled)
{
	fprintf(stderr, "MFCQuaSymbolIndexView begin drag\n");
	NM_TREEVIEW* tvp = (NM_TREEVIEW*)pnmh;

	if (!draggingNow) {
		if  (tvp->itemNew.lParam > QCI_SYMBOL_LPARAM ) {
			StabEnt	*s = (StabEnt *)tvp->itemNew.lParam;
			bool	draggable = false;
			UINT	dragFormat = 0;
			switch (s->type) {
				case TypedValue::S_BUILTIN: {
					draggable = true;
					dragFormat = QuaDrop::builtinFormat;
					break;
				}
				case TypedValue::S_VST_PLUGIN: {
					draggable = true;
					dragFormat = QuaDrop::vstpluginFormat;
					break;
				}
				case TypedValue::S_LAMBDA: {	// move an instance
					draggable = true;
					dragFormat = QuaDrop::methodFormat;
					break;
				}
				case TypedValue::S_TEMPLATE: {
					draggable = true;
					dragFormat = QuaDrop::templateFormat;
					break;
				}
				case TypedValue::S_PORT: {
					draggable = true;
					dragFormat = QuaDrop::portFormat;
					break;
				}
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

	*bHandled = 0;
}

void
MFCQuaContextIndexView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (draggingNow)
	{
		CTreeCtrl& tree = GetTreeCtrl();
		dragImageList->DragEnter(&tree, point);
		dragImageList->DragMove(point);
	}

	CTreeView::OnMouseMove(nFlags, point);
}

void
MFCQuaContextIndexView::OnLButtonUp(UINT nFlags, CPoint point)
{
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

	CTreeView::OnLButtonUp(nFlags, point);
}

int
MFCQuaContextIndexView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	lpCreateStruct->style |= TVS_EDITLABELS;
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;

//	GetDocument()->m_pTreeView = this;

	// Create the Image List
	images.Create(
			16,
			15,
			ILC_COLOR8,
			13,
			1 
		);
	CBitmap bm;
	if (!bm.LoadBitmap(IDB_CTXT_IDX_IMG)) {
		fprintf(stderr, "failed to load bitmap\n");
	}
	if (images.Add(&bm, RGB(0, 0, 0)) < 0) {
		fprintf(stderr, "failed to add bitmap to img list\n");
	}
	images.SetBkColor(GetSysColor(COLOR_WINDOW));

	/// Attach image list to Tree
//	CTreeCtrlEx& ctlTree = (CTreeCtrlEx&) GetTreeCtrl();
	GetTreeCtrl().SetImageList(&images, TVSIL_NORMAL);

//	m_dropTarget.Register(this);

	return 0;
}

void
MFCQuaContextIndexView::OnSysColorChange()
{
	CWnd::OnSysColorChange();

	// Reset the background color of our image list when notified
//	m_ctlImage.SetBkColor(GetSysColor(COLOR_WINDOW));
}

void
MFCQuaContextIndexView::OnKeyDown(NMHDR *pNotifyStruct,LRESULT *result)
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
MFCQuaContextIndexView::OnNodeSelect(NMHDR *pNotifyStruct,LRESULT *result)
{
	*result = 0;
	NMTREEVIEW	*tvp = (NMTREEVIEW *)pNotifyStruct;
	fprintf(stderr, "cintext node select %x %x\n", tvp->itemOld.lParam, tvp->itemNew.lParam);

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
MFCQuaContextIndexView::OnNodeExpand(NMHDR *pNotifyStruct,LRESULT *result)
{
	*result = 0;
	fprintf(stderr, "node expand\n");

	NMTREEVIEW	*tvp = (NMTREEVIEW *)pNotifyStruct;
	if (tvp->itemNew.lParam > QCI_SYMBOL_LPARAM) {
		StabEnt *s = (StabEnt *)tvp->itemNew.lParam;
		/*switch (s->type) {
			
			case S_SAMPLE: {
				if (tvp->itemNew.state & TVIS_EXPANDED) {
					GetTreeCtrl().SetItemImage(tvp->itemNew.hItem, 7, 7);
				} else {
					GetTreeCtrl().SetItemImage(tvp->itemNew.hItem, 6, 6);
				}
				break;
			}
			case S_VOICE: {
				if (tvp->itemNew.state & TVIS_EXPANDED) {
					GetTreeCtrl().SetItemImage(tvp->itemNew.hItem, 9, 9);
				} else {
					GetTreeCtrl().SetItemImage(tvp->itemNew.hItem, 8, 8);
				}
				break;
			}
		}*/
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
MFCQuaContextIndexView::OnBeginLabelEdit(NMHDR *pNotifyStruct,LRESULT *result)
{
	fprintf(stderr, "label edit begins\n");
	NMTVDISPINFO	*tvp = (NMTVDISPINFO *)pNotifyStruct;
	HTREEITEM		it = tvp->item.hItem;
	
	selectedItem = it;
	GetTreeCtrl().Select(selectedItem, TVGN_CARET);
	DWORD_PTR		selectedData = GetTreeCtrl().GetItemData(selectedItem);
	if (selectedData > QCI_SYMBOL_LPARAM) {
		StabEnt		*sym = (StabEnt *)selectedData;
		switch (sym->type) {
			case TypedValue::S_QUA:
			case TypedValue::S_LAMBDA:
			case TypedValue::S_TEMPLATE:
			case TypedValue::S_VST_PLUGIN:
				*result = 0;
				break;
			default:
			case TypedValue::S_PORT:
			case TypedValue::S_BUILTIN:
				*result = 1;
				break;
		}
	} else {
        *result = 1;
	}
}


void
MFCQuaContextIndexView::OnEndLabelEdit(NMHDR *pNotifyStruct,LRESULT *result)
{
	NMTVDISPINFO	*tvp = (NMTVDISPINFO *)pNotifyStruct;
	HTREEITEM		it = tvp->item.hItem;

	cerr << "ctxt:: label edit end " << (tvp->item.pszText ? tvp->item.pszText:"")<< endl;
	DWORD_PTR		selectedData = GetTreeCtrl().GetItemData(selectedItem);
	if (selectedData > QCI_SYMBOL_LPARAM && SymTab::ValidSymbolName(tvp->item.pszText)) {
		StabEnt		*sym = (StabEnt *)selectedData;
		switch (sym->type) {
			case TypedValue::S_QUA:
				sym->QuaValue()->setName(tvp->item.pszText);
				*result = 1;
				break;
			case TypedValue::S_LAMBDA:
				glob.rename(sym, tvp->item.pszText);
				*result = 1;
				break;
			case TypedValue::S_TEMPLATE:
				glob.rename(sym, tvp->item.pszText);
				*result = 1;
				break;
			case TypedValue::S_VST_PLUGIN:
				glob.rename(sym, tvp->item.pszText);
				*result = 1;
				break;
			default:
			case TypedValue::S_PORT:
			case TypedValue::S_BUILTIN:
				*result = 0;
				break;
		}
	} else {
        *result = 0;
	}
}

void
MFCQuaContextIndexView::OnRightClick(NMHDR *pNotifyStruct,LRESULT *result)
{
	CTreeCtrl& ctl = (CTreeCtrl&) GetTreeCtrl();
	fprintf(stderr, "on right click\n");

	UINT nFlags;
	CPoint curPoint;
	GetCursorPos(&curPoint);
	ScreenToClient(&curPoint);
	selectedItem = ctl.HitTest(curPoint, &nFlags);
	ctl.Select(selectedItem, TVGN_CARET);

	DWORD_PTR selectedData = ctl.GetItemData(selectedItem);
	if (selectedData > QCI_SYMBOL_LPARAM) {
		StabEnt	*sym = (StabEnt *) selectedData;
		switch (sym->type) {
			case TypedValue::S_QUA:
				DoPopupMenu(IDR_QUA_RCLICK);
				break;
			case TypedValue::S_BUILTIN:
				DoPopupMenu(IDR_BUILTIN_RCLICK);
				break;
			case TypedValue::S_VST_PLUGIN:
				DoPopupMenu(IDR_VSTPLUGIN_RCLICK);
				break;
			case TypedValue::S_TEMPLATE:
				DoPopupMenu(IDR_TEMPLATE_RCLICK);
				break;
			case TypedValue::S_LAMBDA:
				DoPopupMenu(IDR_METHOD_RCLICK);
				break;
			case TypedValue::S_PORT:
				DoPopupMenu(IDR_PORT_RCLICK);
				break;
		}
	} else {
		switch (selectedData) {
			case TypedValue::S_VST_PLUGIN: {
				DoPopupMenu(IDR_VSTPLUGIN_GLOBAL_RCLICK);
				break;
			}
		}
	}
	*result = 0;
}


void
MFCQuaContextIndexView::DoPopupMenu(UINT nMenuID)
{
	CMenu popMenu;

	if (!popMenu.LoadMenu(nMenuID)) {
		fprintf(stderr, "load menu %d fails\n", nMenuID);
		return;
	}

	CPoint posMouse;
	GetCursorPos(&posMouse);

	CMenu	*sub = popMenu.GetSubMenu(0);
	if (sub == NULL) {
		fprintf(stderr, "load menu %d has no popup\n", nMenuID);
		return;
	}
	sub->TrackPopupMenu(0,posMouse.x,posMouse.y,this);
}

void
MFCQuaContextIndexView::OnPopupDelete()
{
	DWORD_PTR selectedData = GetTreeCtrl().GetItemData(selectedItem);
	if (selectedData > QCI_SYMBOL_LPARAM) {
		StabEnt	*sym = (StabEnt *) selectedData;
		fprintf(stderr, "Popup delete %s\n", sym->uniqueName());
		switch (sym->type) {
			case TypedValue::S_QUA:
//				quaLink->DeleteObject(sym);
				break;
		}
	}
}

void
MFCQuaContextIndexView::OnPopupControl()
{
	DWORD_PTR selectedData = GetTreeCtrl().GetItemData(selectedItem);
	if (selectedData > QCI_SYMBOL_LPARAM) {
		StabEnt	*sym = (StabEnt *) selectedData;
		switch (sym->type) {
			case TypedValue::S_PORT:
//				quaLink->ShowObjectRepresentation(sym);
				break;
		}
	}
}

void
MFCQuaContextIndexView::OnPopupEdit()
{
	DWORD_PTR selectedData = GetTreeCtrl().GetItemData(selectedItem);
	if (selectedData > QCI_SYMBOL_LPARAM) {
		StabEnt	*sym = (StabEnt *) selectedData;
		switch (sym->type) {
			case TypedValue::S_QUA:
//				quaLink->ShowObjectRepresentation(sym);
				break;
		}
	}
}

void
MFCQuaContextIndexView::OnVstPluginView()
{
	DWORD_PTR selectedData = GetTreeCtrl().GetItemData(selectedItem);
	if (selectedData > QCI_SYMBOL_LPARAM) {
		char	buf[1024];
		StabEnt	*sym = (StabEnt *) selectedData;
		switch (sym->type) {
			case TypedValue::S_VST_PLUGIN: {
				VstPlugin	*vst=sym->VstValue();
#ifdef QUA_V_VST_HOST
				reportError("%s\nFile: %s\n%d inputs\n%d outputs\n%d paramaters\n%d programs\n",
					sym->name, 
					vst->pluginExecutable.Path(),
					vst->numInputs,
					vst->numOutputs,
					vst->numParams,
					vst->numPrograms,
				buf);
#endif
//				quaLink->ShowObjectRepresentation(sym);
				break;
			}
		}
	}
}

void
MFCQuaContextIndexView::OnVstPluginSave()
{
	DWORD_PTR selectedData = GetTreeCtrl().GetItemData(selectedItem);
	if (selectedData > QCI_SYMBOL_LPARAM) {
		StabEnt	*sym = (StabEnt *) selectedData;
		switch (sym->type) {
			case TypedValue::S_VST_PLUGIN:
//				quaLink->ShowObjectRepresentation(sym);
				break;
		}
	}
}

void
MFCQuaContextIndexView::OnVstPluginDisable()
{
	DWORD_PTR selectedData = GetTreeCtrl().GetItemData(selectedItem);
	if (selectedData > QCI_SYMBOL_LPARAM) {
		StabEnt	*sym = (StabEnt *) selectedData;
		switch (sym->type) {
			case TypedValue::S_VST_PLUGIN:
//				quaLink->ShowObjectRepresentation(sym);
				break;
		}
	}
}

void
MFCQuaContextIndexView::OnVstPluginReload()
{
	DWORD_PTR selectedData = GetTreeCtrl().GetItemData(selectedItem);
	if (selectedData > QCI_SYMBOL_LPARAM) {
		StabEnt	*sym = (StabEnt *) selectedData;
		switch (sym->type) {
			case TypedValue::S_VST_PLUGIN:
//				quaLink->ShowObjectRepresentation(sym);
				break;
		}
	}
}

void
MFCQuaContextIndexView::OnVstPluginSetDirectory()
{
	fprintf(stderr, "set vst dir\n");
	DWORD_PTR selectedData = GetTreeCtrl().GetItemData(selectedItem);
	if (selectedData > QCI_SYMBOL_LPARAM) {
		StabEnt	*sym = (StabEnt *) selectedData;
		switch (sym->type) {
			case TypedValue::S_VST_PLUGIN: {
				char	buf[1024];
				if (XBrowseForFolder(m_hWnd, "", buf, 1024)) {
					environment.SetVstPluginDir(buf, false, true);
				}
				break;
			}
		}
	} else {
		switch (selectedData) {
			case TypedValue::S_VST_PLUGIN: {
				char	buf[1024];
				if (XBrowseForFolder(m_hWnd, "", buf, 1024)) {
					environment.SetVstPluginDir(buf, false, true);
				}
				break;
			}
		}
	}
}

void
MFCQuaContextIndexView::OnVstPluginAddDirectory()
{
	DWORD_PTR selectedData = GetTreeCtrl().GetItemData(selectedItem);
	if (selectedData > QCI_SYMBOL_LPARAM) {
		StabEnt	*sym = (StabEnt *) selectedData;
		switch (sym->type) {
			case TypedValue::S_VST_PLUGIN: {
				char	buf[1024];
				if (XBrowseForFolder(m_hWnd, "", buf, 1024)) {
					environment.SetVstPluginDir(buf, true, true);
				}
				break;
			}
		}
	} else {
		switch (selectedData) {
			case TypedValue::S_VST_PLUGIN: {
				char	buf[1024];
				if (XBrowseForFolder(m_hWnd, "", buf, 1024)) {
					environment.SetVstPluginDir(buf, true, true);
				}
				break;
			}
		}
	}
}

DROPEFFECT
MFCQuaContextIndexView::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
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
MFCQuaContextIndexView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	/*
	if (m_pDB)
		if (pDataObject->IsDataAvailable(m_nIDClipFormat))
			return DROPEFFECT_COPY;
*/
	return CTreeView::OnDragOver(pDataObject, dwKeyState, point);
}

BOOL
MFCQuaContextIndexView::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
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

