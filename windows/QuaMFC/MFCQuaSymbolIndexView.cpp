// MFCArrangeView.cpp : implementation file
//
//#define _AFXDLL
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE
#include "stdafx.h"
#include "qua_version.h"

#include <stdio.h>
#include <stdlib.h>

#include <afxole.h>

#include "StdDefs.h"

#include "QuaMFC.h"
#include "QuaMFCDoc.h"
#include "MFCQuaSymbolIndexView.h"
#include "QuaDrop.h"

#include "Qua.h"
#include "Channel.h"
#include "Sample.h"
#include "Voice.h"
#include "Lambda.h"

#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MFCArrangeView

IMPLEMENT_DYNCREATE(MFCQuaSymbolIndexView, CTreeView)

#define QSI_SYMBOL_LPARAM	1000

MFCQuaSymbolIndexView::MFCQuaSymbolIndexView():
	CTreeView()
{
	draggingNow		= FALSE;
	draggedItem		= NULL;
	dragImageList	= NULL;
	selectedItem	= NULL;

}

MFCQuaSymbolIndexView::~MFCQuaSymbolIndexView()
{
}


BEGIN_MESSAGE_MAP(MFCQuaSymbolIndexView, CTreeView)
	//{{AFX_MSG_MAP(MFCQuaSymbolIndexView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	ON_COMMAND(ID_CHANNEL_EDITOR, OnPopupEdit)
	ON_COMMAND(ID_CHANNEL_DELETE, OnPopupDelete)
	ON_COMMAND(ID_CHANNEL_CONTROLLER, OnPopupControl)
	ON_COMMAND(ID_SCHEDULABLE_EDITOR, OnPopupEdit)
	ON_COMMAND(ID_SCHEDULABLE_DELETE, OnPopupDelete)
	ON_COMMAND(ID_METHOD_EDITOR, OnPopupEdit)
	ON_COMMAND(ID_METHOD_DELETE, OnPopupDelete)
	ON_COMMAND(ID_INSTANCE_CONTROLLER, OnPopupControl)
	ON_COMMAND(ID_INSTANCE_DELETE, OnPopupDelete)
	ON_COMMAND(ID_EDIT_ADD_CHILD_METHOD, OnPopupAddMethod)
	ON_COMMAND(ID_EDITORCONTEXT_DEL_CLIP, OnPopupDelete)
	ON_COMMAND(ID_EDITORCONTEXT_DEL_MARKER, OnPopupDelete)
	ON_COMMAND(ID_EDITORCONTEXT_SELECT_CLIP,OnPopupSelectClip)
	ON_COMMAND(ID_EDITORCONTEXT_GOTOSTART_CLIP,OnPopupGotoStartClip)
	ON_COMMAND(ID_EDITORCONTEXT_GOTOEND_CLIP,OnPopupGotoEndClip)

