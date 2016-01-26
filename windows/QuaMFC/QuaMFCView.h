// QuaMFCView.h : interface of the CQuaMFCView class
//
// This is the main view class defined by the MDI system
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUAMFCVIEW_H__B50DC6DC_3193_43B3_AC69_8F2BF15B762B__INCLUDED_)
#define AFX_QUAMFCVIEW_H__B50DC6DC_3193_43B3_AC69_8F2BF15B762B__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "QuaMFCDoc.h"

class CQuaMFCView : public CView
{
public: // create from serialization only
	CQuaMFCView();
	DECLARE_DYNCREATE(CQuaMFCView)

// Attributes
public:
	CQuaMFCDoc* GetDocument();
public:
	CSplitterWnd m_wndSplitter;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQuaMFCView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CQuaMFCView();

//	MFCArrangeView	arrange;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CQuaMFCView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in QuaMFCView.cpp
inline CQuaMFCDoc* CQuaMFCView::GetDocument()
   { return (CQuaMFCDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUAMFCVIEW_H__B50DC6DC_3193_43B3_AC69_8F2BF15B762B__INCLUDED_)
