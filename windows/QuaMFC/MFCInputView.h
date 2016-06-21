#if !defined(AFX_MFCINPUTVIEW_H__E95E43F7_980D_439B_B3C3_7EFDD8682EBE__INCLUDED_)
#define AFX_MFCINPUTVIEW_H__E95E43F7_980D_439B_B3C3_7EFDD8682EBE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MFCDeviceSelector.h"

class Input;

/////////////////////////////////////////////////////////////////////////////
// MFCInputView view
/////////////////////////////////////////////////////////////////////////////
class MFCInputView : public CView
{
public:
	MFCInputView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(MFCInputView)

// Attributes
public:
	CRect				bounds;
	CRect				frame;
	MFCDeviceSelector	selector;

// Operations
public:
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnSizing( UINT, LPRECT );
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MFCInputView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL


// Implementation
public:
	Input			*input;
	void			SetInput(Input *d);
	void			ArrangeChildren();

protected:
	virtual			~MFCInputView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(MFCDestinationView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCINPUTVIEW_H__E95E43F7_980D_439B_B3C3_7EFDD8682EBE__INCLUDED_)
