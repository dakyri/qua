// MainFrm.h : interface of the QuaMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__DCAFA933_B07E_4F32_9679_48E81FA64F33__INCLUDED_)
#define AFX_MAINFRM_H__DCAFA933_B07E_4F32_9679_48E81FA64F33__INCLUDED_

#include "PropertyFrame.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class QuaMFCCCDialog;

class QuaMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(QuaMainFrame)
public:
	QuaMainFrame();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(QuaMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~QuaMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	afx_msg void			OnAddChannelPopup();
	afx_msg LRESULT			OnAddChannelPopped(WPARAM,LPARAM);

// Generated message map functions
protected:
	afx_msg void OnProperties();
	afx_msg BOOL OnQueryNewPalette();
	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	CPalette* SetPalette(CPalette* pPalette);
	CPalette* m_pPalette;
	QuaMFCCCDialog* m_pAddChanDialog;
	CPropertyFrame* m_pPropFrame;
//{{AFX_MSG(QuaMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUpdateProperties(CCmdUI* pCmdUI);
	afx_msg void OnUpdateControlBarMenu(CCmdUI* pCmdUI);
	afx_msg BOOL OnViewToolBar(UINT nID);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__DCAFA933_B07E_4F32_9679_48E81FA64F33__INCLUDED_)
