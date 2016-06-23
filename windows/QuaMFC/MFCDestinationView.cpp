#include "qua_version.h"
// MFCChannelView.cpp : implementation file
//
//#define _AFXDLL
#include "stdafx.h"

#include "StdDefs.h"
#include "Colors.h"


#include "QuaMFC.h"
#include "MFCDestinationView.h"
#include "MFCQuaMessageId.h"

#include "Destination.h"
#include "QuaPort.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MFCChannelView

IMPLEMENT_DYNCREATE(MFCDestinationView, CView)
IMPLEMENT_DYNCREATE(MFCInputView, MFCDestinationView)
IMPLEMENT_DYNCREATE(MFCOutputView, MFCDestinationView)

MFCDestinationView::MFCDestinationView()
{

}

MFCDestinationView::~MFCDestinationView()
{
}


BEGIN_MESSAGE_MAP(MFCDestinationView, CView)
	//{{AFX_MSG_MAP(MFCDestinationView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_SIZING()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(MFCInputView, MFCDestinationView)
	//{{AFX_MSG_MAP(MFCInputView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_SIZING()
	ON_MESSAGE(QM_CTL_CHANGED, OnQuaCtlChanged)
	ON_MESSAGE(QM_CTL_BEGIN_MOVE, OnQuaCtlBeginMove)
	ON_MESSAGE(QM_CTL_END_MOVE, OnQuaCtlEndMove)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNeedsTxt)
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipNeedsTxt)
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipNeedsTxt)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(MFCOutputView, MFCDestinationView)
	//{{AFX_MSG_MAP(MFCOutputView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_SIZING()
	ON_MESSAGE(QM_CTL_CHANGED, OnQuaCtlChanged)
	ON_MESSAGE(QM_CTL_BEGIN_MOVE, OnQuaCtlBeginMove)
	ON_MESSAGE(QM_CTL_END_MOVE, OnQuaCtlEndMove)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNeedsTxt)
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipNeedsTxt)
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipNeedsTxt)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MFCDestinationView drawing
/////////////////////////////////////////////////////////////////////////////
void MFCDestinationView::OnDraw(CDC* pdc)
{
	pdc->SetBkColor(TRANSPARENT);
	RaisedBox(pdc, &bounds, rgb_orange, false);
}

afx_msg BOOL
MFCDestinationView::OnEraseBkgnd(CDC* pDC)
{
//	fprintf(stderr, "on erase dest view\n");
	return FALSE; //CStatic::OnEraseBkgnd(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// MFCChannelView diagnostics
/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void MFCDestinationView::AssertValid() const
{
	CView::AssertValid();
}

void MFCDestinationView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// MFCDestinationView message handlers
/////////////////////////////////////////////////////////////////////////////


afx_msg BOOL
MFCDestinationView::OnToolTipNeedsTxt( UINT id, NMHDR * pTTTStruct, LRESULT * pResult )
{
//	NMTTDISPINFO *ttip = ((NMTTDISPINFO*)pTTTStruct);
//    int control_id =  ::GetDlgCtrlID((HWND)ttip->hdr.idFrom);
//	reportError("tool tip %d %d", gainor.GetDlgCtrlID(), control_id);
//	wsprintf(ttip->lpszText,"hmmm");
//	ttip->hinst = quaAppInstance;
//	strcpy(ttip->szText, "hmmm");
	return false;
}
void
MFCDestinationView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	bounds.right = cx;
	bounds.bottom = cy;
	frame.right = frame.left+cx;
	frame.bottom = frame.bottom+cy;
	ArrangeChildren();
}

void
MFCDestinationView::OnSizing( UINT which, LPRECT r)
{
	CView::OnSizing(which, r);
}

afx_msg void
MFCDestinationView::OnMove(int x, int y)
{
	frame.MoveToXY(x,y);
}

