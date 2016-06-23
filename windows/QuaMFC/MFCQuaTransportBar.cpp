#include "qua_version.h"
// MFCQuaTransportBar.cpp : implementation file
//
//#define _AFXDLL
#include "stdafx.h"

#include "StdDefs.h"
#include "Colors.h"

#include "QuaMFC.h"
#include "MFCQuaTransportBar.h"
#include "MFCQuaMessageId.h"

CFont			MFCQuaTransportBar::displayFont;
CFont			MFCQuaTransportBar::labelFont;

#define TIME_DISPLAY_WIDTH	55
#define TIME_DISPLAY_HEIGHT	10

#define TEMPO_DISPLAY_WIDTH	65
#define TEMPO_DISPLAY_HEIGHT 10

// MFCQuaTransportBar

IMPLEMENT_DYNAMIC(MFCQuaTransportBar, CToolBar)
MFCQuaTransportBar::MFCQuaTransportBar()
{
}

MFCQuaTransportBar::~MFCQuaTransportBar()
{
}


BEGIN_MESSAGE_MAP(MFCQuaTransportBar, CToolBar)
	ON_WM_CREATE()
	ON_MESSAGE(QM_CTL_CHANGED, OnQuaCtlChanged)
END_MESSAGE_MAP()


void
MFCQuaTransportBar::displayArrangementTitle(const char *nm)
{
	SetWindowText(nm);
}


// MFCQuaTransportBar message handlers

int
MFCQuaTransportBar::OnCreate(LPCREATESTRUCT lpcs) 
{
	TBBUTTONINFO	tbinfo;
	
	tbinfo.cbSize = sizeof(TBBUTTONINFO);
	tbinfo.dwMask = TBIF_SIZE;

	LoadToolBar(IDR_QUAMFCTYPE);
	int gotit = CToolBar::OnCreate(lpcs);

	for (int index=0; index<GetCount(); index++) {
		int	id = GetItemID(index);
		CRect	rect;
		CRect	tr;
		GetItemRect(index, &rect);
		COLORREF	bgc = GetSysColor(COLOR_MENUBAR);
		if (id == ID_TRANSPORT_PLAYTIME) {
		    SetButtonInfo(index, ID_TRANSPORT_PLAYTIME, TBBS_SEPARATOR, TIME_DISPLAY_WIDTH+20);
			rect.top = 1;
			rect.left +=5;
			rect.right = rect.left+TIME_DISPLAY_WIDTH;
			rect.bottom = 22;
			tr = CRect(3, 5, 3+TIME_DISPLAY_WIDTH, 5+TIME_DISPLAY_HEIGHT);
			pleitym.CreateTimeCtrl(rect, this, ID_TRANSPORT_PLAYTIME, &displayFont, &tr, &bgc, &rgb_black, &rgb_red);
			pleitym.SetVCMsgParams(QM_CTL_CHANGED, QCID_TB_TIME, (WPARAM)&pleitym);
		} else if (id == ID_TRANSPORT_TEMPO) {
			rect.top = 1;
		    SetButtonInfo(index, ID_TRANSPORT_TEMPO, TBBS_SEPARATOR, TEMPO_DISPLAY_WIDTH+10);
			rect.right = rect.left+TEMPO_DISPLAY_WIDTH+2;
			rect.bottom = 22;
			tr = CRect(3+26, 5, 3+TEMPO_DISPLAY_WIDTH, 5+TEMPO_DISPLAY_HEIGHT);
			tempeh.CreateFloatCtrl(rect, this, ID_TRANSPORT_PLAYTIME, &displayFont, &tr, &bgc, &rgb_black, &rgb_red);
			tempeh.SetVCMsgParams(QM_CTL_CHANGED, QCID_TB_TEMPO, (WPARAM)&tempeh);
//			tempeh.SetLabel("BPM", &labelFont);
			tempeh.SetRange(1, 120, 2001);
			tempeh.SetSteps(1,0.1f,0.01f);
			tempeh.SetValue(1029.2345f);
		}
 

	}
	return gotit;
}



afx_msg LRESULT
MFCQuaTransportBar::OnQuaCtlChanged(WPARAM wparam, LPARAM lparam)
{
	switch (wparam) {
		case QCID_TB_TIME: {
			UpdateGlobalTime(pleitym.time);
			break;
		}

		case QCID_TB_TEMPO: {
			UpdateTempo(tempeh.value);
			break;
		}

	}
	return true;
}

void
MFCQuaTransportBar::displayCurrentTime(Time &t, bool async)
{
	pleitym.SetTime(t);
	if (async) {
	} else {
		;
	}
}

void
MFCQuaTransportBar::displayTempo(float t, bool async)
{
	tempeh.SetValue(t);
	if (async) {
	} else {
	}
}

void
MFCQuaTransportBar::displayMetric(Metric *m, bool async)
{
}