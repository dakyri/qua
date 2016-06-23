// ChildFrm.cpp : implementation of the QuaChildFrame class
//
//  this is the frame for the view/s through which a particular open Qua document is
// viewed and accessed.

#define _AFXDLL

#include "stdafx.h"

#include "qua_version.h"

#include "QuaMFC.h"

#include "StdDefs.h"

#include "ChildFrm.h"
#include "QuaMFCView.h"
#include "MFCChannelMountView.h"
#include "MFCChannelView.h"
#include "MFCArrangeView.h"
#include "MFCQuaSymbolIndexView.h"
#include "MFCObjectMountView.h"
#include "QuaMFCDoc.h"

#include "Qua.h"

extern HINSTANCE quaAppInstance;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MIN_OBJ_VIEW_HEIGHT 10
#define INI_OBJ_VIEW_HEIGHT 150
#define MIN_ARRANGER_HEIGHT	10
#define INI_ARRANGER_HEIGHT	150
#define MIN_SYM_INDEX_WIDTH	30
#define INI_SYM_INDEX_WIDTH	100
#define MIN_CHANNEL_MOUNT_WIDTH	30
#define INI_CHANNEL_MOUNT_WIDTH	130

extern FILE *trace_fp;
/////////////////////////////////////////////////////////////////////////////
// QuaChildFrame

IMPLEMENT_DYNCREATE(QuaChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(QuaChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(QuaChildFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
	ON_COMMAND_EX(ID_VIEW_ARRANGERTOOLS,OnViewToolBar)
	ON_COMMAND_EX(ID_ARTOOL_RGSELECT,OnSelectArrangerTool)
	ON_COMMAND_EX(ID_ARTOOL_POINT,OnSelectArrangerTool)
	ON_COMMAND_EX(ID_ARTOOL_SLICE,OnSelectArrangerTool)
	ON_COMMAND_EX(ID_ARTOOL_DRAW,OnSelectArrangerTool)
	ON_COMMAND(ID_EDIT_CUT,OnEditCut)
	ON_COMMAND(ID_EDIT_COPY,OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE,OnEditPaste)
	ON_COMMAND(ID_EDIT_UNDO,OnEditUndo)
//	ON_UPDATE_COMMAND_UI_RANGE(ID_ARTOOL_BASE,ID_ARTOOL_LAST_BUTTON, OnUpdateUIArrangerTools)
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNeedsTxt)
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipNeedsTxt)
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipNeedsTxt)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// QuaChildFrame construction/destruction
/////////////////////////////////////////////////////////////////////////////

QuaChildFrame::QuaChildFrame()
{
	width = -1;
	height = -1;
	document = NULL;
	quaLink = NULL;
}

QuaChildFrame::~QuaChildFrame()
{
}


BOOL QuaChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}


