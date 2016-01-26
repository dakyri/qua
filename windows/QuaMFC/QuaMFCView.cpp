// QuaMFCView.cpp : implementation of the CQuaMFCView class
//

#include "stdafx.h"
#include "QuaMFC.h"

#include "QuaMFCDoc.h"
#include "QuaMFCView.h"
#include "MFCChannelView.h"
#include "MFCArrangeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQuaMFCView

IMPLEMENT_DYNCREATE(CQuaMFCView, CView)

BEGIN_MESSAGE_MAP(CQuaMFCView, CView)
	//{{AFX_MSG_MAP(CQuaMFCView)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNeedsTxt)
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipNeedsTxt)
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipNeedsTxt)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQuaMFCView construction/destruction

CQuaMFCView::CQuaMFCView()
{
	// TODO: add construction code here
//	m_transport.BeginModalState();
//	m_transport.DoModal();
}

CQuaMFCView::~CQuaMFCView()
{
}

BOOL CQuaMFCView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CQuaMFCView drawing

void CQuaMFCView::OnDraw(CDC* pdc)
{
//	CQuaMFCDoc* pDoc = GetDocument();
//	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
	CRect	b(10, 10, 300, 300);
	CBrush	*pOldBrush=(CBrush *)pdc->SelectStockObject(BLACK_BRUSH);
    CPen* pOldPen;
    // Paint the background using the BackColor property
//    pdc->FillRect(&b, pOldBrush);
	pdc->DrawText("QuaMFCView", -1, &b, DT_LEFT|DT_TOP);

// crud to see what works how around here
//	CRect	b(10, 10, 200, 100);
//	CBrush	*pOldBrush=(CBrush *)pdc->SelectStockObject(BLACK_BRUSH);
//	pdc->FillRect(&b, pOldBrush);
//	pdc->Ellipse(20, 50, 360, 150);
//	pdc->DrawText("CQuaMFCView", -1, &b, DT_LEFT|DT_TOP);
}

/////////////////////////////////////////////////////////////////////////////
// CQuaMFCView printing

BOOL CQuaMFCView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CQuaMFCView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CQuaMFCView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CQuaMFCView diagnostics

#ifdef _DEBUG
void CQuaMFCView::AssertValid() const
{
	CView::AssertValid();
}

void CQuaMFCView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CQuaMFCDoc* CQuaMFCView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CQuaMFCDoc)));
	return (CQuaMFCDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CQuaMFCView message handlers


BOOL CQuaMFCView::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	// create a splitter with 1 row, 2 columns
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
	{
		MessageBox("Failed to CreateStaticSplitter\n");
		return FALSE;
	}
	// add the first splitter pane - the default view in column 0
	// this is the document view ... as defined by the doc template
//	if (!m_wndSplitter.CreateView(0, 0,
//		pContext->m_pNewViewClass, CSize(130, 50), pContext))
//	{
//		TRACE0("Failed to create first pane\n");
//		return FALSE;
//	}

	// add the second splitter pane - an input view in column 1
//	if (!m_wndSplitter.CreateView(1, 0,
//		RUNTIME_CLASS(MFCArrangeView), CSize(50, 250), pContext))
//	{
//		TRACE0("Failed to create second pane\n");
//		return FALSE;
//	}
//
//	// add the first splitter pane - the default view in column 0
//	if (!m_wndSplitter.CreateView(0, 0,
//		pContext->m_pNewViewClass, CSize(130, 50), pContext))
//	{
//		TRACE0("Failed to create first pane\n");
//		return FALSE;
//	}
//
//	// add the second splitter pane - an input view in column 1
	if (!m_wndSplitter.CreateView(0, 0,
		RUNTIME_CLASS(MFCChannelView), CSize(50, 50), pContext))
	{
		TRACE0("Failed to create second pane\n");
		return FALSE;
	}
	if (!m_wndSplitter.CreateView(0, 1,
		RUNTIME_CLASS(MFCArrangeView), CSize(50, 50), pContext))
	{
		TRACE0("Failed to create second pane\n");
		return FALSE;
	}


	return TRUE;
//	return CMDIChildWnd::OnCreateClient(lpcs, pContext);
}
