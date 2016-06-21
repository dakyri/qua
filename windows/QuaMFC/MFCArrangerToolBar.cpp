#include "qua_version.h"
// MFCArrangerToolBar.cpp : implementation file
//

#include "stdafx.h"
#include "QuaMFC.h"
#include "MFCArrangerToolBar.h"


// MFCArrangerToolBar

IMPLEMENT_DYNAMIC(MFCArrangerToolBar, CToolBar)
MFCArrangerToolBar::MFCArrangerToolBar()
{

}

MFCArrangerToolBar::~MFCArrangerToolBar()
{
}


BEGIN_MESSAGE_MAP(MFCArrangerToolBar, CToolBar)
	ON_WM_CREATE()
END_MESSAGE_MAP()



// MFCArrangerToolBar message handlers

int
MFCArrangerToolBar::OnCreate(LPCREATESTRUCT lpcs) 
{
	LoadToolBar(IDR_ARRANGER_TOOL);

	return CToolBar::OnCreate(lpcs);
}
