#include "qua_version.h"

#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

#include <math.h>
#include <afxwin.h>
#include <afxcmn.h>


#include "DaBasicTypes.h"
#include "DaErrorCodes.h"
#include "Colors.h"
#include "MemDC.h"
#include "StdDefs.h"
#include "QuaMFC.h"
#include "MFCQuaRotor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CFont	MFCQuaRotor::displayFont;
CFont	MFCSmallQuaRotor::displayFont;

//////////////////////////////////////////////////////////////////////////////////
// MFCQuaSmallRotor
//////////////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNAMIC(MFCQuaRotor, CSliderCtrl)

MFCQuaRotor::MFCQuaRotor()
{
	knobRadius = 0;
	zeroMark = 0;
	isDragging = false;
	
	ptKnob.x = 0;
	ptKnob.y = 0;
	ptCenter.x = 0;
	ptCenter.y = 0;
	ptScale.x = 0;
	ptScale.y = 0;
	ptDot.x = 0;
	ptDot.y = 0;
	labelWidth = 0;

	vcMessid = 0;
	mdMessid = 0;
	muMessid = 0;
	vcLparam = 0;
	mdLparam = 0;
	muLparam = 0;
	vcWparam = 0;
	mdWparam = 0;
	muWparam = 0;

	angMin = 30;
	angMax = 330;
	pixelScaleLength = 200;
}

MFCQuaRotor::~MFCQuaRotor()
{
}


void
MFCQuaRotor::SendVCMsg()
{
	if (GetParent() && vcMessid) {
		GetParent()->SendMessage(vcMessid, vcWparam, vcLparam);
	}
}

void
MFCQuaRotor::SetVCMsgParams(uint32 mid, WPARAM wp, LPARAM lp)
{
	vcMessid = mid;
	vcWparam = wp;
	vcLparam = lp;
}


void
MFCQuaRotor::SendMDMsg()
{
	if (GetParent() && mdMessid) {
		GetParent()->SendMessage(mdMessid, mdWparam, mdLparam);
	}
}

void
MFCQuaRotor::SetMDMsgParams(uint32 mid, WPARAM wp, LPARAM lp)
{
	mdMessid = mid;
	mdWparam = wp;
	mdLparam = lp;
}


void
MFCQuaRotor::SendMUMsg()
{
	if (GetParent() && muMessid) {
		GetParent()->SendMessage(muMessid, muWparam, muLparam);
	}
}

void
MFCQuaRotor::SetMUMsgParams(uint32 mid, WPARAM wp, LPARAM lp)
{
	muMessid = mid;
	muWparam = wp;
	muLparam = lp;
}


BEGIN_MESSAGE_MAP(MFCQuaRotor, CSliderCtrl)
	//{{AFX_MSG_MAP(MFCQuaRotor)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_NOTIFY(TTN_NEEDTEXTW, 0, OnToolTipNotify)
	ON_NOTIFY(TTN_NEEDTEXTA, 0, OnToolTipNotify)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void
MFCQuaRotor::PreSubclassWindow() 
{
	CSliderCtrl::PreSubclassWindow();

	EnableToolTips();
	SetRange(30, 329, FALSE);
	SetLineSize(3);
	SetPageSize(30);
}

afx_msg BOOL
MFCQuaRotor::OnToolTipNeedsTxt( UINT id, NMHDR * pTTTStruct, LRESULT * pResult )
{
	return true;
}


BOOL
MFCQuaRotor::PreCreateWindow(CREATESTRUCT& cs) 
{
	if(!CSliderCtrl::PreCreateWindow(cs)) return FALSE;

//	SetLineSize(3);
//	SetPageSize(30);

	return TRUE;
}

HBITMAP
MFCQuaRotor::GetKnobBitmap()
{
	return(knobBitmap);
}

HBITMAP
MFCQuaRotor::GetDotBitmap()
{
	return(dotBitmap);
}

HBITMAP
MFCQuaRotor::GetScaleBitmap()
{
	return(scaleBitmap);
}


bool
MFCQuaRotor::SetKnobBitmap(HBITMAP hbmp)
{
	if(knobBitmap.GetSafeHandle()) {
		knobBitmap.Detach();
	}

	knobBitmap.Attach(hbmp);
	return true;
}

bool
MFCQuaRotor::SetKnobResource(uint32 hbmp)
{
	if(knobBitmap.GetSafeHandle()) {
		knobBitmap.Detach();
	}

	if (!knobBitmap.LoadBitmap(hbmp)) {
		return false;
	}
	return true;
}

