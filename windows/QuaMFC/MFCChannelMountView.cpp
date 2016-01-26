#include "qua_version.h"
// MFCChannelMountView.cpp : implementation file
//

#include "stdafx.h"
#include "QuaMFC.h"
#include "QuaMFCDoc.h"
#include "MFCChannelMountView.h"
#include "MFCChannelView.h"
#include "MFCArrangeView.h"
#include "MFCQuaMessageId.h"

#include "DaKernel.h"
#include "DaBasicTypes.h"
#include "inx/Qua.h"
#include "StdDefs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define INI_CHANNEL_HEIGHT	60

/////////////////////////////////////////////////////////////////////////////
// MFCChannelMountView
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(MFCChannelMountView, CView)

MFCChannelMountView::MFCChannelMountView()
{
	arranger = NULL;
}

MFCChannelMountView::~MFCChannelMountView()
{
}


BEGIN_MESSAGE_MAP(MFCChannelMountView, CView)
	//{{AFX_MSG_MAP(MFCChannelMountView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_MOVE()
	ON_WM_VSCROLL()
	ON_MESSAGE(QM_ARRANGE_VIEW, OnArrangeView)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MFCArrangeView drawing

void MFCChannelMountView::OnDraw(CDC* pdc)
{
}


void
MFCChannelMountView::DisplayArrangementTitle(const char *nm)
{
	;
}


/////////////////////////////////////////////////////////////////////////////
// MFCArrangeView diagnostics
/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void MFCChannelMountView::AssertValid() const
{
	CView::AssertValid();
}

void MFCChannelMountView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// MFCChannelMountView message handlers
/////////////////////////////////////////////////////////////////////////////

afx_msg LRESULT
MFCChannelMountView::OnArrangeView(WPARAM, LPARAM)
{
	fprintf(stderr, "%%%%channel mount arrange message\n");
	ArrangeChildren();
	return 99;
}

afx_msg void
MFCChannelMountView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	fprintf(stderr, "cmv scroll %d\n", nSBCode);
	switch (nSBCode) {
		case SB_BOTTOM: { //   Scroll to bottom. 
			break;
		}
		case SB_ENDSCROLL: { //   End scroll. 
			break;
		}
		case SB_LINEDOWN: { //   Scroll one line down. 
			break;
		}
		case SB_LINEUP: { //   Scroll one line up. 
			break;
		}
		case SB_PAGEDOWN: { //   Scroll one page down. 
			break;
		}
		case SB_PAGEUP: { //   Scroll one page up. 
			break;
		}
		case SB_THUMBPOSITION:  //   Scroll to the absolute position. The current position is provided in nPos. 
		case SB_THUMBTRACK: { //   Drag scroll box to specified position. The current position is provided in nPos. 
			fprintf(stderr, "scroll thumb %d\n", nPos);
			break;
		}
		case SB_TOP: { //   Scroll to top. 
			break;
		}
	}
	CView::OnVScroll(nSBCode, nPos, pScrollBar);
}

void
MFCChannelMountView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	bounds.right = cx;
	bounds.bottom = cy;
	frame.right = frame.left+cx;
	frame.bottom = frame.bottom+cy;

	ArrangeChildren();
}

void
MFCChannelMountView::OnSizing( UINT which, LPRECT r)
{
	CView::OnSizing(which, r);
}

int
MFCChannelMountView::OnCreate(LPCREATESTRUCT lpCreateStruct )
{
	bounds.left = 0;
	bounds.top = 0;
	bounds.right = lpCreateStruct->cx;
	bounds.bottom = lpCreateStruct->cy;
	frame.left = lpCreateStruct->x;
	frame.top = lpCreateStruct->y;
	frame.right = frame.left+bounds.right; 
	frame.bottom = frame.top+bounds.bottom; 
	rack.Create(NULL, "ChannelRack", WS_CHILD | WS_VISIBLE |WS_CLIPCHILDREN,
       CRect(0, 0, bounds.right, bounds.bottom), this, 1234, NULL);
	return CView::OnCreate(lpCreateStruct);
}


afx_msg BOOL
MFCChannelMountView::OnToolTipNeedsTxt( UINT id, NMHDR * pTTTStruct, LRESULT * pResult )
{
	NMTTDISPINFO *ttip = ((NMTTDISPINFO*)pTTTStruct);
//	ReportError("%x", ttip->lpszText);
//	wsprintf(ttip->lpszText,"hmmm");
//	ttip->hinst = quaAppInstance;
//	strcpy(((NMTTDISPINFO*)pTTTStruct)->szText, "hmmm");
	return false;
}

void
MFCChannelMountView::OnInitialUpdate()
{
	CQuaMFCDoc	*qdoc = (CQuaMFCDoc *)GetDocument();
	if (qdoc == NULL) {
		ReportError("initial update of channel mount finds a null sequencer document");
	} else if (qdoc->qua == NULL) {
		ReportError("initial update finds a null sequencer");
	} else {	// set qua up with our hooks
		SetLinkage(qdoc->qua->bridge.display);
		quaLink->AddChannelRack(this);
		quaLink->AddChannelRepresentations(this);
	}
}



afx_msg void
MFCChannelMountView::OnMove(int x, int y)
{
	fprintf(stderr, "cmv move %d %d\n", x, y);
	frame.MoveToXY(x,y);
}

/////////////////////////////////////////////////////////////////////////////
// MFCChannelMountView methods
/////////////////////////////////////////////////////////////////////////////

void
MFCChannelMountView::AddChannelRepresentation(Channel *c)
{
	MFCChannelView *nv = new MFCChannelView;
	nv->Create(NULL, "Channel", WS_CHILD | WS_VISIBLE,
       CRect(0, 0, bounds.right, INI_CHANNEL_HEIGHT), &rack, 1234, NULL);
	nv->SetChannel(c);
	AddCR(nv);
	ArrangeChildren();
}

void
MFCChannelMountView::RemoveChannelRepresentation(Channel *c)
{
	MFCChannelView *nv = (MFCChannelView *)ChannelRepresentationFor(c);
	if (nv) {
		delete nv;
		RemCR(nv);
		ArrangeChildren();
	}
}


void
MFCChannelMountView::ArrangeChildren()
{
	fprintf(stderr, "cmv: arranging children\n");
	int n = NCR();
	long	atY = 0;
	for (int i=0; i<n; i++) {
		MFCChannelView	*nv = (MFCChannelView *)CR(i);
		if (nv) {
			nv->SetWindowPos(&wndTop, 0, atY, bounds.right, nv->bounds.bottom, NULL); //INI_CHANNEL_HEIGHT, SWP_NOSIZE);
			atY += nv->bounds.bottom;
		}
	}
	rack.SetWindowPos(&wndTop, 0, 0, bounds.right, atY, SWP_NOMOVE);
	if (arranger) {
		arranger->SetHVScroll();
		CPoint sxy = arranger->GetScrollPosition();
		rack.SetWindowPos(&wndTop, 0, -sxy.y, bounds.right, atY, SWP_NOSIZE);
	}
}
