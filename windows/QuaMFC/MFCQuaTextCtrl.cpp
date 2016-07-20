

// MFCQuaTextCtrl.cpp : implementation file
// base class for controls that are shown with text indented in a background with a label
//
//#define _AFXDLL
#include "stdafx.h"
#include "qua_version.h"

#include "QuaMFC.h"
#include "MFCQuaTextCtrl.h"
#include "StdDefs.h"
#include "Colors.h"


// MFCQuaTimeCtrl

CFont			MFCQuaTextCtrl::defaultDisplayFont;

#ifdef QUA_V_TE1

IMPLEMENT_DYNAMIC(MFCQuaTextCtrl, CStatic)
MFCQuaTextCtrl::MFCQuaTextCtrl()
{
	vcMessid  = 0;
}

MFCQuaTextCtrl::~MFCQuaTextCtrl()
{
}


bool
MFCQuaTextCtrl::CreateTextCtrl(CRect &r, CWnd *w, UINT id, CFont *cf, CRect *tr, COLORREF *bgc, COLORREF *tbgc, COLORREF *txc)
{
	int gotoit = Create(NULL, SS_NOTIFY, r, w, id);
	bounds = r;
	bounds.MoveToXY(0,0);
	if (tr) {
		textRect = *tr;
	} else {
		textRect = InnerRect(bounds,2);
	}
	ShowWindow(SW_SHOW);
	if (cf != NULL) {
		displayFont = cf;
	} else {
		displayFont = &defaultDisplayFont;
	}
	if (bgc) {
		fillBg = true;
		bgColor = *bgc;
	} else {
		fillBg = false;
	}
	if (tbgc) {
		tbgColor = *tbgc;
	} else {
		tbgColor = rgb_white;
	}
	if (txc) {
		textColor = *txc;
	} else {
		textColor = rgb_black;
	}
	return gotoit;
}

void
MFCQuaTextCtrl::SetLabel(char *l, CFont *lf)
{
	label = l;
	if (lf != NULL) {
		labelFont = lf;
	} else {
		labelFont = displayFont;
	}
}

void
MFCQuaTextCtrl::RedrawDisplay()
{
//	fprintf(stderr, "rdisp\n");
	RedrawWindow(&textRect);
}


BEGIN_MESSAGE_MAP(MFCQuaTextCtrl, CStatic)
	ON_WM_CREATE()
	ON_WM_PAINT()
//	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_SIZING()
END_MESSAGE_MAP()



// MFCQuaTextCtrl message handlers

//int
MFCQuaTextCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct )
{
	CStatic::OnCreate(lpCreateStruct);
	return 1;
}


void
MFCQuaTextCtrl::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);
	textRect.right += (cx-bounds.right);
	textRect.bottom += (cy-bounds.bottom);
	bounds.right = cx;
	bounds.bottom = cy;
}


void
MFCQuaTextCtrl::OnSizing(UINT nType, LPRECT lr)
{
	CWnd::OnSizing(nType, lr);
}

afx_msg void
MFCQuaTextCtrl::OnPaint()
{
//	fprintf(stderr, "on paint textctrl %s\n", label()?label():"");
	CPaintDC	pdc(this);
	CRect fr = OuterRect(textRect,2);
	if (fillBg) {
		pdc.FillSolidRect(&bounds, bgColor);
	} else {
		pdc.SetBkMode(TRANSPARENT);
	}
	if (label() && *label()) {
		pdc.SelectObject(labelFont);
		CRect	lr = bounds;
		lr.top = textRect.top-1;
		lr.bottom = textRect.bottom+1;
		pdc.DrawText(label(), &lr, DT_VCENTER|DT_LEFT);
	}
	LoweredBox(&pdc, &fr, tbgColor, true);
	pdc.SelectObject(displayFont);
	pdc.SelectObject(GetStockObject(DC_BRUSH));
	pdc.SelectObject(GetStockObject(DC_PEN));
	pdc.SetDCPenColor(textColor);
	pdc.SetTextColor(textColor);
	pdc.SetBkColor(tbgColor);
	DrawContents(&pdc);
}

void
MFCQuaTextCtrl::DrawContents(CDC *pdc)
{
}

afx_msg BOOL
MFCQuaTextCtrl::OnEraseBkgnd(CDC* pDC)
{
	return TRUE; //CStatic::OnEraseBkgnd(pDC);
}