void
MFCQuaRotor::SetKnobParams()
{
	int wScale=0;
	int hScale=0;
	int wKnob=0;
	int hKnob=0;
	int wDot=0;
	int hDot=0;

	BOOL kValid = knobBitmap.GetSize(wKnob, hKnob);
	BOOL dValid = dotBitmap.GetSize(wDot, hDot);
	BOOL sValid = scaleBitmap.GetSize(wScale, hScale);

	if (!sValid) {
		if (kValid) {
			wScale = wKnob;
			hScale = hKnob;
		} else {
			wScale = bounds.right-labelWidth;
			hScale = bounds.bottom;
		}
	}
	if (!dValid) {
		wDot = hDot = 0;
	}
	if (!kValid) {
		wKnob = bounds.right-labelWidth;
		hKnob = bounds.bottom;
	}

	ptCenter.x = labelWidth + wScale / 2;
	ptCenter.y = bounds.bottom / 2;
	ptScale.x = ptCenter.x - (wScale / 2);
	ptScale.y = ptCenter.y - (hScale / 2);
	ptKnob.x = ptCenter.x - (wKnob / 2);
	ptKnob.y = ptCenter.y - (hKnob / 2);

	knobRadius = wDot?(wKnob/2 - wDot/2 -3):(wKnob/2-1);

	SetFloatValue(fValue);
	RedrawWindow();
}

bool
MFCQuaRotor::SetDotBitmap(HBITMAP hbmp)
{
	if(dotBitmap.GetSafeHandle()) {
		dotBitmap.Detach();
	}

	dotBitmap.Attach(hbmp);

	return true;
}

bool
MFCQuaRotor::SetDotResource(uint32 hbmp)
{
	if(dotBitmap.GetSafeHandle()) {
		dotBitmap.Detach();
	}

	if (!dotBitmap.LoadBitmap(hbmp)) {
		return false;
	}

	return true;
}


bool
MFCQuaRotor::SetScaleBitmap(HBITMAP hbmp)
{
	if(scaleBitmap.GetSafeHandle()) {
		scaleBitmap.Detach();
	}

	scaleBitmap.Attach(hbmp);
	return true;
}


bool
MFCQuaRotor::SetScaleResource(uint32 hbmp)
{
	if(scaleBitmap.GetSafeHandle()) {
		scaleBitmap.Detach();
	}

	scaleBitmap.LoadBitmap(hbmp);

	return true;
}


BOOL
MFCQuaRotor::OnEraseBkgnd(CDC* pDC) 
{
//	fprintf(stderr, "on erase %s\n", label());
	return TRUE; // don't draw background
}

// ????? TODO. try and get the transparency to work!!!
void
MFCQuaRotor::PaintRotor(CPaintDC *dc) 
{
//	CMemDC memDC(dc);
//	memDC.SetBkMode(TRANSPARENT);

	if (scaleBitmap.isLoaded) {
		scaleBitmap.DrawTransparent(dc->m_hDC,ptScale.x,ptScale.y,0,0);
	} else {
//		CRect	bg(labelWidth,0,bounds.right, bounds.bottom);
//		CBrush	b(bgColor); // hack because the memdc seems to stuff transparency
//		memDC.FillRect(&bg, &b);
	}
	if (knobBitmap.isLoaded) {
		knobBitmap.DrawTransparent(dc->m_hDC,ptKnob.x,ptKnob.y, 0, 0);
	}
	if (dotBitmap.isLoaded) {
		dotBitmap.DrawTransparent(dc->m_hDC,ptDot.x,ptDot.y, 0, 0);
	} else {
		dc->SetDCPenColor(RGB(0,0,0));
		dc->MoveTo(ptCenter.x, ptCenter.y);
		dc->LineTo(ptDot);
	}
}

void
MFCQuaRotor::RedrawRotor()
{
	CRect	rr(labelWidth,0,bounds.right, bounds.bottom);
	RedrawWindow(&rr);
}
	


void
MFCQuaRotor::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CRect	tr(0,0,labelWidth, bounds.bottom);
	dc.DrawText(label(), -1, &tr, DT_LEFT|DT_VCENTER);
	dc.ExcludeClipRect(0,0,labelWidth, bounds.bottom);
	dc.SetBkMode(TRANSPARENT);
	PaintRotor(&dc);
	return;
}


/**
*
* @desc		MOve by the specified delta position in degrees
*/
bool
MFCQuaRotor::PixelDelta(int nDelta, int deltaScale)
{
	float fDelta = ((float)nDelta)/(pixelScaleLength*deltaScale);
	float newValue = fValue+fDelta*(fMax-fMin);
	bool chg_dot = SetFloatValue(newValue);
	if (nDelta != 0) {
		SendVCMsg();
	}
	return chg_dot;
}


