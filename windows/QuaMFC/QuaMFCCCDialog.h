#pragma once
#include "afxwin.h"


// QuaMFCCCDialog dialog
class CQuaMFCDoc;

class QuaMFCCCDialog : public CDialog
{
	DECLARE_DYNAMIC(QuaMFCCCDialog)

public:
	QuaMFCCCDialog(CQuaMFCDoc *doc, CWnd* pParent);   // standard constructor
	virtual ~QuaMFCCCDialog();

// Dialog Data
	enum { IDD = IDD_CC_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
public:
	CQuaMFCDoc	*doc;
	int			nAudIn;
	int			nAudOut;
	bool		addAuIn;
	bool		addAuOut;
	bool		addStrIn;
	bool		addStrOut;

	DECLARE_MESSAGE_MAP()
public:
};
