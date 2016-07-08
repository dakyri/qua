

// MFCQuaStateDisplay.cpp : implementation file
//
//#define _AFXDLL
#include "stdafx.h"
#include "qua_version.h"

#include "StdDefs.h"
#include "BBitmap.h"

#include "QuaMFC.h"
#include "MFCQuaStateDisplay.h"

#include "QuaDisplay.h"
#include "BaseVal.h"
#include "Stackable.h"
#include "Dictionary.h"

#include "Parse.h"


CFont	MFCQuaStateDisplay::displayFont;

// MFCQuaStateDisplay

IMPLEMENT_DYNAMIC(MFCQuaStateDisplay, CStatic)
MFCQuaStateDisplay::MFCQuaStateDisplay()
{
	nState = 0;
	stateBmp = NULL;
	states = NULL;
	state = -1;
}

MFCQuaStateDisplay::~MFCQuaStateDisplay()
{
}

BEGIN_MESSAGE_MAP(MFCQuaStateDisplay, CStatic)
	ON_WM_DRAWITEM()
//	ON_WM_PAINT()
END_MESSAGE_MAP()



// MFCQuaStateDisplay message handlers


void
MFCQuaStateDisplay::DrawItem(LPDRAWITEMSTRUCT disP)
{
	long statInd=StateInd(state);
	CRect textRect;
	GetClientRect(&textRect);
	textRect.right -= 2;
	textRect.left = 2;
	textRect.top--;
	if (statInd >= 0 && statInd <nState) {
		int	w, h;
		stateBmp[statInd].GetSize(w, h);
		stateBmp[statInd].Draw(disP->hDC, 0, 0, w, h);
		textRect.left = w+2;
	}
	char	buf[1024];
	GetWindowText(buf, 1024);
	CDC	*pdc = CDC::FromHandle(disP->hDC);
	if (pdc) {
		pdc->SelectObject(&displayFont);
		pdc->SetBkMode(TRANSPARENT);
		pdc->DrawText(buf, &textRect, DT_TOP|DT_LEFT);
	}
}

long
MFCQuaStateDisplay::StateInd(long st)
{
	short i;
	long statInd=-1;
	if (states) {
		for (i=0; i<nState; i++) {
			if (states[i] == st) {
				statInd = i;
				break;
			}
		}
	} else {
		statInd = st;
	}
	return statInd;
}

void
MFCQuaStateDisplay::SetState(long st)
{
	state = st;
	RedrawWindow();
}

void
MFCQuaStateDisplay::SetStates(long ns, long *st)
{
	if (states != NULL) {
		delete states;
		states = NULL;
	}
	if (stateBmp) {
		delete stateBmp;
		stateBmp = NULL;
	}
	if (st) {
		states = new long[ns];
		for (short i=0; i<ns; i++) {
			states[i] = st[i];
		}
	}
	stateBmp = new BBitmap[ns];
	nState = ns;
	state = states?states[0]:0;
}

void	
MFCQuaStateDisplay::SetStateBMP(long s, long resid)
{
	long bind = StateInd(s);
	stateBmp[bind].LoadBitmap(resid);
}

long	
MFCQuaStateDisplay::CreateStateDisplay(char *label, CRect &r, CWnd *w, long id, short ns, long *sta)
{
	long ret = Create(label, WS_CHILD|WS_VISIBLE|SS_OWNERDRAW, r, w, id);
	SetStates(ns, sta);

	return ret;
}

IMPLEMENT_DYNAMIC(MFCQuaParseStateDisplay, MFCQuaStateDisplay)

MFCQuaParseStateDisplay::MFCQuaParseStateDisplay()
{
}

MFCQuaParseStateDisplay::~MFCQuaParseStateDisplay()
{
}

BEGIN_MESSAGE_MAP(MFCQuaParseStateDisplay, MFCQuaStateDisplay)
	ON_WM_DRAWITEM()
//	ON_WM_PAINT()
END_MESSAGE_MAP()

long parse_states[] = {Q_PARSE_OK, Q_PARSE_ERR, Q_PARSE_DEFER};

long
MFCQuaParseStateDisplay::CreateStateDisplay(char *label, CRect &r, CWnd *w, long id)
{
	long ret = MFCQuaStateDisplay::CreateStateDisplay(label, r, w, id, 3, parse_states);

	SetStateBMP(Q_PARSE_OK, IDB_QP_STATE_OK);
	SetStateBMP(Q_PARSE_ERR, IDB_QP_STATE_ERR);
	SetStateBMP(Q_PARSE_DEFER, IDB_QP_STATE_DEFER);
	return ret;
}
