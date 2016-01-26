// ErrDialog.cpp : implementation file
//

#include "stdafx.h"
#include "QuaMFC.h"
#include "ErrDialog.h"


// CErrDialog dialog

IMPLEMENT_DYNAMIC(CErrDialog, CDialog)
CErrDialog::CErrDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CErrDialog::IDD, pParent)
{
}

CErrDialog::~CErrDialog()
{
}

void CErrDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PARSE_ERR_TXT, parseErrText);
}


BEGIN_MESSAGE_MAP(CErrDialog, CDialog)
	ON_EN_CHANGE(IDC_PARSE_ERR_TXT, OnEnChangeParseErrTxt)
END_MESSAGE_MAP()


// CErrDialog message handlers

void CErrDialog::OnEnChangeParseErrTxt()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}
