#if !defined(AFX_TRANSPORTCONTROL_H__64CBAC87_4435_4773_9D50_8D35FF8FA39B__INCLUDED_)
#define AFX_TRANSPORTCONTROL_H__64CBAC87_4435_4773_9D50_8D35FF8FA39B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TransportControl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// TransportControl dialog

class TransportControl : public CDialog
{
// Construction
public:
	TransportControl(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(TransportControl)
	enum { IDD = IDD_TRANSPORT_CTL };
	CButton	m_playbut;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TransportControl)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(TransportControl)
	afx_msg void OnPlaybut();
	afx_msg void OnDoubleclickedPlaybut();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRANSPORTCONTROL_H__64CBAC87_4435_4773_9D50_8D35FF8FA39B__INCLUDED_)
