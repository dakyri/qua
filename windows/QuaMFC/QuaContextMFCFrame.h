// QuaContextMFCFrame.h : interface of the QuaContextFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_QuaContextFrame_H__DDDA867C_3C23_4697_8C17_AF56C91DDD23__INCLUDED_)
#define AFX_QuaContextFrame_H__DDDA867C_3C23_4697_8C17_AF56C91DDD23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class MFCContextView;

class QuaContextFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(QuaContextFrame)
public:
	QuaContextFrame();

// Attributes

public:
	CSplitterWnd	sideSplitter;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(QuaContextFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~QuaContextFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnSizing( UINT, LPRECT );
	afx_msg void	OnConnectClick();

	virtual BOOL	OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL	OnNotify(WPARAM wParam, LPARAM lParam,	LRESULT* pResult);

	int				width;
	int				height;

	MFCContextView	*ctxtView;
// Generated message map functions
protected:
	CToolBar		contextTools;
	//{{AFX_MSG(QuaContextFrame)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QuaContextFrame_H__DDDA867C_3C23_4697_8C17_AF56C91DDD23__INCLUDED_)