{ WM_NOTIFY + WM_REFLECT_BASE, (WORD)(int)TVN_KEYDOWN, 0, 0, AfxSigNotify_v, (AFX_PMSG)(static_cast<void (AFX_MSG_CALL CCmdTarget::*)(NMHDR*, LRESULT*) > (OnKeyDown)) },
{ WM_NOTIFY + WM_REFLECT_BASE, (WORD)(int)TVN_SELCHANGED, 0, 0, AfxSigNotify_v, (AFX_PMSG)(static_cast<void (AFX_MSG_CALL CCmdTarget::*)(NMHDR*, LRESULT*) > (OnNodeSelect)) },
{ WM_NOTIFY + WM_REFLECT_BASE, (WORD)(int)TVN_BEGINDRAG, 0, 0, AfxSigNotify_v, (AFX_PMSG)(static_cast<void (AFX_MSG_CALL CCmdTarget::*)(NMHDR*, LRESULT*) > (OnBeginDrag)) },
{ WM_NOTIFY + WM_REFLECT_BASE, (WORD)(int)TVN_ITEMEXPANDED, 0, 0, AfxSigNotify_v, (AFX_PMSG)(static_cast<void (AFX_MSG_CALL CCmdTarget::*)(NMHDR*, LRESULT*) > (OnNodeExpand)) },
{ WM_NOTIFY + WM_REFLECT_BASE, (WORD)(int)TVN_BEGINLABELEDIT, 0, 0, AfxSigNotify_v, (AFX_PMSG)(static_cast<void (AFX_MSG_CALL CCmdTarget::*)(NMHDR*, LRESULT*) > (OnBeginLabelEdit)) },
{ WM_NOTIFY + WM_REFLECT_BASE, (WORD)(int)TVN_ENDLABELEDIT, 0, 0, AfxSigNotify_v, (AFX_PMSG)(static_cast<void (AFX_MSG_CALL CCmdTarget::*)(NMHDR*, LRESULT*) > (OnEndLabelEdit)) },
{ WM_NOTIFY + WM_REFLECT_BASE, (WORD)(int)NM_RCLICK, 0, 0, AfxSigNotify_v, (AFX_PMSG)(static_cast<void (AFX_MSG_CALL CCmdTarget::*)(NMHDR*, LRESULT*) > (OnRightClick)) },
{ WM_NOTIFY + WM_REFLECT_BASE, (WORD)(int)NM_DBLCLK, 0, 0, AfxSigNotify_v, (AFX_PMSG)(static_cast<void (AFX_MSG_CALL CCmdTarget::*)(NMHDR*, LRESULT*) > (OnDoubleClick)) },
{ WM_NOTIFY + WM_REFLECT_BASE, (WORD)(int)NM_DBLCLK, 0, 0, AfxSigNotify_v, (AFX_PMSG)(static_cast<void (AFX_MSG_CALL CCmdTarget::*)(NMHDR*, LRESULT*) > (OnRightDoubleClick)) },

	/*ON_NOTIFY_REFLECT(TVN_KEYDOWN,OnKeyDown)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED,OnNodeSelect)
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG,OnBeginDrag)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED,OnNodeExpand)
	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT ,OnBeginLabelEdit)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT ,OnEndLabelEdit)
	ON_NOTIFY_REFLECT(NM_RCLICK,OnRightClick)
	ON_NOTIFY_REFLECT(NM_DBLCLK,OnDoubleClick) 
	ON_NOTIFY_REFLECT(NM_RDBLCLK,OnRightDoubleClick) */
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MFCArrangeView drawing

void
MFCQuaSymbolIndexView::displayArrangementTitle(const char *nm)
{
	;
}

/////////////////////////////////////////////////////////////////////////////
// MFCArrangeView diagnostics

#ifdef _DEBUG
void MFCQuaSymbolIndexView::AssertValid() const
{
	CView::AssertValid();
}

void MFCQuaSymbolIndexView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// MFCQuaSymbolIndexView message handlers
/////////////////////////////////////////////////////////////////////////////
void
MFCQuaSymbolIndexView::OnInitialUpdate()
{
	CQuaMFCDoc	*qdoc = (CQuaMFCDoc *)GetDocument();
	if (qdoc == NULL) {
		reportError("SymbolIndexView: initial update of symbol index finds a null sequencer document");
	} else if (qdoc->qua == NULL) {
		reportError("SymbolIndexView: initial update finds a null sequencer");
	} else {	// set qua up with our hooks
		SetLinkage(&qdoc->display);
		quaLink->AddIndexer(this);
//		top = AddTopLevelClass(qdoc->qua->sym->name, S_QUA, TVI_ROOT);
		channels = AddTopLevelClass("Channels", TypedValue::S_CHANNEL, TVI_ROOT, 0);
		samples = AddTopLevelClass("Samples", TypedValue::S_SAMPLE, TVI_ROOT, 0);
		voices = AddTopLevelClass("Voices", TypedValue::S_VOICE, TVI_ROOT, 0);
		methods = AddTopLevelClass("Methods", TypedValue::S_LAMBDA, TVI_ROOT, 0);
		regions = AddTopLevelClass("Regions and Markers", TypedValue::S_CLIP, TVI_ROOT, 0);

		AddAllIndexItems();
	}
	GetTreeCtrl().ModifyStyle(TVS_DISABLEDRAGDROP,NULL);
}