// mouse
void
MFCQuaRotor::OnLButtonDown(UINT nFlags, CPoint point) 
{
	ptHit = point;

//	EnableTrackingToolTips(true);
	if(!isDragging)	{
		SendMDMsg();
		isDragging = true;
		SetCapture();
		SetFocus();
		RedrawRotor();
	} else {
		CSliderCtrl::OnLButtonDown(nFlags, point);
	}
//	if (GetToolTips()) {
//		GetToolTips()->Update();
//	}
}


void
MFCQuaRotor::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(isDragging) {
		int	deltaScale=1;
		if (nFlags & MK_CONTROL) {
			deltaScale *= 5;
		}
		if (nFlags & MK_SHIFT) {
			deltaScale *= 10;
		}
		if(PixelDelta(ptHit.y-point.y, deltaScale)) {
			RedrawRotor();
			ptHit = point;
		}
	} else {
		CSliderCtrl::OnMouseMove(nFlags, point);
	}
//	if (GetToolTips()) {
//		GetToolTips()->SetDelayTime(0);
//		GetToolTips()->UpdateTipText(LPSTR_TEXTCALLBACK, this);
//		fprintf(stderr, "moving update\n");
//		GetToolTips()->Update();
//	}
}

void
MFCQuaRotor::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if(isDragging) {
		isDragging = false;
		SendMUMsg();
		ReleaseCapture();
		PixelDelta(ptHit.y-point.y, 1);
		RedrawRotor();
	} else {
		CSliderCtrl::OnLButtonUp(nFlags, point);
	}
//	if (GetToolTips()) {
//		GetToolTips()->Pop();
//		EnableTrackingToolTips(false);
//		GetToolTips()->UpdateTipText("test", this);
//	}
}

// keyboard

void
MFCQuaRotor::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	int nMin = GetRangeMin();
	int nMax = GetRangeMax()+1;

	switch(nChar) {
		case VK_LEFT: {
		case VK_DOWN:
			PixelDelta(-1, 1);
			RedrawRotor();
		}
		break;


		case VK_RIGHT: 
		case VK_UP:	{
			PixelDelta(+1, 1);
			RedrawRotor();
		}
		break;
	
		case VK_PRIOR: {
			SetFloatValue(fMid);
			SendVCMsg();
			RedrawRotor();
		}
		break;

		case VK_HOME: {
			SetFloatValue(fMin);
			SendVCMsg();
			RedrawRotor();
		}
		break;

		case VK_END: {
			SetFloatValue(fMax);
			SendVCMsg();
			RedrawRotor();
		}
		break;

		default:
			CSliderCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
	}
}


void MFCQuaRotor::SetKnobRadius(int nKnobRadius)
{
	knobRadius = nKnobRadius;
	SetFloatValue(fValue);
	RedrawWindow();
}


int MFCQuaRotor::GetKnobRadius() const
{
	return knobRadius;
}

/**
* @todo		Zero is not yet implemented
*
*/
void MFCQuaRotor::SetZero(int nZero)
{
	zeroMark = nZero;
}

int MFCQuaRotor::GetZero() const
{
	return zeroMark;
}


bool
MFCQuaRotor::SetFloatValue(float v)
{
	if (v < fMin) {
		v = fMin;
	} else if (v > fMax) {
		v = fMax;
	}
	fValue = v;

	double anglePos = angMin + (fValue-fMin)*(angMax-angMin)/(fMax-fMin);
	double dbDegree = 360 - ((double)(anglePos));
	double radPos = dbDegree*M_PI/180.0; // degrees to radians

	int nwDot=2;
	int nhDot=2;
	if (!dotBitmap.GetSize(nwDot, nhDot)) {
		nwDot = 0;
		nhDot = 0;
	}
	
	// radians
	CPoint	oldDot = ptDot;

	ptDot.x = ptCenter.x - (nwDot/2) + knobRadius * sin(radPos);
	ptDot.y = ptCenter.y - (nhDot/2) + knobRadius * cos(radPos);

	return (ptDot.x != oldDot.x) || (ptDot.y != oldDot.y);
}

void
MFCQuaRotor::SetFloatRange(float min, float mid, float max)
{
	if (min > max) min = max;
	if (mid < min) mid = min;
	if (mid > max) mid = max;
	fMin = min;
	fMid = mid;
	fMax = max;
}

void
MFCQuaRotor::SetAngleRange(long angmi, long angmx)
{
	angMin = angmi;
	angMax = angmx;
}

void
MFCQuaRotor::SetPixelScaleLength(long psl)
{
	pixelScaleLength = psl;
}

