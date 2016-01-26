#include "qua_version.h"
// MFCQuaButton.cpp : implementation file
//

#include "stdafx.h"
#include "QuaMFC.h"
#include "MFCQuaButton.h"
#include "StdDefs.h"
#include "Colors.h"


CFont	MFCQuaButton::displayFont;
CFont	MFCQuaChkButton::displayFont;
CFont	MFCSmallQuaChkBut::displayFont;
CFont	MFCSmallQuaLetterBut::displayFont;

/////////////////////////////////////////////////////////////////
// MFCQuaButton
/////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(MFCQuaButton, CButton)

MFCQuaButton::MFCQuaButton()
{
	buttonPressState = QUA_BPS_UP;
	mouseTracking = false;
	lcMessid = 0;
	lcWparam = 0;
	lcLparam = 0;
	clcMessid = 0;
	clcWparam = 0;
	clcLparam = 0;
	slcMessid = 0;
	slcWparam = 0;
	slcLparam = 0;
	cslcMessid = 0;
	cslcWparam = 0;
	cslcLparam = 0;
}

MFCQuaButton::~MFCQuaButton()
{
}

void
MFCQuaButton::SendLCMsg()
{
	if (GetParent() && lcMessid) {
		GetParent()->SendMessage(lcMessid, lcWparam, lcLparam);
	}
}

void
MFCQuaButton::SendCtlLCMsg()
{
	if (GetParent() && clcMessid) {
		GetParent()->SendMessage(clcMessid, clcWparam, clcLparam);
	}
}

void
MFCQuaButton::SendShLCMsg()
{
	if (GetParent() && slcMessid) {
		GetParent()->SendMessage(slcMessid, slcWparam, slcLparam);
	}
}

void
MFCQuaButton::SendCtlShLCMsg()
{
	if (GetParent() && cslcMessid) {
		GetParent()->SendMessage(cslcMessid, cslcWparam, cslcLparam);
	}
}

void
MFCQuaButton::SetLCMsgParams(uint32 mid, WPARAM wp, LPARAM lp)
{
	lcMessid = mid;
	lcWparam = wp;
	lcLparam = lp;
}


void
MFCQuaButton::SetCtlLCMsgParams(uint32 mid, WPARAM wp, LPARAM lp)
{
	clcMessid = mid;
	clcWparam = wp;
	clcLparam = lp;
}

void
MFCQuaButton::SetShLCMsgParams(uint32 mid, WPARAM wp, LPARAM lp)
{
	slcMessid = mid;
	slcWparam = wp;
	slcLparam = lp;
}

void
MFCQuaButton::SetCtlShLCMsgParams(uint32 mid, WPARAM wp, LPARAM lp)
{
	cslcMessid = mid;
	cslcWparam = wp;
	cslcLparam = lp;
}

void
MFCQuaButton::DrawItem(LPDRAWITEMSTRUCT disP)
{
	int	w, h;
//	if (tooltipText())
//		fprintf(stderr, "draw item %s", tooltipText());
	if (buttonPressState == QUA_BPS_UP) {
		upBitmap.GetSize(w, h);
		upBitmap.Draw(disP->hDC, 0, 0, w, h);
	} else if (buttonPressState == QUA_BPS_DOWN) {
		dnBitmap.GetSize(w, h);
		dnBitmap.Draw(disP->hDC, 0, 0, w, h);
	} else if (buttonPressState == QUA_BPS_OVER) {
		ovrBitmap.GetSize(w, h);
		ovrBitmap.Draw(disP->hDC, 0, 0, w, h);
	} else { // we are screwed
	}
}


afx_msg void
MFCQuaButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	uchar	priorState = buttonPressState;
	buttonPressState = QUA_BPS_UP;
	if (bounds.Contains(point)) {
	} else {
	}
	if (mouseTracking) {
		ReleaseCapture();
		mouseTracking = false;
	}
	if (priorState != buttonPressState) {
		Invalidate();
	}
}

afx_msg void
MFCQuaButton::OnLButtonUp(UINT nFlags,  CPoint point)
{
	uchar	priorState = buttonPressState;
	buttonPressState = QUA_BPS_UP;
	if (bounds.Contains(point)) {
		if (cslcMessid && (nFlags & MK_CONTROL) && (nFlags & MK_SHIFT)) {
			SendCtlShLCMsg();
		} else if (clcMessid && (nFlags & MK_CONTROL)) {
			SendCtlLCMsg();
		} else if (slcMessid && (nFlags & MK_SHIFT)) {
			SendShLCMsg();
		} else {
			SendLCMsg();
		}
	} else {
	}
	if (mouseTracking) {
		ReleaseCapture();
		mouseTracking = false;
	}
	if (priorState != buttonPressState) {
		Invalidate();
	}
}