/////////////////////////////////////////////////////////////////////////////
// MFCQuaSymbolIndexView members
/////////////////////////////////////////////////////////////////////////////
HTREEITEM
MFCQuaSymbolIndexView::AddToSymbolIndex(StabEnt *s, HTREEITEM iti)
{
	HTREEITEM	it = NULL;
	if (s!= NULL) {
		switch (s->type) {
			case TypedValue::S_CLIP: {
				it = IndexItemFor(s, iti);
				if (it == NULL) {
					Clip	*c = s->ClipValue(NULL);
					short	img=10;
					if (c && c->duration.ticks <= 0) {
						img = 11;
					} else {
						img = 10;
					}
					it = AddIndexItem(s->uniqueName(), (LPARAM)s, iti, img);
				}
				break;
			}
			case TypedValue::S_CHANNEL: {
				it = IndexItemFor(s, iti);
				if (it == NULL) {
					it = AddIndexItem(s->uniqueName(), (LPARAM)s, iti, 2);
				}
				break;
			}
			case TypedValue::S_SAMPLE: {
				it = IndexItemFor(s, iti);
				if (it == NULL) {
					it = AddIndexItem(s->uniqueName(), (LPARAM)s, iti, 6);
				}
				break;
			  }
			case TypedValue::S_VOICE: {
				it = IndexItemFor(s, iti);
				if (it == NULL) {
					it = AddIndexItem(s->uniqueName(), (LPARAM)s, iti, 8);
				}
				break;
			 }
			case TypedValue::S_LAMBDA: {
				it = IndexItemFor(s);
				if (it == NULL) {
					it = AddIndexItem(s->uniqueName(), (LPARAM)s, iti, 5);
				}
				break;
			  }
			case TypedValue::S_INSTANCE: {
				StabEnt	*p = s->context;
				fprintf(stderr, "adding instance item, parent %x %s\n", p, p->name.c_str());
				switch (p->type) {
				case TypedValue::S_VOICE: {
						if (iti != NULL) {
							it = IndexItemFor(s, iti);
							if (it == NULL) {
								it = AddIndexItem(s->uniqueName(), (LPARAM)s, iti, 4);
							}
						}
						break;
					}
					case TypedValue::S_SAMPLE: {
						if (iti != NULL) {
							it = IndexItemFor(s, iti);
							if (it == NULL) {
								it = AddIndexItem(s->uniqueName(), (LPARAM)s, iti, 3);
							}
						} else {
							fprintf(stderr, "parent index item not found\n");
						}
						break;
					}
					default: {
						break;
					}
				}
				break;
			}
		}
	}
	return it;
}

void
MFCQuaSymbolIndexView::addToSymbolIndex(StabEnt *s)
{
	if (s!= NULL) {
		switch (s->type) {
			case TypedValue::S_CLIP: {
				HTREEITEM	it = IndexItemFor(s, regions);
				if (it == NULL) {
					Clip	*c = s->ClipValue(NULL);
					short	img=10;
					if (c && c->duration.ticks <= 0) {
						img = 11;
					} else {
						img = 10;
					}
					it = AddIndexItem(s->uniqueName(), (LPARAM)s, regions, img);
				}
				break;
			}
			case TypedValue::S_CHANNEL: {
				HTREEITEM	it = IndexItemFor(s, channels);
				if (it == NULL) {
					it = AddIndexItem(s->uniqueName(), (LPARAM)s, channels, 2);
				}
				break;
			}
			case TypedValue::S_SAMPLE: {
				HTREEITEM	it = IndexItemFor(s, samples);
				if (it == NULL) {
					it = AddIndexItem(s->uniqueName(), (LPARAM)s, samples, 6);
				}
				break;
			  }
			case TypedValue::S_VOICE: {
				HTREEITEM	it = IndexItemFor(s, voices);
				if (it == NULL) {
					it = AddIndexItem(s->uniqueName(), (LPARAM)s, voices, 8);
				}
				break;
			 }
			case TypedValue::S_LAMBDA: {
				HTREEITEM	it = IndexItemFor(s);
				if (it == NULL) {
					HTREEITEM	pit = NULL;
					if (s->context == NULL || s->context->type == TypedValue::S_QUA) {
						it = AddIndexItem(s->uniqueName(), (LPARAM)s, methods, 5);
					} else {
						pit = IndexItemFor(s->context);
						if (pit == NULL) {
							pit = methods;
						}
						it = AddIndexItem(s->uniqueName(), (LPARAM)s, pit, 5);
					}
				}
				break;
			  }
			case TypedValue::S_INSTANCE: {
				StabEnt	*p = s->context;
				fprintf(stderr, "adding instance item, parent %x %s\n", p, p->name.c_str());
				switch (p->type) {
					case  TypedValue::S_VOICE: {
						HTREEITEM	ipt = IndexItemFor(p, voices);
						if (ipt != NULL) {
							HTREEITEM	it = IndexItemFor(s, ipt);
							if (it == NULL) {
								it = AddIndexItem(s->uniqueName(), (LPARAM)s, ipt, 4);
							}
						}
						break;
					}
					case  TypedValue::S_SAMPLE: {
						HTREEITEM	ipt = IndexItemFor(p, samples);
						if (ipt != NULL) {
							HTREEITEM	it = IndexItemFor(s, ipt);
							if (it == NULL) {
								it = AddIndexItem(s->uniqueName(), (LPARAM)s, ipt, 3);
							}
						} else {
							fprintf(stderr, "parent index item not found\n");
						}
						break;
					}
					default: {
						break;
					}
				}
				break;
			}
		}
	}
}

