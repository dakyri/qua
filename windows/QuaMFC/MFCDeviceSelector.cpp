
// E:\source\Qua\QuaMFC\MFCDeviceSelector.cpp : implementation file
//
//#define _AFXDLL
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE
#include "stdafx.h"

#include "qua_version.h"

#include "StdDefs.h"

#include "Colors.h"

#include "QuaMFC.h"
#include "MFCDeviceSelector.h"
#include "MFCDestinationView.h"
#include "MFCChannelView.h"

#include "Qua.h"
#include "Destination.h"
#include "QuaPort.h"

CFont	MFCDeviceSelector::displayFont;
BBitmap	MFCDeviceSelector::midiImg;
BBitmap	MFCDeviceSelector::audioImg;
BBitmap	MFCDeviceSelector::otherImg;
BBitmap	MFCDeviceSelector::joyImg;


IMPLEMENT_DYNAMIC(MFCDeviceSelector, CStatic)

MFCDeviceSelector::MFCDeviceSelector()
{
	isInput = false;
	destinationView = NULL;
}

MFCDeviceSelector::~MFCDeviceSelector()
{
}

void
MFCDeviceSelector::OnInitMenuPopup(
   CMenu* pPopupMenu,
   UINT nIndex,
   BOOL bSysMenu 
   )
{
	fprintf(stderr, "oimp %x %d\n", (unsigned)pPopupMenu, nIndex);
}

void
MFCDeviceSelector::CreateSelector(CRect &r, MFCDestinationView *w, UINT id, bool isi)
{
	Create(NULL, SS_NOTIFY, r, (CWnd *)w, id);
	isInput = isi;
	bounds = r;
	bounds.MoveToXY(0,0);
	destinationView = w;
	textRect = InnerRect(bounds);
	textRect.left += 13;
	ShowWindow(SW_SHOW);
	SetFont(&displayFont);
}

BEGIN_MESSAGE_MAP(MFCDeviceSelector, CStatic)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_INITMENUPOPUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_NOTIFY(TTN_NEEDTEXTW, 0, OnToolTipNotify)
	ON_NOTIFY(TTN_NEEDTEXTA, 0, OnToolTipNotify)
//	ON_COMMAND_RANGE(WM_MDS_LOWEST_MESSAGE,WM_MDS_HIGHEST_MESSAGE, OnMenuSelection)
//	ON_MESSAGE(WM_MENUCOMMAND, OnMenuCommand)
END_MESSAGE_MAP()



// MFCDeviceSelector message handlers

int
MFCDeviceSelector::OnCreate(LPCREATESTRUCT lpCreateStruct )
{
	EnableToolTips();
	CStatic::OnCreate(lpCreateStruct);
//	char buf[10];
//	InsertString(0, "device 1");
//	SetItemData(0,0);
//	InsertString(1, "device 2");
//	SetItemData(0,1);
	return 1;
}


void
MFCDeviceSelector::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);
	bounds.right = cx;
	bounds.bottom = cy;
	textRect = InnerRect(bounds);
	textRect.left += 13;
}


void
MFCDeviceSelector::OnSizing(UINT nType, LPRECT lr)
{
	CWnd::OnSizing(nType, lr);
}

afx_msg void
MFCDeviceSelector::OnPaint()
{
//	fprintf(stderr, "on paint device selector\n");
	CPaintDC	pdc(this);
	BBitmap		*bmp=NULL;
	CRect	r = bounds;

	switch (DeviceType()) {
		case QuaPort::Device::JOYSTICK:
			bmp = &joyImg;
			break;
		case QuaPort::Device::MIDI:
			bmp = &midiImg;
			break;
		case QuaPort::Device::AUDIO:
			bmp = &audioImg;
			break;
		case QuaPort::Device::PARALLEL:
		case QuaPort::Device::NOT:
		default:
			bmp = &otherImg;
			break;
	}
	if (bmp) {
		bmp->DrawTransparent(pdc.m_hDC, 0, 0, 0, 0);
		r.left += bmp->GetWidth();
	}
	LoweredBox(&pdc, &r, rgb_mdGray, true);
	pdc.SelectObject(&displayFont);
	pdc.SetBkMode(TRANSPARENT);
	pdc.DrawText(DeviceName(NMFMT_SYM,NMFMT_NUM), &textRect, DT_VCENTER|DT_LEFT);
}

