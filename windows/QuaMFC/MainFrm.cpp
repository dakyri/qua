#include "qua_version.h"

// MainFrm.cpp : implementation of the QuaMainFrame class
//
//  For a bog standard MDI implementation of Qua, this is the main window frame in
// which the open Qua document frames will appear ...

#include "stdafx.h"
#include "QuaMFC.h"
#include "StdDefs.h"

#include "MainFrm.h"
#include "Splash.h"

#include "QuaMFCCCDialog.h"

#include "DaKernel.h"
#include "QuaMFCDoc.h"
#include "inx/Qua.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// QuaMainFrame

IMPLEMENT_DYNAMIC(QuaMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(QuaMainFrame, CMDIFrameWnd)
	ON_WM_QUERYNEWPALETTE()
	ON_WM_PALETTECHANGED()
	//{{AFX_MSG_MAP(QuaMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_FILE_PREFERENCES, OnProperties)
	ON_COMMAND(ID_EDIT_ADD_CHANNEL, OnAddChannelPopup)
	ON_MESSAGE(ID_EDIT_ADD_CHANNEL_POPPED, OnAddChannelPopped)
	ON_UPDATE_COMMAND_UI(ID_FILE_PREFERENCES, OnUpdateProperties)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// QuaMainFrame construction/destruction

QuaMainFrame::QuaMainFrame()
{
	m_pAddChanDialog = NULL;
	m_pPropFrame = NULL;
	// TODO: add member initialization code here
	
}

QuaMainFrame::~QuaMainFrame()
{
}

LRESULT
QuaMainFrame::OnAddChannelPopped(WPARAM wp, LPARAM lp)
{
	if (m_pAddChanDialog) {
		CQuaMFCDoc *doc = (CQuaMFCDoc *)GetActiveFrame()->GetActiveDocument();
		if (doc && doc == m_pAddChanDialog->doc) {
			if (doc->qua && doc->qua->bridge.display) {
				doc->qua->bridge.display->CreateChannel(
					NULL, -1,
					m_pAddChanDialog->nAudIn,
					m_pAddChanDialog->nAudOut,
					m_pAddChanDialog->addAuIn,
					m_pAddChanDialog->addAuOut,
					m_pAddChanDialog->addStrIn,
					m_pAddChanDialog->addStrOut
					);					;
			}
		}
		delete m_pAddChanDialog;
		m_pAddChanDialog = NULL;
	}
	return true;
}

void
QuaMainFrame::OnAddChannelPopup()
{
	if (m_pAddChanDialog == NULL) {
		CQuaMFCDoc *doc = (CQuaMFCDoc *)GetActiveFrame()->GetActiveDocument();
		if (doc) {
			m_pAddChanDialog = new QuaMFCCCDialog(doc,this);
			if (m_pAddChanDialog->Create(QuaMFCCCDialog::IDD,this) == TRUE) {
				m_pAddChanDialog->ShowWindow(SW_SHOW);
			}
		}
	} else {
		m_pAddChanDialog->SetActiveWindow();
	}
}

int QuaMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// CG: This line was added by the Palette Support component
	m_pPalette = NULL;

	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	// CG: The following line was added by the Splash Screen component.
	CSplashWnd::ShowSplashScreen(this);
	return 0;
}

BOOL QuaMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// QuaMainFrame diagnostics

#ifdef _DEBUG
void QuaMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void QuaMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// QuaMainFrame message handlers


void QuaMainFrame::OnProperties()
{
	// TODO: The property sheet attached to your project
	// via this function is not hooked up to any message
	// handler.  In order to actually use the property sheet,
	// you will need to associate this function with a control
	// in your project such as a menu item or tool bar button.
	//
	// If mini frame does not already exist, create a new one.
	// Otherwise, unhide it

	if (m_pPropFrame == NULL)
	{
		m_pPropFrame = new CPropertyFrame;
		CRect rect(0, 0, 0, 0);
		CString strTitle;
		VERIFY(strTitle.LoadString(IDS_PROPSHT_CAPTION));

		if (!m_pPropFrame->Create(NULL, strTitle,
			WS_POPUP | WS_CAPTION | WS_SYSMENU, rect, this))
		{
			delete m_pPropFrame;
			m_pPropFrame = NULL;
			return;
		}
		m_pPropFrame->CenterWindow();
	}

	// Before unhiding the modeless property sheet, update its
	// settings appropriately.  For example, if you are reflecting
	// the state of the currently selected item, pick up that
	// information from the active view and change the property
	// sheet settings now.

	if (m_pPropFrame != NULL && !m_pPropFrame->IsWindowVisible())
		m_pPropFrame->ShowWindow(SW_SHOW);
}


