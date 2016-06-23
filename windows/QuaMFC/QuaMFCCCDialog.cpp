// QuaMFCCCDialog.cpp : implementation file
//
//#define _AFXDLL
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE
#include "stdafx.h"

#include "StdDefs.h"

#include "QuaMFC.h"
#include "QuaMFCCCDialog.h"
// QuaMFCCCDialog dialog

IMPLEMENT_DYNAMIC(QuaMFCCCDialog, CDialog)
QuaMFCCCDialog::QuaMFCCCDialog(CQuaMFCDoc *d, CWnd* pParent)
	: CDialog(QuaMFCCCDialog::IDD, pParent)
{
	doc = d;
	nAudIn=2;
	nAudOut=2;
	addAuIn=true;
	addAuOut=true;
	addStrIn=true;
	addStrOut=true;

}

QuaMFCCCDialog::~QuaMFCCCDialog()
{
}

void QuaMFCCCDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL
QuaMFCCCDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
  
	CComboBox	*ccInMode = (CComboBox*)GetDlgItem(IDC_CC_INMODE);
	if (ccInMode != NULL) {
		int indm = ccInMode->AddString("Mono");
		ccInMode->SetItemData(indm,1);
		int inds = ccInMode->AddString("Stereo");
		ccInMode->SetItemData(inds,2);
		ccInMode->SetCurSel(inds);
	}
	CComboBox	*ccOutMode = (CComboBox*)GetDlgItem(IDC_CC_OUTMODE);
	if (ccOutMode != NULL) {
		int indm = ccOutMode->AddString("Mono");
		ccOutMode->SetItemData(indm,1);
		int inds = ccOutMode->AddString("Stereo");
		ccOutMode->SetItemData(inds,2);
		ccOutMode->SetCurSel(inds);
	}

	CButton		*addAudioIn = (CButton*)GetDlgItem(IDC_CC_ADDAUDIN);
	if (addAudioIn != NULL) {
		addAudioIn->SetCheck(BST_CHECKED);
	}
	CButton		*addAudioOut = (CButton*)GetDlgItem(IDC_CC_ADDAUDOUT);
	if (addAudioOut != NULL) {
		addAudioOut->SetCheck(BST_CHECKED);
	}
	CButton		*addStreamIn = (CButton*)GetDlgItem(IDC_CC_ADDSTREAMIN);
	if (addStreamIn != NULL) {
		addStreamIn->SetCheck(BST_CHECKED);
	}
	CButton		*addStreamOut = (CButton*)GetDlgItem(IDC_CC_ADDSTREAMOUT);
	if (addStreamOut != NULL) {
		addStreamOut->SetCheck(BST_CHECKED);
	}

	return TRUE;   // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

void QuaMFCCCDialog::OnOK() 
{
   // TODO: Add extra validation here
   
   // Ensure that your UI got the necessary input 
   // from the user before closing the dialog. The 
   // default OnOK will close this.

	CComboBox	*ccInMode = (CComboBox*)GetDlgItem(IDC_CC_INMODE);
	if (ccInMode != NULL) {
		nAudIn = ccInMode->GetItemData(ccInMode->GetCurSel());
	}
	CComboBox	*ccOutMode = (CComboBox*)GetDlgItem(IDC_CC_OUTMODE);
	if (ccOutMode != NULL) {
		nAudOut = ccOutMode->GetItemData(ccOutMode->GetCurSel());
	}

	CButton		*addAudioIn = (CButton*)GetDlgItem(IDC_CC_ADDAUDIN);
	if (addAudioIn != NULL) {
		addAuIn=(addAudioIn->GetCheck()==BST_CHECKED);
	}
	CButton		*addAudioOut = (CButton*)GetDlgItem(IDC_CC_ADDAUDOUT);
	if (addAudioOut != NULL) {
		addAuOut=(addAudioIn->GetCheck()==BST_CHECKED);
	}
	CButton		*addStreamIn = (CButton*)GetDlgItem(IDC_CC_ADDSTREAMIN);
	if (addStreamIn != NULL) {
		addStrIn=(addAudioIn->GetCheck()==BST_CHECKED);
	}
	CButton		*addStreamOut = (CButton*)GetDlgItem(IDC_CC_ADDSTREAMOUT);
	if (addStreamOut != NULL) {
		addStrOut=(addAudioIn->GetCheck()==BST_CHECKED);
	}
	CWnd	*w=GetParent();
	if (w != NULL) {
		w->PostMessage(ID_EDIT_ADD_CHANNEL_POPPED,1,0);
	}
	CDialog::OnOK(); // This will close the dialog and DoModal will return.
}

void QuaMFCCCDialog::OnCancel() 
{
   // TODO: Add extra cleanup here
   
   // Ensure that you reset all the values back to the
   // ones before modification. This handler is called
   // when the user doesn't want to save the changes.
   
	CWnd	*w=GetParent();
	if (w != NULL) {
		w->PostMessage(ID_EDIT_ADD_CHANNEL_POPPED,0,0);
	}
	CDialog::OnCancel();
}


BEGIN_MESSAGE_MAP(QuaMFCCCDialog, CDialog)
END_MESSAGE_MAP()


// QuaMFCCCDialog message handlers
