
// MFCQuaTimeCtrl.cpp : implementation file
//
//#define _AFXDLL
#include "stdafx.h"
#include "qua_version.h"

#include "QuaMFC.h"
#include "MFCQuaTimeCtrl.h"
#include "StdDefs.h"
#include "Colors.h"


// MFCQuaTimeCtrl


#ifdef QUA_V_TE1

IMPLEMENT_DYNAMIC(MFCQuaTimeCtrl, MFCQuaTextCtrl)
MFCQuaTimeCtrl::MFCQuaTimeCtrl()
{
}

MFCQuaTimeCtrl::~MFCQuaTimeCtrl()
{
}


bool
MFCQuaTimeCtrl::CreateTimeCtrl(CRect &r, CWnd *w, UINT id, CFont *cf, CRect *tr, COLORREF *bgc, COLORREF *tbgc, COLORREF *tc)
{
	return CreateTextCtrl(r, w, id, cf, tr, bgc, tbgc, tc);
}


BEGIN_MESSAGE_MAP(MFCQuaTimeCtrl, MFCQuaTextCtrl)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()



// MFCQuaTimeCtrl message handlers


char *
MFCQuaTimeCtrl::GetText()
{
	static char	buf[100];
	strcpy(buf, time.StringValue());
	return buf;
}

void
MFCQuaTimeCtrl::SetText(char *txt)
{
	time.Set(txt);
}

void
MFCQuaTimeCtrl::SetTime(Time &t)
{
	time=t;
	RedrawWindow();
}

void
MFCQuaTimeCtrl::GetTime(Time & t)
{
	t = time;
}



afx_msg void
MFCQuaTimeCtrl::OnSetFocus(CWnd* pOldWnd)
{
	fprintf(stderr, "time ctrl on set focus\n");
	if (pOldWnd == &edit) {
		char	buf[100];
		edit.GetWindowText(buf, 99);
		SetText(buf);
//		edit.RemoveEdit();
	}
	MFCQuaTextCtrl::OnSetFocus(pOldWnd);
}

void
MFCQuaTimeCtrl::DrawContents(CDC *pdc)
{
	pdc->DrawText(time.StringValue(), &textRect, DT_VCENTER|DT_LEFT);
}


class CT: public CEdit
{
	DECLARE_DYNAMIC(CT)
public:
afx_msg void	OnKillFocus(CWnd* pNewWnd);
	DECLARE_MESSAGE_MAP()

};
BEGIN_MESSAGE_MAP(CT, CEdit)
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CT, CEdit)

void
CT::OnKillFocus(CWnd* pNewWnd)
{
	if (pNewWnd) {
		CString	wt;
		pNewWnd->GetWindowText(wt);
		fprintf(stderr, "text ctrl edit kill focus %x new is %x, with text <%s>\n", m_hWnd, pNewWnd->m_hWnd, (LPCSTR)wt);
	} else {
		fprintf(stderr, "text ctrl edit kill focus %x to null win\n", m_hWnd);
	}
	CEdit::OnKillFocus(pNewWnd);
}

afx_msg void
MFCQuaTimeCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	fprintf(stderr, "time ctrl lbd %x\n", m_hWnd);
	edit.CreateEdit(this);
//	CT *e = new CT;
//	CRect	r = OuterRect(textRect);
//	e->Create(ES_LEFT|WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL
//			|ES_MULTILINE
//			, r, this, 5656);
//	e->SetFont(displayFont);
//	e->SetWindowText(GetText());
//	e->SetFocus();
//	e->SetActiveWindow();
//	e->SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
}


/////////////////////////////////////////////////////////
// MFCQuaTimeEdit
/////////////////////////////////////////////////////////


IMPLEMENT_DYNAMIC(MFCQuaTimeEdit, MFCQuaTextCtrlEdit)

MFCQuaTimeEdit::MFCQuaTimeEdit()
{
	;
}


MFCQuaTimeEdit::~MFCQuaTimeEdit()
{
	;
}