afx_msg void
MFCQuaButton::OnMouseMove(UINT nFlags,CPoint point)
{
	uchar	priorState = buttonPressState;
	bool	mouseButton = false;
	if (nFlags & (MK_LBUTTON|MK_MBUTTON|MK_RBUTTON)) {
		mouseButton = true;
	}
	if (bounds.Contains(point)) {
		if (mouseButton) {
			buttonPressState = QUA_BPS_DOWN;
		} else {
			buttonPressState = QUA_BPS_OVER;
		}
		if (!mouseTracking) {
			mouseTracking = true;
			SetCapture();
		}
	} else {
		buttonPressState = QUA_BPS_UP;
		if (mouseTracking) {
			mouseTracking = false;
			ReleaseCapture();
		}
	}
	CButton::OnMouseMove(nFlags, point);
	if (priorState != buttonPressState) {
		Invalidate();
	}
}

bool
MFCQuaButton::CreateButton(char *label, CRect &r, CWnd *w, UINT id)
{
	if (!Create(label, BS_OWNERDRAW|BS_PUSHBUTTON|TBS_TOOLTIPS, r, w, id)) {
		return false;
	}
	SetDlgCtrlID(id);
	if (label) {
		tooltipText = label;
	} else {
		tooltipText = "Button";
	}
	EnableToolTips();
	bounds = r;
	bounds.MoveToXY(0,0);
	return true;
}