afx_msg BOOL
QuaChildFrame::OnToolTipNeedsTxt( UINT id, NMHDR * pTTTStruct, LRESULT * pResult )
{
	NMTTDISPINFO *ttip = ((NMTTDISPINFO*)pTTTStruct);
//	ReportError("%x", ttip->lpszText);
//	wsprintf(ttip->lpszText,"hmmm");
//	ttip->hinst = quaAppInstance;
//	strcpy(((NMTTDISPINFO*)pTTTStruct)->szText, "hmmm");
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// QuaChildFrame diagnostics
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
void QuaChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void QuaChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// QuaChildFrame message handlers
/////////////////////////////////////////////////////////////////////////////

BOOL
QuaChildFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	EnableDocking(CBRS_ALIGN_ANY);

	document = (CQuaMFCDoc *)pContext->m_pCurrentDoc;

	if (!transportBar.CreateEx(
			this,
			TBSTYLE_FLAT,
			WS_CHILD | WS_VISIBLE | CBRS_TOP |
				CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC)) {
		TRACE0("Failed to create transport bar\n");
		return -1;      // fail to create
	}
	transportBar.EnableDocking(CBRS_ALIGN_ANY);

	if (!arrangerToolBar.CreateEx(
			this,
			TBSTYLE_FLAT,
			WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | 
				CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC,
			CRect(0,0,0,0),
			IDR_ARRANGER_TOOL)) {
		TRACE0("Failed to create arranger tools bar\n");
		return -1;      // fail to create
	}
	arrangerToolBar.EnableDocking(CBRS_ALIGN_ANY);

	DockControlBar(&transportBar);
	RecalcLayout(true);
	CRect	tbRect;
	transportBar.GetWindowRect(&tbRect);
	tbRect.left = tbRect.right;
	tbRect.right = tbRect.left+600;
	DockControlBar(&arrangerToolBar, (UINT)0, &tbRect);

	char	buf[256];
	char	rbuf[256];
	CString	title;
	transportBar.SetWindowText("Transport");

//	SetActiveView((CView*)m_wndSplitter.GetPane(0,1));

// 3way splitter adapted from viewex
// create a splitter with 2 row, 1 columns
	if (!topSplitter.CreateStatic(this, 2, 1))	{
		MessageBox("Failed to topSplitter\n");
		return FALSE;
	}

	// add the first splitter pane - the obect view pane in row 1
	if (!topSplitter.CreateView(1, 0,
			RUNTIME_CLASS(MFCObjectMountView), CSize(250, INI_OBJ_VIEW_HEIGHT), pContext)) {
		MessageBox("Failed to create first pane\n");
		return FALSE;
	}

	// add the second splitter pane - which is a nested splitter with 2 rows
	if (!sideSplitter.CreateStatic(
		&topSplitter,     // our parent window is the first splitter
		1, 3,               // the new splitter is 1 row, 3 columns
		WS_CHILD | WS_VISIBLE | WS_BORDER,  // style, WS_BORDER is needed
		topSplitter.IdFromRowCol(0, 0)
			// new splitter is in the first row, 1st column of first splitter
	   )) {
		MessageBox("Failed to create nested splitter\n");
		return FALSE;
	}

	if (!sideSplitter.CreateView(0, 0,
			RUNTIME_CLASS(MFCQuaSymbolIndexView), CSize(INI_SYM_INDEX_WIDTH, INI_ARRANGER_HEIGHT), pContext)) {
		MessageBox("Failed to create index pane\n");
		return FALSE;
	}
	if (!sideSplitter.CreateView(0, 1,
			RUNTIME_CLASS(MFCChannelMountView), CSize(INI_CHANNEL_MOUNT_WIDTH, INI_ARRANGER_HEIGHT), pContext)) {
		MessageBox("Failed to create channel pane\n");
		return FALSE;
	}
	if (!sideSplitter.CreateView(0, 2,
			RUNTIME_CLASS(MFCArrangeView), CSize(250, INI_ARRANGER_HEIGHT), pContext)) {
		MessageBox("Failed to create arranger pane\n");
		return FALSE;
	}

	arranger = (MFCArrangeView *)sideSplitter.GetPane(0, 2);
	channeler = (MFCChannelMountView *)sideSplitter.GetPane(0, 1);
	indexer = (MFCQuaSymbolIndexView *)sideSplitter.GetPane(0, 0);
	objectifier = (MFCObjectMountView *)topSplitter.GetPane(1, 0);

	arranger->SetWindowText("Qua Arranger");
	channeler->SetWindowText("Qua Channeler");
	indexer->SetWindowText("Qua Indexer");
	objectifier->SetWindowText("Qua Objectifier");

	arranger->channeler = channeler;	
	channeler->arranger = arranger;

	// it all worked, we now have two splitter windows which contain
	//  three different views

	sideSplitter.SetColumnInfo(0, INI_SYM_INDEX_WIDTH, MIN_SYM_INDEX_WIDTH);
	sideSplitter.SetColumnInfo(1, INI_CHANNEL_MOUNT_WIDTH, MIN_CHANNEL_MOUNT_WIDTH);
	sideSplitter.SetRowInfo(0, INI_ARRANGER_HEIGHT, MIN_ARRANGER_HEIGHT);
	topSplitter.SetRowInfo(0, INI_ARRANGER_HEIGHT, MIN_ARRANGER_HEIGHT);
	topSplitter.SetRowInfo(1, INI_OBJ_VIEW_HEIGHT, MIN_OBJ_VIEW_HEIGHT);

//	SendMessage(WM_COMMAND, arranger->currentTool, (LPARAM)0);
	SendMessage(WM_COMMAND, currentTool, (LPARAM)0);

	return true; //CMDIChildWnd::OnCreateClient(lpcs, pContext);
}


