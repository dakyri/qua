#pragma once


// test dialog

class test : public CDHtmlDialog
{
	DECLARE_DYNCREATE(test)

public:
	test(CWnd* pParent = NULL);   // standard constructor
	virtual ~test();
// Overrides
	HRESULT OnButtonOK(IHTMLElement *pElement);
	HRESULT OnButtonCancel(IHTMLElement *pElement);

// Dialog Data
	enum { IDD = IDD_DIALOG1, IDH = IDR_HTML_TEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
};