afx_msg void
MFCQuaTextCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	CStatic::OnMouseMove(nFlags, point);
} //End of OnMouseMove

afx_msg BOOL
MFCQuaTextCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	return CStatic::OnSetCursor(pWnd, nHitTest, message);
} //End of OnSetCursor

afx_msg void
MFCQuaTextCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CStatic::OnLButtonDown(nFlags, point);
} //End of OnLButtonDown


void
MFCQuaTextCtrl::SendVCMsg()
{
	if (GetParent() && vcMessid) {
		GetParent()->SendMessage(vcMessid, vcWparam, vcLparam);
	}
}

void
MFCQuaTextCtrl::SetVCMsgParams(uint32 mid, WPARAM wp, LPARAM lp)
{
	vcMessid = mid;
	vcWparam = wp;
	vcLparam = lp;
}

IMPLEMENT_DYNAMIC(MFCQuaTextCtrlEdit, CEdit)


MFCQuaTextCtrlEdit::MFCQuaTextCtrlEdit()
{
	ctrl = NULL;
}

MFCQuaTextCtrlEdit::~MFCQuaTextCtrlEdit()
{
	;
}

int
MFCQuaTextCtrlEdit::CreateEdit(MFCQuaTextCtrl * pParentWnd)
{
	int		rv=0;
	ctrl = pParentWnd;
	CRect	r = OuterRect(ctrl->textRect);
	if (this->m_hWnd == NULL) {
		rv = Create(ES_LEFT|WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL
//			|ES_MULTILINE
			, r, pParentWnd, 5656);
	}
	SetFont(pParentWnd->displayFont);
	SetWindowText(pParentWnd->GetText());
	SetFocus();
	SetActiveWindow();
	SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);

	return rv;
}

void
MFCQuaTextCtrlEdit::RemoveEdit()
{
	DestroyWindow();
}

void
MFCQuaTextCtrlEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch(nChar) {
		case VK_RETURN: { // ???? don't seem to receive this in single line mode
			ctrl->SetFocus();
			break;
		}
		case VK_NEXT: {
			ctrl->SetFocus();
			break;
		}
		case VK_TAB: {
			ctrl->SetFocus();
			break;
		}
		default:
			CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
	}
}


void
MFCQuaTextCtrlEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch(nChar) {
		case VK_RETURN: {
			break;
		}
		case VK_TAB: {
			break;
		}
		default:
			CEdit::OnChar(nChar, nRepCnt, nFlags);
	}
}

void
MFCQuaTextCtrlEdit::OnKillFocus(CWnd* pNewWnd)
{
	if (pNewWnd) {
		CString	wt;
		pNewWnd->GetWindowText(wt);
		fprintf(stderr, "text ctrl edit kill focus %x new is %x, with text <%s>\n", m_hWnd, pNewWnd->m_hWnd, (LPCSTR)wt);
	} else {
		fprintf(stderr, "text ctrl edit kill focus %x to null win\n", m_hWnd);
	}
	char	buf[1000];
	GetWindowText(buf, 1000);
	ctrl->SetText(buf);
	ctrl->SendVCMsg();
//	RemoveEdit();
	CEdit::OnKillFocus(pNewWnd);
}



BEGIN_MESSAGE_MAP(MFCQuaTextCtrlEdit, CEdit)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

#else

IMPLEMENT_DYNAMIC(MFCQuaTextCtrl, CEdit)
MFCQuaTextCtrl::MFCQuaTextCtrl()
{
	vcMessid  = 0;
}

MFCQuaTextCtrl::~MFCQuaTextCtrl()
{
}


bool
MFCQuaTextCtrl::CreateTextCtrl(CRect &r, CWnd *w, UINT id, CFont *cf, CRect *tr, COLORREF *bgc, COLORREF *tbgc, COLORREF *txc)
{
	int gotoit = Create(ES_LEFT|WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL
//			|ES_MULTILINE
			, r, w, 5656);
	bounds = r;
	bounds.MoveToXY(0,0);
	if (tr) {
		textRect = *tr;
	} else {
		textRect = InnerRect(bounds,2);
	}
	ShowWindow(SW_SHOW);
	if (cf != NULL) {
		displayFont = cf;
	} else {
		displayFont = &defaultDisplayFont;
	}
	if (displayFont != NULL) {
		SetFont(displayFont);
	}
	if (bgc) {
		fillBg = true;
		bgColor = *bgc;
	} else {
		fillBg = false;
	}
	if (tbgc) {
		tbgColor = *tbgc;
	} else {
		tbgColor = rgb_white;
	}
	if (txc) {
		textColor = *txc;
	} else {
		textColor = rgb_black;
	}
	return gotoit;
}