afx_msg void
MFCQuaTimeEdit::OnLButtonDown(UINT nFlags, CPoint point) 
{
	fprintf(stderr, "time ed lbd hw %x\n", m_hWnd);
	MFCQuaTextCtrlEdit::OnLButtonDown(nFlags, point);
} 

afx_msg void
MFCQuaTimeEdit::OnMouseMove(UINT nFlags, CPoint point) 
{
	MFCQuaTextCtrlEdit::OnMouseMove(nFlags, point);
}

void
MFCQuaTimeEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch(nChar) {
		case VK_DOWN: {
		}
		break;


		case VK_UP:	{
		}
		break;
	
		case VK_PRIOR: {
		}
		break;

		case VK_HOME: {
		}
		break;

		case VK_END: {
		}
		break;

		default:
			MFCQuaTextCtrlEdit::OnKeyDown(nChar, nRepCnt, nFlags);
	}
}



afx_msg void
MFCQuaTimeEdit::OnSetFocus(CWnd* pOldWnd)
{
	if (pOldWnd != NULL) {
		fprintf(stderr, "time ctrl edit on set focus from %x\n", pOldWnd->m_hWnd);
	}
	MFCQuaTextCtrlEdit::OnSetFocus(pOldWnd);
}

BEGIN_MESSAGE_MAP(MFCQuaTimeEdit, MFCQuaTextCtrlEdit)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

#else

IMPLEMENT_DYNAMIC(MFCQuaTimeCtrl, MFCQuaTextCtrl)
MFCQuaTimeCtrl::MFCQuaTimeCtrl()
{
}

MFCQuaTimeCtrl::~MFCQuaTimeCtrl()
{
}


bool
MFCQuaTimeCtrl::CreateTimeCtrl(CRect &r, CWnd *w, UINT id, CFont *cf, CRect *tr, COLORREF *bgc, COLORREF *tbgc, COLORREF *tc)
{
	return CreateTextCtrl(r, w, id, cf, tr, bgc, tbgc, tc);
}


BEGIN_MESSAGE_MAP(MFCQuaTimeCtrl, MFCQuaTextCtrl)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
//	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_SETFOCUS()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()



// MFCQuaTimeCtrl message handlers


string
MFCQuaTimeCtrl::getText()
{
	static char	buf[100];
	GetWindowText(buf, 100);
//	strcpy(buf, time.StringValue());
	return buf;
}

void
MFCQuaTimeCtrl::setText(const char *txt)
{
	time.Set(txt);
	CEdit::SetWindowText(time.StringValue());
}

void
MFCQuaTimeCtrl::SetTime(Time &t)
{
	time=t;
	CEdit::SetWindowText(time.StringValue());
}

void
MFCQuaTimeCtrl::GetTime(Time & t)
{
	t = time;
}



afx_msg void
MFCQuaTimeCtrl::OnSetFocus(CWnd* pOldWnd)
{
	fprintf(stderr, "time ctrl on set focus\n");
	MFCQuaTextCtrl::OnSetFocus(pOldWnd);
}

void
MFCQuaTimeCtrl::DrawContents(CDC *pdc)
{
//	pdc->DrawText(time.StringValue(), &textRect, DT_VCENTER|DT_LEFT);
}

void
MFCQuaTimeCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch(nChar) {
		case VK_DOWN: {
		}
		break;


		case VK_UP:	{
		}
		break;
	
		case VK_PRIOR: {
		}
		break;

		case VK_HOME: {
		}
		break;

		case VK_END: {
		}
		break;

		default:
			MFCQuaTextCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
	}
}

afx_msg void
MFCQuaTimeCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	fprintf(stderr, "time ctrl lbd %x\n", m_hWnd);
	MFCQuaTextCtrl::OnLButtonDown(nFlags, point);
//	edit.CreateEdit(this);
//	CT *e = new CT;
//	CRect	r = OuterRect(textRect);
//	e->Create(ES_LEFT|WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL
//			|ES_MULTILINE
//			, r, this, 5656);
//	e->SetFont(displayFont);
//	e->SetWindowText(GetText());
//	e->SetFocus();
//	e->SetActiveWindow();
//	e->SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
}


#endif