// PreviewWnd.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "PreviewWnd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreviewWnd

CPreviewWnd::CPreviewWnd()
{
}

CPreviewWnd::~CPreviewWnd()
{
}


BEGIN_MESSAGE_MAP(CPreviewWnd, CWnd)
	//{{AFX_MSG_MAP(CPreviewWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPreviewWnd message handlers

void CPreviewWnd::OnPaint() 
{
	// TODO: Add your custom preview paint code.
	// For an example, we are drawing a blue ellipse.

	CPaintDC dc(this); // device context for painting
	CRect rect;
	GetClientRect(rect);
}

BOOL CPreviewWnd::OnEraseBkgnd(CDC* pDC) 
{
	// Use the same background color as that of the dialog
	//  (property sheet).

	CWnd* pParentWnd = GetParent();
	HBRUSH hBrush = (HBRUSH)pParentWnd->SendMessage(WM_CTLCOLORDLG,
		(WPARAM)pDC->m_hDC, (LPARAM)pParentWnd->m_hWnd);
	CRect rect;
	GetClientRect(rect);
	pDC->FillRect(&rect, CBrush::FromHandle(hBrush));
	return TRUE;
}
