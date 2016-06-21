#pragma once
#include "afxwin.h"


// CErrDialog dialog

class CErrDialog : public CDialog
{
	DECLARE_DYNAMIC(CErrDialog)

public:
	CErrDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CErrDialog();

// Dialog Data
	enum { IDD = IDD_PARSE_ERROR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeParseErrTxt();
	CEdit parseErrText;
};
