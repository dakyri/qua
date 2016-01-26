// PropertyFrame.h : header file
//
// This file contains the mini-frame that controls modeless 
// property sheet PreferencesPropertySheet.

#ifndef __PROPERTYFRAME_H__
#define __PROPERTYFRAME_H__

#include "PreferencesPropertySheet.h"

/////////////////////////////////////////////////////////////////////////////
// CPropertyFrame frame

class CPropertyFrame : public CMiniFrameWnd
{
	DECLARE_DYNCREATE(CPropertyFrame)
//Construction
public:
	CPropertyFrame();

// Attributes
public:
	PreferencesPropertySheet* m_pModelessPropSheet;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyFrame)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPropertyFrame();

	// Generated message map functions
	//{{AFX_MSG(CPropertyFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif	// __PROPERTYFRAME_H__