afx_msg BOOL
MFCDeviceSelector::OnEraseBkgnd(CDC* pDC)
{
	return FALSE; //CStatic::OnEraseBkgnd(pDC);
}

afx_msg void
MFCDeviceSelector::OnMouseMove(UINT nFlags, CPoint point) 
{
	/*
	// Create TRACKMOUSEEVENT structure
	TRACKMOUSEEVENT tmeMouseEvent;		
	
	// Initialize members of structure
	tmeMouseEvent.cbSize = sizeof(TRACKMOUSEEVENT);
	tmeMouseEvent.dwFlags = TME_LEAVE;
	tmeMouseEvent.hwndTrack = m_hWnd;
	
	// Track mouse leave event
	_TrackMouseEvent(&tmeMouseEvent);
	*/
	CStatic::OnMouseMove(nFlags, point);
} //End of OnMouseMove

afx_msg BOOL
MFCDeviceSelector::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	/*
	if (m_bIsEnabled)
	{
		POINT point;
		::GetCursorPos(&point);
		ScreenToClient(&point);
//		ptClient.x -= m_rcArea.left;
//		ptClient.y -= m_rcArea.top;
		if (m_drawer.OnSetCursor(&point))
			return TRUE; //The cursor over the hyperlink
	} //if
	*/
	return CStatic::OnSetCursor(pWnd, nHitTest, message);
} //End of OnSetCursor

afx_msg void
MFCDeviceSelector::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CPoint	popPt(0,0);
	ClientToScreen(&popPt);
	PortPopup	pp;
	QuaPort			*selPort;
	port_chan_id	selChannel;
	bool sel=pp.DoPopup(this, popPt, selPort, selChannel, isInput);
	if (sel) {
		if (destinationView) {
			if (isInput) {
				((MFCInputView *)destinationView)->UpdateDestination(selPort, selChannel, 0);
			} else {
				((MFCOutputView *)destinationView)->UpdateDestination(selPort, selChannel, 0);
			}
		}
	}
} //End of OnLButtonDown

void
MFCDeviceSelector::OnMenuSelection(UINT nID)
{
}

LRESULT
MFCDeviceSelector::OnMenuCommand(WPARAM w, LPARAM l)
{
	return 0;
}

int
MFCDeviceSelector::OnToolHitTest(CPoint point, TOOLINFO *pTI) const
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

afx_msg BOOL
MFCDeviceSelector::OnToolTipNeedsTxt( UINT id, NMHDR * pTTTStruct, LRESULT * pResult )
{
	return true;
}

void
MFCDeviceSelector::OnToolTipNotify(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 1;	
	// need to handle both ANSI and UNICODE versions of the message
	TOOLTIPTEXTA* pttta = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* ptttw = (TOOLTIPTEXTW*)pNMHDR;
	CString csToolText;
	
	UINT uiID = pNMHDR->idFrom;
	
	int iID = GetDlgCtrlID();

	string buf = DeviceName(NMFMT_NAME,NMFMT_NAME);
	if (pNMHDR->code == TTN_NEEDTEXTA) {
	//	pttta->lpszText = buf;
		strcpy(pttta->szText, buf.c_str());
	} else { 
		WCHAR	*p=ptttw->szText;
		char	*q=const_cast<char*>(buf.c_str());
		while (*p++=*q++);
	} 
//	return TRUE;
}

const char *
MFCDeviceSelector::DeviceName(uchar df, uchar cf)
{
	if (destinationView != NULL) {
		if (isInput) {
			return ((MFCInputView *)destinationView)->DeviceName(df, cf);
		} else {
			return ((MFCOutputView *)destinationView)->DeviceName(df, cf);
		}
	}
	return "No Device";
}

long
MFCDeviceSelector::DeviceType()
{
	if (destinationView != NULL) {
		if (isInput) {
			return ((MFCInputView *)destinationView)->DeviceType();
		} else {
			return ((MFCOutputView *)destinationView)->DeviceType();
		}
	}
	return QuaPort::Device::NOT;
}