int
MFCDestinationView::OnCreate(LPCREATESTRUCT lpCreateStruct )
{
	bounds.left = 0;
	bounds.top = 0;
	bounds.right = lpCreateStruct->cx;
	bounds.bottom = lpCreateStruct->cy;
	frame.left = lpCreateStruct->x;
	frame.top = lpCreateStruct->y;
	frame.right = frame.left+bounds.right; 
	frame.bottom = frame.top+bounds.bottom; 
	return CView::OnCreate(lpCreateStruct);
}


void
MFCDestinationView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	EnableToolTips(true);
}

void
MFCDestinationView::ArrangeChildren()
{
	fprintf(stderr, "dv: arrange children()\n");
	int	atX = bounds.right - (panor.bounds.right+2);
	panor.SetWindowPos(&wndTop, atX, 2, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
	atX -= (gainor.bounds.right+2);
	gainor.SetWindowPos(&wndTop, atX, 2, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
	atX -= (enablor.bounds.right+2);
	enablor.SetWindowPos(&wndTop, atX, 2, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
	atX -= 4;
	selector.SetWindowPos(&wndTop, 0, 0, atX, 12, SWP_NOMOVE|SWP_NOZORDER);
}

/////////////////////////////////////////////////////////////////////////////
// MFCOutputView methods
/////////////////////////////////////////////////////////////////////////////

MFCOutputView::MFCOutputView()
{
	;
}

int
MFCOutputView::OnCreate(LPCREATESTRUCT lpCreateStruct )
{
	MFCDestinationView::OnCreate(lpCreateStruct);

	selector.CreateSelector(CRect(2, 2, 50, 14), this, QCID_DV_SELECT, false);

	enablor.CreateButton("Enable", CRect(54, 2, 66, 14), this, QCID_DV_ENABLE);
	enablor.SetLCMsgParams(QM_CTL_CHANGED, QCID_DV_ENABLE, (WPARAM)&enablor);

	gainor.CreateRotor("Gain", CRect(68, 2, 88, 14), this, QCID_DV_GAIN, "G", 1.0f, 0.0f, 0.6f, 1.0f, 30, 330);
	gainor.SetVCMsgParams(QM_CTL_CHANGED, QCID_DV_GAIN, (WPARAM)&gainor);
	gainor.SetMDMsgParams(QM_CTL_BEGIN_MOVE, QCID_DV_GAIN, (WPARAM)&gainor);
	gainor.SetMUMsgParams(QM_CTL_END_MOVE, QCID_DV_GAIN, (WPARAM)&gainor);

	panor.CreateRotor("Pan", CRect(90, 2, 110, 14), this, QCID_DV_PAN, "P", 0.0f, -1.0f, 0.0f, 1.0f, 30, 330);
	panor.SetVCMsgParams(QM_CTL_CHANGED, QCID_DV_PAN, (WPARAM)&panor);
	panor.SetMDMsgParams(QM_CTL_BEGIN_MOVE, QCID_DV_PAN, (WPARAM)&panor);
	panor.SetMUMsgParams(QM_CTL_END_MOVE, QCID_DV_PAN, (WPARAM)&panor);

	return true;
}

afx_msg LRESULT
MFCOutputView::OnQuaCtlChanged(WPARAM wparam, LPARAM lparam)
{
	switch (wparam) {
		case QCID_DV_GAIN: {
			UpdateGain(gainor.fValue);
			break;
		}

		case QCID_DV_PAN: {
			UpdatePan(panor.fValue);
			break;
		}

		case QCID_DV_SELECT: {
//			UpdateDestination();
			break;
		 }

		case QCID_DV_ENABLE: {
			UpdateEnable(enablor.checked);
			break;
		}

	}
	return true;
}

// mainly to check movement of continuous controllers for envelope edits
afx_msg LRESULT
MFCOutputView::OnQuaCtlBeginMove(WPARAM wparam, LPARAM lparam)
{
	return true;
}

afx_msg LRESULT
MFCOutputView::OnQuaCtlEndMove(WPARAM wparam, LPARAM lparam)
{
	return true;
}

void
MFCOutputView::OnInitialUpdate()
{
	DisplayDestination();
	DisplayEnable();
	DisplayGain();
	DisplayPan();
}

void
MFCOutputView::DisplayDestination()
{
	selector.Invalidate();
}

void
MFCOutputView::DisplayEnable()
{
	if (output) {
		enablor.SetCheckState(output->enabled);
	}
}

void
MFCOutputView::DisplayPan()
{
	if (output) {
		panor.SetFloatValue(output->pan);
		panor.RedrawRotor();
	}
}

void
MFCOutputView::DisplayGain()
{
	if (output) {
		gainor.SetFloatValue(output->gain);
		gainor.RedrawRotor();
	}
}

void
MFCOutputView::NameChanged()
{
}

/////////////////////////////////////////////////////////////////////////////
// MFCInputView methods
/////////////////////////////////////////////////////////////////////////////


MFCInputView::MFCInputView()
{
	;
}

int
MFCInputView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	MFCDestinationView::OnCreate(lpCreateStruct);

	selector.CreateSelector(CRect(2, 2, 50, 14), this, QCID_DV_SELECT, true);

	enablor.CreateButton("Enable", CRect(54, 2, 66, 14), this, QCID_DV_ENABLE);
	enablor.SetLCMsgParams(QM_CTL_CHANGED, QCID_DV_ENABLE, (WPARAM)&enablor);

	gainor.CreateRotor("Gain", CRect(68, 2, 88, 14), this, QCID_DV_GAIN, "G", 1.0f, 0.0f, 0.6f, 1.0f, 30, 330);
	gainor.SetVCMsgParams(QM_CTL_CHANGED, QCID_DV_GAIN, (WPARAM)&gainor);
	gainor.SetMDMsgParams(QM_CTL_BEGIN_MOVE, QCID_DV_GAIN, (WPARAM)&gainor);
	gainor.SetMUMsgParams(QM_CTL_END_MOVE, QCID_DV_GAIN, (WPARAM)&gainor);

	panor.CreateRotor("Pan", CRect(90, 2, 110, 14), this, QCID_DV_PAN, "P", 0.0f, -1.0f, 0.0f, 1.0f, 30, 330);
	panor.SetVCMsgParams(QM_CTL_CHANGED, QCID_DV_PAN, (WPARAM)&panor);
	panor.SetMDMsgParams(QM_CTL_BEGIN_MOVE, QCID_DV_PAN, (WPARAM)&panor);
	panor.SetMUMsgParams(QM_CTL_END_MOVE, QCID_DV_PAN, (WPARAM)&panor);
	return true;
}

afx_msg LRESULT
MFCInputView::OnQuaCtlChanged(WPARAM wparam, LPARAM lparam)
{
	switch (wparam) {
		case QCID_DV_GAIN: {
			UpdateGain(gainor.fValue);
			break;
		}

		case QCID_DV_PAN: {
			UpdatePan(panor.fValue);
			break;
		}

		case QCID_DV_SELECT: {
//			UpdateDestination();
			break;
		 }

		case QCID_DV_ENABLE: {
			UpdateEnable(enablor.checked);
			break;
		}

	}
	return true;
}

// mainly to check movement of continuous controllers for envelope edits
afx_msg LRESULT
MFCInputView::OnQuaCtlBeginMove(WPARAM wparam, LPARAM lparam)
{
	return true;
}

afx_msg LRESULT
MFCInputView::OnQuaCtlEndMove(WPARAM wparam, LPARAM lparam)
{
	return true;
}

void
MFCInputView::OnInitialUpdate()
{
	DisplayDestination();
	DisplayEnable();
	DisplayGain();
	DisplayPan();
}

void
MFCInputView::DisplayDestination()
{
	selector.Invalidate();
}

void
MFCInputView::DisplayEnable()
{
	if (input) {
		enablor.SetCheckState(input->enabled);
	}
}

void
MFCInputView::DisplayPan()
{
	if (input) {
		panor.SetFloatValue(input->pan);
		panor.RedrawRotor();
	}
}

void
MFCInputView::DisplayGain()
{
	if (input) {
		gainor.SetFloatValue(input->gain);
		gainor.RedrawRotor();
	}
}

void
MFCInputView::NameChanged()
{
}