BOOL
QuaChildFrame::OnChildNotify(
				UINT message,
				WPARAM wParam,
				LPARAM lParam,
				LRESULT* pResult 
			)
{
	return CMDIChildWnd::OnChildNotify(message, wParam, lParam, pResult);
}

BOOL
QuaChildFrame::OnNotify(
				WPARAM wParam,
				LPARAM lParam,
				LRESULT* pResult 
			)
{
	return CMDIChildWnd::OnNotify(wParam, lParam, pResult);
}

void
QuaChildFrame::OnSize(UINT nType, int cx, int cy)
{
	if (width < 0 || height < 0) {
		width = cx;
		height = cy;
//		fprintf(trace_fp, "ini: cx %d cy %d dx %d dy %d\n", width, height);
	} else {
		int	dx = cx - width;
		int	dy = cy - height;
		int	arrangerHeight;
		int	arrangerMinHeight;
		int	objHeight;
		int	objMinHeight;
//		fprintf(trace_fp, "width %d height %d cx %d cy %d dx %d dy %d\n", width, height, cx, cy, dx, dy);
		topSplitter.GetRowInfo(0, arrangerHeight, arrangerMinHeight);
//		fprintf(trace_fp, "ah %d amh %d\n", arrangerHeight, arrangerMinHeight);
		if (arrangerHeight+dy > MIN_ARRANGER_HEIGHT) {
			topSplitter.SetRowInfo(0, arrangerHeight+dy, MIN_ARRANGER_HEIGHT);
		}
		topSplitter.GetRowInfo(1, objHeight, objMinHeight);
//		fprintf(trace_fp, "ah %d amh %d\n", objHeight, objMinHeight);
		if (objHeight-dy > MIN_OBJ_VIEW_HEIGHT) {
			topSplitter.SetRowInfo(1, objHeight-dy, MIN_OBJ_VIEW_HEIGHT);
		}

		width = cx;
		height = cy;
	}
	CMDIChildWnd::OnSize(nType, cx, cy);
}

void
QuaChildFrame::OnSizing( UINT which, LPRECT r)
{
	CMDIChildWnd::OnSizing(which, r);
}


BOOL
QuaChildFrame::OnViewToolBar(UINT nID)
{
	CWnd* pBar;
	switch (nID) {
		case ID_VIEW_ARRANGERTOOLS:
			pBar = &arrangerToolBar;
			break;
		default:
			if ((pBar = GetDlgItem(nID)) == NULL) {
				fprintf(stderr, "failed to find tool bar %d\n", nID);
				return false;
			}
	}
	// toggle visible state
	pBar->ShowWindow((pBar->GetStyle() & WS_VISIBLE) == 0);
	RecalcLayout();
	return TRUE;
}

BOOL
QuaChildFrame::OnSelectArrangerTool(UINT nID)
{
	fprintf(stderr, "select arranger tool %d\n", nID);
	if (arranger) {
//		arranger->currentTool = nID;
		currentTool = nID;
		for (short i=ID_ARTOOL_BASE; i<=ID_ARTOOL_LAST_BUTTON; i++) {
			arrangerToolBar.GetToolBarCtrl().CheckButton(i, false);
		}
		arrangerToolBar.GetToolBarCtrl().CheckButton(nID, true);
	}
	return TRUE;
}


// may be ok, but may need to watch performance of this mechanism
// unnecessary at the moment, idle thread generates piles of these
void
QuaChildFrame::OnUpdateUIArrangerTools(CCmdUI* pCmdUI)
{
	if (
//			arranger && arranger->currentTool == pCmdUI->m_nID
			currentTool == pCmdUI->m_nID
							) {
		pCmdUI->SetCheck(true);
	} else {
		pCmdUI->SetCheck(false);
	}
}

// control and menu handlers
void QuaChildFrame::OnEditUndo()
{
    ReportError("Edit undo");
}

void QuaChildFrame::OnEditCut()
{
    ReportError("Edit cut");
}

void QuaChildFrame::OnEditCopy()
{
    ReportError("Edit copy");
}

void QuaChildFrame::OnEditPaste()
{
    ReportError("Edit paste");
}