void
MFCQuaTextCtrl::SetLabel(char *l, CFont *lf)
{
	label = l;
	if (lf != NULL) {
		labelFont = lf;
	} else {
		labelFont = displayFont;
	}
}

void
MFCQuaTextCtrl::RedrawDisplay()
{
//	fprintf(stderr, "rdisp\n");
	RedrawWindow(&textRect);
}


BEGIN_MESSAGE_MAP(MFCQuaTextCtrl, CEdit)
	ON_WM_CREATE()
	ON_WM_PAINT()
//	ON_WM_SETCURSOR()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()



// MFCQuaTextCtrl message handlers

HBRUSH
MFCQuaTextCtrl::CtlColor(CDC* pDC, UINT nCtlColor)
{
	pDC->SetBkColor(tbgColor);
	pDC->SetTextColor(textColor);
	pDC->SetBkMode(OPAQUE);
	return CreateSolidBrush(bgColor);
}

int
MFCQuaTextCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct )
{
	CEdit::OnCreate(lpCreateStruct);
	return 1;
}


void
MFCQuaTextCtrl::OnSize(UINT nType, int cx, int cy)
{
	CEdit::OnSize(nType, cx, cy);
	textRect.right += (cx-bounds.right);
	textRect.bottom += (cy-bounds.bottom);
	bounds.right = cx;
	bounds.bottom = cy;
}


void
MFCQuaTextCtrl::OnSizing(UINT nType, LPRECT lr)
{
	CEdit::OnSizing(nType, lr);
}

afx_msg void
MFCQuaTextCtrl::OnPaint()
{
	CEdit::OnPaint();
}

void
MFCQuaTextCtrl::DrawContents(CDC *pdc)
{
}

afx_msg BOOL
MFCQuaTextCtrl::OnEraseBkgnd(CDC* pDC)
{
	return CEdit::OnEraseBkgnd(pDC);
}

afx_msg void
MFCQuaTextCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	CEdit::OnMouseMove(nFlags, point);
} //End of OnMouseMove

afx_msg BOOL
MFCQuaTextCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	return CEdit::OnSetCursor(pWnd, nHitTest, message);
} //End of OnSetCursor

afx_msg void
MFCQuaTextCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CEdit::OnLButtonDown(nFlags, point);
} //End of OnLButtonDown


void
MFCQuaTextCtrl::SendVCMsg()
{
	if (GetParent() && vcMessid) {
		GetParent()->SendMessage(vcMessid, vcWparam, vcLparam);
	}
}

void
MFCQuaTextCtrl::SetVCMsgParams(uint32 mid, WPARAM wp, LPARAM lp)
{
	vcMessid = mid;
	vcWparam = wp;
	vcLparam = lp;
}


void
MFCQuaTextCtrl::OnKillFocus(CWnd* pNewWnd)
{
	if (pNewWnd) {
		CString	wt;
		pNewWnd->GetWindowText(wt);
		fprintf(stderr, "text ctrl edit kill focus %x new is %x, with text <%s>\n", m_hWnd, pNewWnd->m_hWnd, (LPCSTR)wt);
	} else {
		fprintf(stderr, "text ctrl edit kill focus %x to null win\n", m_hWnd);
	}
	int n = GetWindowTextLength();
	char	*buf = new char[n+1];
	GetWindowText(buf, n+1);
	setText(buf);
	fprintf(stderr, "okf: window text %s\n", buf);
	delete buf;
	SendVCMsg();
	CEdit::OnKillFocus(pNewWnd);
}

void
MFCQuaTextCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch(nChar) {
		case VK_RETURN: { // ???? don't seem to receive this in single line mode
			SetWindowText(getText().c_str());
			::SetFocus(NULL);
			break;
		}
		case VK_NEXT: {
			SetWindowText(getText().c_str());
			::SetFocus(NULL);
			break;
		}
		case VK_TAB: {
			SetWindowText(getText().c_str());
			::SetFocus(NULL);
			break;
		}
		default:
			CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
	}
}

#endif