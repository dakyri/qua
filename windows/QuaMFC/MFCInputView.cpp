// MFCChannelView.cpp : implementation file
//

#include "stdafx.h"

#include "BaseTypes.h"
#include "Colors.h"

#include "QuaMFC.h"
#include "MFCInputView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MFCChannelView

IMPLEMENT_DYNCREATE(MFCInputView, CView)

MFCInputView::MFCInputView()
{

}

MFCInputView::~MFCInputView()
{
}


BEGIN_MESSAGE_MAP(MFCInputView, CView)
	//{{AFX_MSG_MAP(MFCInputView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SIZING()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MFCDestinationView drawing
/////////////////////////////////////////////////////////////////////////////
void MFCInputView::OnDraw(CDC* pdc)
{
	CDocument* pDoc = GetDocument();

	RaisedBox(pdc, &bounds, rgb_orange, true);
}

/////////////////////////////////////////////////////////////////////////////
// MFCChannelView diagnostics
/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void MFCInputView::AssertValid() const
{
	CView::AssertValid();
}

void MFCInputView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// MFCDestinationView message handlers
/////////////////////////////////////////////////////////////////////////////

void
MFCInputView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	bounds.right = cx;
	bounds.bottom = cy;
	GetWindowRect(&frame);
}

void
MFCInputView::OnSizing( UINT which, LPRECT r)
{
	CView::OnSizing(which, r);
}

int
MFCInputView::OnCreate(LPCREATESTRUCT lpCreateStruct )
{
	GetWindowRect(&frame);
	bounds.left = 0;
	bounds.top = 0;
	bounds.right = frame.right-frame.left;
	bounds.bottom = frame.bottom-frame.top;
	selector.CreateSelector(CRect(2, 2, 40, 12), this, 5656, true);
	return CView::OnCreate(lpCreateStruct);
}

/////////////////////////////////////////////////////////////////////////////
// MFCInputView methods
/////////////////////////////////////////////////////////////////////////////

void
MFCInputView::SetInput(Input *d)
{
	input = d;
}