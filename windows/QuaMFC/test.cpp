// test.cpp : implementation file
//

#include "stdafx.h"
#include "QuaMFC.h"
#include "test.h"


// test dialog

IMPLEMENT_DYNCREATE(test, CDHtmlDialog)

test::test(CWnd* pParent /*=NULL*/)
	: CDHtmlDialog(test::IDD, test::IDH, pParent)
{
}

test::~test()
{
}

void test::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
}

BOOL test::OnInitDialog()
{
	CDHtmlDialog::OnInitDialog();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(test, CDHtmlDialog)
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(test)
	DHTML_EVENT_ONCLICK(_T("ButtonOK"), OnButtonOK)
	DHTML_EVENT_ONCLICK(_T("ButtonCancel"), OnButtonCancel)
END_DHTML_EVENT_MAP()



// test message handlers

HRESULT test::OnButtonOK(IHTMLElement* /*pElement*/)
{
	OnOK();
	return S_OK;  // return TRUE  unless you set the focus to a control
}

HRESULT test::OnButtonCancel(IHTMLElement* /*pElement*/)
{
	OnCancel();
	return S_OK;  // return TRUE  unless you set the focus to a control
}
