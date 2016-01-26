#include "qua_version.h"
// MFCQuaIntCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "QuaMFC.h"
#include "MFCQuaIntCtrl.h"
#include "DaBasicTypes.h"
#include "Colors.h"
#include "StdDefs.h"

// MFCQuaTimeCtrl

#ifdef QUA_V_TE1

IMPLEMENT_DYNAMIC(MFCQuaIntCtrl, MFCQuaTextCtrl)
MFCQuaIntCtrl::MFCQuaIntCtrl()
{
	value = 0;
	min = 0;
	max = 1;
	mid = 1;
	step = 1;
}

MFCQuaIntCtrl::~MFCQuaIntCtrl()
{
}


bool
MFCQuaIntCtrl::CreateIntCtrl(CRect &r, CWnd *w, UINT id, CFont *cf, CRect *tr, COLORREF *bgc, COLORREF *tbgc, COLORREF *tc)
{
	return CreateTextCtrl(r, w, id, cf, tr, bgc, tbgc, tc);
}

void
MFCQuaIntCtrl::SetValue(long v)
{
	if (v < min) {
		v = min;
	} else if (v > max) {
		v = max;
	}
	value = v;
	RedrawDisplay();
}

void
MFCQuaIntCtrl::SetRange(long mi, long md, long mx)
{
	min = mi;
	mid = md;
	max = mx;
	if (min > max) {
		min = max;
	}
	if (mid > max) {
		mid = max;
	} else if (mid < min) {
		mid = min;
	}
}

void
MFCQuaIntCtrl::SetSteps(long s, long s2, long s4)
{
	step = s;
	step_2 = s2;
	step_4 = s4;
}

BEGIN_MESSAGE_MAP(MFCQuaIntCtrl, MFCQuaTextCtrl)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_KEYDOWN()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()



// MFCQuaTimeCtrl message handlers


void
MFCQuaIntCtrl::DrawContents(CDC *pdc)
{
	pdc->DrawText(GetText(), &textRect, DT_VCENTER|DT_LEFT);
}

char *
MFCQuaIntCtrl::GetText()
{
	static char	buf[100];
	sprintf(buf, "%d", value);
	return buf;
}

void
MFCQuaIntCtrl::SetText(char *txt)
{
	SetValue(atoi(txt));
}

afx_msg void
MFCQuaIntCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	edit.CreateEdit(this);
}

afx_msg void
MFCQuaIntCtrl::OnSetFocus(CWnd* pOldWnd)
{
}

/////////////////////////////////////
// MFCQuaIntEdit
/////////////////////////////////////

IMPLEMENT_DYNAMIC(MFCQuaIntEdit, MFCQuaTextCtrlEdit)

MFCQuaIntEdit::MFCQuaIntEdit()
{
	;
}

MFCQuaIntEdit::~MFCQuaIntEdit()
{
	;
}

afx_msg void
MFCQuaIntEdit::OnMouseMove(UINT nFlags, CPoint point) 
{
	MFCQuaTextCtrlEdit::OnMouseMove(nFlags, point);
} //End of OnMouseMove

 //End of OnSetCursor

afx_msg void
MFCQuaIntEdit::OnLButtonDown(UINT nFlags, CPoint point) 
{
	MFCQuaTextCtrlEdit::OnLButtonDown(nFlags, point);
} //End of OnLButtonDown

void
MFCQuaIntEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	MFCQuaIntCtrl	*fc = (MFCQuaIntCtrl *)ctrl;
	switch(nChar) {
		case VK_DOWN: {
			fc->SetValue(fc->value-fc->step);
			SetWindowText(fc->GetText());
			fc->SendVCMsg();
			break;
		}

		case VK_UP:	{
			fc->SetValue(fc->value+fc->step);
			SetWindowText(fc->GetText());
			fc->SendVCMsg();
			break;
		}
	
		case VK_PRIOR: {
			fc->SetValue(fc->mid);
			SetWindowText(fc->GetText());
			fc->SendVCMsg();
			break;
		}

		case VK_HOME: {
			fc->SetValue(fc->min);
			SetWindowText(fc->GetText());
			fc->SendVCMsg();
			break;
		}

		case VK_END: {
			fc->SetValue(fc->max);
			SetWindowText(fc->GetText());
			fc->SendVCMsg();
			break;
		}

		default:
			fprintf(stderr, "fgfg %d\n", nChar);
			MFCQuaTextCtrlEdit::OnKeyDown(nChar, nRepCnt, nFlags);
			break;
	}
}


BEGIN_MESSAGE_MAP(MFCQuaIntEdit, MFCQuaTextCtrlEdit)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_KILLFOCUS()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

#else

IMPLEMENT_DYNAMIC(MFCQuaIntCtrl, MFCQuaTextCtrl)
MFCQuaIntCtrl::MFCQuaIntCtrl()
{
	value = 0;
	min = 0;
	max = 1;
	mid = 1;
	step = 1;
}

MFCQuaIntCtrl::~MFCQuaIntCtrl()
{
}


bool
MFCQuaIntCtrl::CreateIntCtrl(CRect &r, CWnd *w, UINT id, CFont *cf, CRect *tr, COLORREF *bgc, COLORREF *tbgc, COLORREF *tc)
{
	return CreateTextCtrl(r, w, id, cf, tr, bgc, tbgc, tc);
}

void
MFCQuaIntCtrl::SetValue(long v)
{
	if (v < min) {
		v = min;
	} else if (v > max) {
		v = max;
	}
	value = v;
	SetWindowText(GetText());
}

void
MFCQuaIntCtrl::SetRange(long mi, long md, long mx)
{
	min = mi;
	mid = md;
	max = mx;
	if (min > max) {
		min = max;
	}
	if (mid > max) {
		mid = max;
	} else if (mid < min) {
		mid = min;
	}
}

void
MFCQuaIntCtrl::SetSteps(long s, long s2, long s4)
{
	step = s;
	step_2 = s2;
	step_4 = s4;
}


// MFCQuaTimeCtrl message handlers


void
MFCQuaIntCtrl::DrawContents(CDC *pdc)
{
//	pdc->DrawText(GetText(), &textRect, DT_VCENTER|DT_LEFT);
}

char *
MFCQuaIntCtrl::GetText()
{
	static char	buf[100];
	sprintf(buf, "%d", value);
	return buf;
}

void
MFCQuaIntCtrl::SetText(char *txt)
{
	SetValue(atoi(txt));
}

afx_msg void
MFCQuaIntCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
}

afx_msg void
MFCQuaIntCtrl::OnSetFocus(CWnd* pOldWnd)
{
}

void
MFCQuaIntCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch(nChar) {
		case VK_DOWN: {
			SetValue(value-step);
			SendVCMsg();
			break;
		}

		case VK_UP:	{
			SetValue(value+step);
			SendVCMsg();
			break;
		}
	
		case VK_PRIOR: {
			SetValue(mid);
			SendVCMsg();
			break;
		}

		case VK_HOME: {
			SetValue(min);
			SendVCMsg();
			break;
		}

		case VK_END: {
			SetValue(max);
			SendVCMsg();
			break;
		}

		default:
			fprintf(stderr, "fgfg %d\n", nChar);
			MFCQuaTextCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
			break;
	}
}

BEGIN_MESSAGE_MAP(MFCQuaIntCtrl, MFCQuaTextCtrl)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_KEYDOWN()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()


#endif