bool
MFCQuaRotor::CreateRotor(char *lbl, CRect &r, CWnd *w, UINT id, float ini, float fmin, float fmid, float fmax, int angmi, int angmx, int lw)
{
	if (!CreateEx(NULL, WS_CHILD|WS_VISIBLE|TBS_TOOLTIPS, r, w, id)) {
		return false;
	}
	SetDlgCtrlID(id);
	CToolTipCtrl	*t=GetToolTips();
	if (t) {
		t->SetDelayTime(0);
	}
	label = lbl;

	bounds = r;
	bounds.MoveToXY(0,0);

	SetFloatRange(fmin, fmid, fmax);
	SetAngleRange(angmi, angmx);
	SetPixelScaleLength(200);

// causes a messy z-order redrawing issue!!!!
//	ModifyStyleEx(NULL, WS_EX_TRANSPARENT, NULL);

	if (!SetKnobResource(IDB_ROT_SM_K)) {
		return false;
	}
	if (lw >= 0) {
		labelWidth = lw;
	} else {
		labelWidth = bounds.right-knobBitmap.GetWidth();
		if (labelWidth < 0) {
			labelWidth = 0;
		}
	}
	bgColor = rgb_orange;
	SetKnobParams();
	SetFloatValue(ini);

	return true;
}

int
MFCQuaRotor::OnToolHitTest(CPoint point, TOOLINFO *pTI) const
{
	pTI->hwnd = m_hWnd;
	pTI->uFlags = 0; // we need to differ tools by ID not window handle
	pTI->lpszText = LPSTR_TEXTCALLBACK; // tell tooltips to send TTN_NEEDTEXT	
		// point is in this rectangle. 
	pTI->rect = CRect(-300,-300,300,300);
		// TTN_NEEDTEXT Message will use this ID to fill NMHDR structure
	pTI->uId = 0;
	return 0; // return index of rectangle
}
	
void
MFCQuaRotor::OnToolTipNotify(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 1;	
	// need to handle both ANSI and UNICODE versions of the message
	TOOLTIPTEXTA* pttta = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* ptttw = (TOOLTIPTEXTW*)pNMHDR;
	CString csToolText;
	
	UINT uiID = pNMHDR->idFrom;
	
	int iID = GetDlgCtrlID();
//	fprintf(stderr, "ottn\n");

	char buf[256];
	if (label()) {
		sprintf(buf, "%s:%g", label(), fValue);
	} else {
		sprintf(buf, "%g", fValue);
	}
	if (pNMHDR->code == TTN_NEEDTEXTA) {
	//	pttta->lpszText = buf;
		strcpy(pttta->szText, buf);
	} else { 
		WCHAR	*p=ptttw->szText;
		char	*q=buf;
		while (*p++=*q++);
	} 
//	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// MFCSmallQuaRotor
//////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(MFCSmallQuaRotor, CSliderCtrl)


MFCSmallQuaRotor::MFCSmallQuaRotor()
{
}

MFCSmallQuaRotor::~MFCSmallQuaRotor()
{
}

bool
MFCSmallQuaRotor::CreateRotor(char *lbl, CRect &r, CWnd *w, UINT id, char*short_label, float ini, float fmin, float fmid, float fmax, int angMin, int angMax, int lw)
{
	if (!MFCQuaRotor::CreateRotor(lbl, r, w, id, ini, fmin, fmid, fmax, angMin, angMax, lw)) {
		return false;
	}
	shortLabel = short_label;
	if (!SetKnobResource(IDB_ROT_SM_K)) {
		return false;
	}

	if (lw >= 0) {
		labelWidth = lw;
	} else {
		labelWidth = bounds.right-knobBitmap.GetWidth();
		if (labelWidth < 0) {
			labelWidth = 0;
		}
	}
	SetKnobParams();
	SetFloatValue(ini);
	return true;
}


void
MFCSmallQuaRotor::OnPaint() 
{
//	fprintf(stderr, "on paint small rotor %s\n", shortLabel());
	CPaintDC dc(this); // device context for painting
	dc.SetBkMode(TRANSPARENT);
	CRect	tr(0,0,labelWidth, bounds.bottom);
	dc.SelectObject(&displayFont);
	dc.DrawText(shortLabel(), -1, &tr, DT_CENTER|DT_VCENTER);
	dc.ExcludeClipRect(0,0,labelWidth, bounds.bottom);
	PaintRotor(&dc);
	return;
}


BEGIN_MESSAGE_MAP(MFCSmallQuaRotor, MFCQuaRotor)
	//{{AFX_MSG_MAP(MFCQuaRotor)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_NOTIFY(TTN_NEEDTEXTW, 0, OnToolTipNotify)
	ON_NOTIFY(TTN_NEEDTEXTA, 0, OnToolTipNotify)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()
