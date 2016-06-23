#if !defined(AFX_MFCCONTEXTVIEW_H__10C81D5E_BD16_435A_B5A3_F4825AAD2037__INCLUDED_)
#define AFX_MFCCONTEXTVIEW_H__10C81D5E_BD16_435A_B5A3_F4825AAD2037__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MFCArrangeView.h : header file
//

#include "QuaDisplay.h"

/////////////////////////////////////////////////////////////////////////////
// MFCArrangeView view

class MFCContextView : public CView
{
public:
	MFCContextView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(MFCContextView)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MFCContextView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~MFCContextView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(MFCContextView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCCONTEXTVIEW_H__10C81D5E_BD16_435A_B5A3_F4825AAD2037__INCLUDED_)