int
MFCQuaButton::OnToolHitTest(CPoint point, TOOLINFO *pTI) const
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
MFCQuaButton::OnToolTipNotify(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 1;	
	// need to handle both ANSI and UNICODE versions of the message
	TOOLTIPTEXTA* pttta = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* ptttw = (TOOLTIPTEXTW*)pNMHDR;
	CString csToolText;
	
	UINT uiID = pNMHDR->idFrom;
	
	int iID = GetDlgCtrlID();

	char buf[256];
	if (tooltipText()) {
		sprintf(buf, "%s", tooltipText());
	} else {
		sprintf(buf, "Button");
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

BEGIN_MESSAGE_MAP(MFCQuaButton, CButton)
	ON_WM_DRAWITEM()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_NOTIFY(TTN_NEEDTEXTW, 0, OnToolTipNotify)
	ON_NOTIFY(TTN_NEEDTEXTA, 0, OnToolTipNotify)
//	ON_WM_PAINT()
END_MESSAGE_MAP()



// MFCQuaButton message handlers

/////////////////////////////////////////////////////////////////
// MFCQuaChkButton
/////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(MFCQuaChkButton, MFCQuaButton)

MFCQuaChkButton::MFCQuaChkButton()
{
	checked = false;
	enableChecking = true;
}

MFCQuaChkButton::~MFCQuaChkButton()
{
}

void
MFCQuaChkButton::DrawItem(LPDRAWITEMSTRUCT disP)
{
	int	w, h;
	if (!checked || !enableChecking) {
		if (buttonPressState == QUA_BPS_UP) {
			upBitmap.GetSize(w, h);
			upBitmap.Draw(disP->hDC, 0, 0, w, h);
		} else if (buttonPressState == QUA_BPS_DOWN) {
			dnBitmap.GetSize(w, h);
			dnBitmap.Draw(disP->hDC, 0, 0, w, h);
		} else if (buttonPressState == QUA_BPS_OVER) {
			ovrBitmap.GetSize(w, h);
			ovrBitmap.Draw(disP->hDC, 0, 0, w, h);
		} else { // we are screwed
		}
	} else {
		if (buttonPressState == QUA_BPS_UP) {
			chkUBitmap.GetSize(w, h);
			chkUBitmap.Draw(disP->hDC, 0, 0, w, h);
		} else if (buttonPressState == QUA_BPS_DOWN) {
			chkDBitmap.GetSize(w, h);
			chkDBitmap.Draw(disP->hDC, 0, 0, w, h);
		} else if (buttonPressState == QUA_BPS_OVER) {
			chkOBitmap.GetSize(w, h);
			chkOBitmap.Draw(disP->hDC, 0, 0, w, h);
		} else { // we are screwed
		}
	}
}

afx_msg void
MFCQuaChkButton::OnLButtonUp(UINT nFlags,  CPoint point)
{
	uchar	priorState = buttonPressState;
	buttonPressState = QUA_BPS_UP;
	if (bounds.Contains(point)) {
		checked = !checked;
		SendLCMsg();
	} else {
	}
	if (mouseTracking) {
		ReleaseCapture();
		mouseTracking = false;
	}
	if (priorState != buttonPressState) {
		RedrawWindow();
	}
}

bool
MFCQuaChkButton::CreateButton(char *label, CRect &r, CWnd *w, UINT id)
{
	if (!Create(label, BS_OWNERDRAW|BS_CHECKBOX|TBS_TOOLTIPS, r, w, id)) {
		return false;
	}
	SetDlgCtrlID(id);
	if (label) {
		tooltipText = label;
	} else {
		tooltipText = "Button";
	}
	EnableToolTips();
	bounds = r;
	bounds.MoveToXY(0,0);
	return true;
}

void
MFCQuaChkButton::SetCheckState(short state)
{
	if (state) {
		if (!checked) {
			checked = true;
			RedrawWindow();
		}
	} else {
		if (checked) {
			checked = false;
			RedrawWindow();
		}
	}
}


BEGIN_MESSAGE_MAP(MFCQuaChkButton, MFCQuaButton)
	ON_WM_DRAWITEM()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
//	ON_WM_PAINT()
END_MESSAGE_MAP()

// MFCQuaChkButton message handlers

/////////////////////////////////////////////////////////////////
// MFCSmallQuaChkButton
/////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(MFCSmallQuaChkBut, MFCQuaChkButton)

MFCSmallQuaChkBut::MFCSmallQuaChkBut()
{
}

MFCSmallQuaChkBut::~MFCSmallQuaChkBut()
{
}

void
MFCSmallQuaChkBut::DrawItem(LPDRAWITEMSTRUCT disP)
{
	MFCQuaChkButton::DrawItem(disP);
}


bool
MFCSmallQuaChkBut::CreateButton(char *label, CRect &r, CWnd *w, UINT id)
{
	if (!Create(label, WS_CHILD|WS_VISIBLE|TBS_TOOLTIPS|BS_OWNERDRAW|BS_CHECKBOX, r, w, id)) {
		return false;
	}
	if (!upBitmap.LoadBitmap(IDB_BUT_SM_U)) {
		return false;
	}
	if (!ovrBitmap.LoadBitmap(IDB_BUT_SM_O)) {
		return false;
	}
	if (!dnBitmap.LoadBitmap(IDB_BUT_SM_D)) {
		return false;
	}
	if (!chkUBitmap.LoadBitmap(IDB_BUT_SM_UE)) {
		return false;
	}
	if (!chkDBitmap.LoadBitmap(IDB_BUT_SM_DE)) {
		return false;
	}
	if (!chkOBitmap.LoadBitmap(IDB_BUT_SM_OE)) {
		return false;
	}
	bounds = r;
	bounds.MoveToXY(0,0);
	if (label) {
		tooltipText = label;
	} else {
		tooltipText = "Button";
	}
	EnableToolTips();

	return true;
}


BEGIN_MESSAGE_MAP(MFCSmallQuaChkBut, MFCQuaChkButton)
	ON_WM_DRAWITEM()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
//	ON_WM_PAINT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////
// MFCSmallQuaMinButton
/////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(MFCSmallQuaMinBut, MFCQuaChkButton)

MFCSmallQuaMinBut::MFCSmallQuaMinBut()
{
}

MFCSmallQuaMinBut::~MFCSmallQuaMinBut()
{
}

void
MFCSmallQuaMinBut::DrawItem(LPDRAWITEMSTRUCT disP)
{
	MFCQuaChkButton::DrawItem(disP);
}


bool
MFCSmallQuaMinBut::CreateButton(char *label, CRect &r, CWnd *w, UINT id)
{
	if (!Create(label, WS_CHILD|WS_VISIBLE|TBS_TOOLTIPS|BS_OWNERDRAW|BS_CHECKBOX, r, w, id)) {
		return false;
	}
	if (!upBitmap.LoadBitmap(IDB_BUT_MIN_SM_U)) {
		return false;
	}
	if (!ovrBitmap.LoadBitmap(IDB_BUT_MIN_SM_O)) {
		return false;
	}
	if (!dnBitmap.LoadBitmap(IDB_BUT_MIN_SM_D)) {
		return false;
	}
	if (!chkUBitmap.LoadBitmap(IDB_BUT_MIN_SM_UE)) {
		return false;
	}
	if (!chkDBitmap.LoadBitmap(IDB_BUT_MIN_SM_DE)) {
		return false;
	}
	if (!chkOBitmap.LoadBitmap(IDB_BUT_MIN_SM_OE)) {
		return false;
	}
	bounds = r;
	bounds.MoveToXY(0,0);
	if (label) {
		tooltipText = label;
	} else {
		tooltipText = "Button";
	}
	EnableToolTips();

	return true;
}


BEGIN_MESSAGE_MAP(MFCSmallQuaMinBut, MFCQuaChkButton)
	ON_WM_DRAWITEM()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
//	ON_WM_PAINT()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////
// MFCSmallQuaClsButton
/////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(MFCSmallQuaClsBut, MFCQuaButton)

MFCSmallQuaClsBut::MFCSmallQuaClsBut()
{
}

MFCSmallQuaClsBut::~MFCSmallQuaClsBut()
{
}

void
MFCSmallQuaClsBut::DrawItem(LPDRAWITEMSTRUCT disP)
{
	MFCQuaButton::DrawItem(disP);
}


bool
MFCSmallQuaClsBut::CreateButton(char *label, CRect &r, CWnd *w, UINT id)
{
	if (!Create(label, WS_CHILD|WS_VISIBLE|TBS_TOOLTIPS|BS_OWNERDRAW, r, w, id)) {
		return false;
	}
	if (!upBitmap.LoadBitmap(IDB_CLSBUT_SM_U)) {
		return false;
	}
	if (!ovrBitmap.LoadBitmap(IDB_CLSBUT_SM_O)) {
		return false;
	}
	if (!dnBitmap.LoadBitmap(IDB_CLSBUT_SM_D)) {
		return false;
	}
	bounds = r;
	bounds.MoveToXY(0,0);
	if (label) {
		tooltipText = label;
	} else {
		tooltipText = "Button";
	}
	EnableToolTips();

	return true;
}


BEGIN_MESSAGE_MAP(MFCSmallQuaClsBut, MFCQuaButton)
	ON_WM_DRAWITEM()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
//	ON_WM_PAINT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////
// MFCSmallQuaLetterButton
/////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(MFCSmallQuaLetterBut, MFCQuaChkButton)

MFCSmallQuaLetterBut::MFCSmallQuaLetterBut(char c)
{
	butChar = c;
}

MFCSmallQuaLetterBut::~MFCSmallQuaLetterBut()
{
}

void
MFCSmallQuaLetterBut::DrawItem(LPDRAWITEMSTRUCT disP)
{
	MFCQuaChkButton::DrawItem(disP);
	CDC	*pdc = CDC::FromHandle(disP->hDC);

	CRect textRect(1,1,bounds.right-1,bounds.bottom-1);
	pdc->SelectObject(&displayFont);
	pdc->SetBkMode(TRANSPARENT);
	pdc->SetTextColor(rgb_black);
	char	buf[2];
	buf[0] = butChar;
	buf[1] = 0;
	pdc->DrawText(buf, 1, &textRect, DT_VCENTER|DT_CENTER);
}


bool
MFCSmallQuaLetterBut::CreateButton(char *label, CRect &r, CWnd *w, UINT id)
{
	if (!Create(label, WS_CHILD|WS_VISIBLE|TBS_TOOLTIPS|BS_OWNERDRAW, r, w, id)) {
		return false;
	}
	if (!upBitmap.LoadBitmap(IDB_BUT_BLANK_SM_U)) {
		return false;
	}
	if (!ovrBitmap.LoadBitmap(IDB_BUT_BLANK_SM_O)) {
		return false;
	}
	if (!dnBitmap.LoadBitmap(IDB_BUT_BLANK_SM_D)) {
		return false;
	}
	if (!chkUBitmap.LoadBitmap(IDB_BUT_BLANK_SM_UE)) {
		return false;
	}
	if (!chkDBitmap.LoadBitmap(IDB_BUT_BLANK_SM_DE)) {
		return false;
	}
	if (!chkOBitmap.LoadBitmap(IDB_BUT_BLANK_SM_OE)) {
		return false;
	}
	bounds = r;
	bounds.MoveToXY(0,0);
	if (label) {
		tooltipText = label;
	} else {
		tooltipText = "Button";
	}
	EnableToolTips();

	return true;
}


BEGIN_MESSAGE_MAP(MFCSmallQuaLetterBut, MFCQuaButton)
	ON_WM_DRAWITEM()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
//	ON_WM_PAINT()
END_MESSAGE_MAP()

// MFCQuaChkButton message handlers
