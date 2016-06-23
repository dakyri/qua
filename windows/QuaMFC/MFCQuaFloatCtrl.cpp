
// MFCQuaFloatCtrl.cpp : implementation file
//
#define _AFXDLL
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE
#include "stdafx.h"
#include "qua_version.h"
#include "QuaMFC.h"
#include "MFCQuaFloatCtrl.h"
#include "StdDefs.h"
#include "Colors.h"


// MFCQuaTimeCtrl
#ifdef QUA_V_TE1
IMPLEMENT_DYNAMIC(MFCQuaFloatCtrl, MFCQuaTextCtrl)
MFCQuaFloatCtrl::MFCQuaFloatCtrl()
{
	value = 0;
	min = 0;
	max = 1;
	mid = 0.5f;
	step = 0.1f;
}

MFCQuaFloatCtrl::~MFCQuaFloatCtrl()
{
}


bool
MFCQuaFloatCtrl::CreateFloatCtrl(CRect &r, CWnd *w, UINT id, CFont *cf, CRect *tr, COLORREF *bgc, COLORREF *tbgc, COLORREF *tc)
{
	return CreateTextCtrl(r, w, id, cf, tr, bgc, tbgc, tc);
}

void
MFCQuaFloatCtrl::SetValue(float v)
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
MFCQuaFloatCtrl::SetRange(float mi, float md, float mx)
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
MFCQuaFloatCtrl::SetSteps(float s, float s2, float s4)
{
	step = s;
	step_2 = s2;
	step_4 = s4;
}

BEGIN_MESSAGE_MAP(MFCQuaFloatCtrl, MFCQuaTextCtrl)
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
MFCQuaFloatCtrl::DrawContents(CDC *pdc)
{
	pdc->DrawText(GetText(), &textRect, DT_VCENTER|DT_LEFT);
}

char *
MFCQuaFloatCtrl::GetText()
{
	static char	buf[100];
	sprintf(buf, "%g", value);
	return buf;
}

void
MFCQuaFloatCtrl::SetText(char *txt)
{
	SetValue((float)atof(txt));
}

afx_msg void
MFCQuaFloatCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	edit.CreateEdit(this);
}

afx_msg void
MFCQuaFloatCtrl::OnSetFocus(CWnd* pOldWnd)
{
}

/////////////////////////////////////
// MFCQuaFloatEdit
/////////////////////////////////////

IMPLEMENT_DYNAMIC(MFCQuaFloatEdit, MFCQuaTextCtrlEdit)

MFCQuaFloatEdit::MFCQuaFloatEdit()
{
	;
}

MFCQuaFloatEdit::~MFCQuaFloatEdit()
{
	;
}

afx_msg void
MFCQuaFloatEdit::OnMouseMove(UINT nFlags, CPoint point) 
{
	MFCQuaTextCtrlEdit::OnMouseMove(nFlags, point);
} //End of OnMouseMove

 //End of OnSetCursor

afx_msg void
MFCQuaFloatEdit::OnLButtonDown(UINT nFlags, CPoint point) 
{
	MFCQuaTextCtrlEdit::OnLButtonDown(nFlags, point);
} //End of OnLButtonDown

void
MFCQuaFloatEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	MFCQuaFloatCtrl	*fc = (MFCQuaFloatCtrl *)ctrl;
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


BEGIN_MESSAGE_MAP(MFCQuaFloatEdit, MFCQuaTextCtrlEdit)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_KILLFOCUS()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()
#else
IMPLEMENT_DYNAMIC(MFCQuaFloatCtrl, MFCQuaTextCtrl)
MFCQuaFloatCtrl::MFCQuaFloatCtrl()
{
	value = 0;
	min = 0;
	max = 1;
	mid = 0.5f;
	step = 0.1f;
}

MFCQuaFloatCtrl::~MFCQuaFloatCtrl()
{
}


bool
MFCQuaFloatCtrl::CreateFloatCtrl(CRect &r, CWnd *w, UINT id, CFont *cf, CRect *tr, COLORREF *bgc, COLORREF *tbgc, COLORREF *tc)
{
	return CreateTextCtrl(r, w, id, cf, tr, bgc, tbgc, tc);
}

void
MFCQuaFloatCtrl::SetValue(float v)
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
MFCQuaFloatCtrl::SetRange(float mi, float md, float mx)
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
MFCQuaFloatCtrl::SetSteps(float s, float s2, float s4)
{
	step = s;
	step_2 = s2;
	step_4 = s4;
}



// MFCQuaTimeCtrl message handlers


void
MFCQuaFloatCtrl::DrawContents(CDC *pdc)
{
//	pdc->DrawText(GetText(), &textRect, DT_VCENTER|DT_LEFT);
}

char *
MFCQuaFloatCtrl::GetText()
{
	static char	buf[100];
	sprintf(buf, "%g", value);
//	GetWindowText(buf, 100);
	return buf;
}

void
MFCQuaFloatCtrl::SetText(char *txt)
{
	SetValue((float)atof(txt));
}

afx_msg void
MFCQuaFloatCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	fprintf(stderr, "float ctrl lbd %x\n", m_hWnd);
	MFCQuaTextCtrl::OnLButtonDown(nFlags, point);
}

afx_msg void
MFCQuaFloatCtrl::OnSetFocus(CWnd* pOldWnd)
{
	MFCQuaTextCtrl::OnSetFocus(pOldWnd);
}


void
MFCQuaFloatCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
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


BEGIN_MESSAGE_MAP(MFCQuaFloatCtrl, MFCQuaTextCtrl)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
//	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_KEYDOWN()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

#endif