void
MFCQuaSymbolIndexView::removeFromSymbolIndex(StabEnt *s)
{
	HTREEITEM ht = IndexItemFor(s);
	if (ht != NULL) {
		GetTreeCtrl().DeleteItem(ht);
	}
}
#include <algorithm>

void
MFCQuaSymbolIndexView::updateClipIndexDisplay()
{
	StabEnt	*sym = quaLink->QuaSym();
	StabEnt	*cs = sym->children;
	vector<StabEnt*> clips;
	while (cs != NULL) {
		if (cs->type == TypedValue::S_CLIP) {
			clips.push_back(cs);
		}
		cs = cs->sibling;
	}
	vector<StabEnt*> present = quaLink->ClipSyms(0);

	long	i;
	for (i=0; i<present.size(); i++) {
		StabEnt	*s =present[i];
		if (s && s->type == TypedValue::S_CLIP) {
			fprintf(stderr, "\tadd %x %s\n", (unsigned)s, s->name.c_str());
			addToSymbolIndex(s);
		}
	}
	fprintf(stderr, "update master clip index display\n");
	HTREEITEM hItem=NULL;
	hItem = GetTreeCtrl().GetNextItem(regions, TVGN_CHILD);
	while (hItem != NULL) {
		TVITEM item;
		item.hItem = hItem;
		item.mask = TVIF_PARAM | TVIF_HANDLE;

		BOOL bWorked = GetTreeCtrl().GetItem(&item);
		if (item.lParam >= QSI_SYMBOL_LPARAM) {
			StabEnt	*s = (StabEnt *)item.lParam;
			if (s->type == TypedValue::S_CLIP) {
				fprintf(stderr, "\tfound %x %s\n", (unsigned)s, s->name.c_str());
				if (find(present.begin(), present.end(), s) != present.end()) {
					GetTreeCtrl().DeleteItem(hItem);
				}
			}
		}
		hItem = GetTreeCtrl().GetNextItem(hItem, TVGN_NEXT);
	}
}

void
MFCQuaSymbolIndexView::symbolNameChanged(StabEnt *s)
{
	HTREEITEM ht = IndexItemFor(s);
	if (ht != NULL) {
		GetTreeCtrl().SetItemText(ht, s->uniqueName());
	}
}


