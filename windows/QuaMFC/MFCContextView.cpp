#include "qua_version.h"
// MFCContextView.cpp : implementation file
//
//#define _AFXDLL
#include "stdafx.h"
#include "QuaMFC.h"
#include "MFCContextView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MFCArrangeView

IMPLEMENT_DYNCREATE(MFCContextView, CView)

MFCContextView::MFCContextView()
{
}

MFCContextView::~MFCContextView()
{
}


BEGIN_MESSAGE_MAP(MFCContextView, CView)
	//{{AFX_MSG_MAP(MFCContextView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MFCArrangeView drawing

void
MFCContextView::OnDraw(CDC* pdc)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here

// crud to see what works how around here
	CRect	b(10, 10, 300, 300);
	CBrush	*pOldBrush=(CBrush *)pdc->SelectStockObject(BLACK_BRUSH);
//	pdc->FillRect(&b, pOldBrush);
//	pdc->Ellipse(20, 1000, 20, 1000);
	pdc->DrawText("MFCContextView", -1, &b, DT_LEFT|DT_TOP);
    CPen* pOldPen;
    // Paint the background using the BackColor property
//    pdc->FillRect(&b, pOldBrush);

	// draw grid
	// draw instances
	// draw envelopes
	// draw time cursor

    // Draw the ellipse using the BackColor property and a black pen
    pOldBrush = pdc->SelectObject(pOldBrush);
    pOldPen = (CPen*)pdc->SelectStockObject(BLACK_PEN);
//    pdc->Ellipse(b);
    pdc->SelectObject(pOldPen);
    pdc->SelectObject(pOldBrush);
}

/////////////////////////////////////////////////////////////////////////////
// MFCArrangeView diagnostics

#ifdef _DEBUG
void MFCContextView::AssertValid() const
{
	CView::AssertValid();
}

void MFCContextView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// MFCContextView message handlers
