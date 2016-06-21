#include "qua_version.h"

// ChildFrm.cpp : implementation of the QuaChildFrame class
//
//  this is the frame for the view/s through which a particular open Qua document is
// viewed and accessed.

#include "stdafx.h"
#include "QuaMFC.h"

#include "QuaContextMFCFrame.h"
#include "MFCContextView.h"
#include "MFCQuaContextIndexView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MIN_CONTEXT_HEIGHT	10
#define INI_CONTEXT_HEIGHT	450
#define MIN_SYM_INDEX_WIDTH	30
#define INI_SYM_INDEX_WIDTH	100
#define MIN_CONTEXT_ED_WIDTH	10
#define INI_CONTEXT_ED_WIDTH	250

/////////////////////////////////////////////////////////////////////////////
// QuaChildFrame

IMPLEMENT_DYNCREATE(QuaContextFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(QuaContextFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(QuaContextFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_CONNECT_BUTTON,OnConnectClick)
	ON_WM_SIZE()
	ON_WM_SIZING()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// QuaContextFrame construction/destruction

QuaContextFrame::QuaContextFrame()
{
	width = -1;
	height = -1;
}

QuaContextFrame::~QuaContextFrame()
{
}


BOOL QuaContextFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// QuaContextFrame diagnostics

#ifdef _DEBUG
void QuaContextFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void QuaContextFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// QuaContextFrame message handlers

BOOL
QuaContextFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	if (!contextTools.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!contextTools.LoadToolBar(IDR_QUACONTEXTTYPE))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	contextTools.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&contextTools);
	contextTools.SetWindowText("Qua Context: Tools");

	SetTitle("Qua Context");
	SetWindowPos(&wndBottom, 20, 20, INI_SYM_INDEX_WIDTH+INI_CONTEXT_ED_WIDTH+20, 20+INI_CONTEXT_HEIGHT, SWP_NOOWNERZORDER);
	

#define CONTEXT_SPLITTA
#ifdef CONTEXT_SPLITTA
// create a splitter with 2 row, 1 columns
	if (!sideSplitter.CreateStatic(this, 1, 2))
	{
		MessageBox("Failed to topSplitter\n");
		return FALSE;
	}
	if (!sideSplitter.CreateView(0, 0,
		RUNTIME_CLASS(MFCQuaContextIndexView), CSize(INI_SYM_INDEX_WIDTH, INI_CONTEXT_HEIGHT), pContext))
	{
		MessageBox("Failed to create second pane\n");
		return FALSE;
	}
	if (!sideSplitter.CreateView(0, 1,
		RUNTIME_CLASS(MFCContextView), CSize(INI_CONTEXT_ED_WIDTH, INI_CONTEXT_HEIGHT), pContext))
	{
		MessageBox("Failed to create third pane\n");
		return FALSE;
	}

	// it all worked, we now have two splitter windows which contain
	//  three different views

//	sideSplitter.SetColuInfo(0, INI_ARRANGER_HEIGHT, MIN_ARRANGER_HEIGHT);
//	sideSplitter.SetRowInfo(0, INI_ARRANGER_HEIGHT, MIN_ARRANGER_HEIGHT);
#else
	ctxtView = new MFCContextView;
	ctxtView->Create(_T("STATIC"), "Hi", WS_CHILD | WS_VISIBLE,
       CRect(0, 40, 250, 300), this, 1234, NULL);
	pContext->m_pCurrentDoc->AddView(ctxtView);

//	return CMDIChildWnd::OnCreateClient(lpcs, pContext); 
#endif
	return true;
}

BOOL
QuaContextFrame::OnChildNotify(
				UINT message,
				WPARAM wParam,
				LPARAM lParam,
				LRESULT* pResult 
			)
{
	return CMDIChildWnd::OnChildNotify(message, wParam, lParam, pResult);
}
BOOL
QuaContextFrame::OnNotify(
				WPARAM wParam,
				LPARAM lParam,
				LRESULT* pResult 
			)
{
	return CMDIChildWnd::OnNotify(wParam, lParam, pResult);
}

void
QuaContextFrame::OnSize(UINT nType, int cx, int cy)
{
	CMDIChildWnd::OnSize(nType, cx, cy);
}

void
QuaContextFrame::OnSizing( UINT which, LPRECT r)
{
	CMDIChildWnd::OnSizing(which, r);
}

void
QuaContextFrame::OnConnectClick()
{
    MessageBox("Connect clicked");
}

