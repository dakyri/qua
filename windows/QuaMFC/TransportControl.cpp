// TransportControl.cpp : implementation file
//

#include "stdafx.h"
#include "QuaMFC.h"
#include "TransportControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TransportControl dialog


TransportControl::TransportControl(CWnd* pParent /*=NULL*/)
	: CDialog(TransportControl::IDD, pParent)
{
	//{{AFX_DATA_INIT(TransportControl)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void TransportControl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TransportControl)
	DDX_Control(pDX, IDC_PLAYBUT, m_playbut);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(TransportControl, CDialog)
	//{{AFX_MSG_MAP(TransportControl)
	ON_BN_CLICKED(IDC_PLAYBUT, OnPlaybut)
	ON_BN_DOUBLECLICKED(IDC_PLAYBUT, OnDoubleclickedPlaybut)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TransportControl message handlers

void TransportControl::OnPlaybut() 
{
	// TODO: Add your control notification handler code here
	
}

void TransportControl::OnDoubleclickedPlaybut() 
{
	// TODO: Add your control notification handler code here
	
}