HTREEITEM
MFCQuaSymbolIndexView::AddTopLevelClass(char *s, LPARAM type, HTREEITEM parent, int img)
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
MFCQuaSymbolIndexView::AddIndexItem(const char *s, LPARAM type, HTREEITEM parent, int img)
{
	fprintf(stderr, "ai %s %x %x %d\n", s, (unsigned)type, (unsigned)parent, img);
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
MFCQuaSymbolIndexView::IndexItemFor(StabEnt *s, HTREEITEM parent)
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
MFCQuaSymbolIndexView::IndexItemFor(StabEnt *s)
{
	if (s!= NULL) {
		switch (s->type) {
		case TypedValue::S_CLIP: {
				return IndexItemFor(s, regions);
				break;
			}
			case TypedValue::S_CHANNEL: {
				return IndexItemFor(s, channels);
				break;
			}
			case TypedValue::S_SAMPLE: {
				return IndexItemFor(s, samples);
				break;
			  }
			case TypedValue::S_VOICE: {
				return IndexItemFor(s, voices);
				break;
			 }
			case TypedValue::S_LAMBDA: {
				if (s->context == NULL || s->context->type == TypedValue::S_QUA) {
					return IndexItemFor(s, methods);
				} else {
					return IndexItemFor(s, IndexItemFor(s->context));
				}
				break;
			  }
			case TypedValue::S_INSTANCE: {
				StabEnt	*p = s->context;
				fprintf(stderr, "adding instance item, parent %x %s\n", p, p->name.c_str());
				switch (p->type) {
					case TypedValue::S_VOICE: {
						HTREEITEM	ipt = IndexItemFor(p, voices);
						if (ipt != NULL) {
							return IndexItemFor(s, ipt);
						}
						break;
					}
					case TypedValue::S_SAMPLE: {
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
			}
		}
	}
	return NULL;
}

#include <vector>
using namespace std;
void
MFCQuaSymbolIndexView::AddAllIndexItems()
{
	for (unsigned i=0; i<quaLink->NChannel(); i++) {
		addToSymbolIndex(quaLink->ChannelSym(i));
	}
	vector<StabEnt*> scheds = quaLink->SchedulableSyms();
	for (unsigned i = 0; i<scheds.size(); i++) {
		StabEnt	*sym = scheds[i];
		if (sym) {
			addToSymbolIndex(sym);
			HTREEITEM	mit = IndexItemFor(sym);
			AddMethodIndexItems(sym, mit);
			AddInstanceIndexItems(sym, mit);
		}
	}
	AddMethodIndexItems(quaLink->QuaSym(), methods);
	vector<StabEnt*> clips = quaLink->ClipSyms(0);
	for (short i = 0; i<clips.size(); i++) {
		StabEnt	*sym = clips[i];
		if (sym != nullptr) {
			addToSymbolIndex(sym);
		}
	}
}

void
MFCQuaSymbolIndexView::AddMethodIndexItems(StabEnt *sym, HTREEITEM it)
{
	vector<StabEnt*> meths = quaLink->MethodSyms(sym);
	for (unsigned i=0; i<meths.size(); i++) {
		StabEnt	*sym = meths[i];
		if (sym) {
			HTREEITEM	mit = AddToSymbolIndex(sym, it);
			AddMethodIndexItems(sym, mit);
		}
	}
}

void
MFCQuaSymbolIndexView::AddInstanceIndexItems(StabEnt *sym, HTREEITEM it)
{
	vector<StabEnt*> insts = quaLink->InstanceSyms(sym);
	for (unsigned i = 0; i<insts.size(); i++) {
		StabEnt	*sym = (StabEnt *)insts[i];
		if (sym != nullptr) {
			HTREEITEM	mit = AddToSymbolIndex(sym, it);
		}
	}
}

afx_msg void
MFCQuaSymbolIndexView::OnBeginDrag(NMHDR *pnmh, LRESULT* bHandled)
{
	fprintf(stderr, "MFCQuaSymbolIndexView begin drag\n");
	NM_TREEVIEW* tvp = (NM_TREEVIEW*)pnmh;

	if (!draggingNow) {
		if  (tvp->itemNew.lParam > QSI_SYMBOL_LPARAM ) {
			StabEnt	*s = (StabEnt *)tvp->itemNew.lParam;
			bool	draggable = false;
			UINT	dragFormat = 0;
			switch (s->type) {
			case TypedValue::S_CLIP:	// move an instance
					draggable = true;
					dragFormat = QuaDrop::clipFormat;
					break;
				case TypedValue::S_INSTANCE:	// move an instance
					draggable = true;
					dragFormat = QuaDrop::instanceFormat;
					break;
				case TypedValue::S_SAMPLE:	// create a sample instance
					draggable = true;
					dragFormat = QuaDrop::sampleFormat;
					break;
				case TypedValue::S_VOICE:	// create a voice instance
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

	*bHandled = 0;
}

void
MFCQuaSymbolIndexView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (draggingNow)
	{
		CTreeCtrl& tree = GetTreeCtrl();
		dragImageList->DragEnter(&tree, point);
		dragImageList->DragMove(point);
	}

	CTreeView::OnMouseMove(nFlags, point);
}

void MFCQuaSymbolIndexView::OnLButtonUp(UINT nFlags, CPoint point)
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
MFCQuaSymbolIndexView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	lpCreateStruct->style |= TVS_EDITLABELS|TVS_HASBUTTONS;
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;

//	GetDocument()->m_pTreeView = this;

	// Create the Image List
	images.Create(
			16,
			15,
			ILC_COLOR8,
			12,
			1 
		);
	CBitmap bm;
	bm.LoadBitmap(IDB_SYM_IDX_IMG);
	images.Add(&bm, RGB(0, 0, 0));
	images.SetBkColor(GetSysColor(COLOR_WINDOW));

	/// Attach image list to Tree
//	CTreeCtrlEx& ctlTree = (CTreeCtrlEx&) GetTreeCtrl();
	GetTreeCtrl().SetImageList(&images, TVSIL_NORMAL);

//	m_dropTarget.Register(this);

	return 0;
}

void
MFCQuaSymbolIndexView::OnSysColorChange()
{
	CWnd::OnSysColorChange();

	// Reset the background color of our image list when notified
//	m_ctlImage.SetBkColor(GetSysColor(COLOR_WINDOW));
}

void
MFCQuaSymbolIndexView::OnKeyDown(NMHDR *pNotifyStruct,LRESULT *result)
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


void
MFCQuaSymbolIndexView::OnNodeSelect(NMHDR *pNotifyStruct,LRESULT *result)
{
	*result = 0;
	NMTREEVIEW	*tvp = (NMTREEVIEW *)pNotifyStruct;
	fprintf(stderr, "symbol index node select %x %x\n", tvp->itemOld.lParam, tvp->itemNew.lParam);

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
MFCQuaSymbolIndexView::OnNodeExpand(NMHDR *pNotifyStruct,LRESULT *result)
{
	*result = 0;
	fprintf(stderr, "node expand\n");

	NMTREEVIEW	*tvp = (NMTREEVIEW *)pNotifyStruct;
	if (tvp->itemNew.lParam > QSI_SYMBOL_LPARAM) {
		StabEnt *s = (StabEnt *)tvp->itemNew.lParam;
		switch (s->type) {
		case TypedValue::S_SAMPLE: {
				if (tvp->itemNew.state & TVIS_EXPANDED) {
					GetTreeCtrl().SetItemImage(tvp->itemNew.hItem, 7, 7);
				} else {
					GetTreeCtrl().SetItemImage(tvp->itemNew.hItem, 6, 6);
				}
				break;
			}
			case TypedValue::S_VOICE: {
				if (tvp->itemNew.state & TVIS_EXPANDED) {
					GetTreeCtrl().SetItemImage(tvp->itemNew.hItem, 9, 9);
				} else {
					GetTreeCtrl().SetItemImage(tvp->itemNew.hItem, 8, 8);
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


}


void
MFCQuaSymbolIndexView::OnBeginLabelEdit(NMHDR *pNotifyStruct,LRESULT *result)
{
	*result = 1;
	NMTVDISPINFO	*tvp = (NMTVDISPINFO *)pNotifyStruct;
	HTREEITEM		it = tvp->item.hItem;
	
	selectedItem = it;
	GetTreeCtrl().Select(selectedItem, TVGN_CARET);
	DWORD_PTR		selectedData = GetTreeCtrl().GetItemData(selectedItem);
	if (selectedData > QSI_SYMBOL_LPARAM) {
		StabEnt		*sym = (StabEnt *)selectedData;
		switch (sym->type) {
			case  TypedValue::S_SAMPLE:
				*result = 0;
				break;
			case  TypedValue::S_LAMBDA:
				*result = 0;
				break;
			case  TypedValue::S_VOICE:
				*result = 0;
				break;
			case TypedValue::S_INSTANCE:
				*result = 0;
				break;
			case  TypedValue::S_CHANNEL:
				*result = 0;
				break;
			case TypedValue::S_CLIP:
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
MFCQuaSymbolIndexView::OnEndLabelEdit(NMHDR *pNotifyStruct,LRESULT *result)
{
	NMTVDISPINFO	*tvp = (NMTVDISPINFO *)pNotifyStruct;
	HTREEITEM		it = tvp->item.hItem;

	string name;

//	fprintf(stderr, "label edit end %s\n", tvp->item.pszText?tvp->item.pszText:"");
	DWORD_PTR		selectedData = GetTreeCtrl().GetItemData(selectedItem);
	if (selectedData > QSI_SYMBOL_LPARAM && (name=SymTab::MakeValidSymbolName(tvp->item.pszText)).size()) {
		StabEnt		*sym = (StabEnt *)selectedData;
		switch (sym->type) {
			case  TypedValue::S_LAMBDA:
				quaLink->Rename(sym, name);
				*result = 1;
				break;
			case  TypedValue::S_SAMPLE:
				quaLink->Rename(sym, name);
				*result = 1;
				break;
			case  TypedValue::S_VOICE:
				quaLink->Rename(sym, name);
				*result = 1;
				break;
			case  TypedValue::S_INSTANCE:
				quaLink->Rename(sym, name);
				*result = 1;
				break;
			case TypedValue::S_CHANNEL:
				quaLink->Rename(sym, name);
				*result = 1;
				break;
			case TypedValue::S_CLIP:
				quaLink->Rename(sym, name);
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
MFCQuaSymbolIndexView::OnRightClick(NMHDR *pNotifyStruct,LRESULT *result)
{
	CTreeCtrl& ctl = (CTreeCtrl&) GetTreeCtrl();
	cout << "on right click\n";

	UINT nFlags;
	CPoint curPoint;
	GetCursorPos(&curPoint);
	ScreenToClient(&curPoint);
	selectedItem = ctl.HitTest(curPoint, &nFlags);
	if (selectedItem == nullptr) {
		cout << "symbol index view right click but null selection " << endl;
		return;
	}
	ctl.Select(selectedItem, TVGN_CARET);

	if (selectedItem == channels) {
		DoPopupMenu(IDR_CHANNEL_TOP_RCLICK);
	} else if (selectedItem == voices) {
		DoPopupMenu(IDR_VOICE_TOP_RCLICK);
	} else if (selectedItem == samples) {
		DoPopupMenu(IDR_SAMPLE_TOP_RCLICK);
	} else if (selectedItem == methods) {
		DoPopupMenu(IDR_METHOD_TOP_RCLICK);
	} else if (selectedItem == regions) {
		DoPopupMenu(IDR_MAINCLIP_TOP_RCLICK);
	} else {
		DWORD_PTR selectedData = ctl.GetItemData(selectedItem);
		if (selectedData > QSI_SYMBOL_LPARAM) {
			StabEnt	*sym = (StabEnt *) selectedData;
			switch (sym->type) {
				case TypedValue::S_LAMBDA:
					DoPopupMenu(IDR_METHOD_RCLICK);
					break;
				case TypedValue::S_SAMPLE:
					DoPopupMenu(IDR_SCHEDULABLE_RCLICK);
					break;
				case TypedValue::S_VOICE:
					DoPopupMenu(IDR_SCHEDULABLE_RCLICK);
					break;
				case TypedValue::S_INSTANCE:
					DoPopupMenu(IDR_INSTANCE_RCLICK);
					break;
				case  TypedValue::S_CHANNEL:
					DoPopupMenu(IDR_CHANNEL_RCLICK);
					break;
				case  TypedValue::S_CLIP: {
					Clip	*c = sym->ClipValue(NULL);
					if (c->duration.ticks <= 0) {
						DoPopupMenu(IDR_MAINMARK_RCLICK);
					} else {
						DoPopupMenu(IDR_MAINCLIP_RCLICK);
					}
					break;
				}
			}
		}
	}
	*result = 0;
}


void
MFCQuaSymbolIndexView::OnDoubleClick(NMHDR *pNotifyStruct,LRESULT *result)
{
	CTreeCtrl& ctl = (CTreeCtrl&) GetTreeCtrl();
	fprintf(stderr, "on double click\n");

	UINT nFlags;
	CPoint curPoint;
	GetCursorPos(&curPoint);
	ScreenToClient(&curPoint);
	selectedItem = ctl.HitTest(curPoint, &nFlags);
	ctl.Select(selectedItem, TVGN_CARET);

	if (selectedItem == channels) {
	} else if (selectedItem == voices) {
	} else if (selectedItem == samples) {
	} else if (selectedItem == methods) {
	} else if (selectedItem == regions) {
	} else {
		DWORD_PTR selectedData = ctl.GetItemData(selectedItem);
		if (selectedData > QSI_SYMBOL_LPARAM) {
			StabEnt	*sym = (StabEnt *) selectedData;
			switch (sym->type) {
				case TypedValue::S_CLIP: {
					quaLink->GotoStartOfClip(sym);
					break;
				}
			}
		}
	}
}

void
MFCQuaSymbolIndexView::OnRightDoubleClick(NMHDR *pNotifyStruct,LRESULT *result)
{
	CTreeCtrl& ctl = (CTreeCtrl&) GetTreeCtrl();
	fprintf(stderr, "on right double click\n");

	UINT nFlags;
	CPoint curPoint;
	GetCursorPos(&curPoint);
	ScreenToClient(&curPoint);
	selectedItem = ctl.HitTest(curPoint, &nFlags);
	ctl.Select(selectedItem, TVGN_CARET);

	if (selectedItem == channels) {
	} else if (selectedItem == voices) {
	} else if (selectedItem == samples) {
	} else if (selectedItem == methods) {
	} else if (selectedItem == regions) {
	} else {
		DWORD_PTR selectedData = ctl.GetItemData(selectedItem);
		if (selectedData > QSI_SYMBOL_LPARAM) {
			StabEnt	*sym = (StabEnt *) selectedData;
			switch (sym->type) {
			}
		}
	}
}


void
MFCQuaSymbolIndexView::DoPopupMenu(UINT nMenuID)
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
MFCQuaSymbolIndexView::OnPopupAddMethod()
{
	DWORD_PTR selectedData = GetTreeCtrl().GetItemData(selectedItem);
	if (selectedData > QSI_SYMBOL_LPARAM) {
		StabEnt	*pSym = (StabEnt *) selectedData;
		fprintf(stderr, "Popup add child method %s\n", pSym->uniqueName());
		switch (pSym->type) {
			case TypedValue::S_LAMBDA:
			case TypedValue::S_CHANNEL:
			case TypedValue::S_VOICE:
			case TypedValue::S_SAMPLE: {
				string nmbuf = glob.makeUniqueName(pSym, "action", 1);
				if (quaLink) {
					StabEnt *mSym = quaLink->CreateMethod(nmbuf.c_str(), pSym);
					if (mSym) {
//						AddSym(mSym);
					}
				}
				break;
			}
			case TypedValue::S_INSTANCE:
// doesn't do this
				break;
		}
	}
}

void
MFCQuaSymbolIndexView::OnPopupDelete()
{
	DWORD_PTR selectedData = GetTreeCtrl().GetItemData(selectedItem);
	if (selectedData > QSI_SYMBOL_LPARAM) {
		StabEnt	*sym = (StabEnt *) selectedData;
		fprintf(stderr, "Popup delete %s\n", sym->uniqueName());
		switch (sym->type) {
			case TypedValue::S_LAMBDA:
				quaLink->DeleteObject(sym);
				break;
			case TypedValue::S_CHANNEL:
				quaLink->DeleteObject(sym);
				break;
			case TypedValue::S_VOICE:
				quaLink->DeleteObject(sym);
				break;
			case TypedValue::S_SAMPLE:
				quaLink->DeleteObject(sym);
				break;
			case TypedValue::S_CLIP:
				quaLink->DeleteObject(sym);
				break;
			case  TypedValue::S_INSTANCE:
				quaLink->DeleteObject(sym);
				break;
		}
	}
}

void
MFCQuaSymbolIndexView::OnPopupControl()
{
	DWORD_PTR selectedData = GetTreeCtrl().GetItemData(selectedItem);
	if (selectedData > QSI_SYMBOL_LPARAM) {
		StabEnt	*sym = (StabEnt *) selectedData;
		switch (sym->type) {
			case  TypedValue::S_INSTANCE:
				quaLink->ShowObjectRepresentation(sym);
				break;
			case  TypedValue::S_CHANNEL:
				quaLink->ShowObjectRepresentation(sym);
				break;
		}
	}
}

void
MFCQuaSymbolIndexView::OnPopupSelectClip()
{
	DWORD_PTR selectedData = GetTreeCtrl().GetItemData(selectedItem);
	if (selectedData > QSI_SYMBOL_LPARAM) {
		StabEnt	*sym = (StabEnt *) selectedData;
		switch (sym->type) {
			case TypedValue::S_CLIP: {
				quaLink->SelectRegion(sym);
				break;
			}
		}
	}
}

void
MFCQuaSymbolIndexView::OnPopupGotoStartClip()
{
	DWORD_PTR selectedData = GetTreeCtrl().GetItemData(selectedItem);
	if (selectedData > QSI_SYMBOL_LPARAM) {
		StabEnt	*sym = (StabEnt *) selectedData;
		switch (sym->type) {
			case TypedValue::S_CLIP: {
				quaLink->GotoStartOfClip(sym);
				break;
			}
		}
	}
}

void
MFCQuaSymbolIndexView::OnPopupGotoEndClip()
{
	DWORD_PTR selectedData = GetTreeCtrl().GetItemData(selectedItem);
	if (selectedData > QSI_SYMBOL_LPARAM) {
		StabEnt	*sym = (StabEnt *) selectedData;
		switch (sym->type) {
			case  TypedValue::S_CLIP: {
				quaLink->GotoEndOfClip(sym);
				break;
			}
		}
	}
}

void
MFCQuaSymbolIndexView::OnPopupEdit()
{
	DWORD_PTR selectedData = GetTreeCtrl().GetItemData(selectedItem);
	if (selectedData > QSI_SYMBOL_LPARAM) {
		StabEnt	*sym = (StabEnt *) selectedData;
		switch (sym->type) {
			case TypedValue::S_LAMBDA:
				if (sym->context == NULL || sym->context->type == TypedValue::S_QUA) {
					quaLink->ShowObjectRepresentation(sym);
				} else { // show parent and scroll to right bit
					;
				}
				break;
			case TypedValue::S_SAMPLE:
				quaLink->ShowObjectRepresentation(sym);
				break;
			case TypedValue::S_VOICE:
				quaLink->ShowObjectRepresentation(sym);
				break;
			case TypedValue::S_CHANNEL:
				quaLink->ShowObjectRepresentation(sym);
				break;
		}
	}
}


DROPEFFECT
MFCQuaSymbolIndexView::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
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
MFCQuaSymbolIndexView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	/*
	if (m_pDB)
		if (pDataObject->IsDataAvailable(m_nIDClipFormat))
			return DROPEFFECT_COPY;
*/
	return CTreeView::OnDragOver(pDataObject, dwKeyState, point);
}

BOOL
MFCQuaSymbolIndexView::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
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

