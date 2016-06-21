#if !defined(AFX_MFCChannelMountView_H__10C81D5E_BD16_435A_B5A3_F4825AAD2037__INCLUDED_)
#define AFX_MFCChannelMountView_H__10C81D5E_BD16_435A_B5A3_F4825AAD2037__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MFCChannelMountView.h : header file
//

#include "inx/QuaDisplay.h"
#include "DaList.h"

class Channel;
class MFCChannelView;
class MFCArrangeView;

/////////////////////////////////////////////////////////////////////////////
// MFCChannelMountView view

class MFCChannelMountView : public CView, public QuaChannelRackPerspective
{
public:
	MFCChannelMountView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(MFCChannelMountView)

// Attributes
public:
	CRect					bounds;
	CRect					frame;

	MFCArrangeView			*arranger;

	CWnd					rack;

// Operations
public:
	afx_msg void			OnSize(UINT nType, int cx, int cy);
	afx_msg void			OnSizing( UINT, LPRECT );
	afx_msg int				OnCreate(LPCREATESTRUCT lpCreateStruct );
	afx_msg BOOL			OnToolTipNeedsTxt( UINT id, NMHDR * pTTTStruct, LRESULT * pResult );
	afx_msg void			OnMove(int x, int y);
	afx_msg void			OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg LRESULT			OnArrangeView(WPARAM, LPARAM);

	virtual void			OnInitialUpdate();

	virtual void			DisplayArrangementTitle(const char *);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MFCChannelMountView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual					~MFCChannelMountView();
#ifdef _DEBUG
	virtual void			AssertValid() const;
	virtual void			Dump(CDumpContext& dc) const;
#endif

public:

	void					AddChannelRepresentation(Channel *c);
	void					RemoveChannelRepresentation(Channel *c);
	void					ArrangeChildren();


	// Generated message map functions
protected:
	//{{AFX_MSG(MFCChannelMountView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCChannelMountView_H__10C81D5E_BD16_435A_B5A3_F4825AAD2037__INCLUDED_)