BOOL QuaMainFrame::OnQueryNewPalette()
{
	// CG: This function was added by the Palette Support component

	if (m_pPalette == NULL)
		return FALSE;
	
	// BLOCK
	{
		CClientDC dc(this);
		CPalette* pOldPalette = dc.SelectPalette(m_pPalette,
			GetCurrentMessage()->message == WM_PALETTECHANGED);
		UINT nChanged = dc.RealizePalette();
		dc.SelectPalette(pOldPalette, TRUE);

		if (nChanged == 0)
			return FALSE;
	}
	
	Invalidate();
	
	return TRUE;
}

void QuaMainFrame::OnPaletteChanged(CWnd* pFocusWnd)
{
	// CG: This function was added by the Palette Support component

	if (pFocusWnd == this || IsChild(pFocusWnd))
		return;
	
	OnQueryNewPalette();
}

CPalette* QuaMainFrame::SetPalette(CPalette* pPalette)
{
	// CG: This function was added by the Palette Support component

	// Call this function when the palette changes.  It will
	// realize the palette in the foreground to cause the screen
	// to repaint correctly.  All calls to CDC::SelectPalette in
	// painting code should select palettes in the background.

	CPalette* pOldPalette = m_pPalette;
	m_pPalette = pPalette;
	OnQueryNewPalette();
	return pOldPalette;
}


BOOL
QuaMainFrame::OnViewToolBar(UINT nID)
{
	ReportError("qmf: view tool bar %d", nID);
	CWnd* pBar;
	if ((pBar = GetDlgItem(nID)) == NULL) {
		ReportError("don't find it");
		return FALSE;   // not for us
	}
	// toggle visible state
	pBar->ShowWindow((pBar->GetStyle() & WS_VISIBLE) == 0);
	RecalcLayout();
	return TRUE;
}

void QuaMainFrame::OnUpdateProperties(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}

void
QuaMainFrame::OnUpdateControlBarMenu(CCmdUI* pCmdUI)
{
	fprintf(stderr, "on upd ctrl bar menu\n");
}

/*
void CDynaMDIChildWnd::RefreshColorMenu()
{
	// Get the active document
	CDynaMenuDoc* pDoc = (CDynaMenuDoc*)GetActiveDocument();
	ASSERT_KINDOF(CDynaMenuDoc, pDoc);

	// Locate the color submenu
	CMenu* pColorMenu = NULL;
	CMenu* pTopMenu = AfxGetMainWnd()->GetMenu();
	int iPos;
	for (iPos = pTopMenu->GetMenuItemCount()-1; iPos >= 0; iPos--)
	{
		CMenu* pMenu = pTopMenu->GetSubMenu(iPos);
		if (pMenu && pMenu->GetMenuItemID(0) == ID_COLOR_OPTIONS)
		{
			pColorMenu = pMenu;
			break;
		}
	}
	ASSERT(pColorMenu != NULL);

	// Update the color submenu to reflect the text colors available to
	// the active document

	// First, delete all items but ID_COLOR_OPTIONS at position 0
	for (iPos = pColorMenu->GetMenuItemCount()-1; iPos > 0; iPos--)
		pColorMenu->DeleteMenu(iPos, MF_BYPOSITION);

	// Then, add a separator and an item for each available text color
	BOOL bNeedSeparator = TRUE;
	for (int i = 0; i < NUM_TEXTCOLOR; i++)
	{
		if (pDoc->m_abAllowColor[i] == TRUE)
		{
			if (bNeedSeparator)
			{
				pColorMenu->AppendMenu(MF_SEPARATOR);
				bNeedSeparator = FALSE;
			}
			CString strColor;
			strColor.LoadString(pDoc->m_aColorDef[i].m_nString);
			pColorMenu->AppendMenu(MF_STRING,
				pDoc->m_aColorDef[i].m_nID, strColor);
		}
	}
}
*/