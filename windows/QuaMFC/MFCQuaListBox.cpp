// MFCQuaListBox.cpp : implementation file
//

#include "stdafx.h"
#include "QuaMFC.h"
#include "MFCQuaListBox.h"

#include "inx/Sym.h"


// MFCQuaListBox

IMPLEMENT_DYNAMIC(MFCQuaListBox, CListBox)
MFCQuaListBox::MFCQuaListBox()
{
	vcMessid  = 0;
	vcWparam = 0;
	vcLparam = 0;
	dcMessid  = 0;
	dcWparam = 0;
	dcLparam = 0;
}

MFCQuaListBox::~MFCQuaListBox()
{
}

bool
MFCQuaListBox::CreateListBox(CRect &r, CWnd *w, uint32 id)
{
	bool	ret = (Create(WS_CHILD|WS_VISIBLE|LBS_NOTIFY, r, w, id) != 0);
	if (ret) {
		;
	}
	return ret;
}

int
MFCQuaListBox::OnCreate(LPCREATESTRUCT lpCreateStruct )
{
	bounds.left = 0;
	bounds.top = 0;
	bounds.right = lpCreateStruct->cx;
	bounds.bottom = lpCreateStruct->cy;
	frame.left = lpCreateStruct->x;
	frame.top = lpCreateStruct->y;
	frame.right = frame.left+bounds.right; 
	frame.bottom = frame.top+bounds.bottom; 

	return CListBox::OnCreate(lpCreateStruct);
}

void
MFCQuaListBox::SendVCMsg()
{
	if (GetParent() && vcMessid) {
		GetParent()->SendMessage(vcMessid, vcWparam, vcLparam);
	}
}

void
MFCQuaListBox::SetVCMsgParams(uint32 mid, WPARAM wp, LPARAM lp)
{
	vcMessid = mid;
	vcWparam = wp;
	vcLparam = lp;
}

void
MFCQuaListBox::SendDCMsg()
{
	if (GetParent() && dcMessid) {
		GetParent()->SendMessage(dcMessid, dcWparam, dcLparam);
	}
}

void
MFCQuaListBox::SetDCMsgParams(uint32 mid, WPARAM wp, LPARAM lp)
{
	dcMessid = mid;
	dcWparam = wp;
	dcLparam = lp;
}


afx_msg void
MFCQuaListBox::OnMove(int x, int y)
{
	fprintf(stderr, "MFCQuaListBox::OnMove() %d %d\n", x, y);
	frame.MoveToXY(x,y);
}

void
MFCQuaListBox::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	if (cx != 0) bounds.right = cx;
	if (cy != 0) bounds.bottom = cy;
	frame.right = frame.left+bounds.right;
	frame.bottom = frame.top+bounds.bottom;
	fprintf(stderr, "MFCQuaListBox::OnSize() %d %d, fr %d %d %d %d\n", cx, cy, frame.left, frame.top, frame.right, frame.bottom);
}

void
MFCQuaListBox::OnSizing( UINT which, LPRECT r)
{
	CWnd::OnSizing(which, r);
}

void
MFCQuaListBox::ClearValues()
{
	ResetContent();
}

void
MFCQuaListBox::OnSelChanged()
{
	SendVCMsg();
}

void
MFCQuaListBox::OnDblClk()
{
	SendDCMsg();
}

void
MFCQuaListBox::AddValue(void *s, char *nm)
{
	long ind = AddString(nm);
	SetItemData(ind, (DWORD_PTR)s);
}

void *
MFCQuaListBox::Value(long i)
{
	return (void *)GetItemData(i);
}

//void
//MFCQuaListBox::SelectValue(void *s)
//{
//	SelectString(0, "");
//}


BEGIN_MESSAGE_MAP(MFCQuaListBox, CListBox)
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_MOVE()
	ON_WM_CREATE()
//	ON_LBN_ERRSPACE()   //The list box cannot allocate enough memory to meet the request. 
//	ON_LBN_KILLFOCUS()  // The list box is losing the input focus. 
//	ON_LBN_SELCANCEL()  // The current list-box selection is canceled. This message is only sent when a list box has the LBS_NOTIFY style. 
//	ON_LBN_SETFOCUS()  // The list box is receiving the input focus. 
	ON_CONTROL_REFLECT(LBN_DBLCLK, OnDblClk)  // The selection in the list box is about to change. This notification is not sent if the selection is changed by the CListBox::SetCurSel member function. This notification applies only to a list box that has the LBS_NOTIFY style. The LBN_SELCHANGE notification message is sent for a multiple-selection list box whenever the user presses an arrow key, even if the selection does not change. 
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelChanged)  // The selection in the list box is about to change. This notification is not sent if the selection is changed by the CListBox::SetCurSel member function. This notification applies only to a list box that has the LBS_NOTIFY style. The LBN_SELCHANGE notification message is sent for a multiple-selection list box whenever the user presses an arrow key, even if the selection does not change. 
	ON_WM_CHARTOITEM()  // An owner-draw list box that has no strings receives a WM_CHAR message. 
	ON_WM_VKEYTOITEM()   //A list box with the LBS_WANTKEYBOARDINPUT style receives a WM_KEYDOWN message. 
END_MESSAGE_MAP()



// MFCQuaListBox message handlers


IMPLEMENT_DYNAMIC(MFCQuaSymListBox, CListBox)

void
MFCQuaSymListBox::AddValue(StabEnt *s, char *nm)
{
	long ind = AddString(nm);
	SetItemData(ind, (DWORD_PTR)s);
}

StabEnt *
MFCQuaSymListBox::Value(long i)
{
	return (StabEnt *)GetItemData(i);
}

char *
MFCQuaSymListBox::ValueStr(long i)
{
	StabEnt	*p = Value(i);
	return p?p->name:"";
}

void
MFCQuaSymListBox::SelectValue(StabEnt *s)
{
	SelectString(0, s->name);
}

