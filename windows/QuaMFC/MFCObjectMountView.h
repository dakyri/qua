#if !defined(AFX_MFCObjectMountView_H__10C81D5E_BD16_435A_B5A3_F4825AAD2037__INCLUDED_)
#define AFX_MFCObjectMountView_H__10C81D5E_BD16_435A_B5A3_F4825AAD2037__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MFCObjectMountView.h : header file
//

#include "inx/QuaDisplay.h"
#include "DaList.h"

class Channel;
class MFCObjectView;

/////////////////////////////////////////////////////////////////////////////
// MFCObjectMountView view

class MFCObjectMountView : public CScrollView, public QuaObjectRackPerspective
{
public:
	MFCObjectMountView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(MFCObjectMountView)

// Attributes
public:
	CRect						bounds;
	CRect						frame;
	CWnd						rack;

// Operations
public:
	afx_msg void				OnSize(UINT nType, int cx, int cy);
	afx_msg void				OnSizing( UINT, LPRECT );
	afx_msg int					OnCreate(LPCREATESTRUCT lpCreateStruct );
	afx_msg void				OnDestroy();
	afx_msg LRESULT				OnQuaCloseView(WPARAM, LPARAM);
	afx_msg LRESULT				OnQuaMinView(WPARAM, LPARAM);
	afx_msg LRESULT				OnQuaMaxView(WPARAM, LPARAM);

	afx_msg BOOL				OnToolTipNeedsTxt( UINT id, NMHDR * pTTTStruct, LRESULT * pResult );

	virtual void				OnInitialUpdate();

	virtual bool				CanShow(StabEnt *c);
	virtual bool				AddObjectRepresentation(StabEnt *c);
	virtual void				RemoveObjectRepresentation(StabEnt *c);
	virtual void				DisplayArrangementTitle(const char *);

	bool						DeleteObjectRepresentation(QuaObjectRepresentation *or);

	void						SetHVScroll();
	MFCObjectView				*MFCOV(long i);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MFCObjectMountView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual			~MFCObjectMountView();
#ifdef _DEBUG
	virtual void	AssertValid() const;
	virtual void	Dump(CDumpContext& dc) const;
#endif

public:

	void			AddObjectRepresentation(Channel *c);
	void			RemoveObjectRepresentation(Channel *c);
	QuaObjectRepresentation	*ObjectRepresentationFor(Channel *c);
	void			ArrangeChildren();
	// Generated message map functions
protected:
	//{{AFX_MSG(MFCObjectMountView)
		// NOTE - the ClassWizard will add and remove member functions here.
	afx_msg void		OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCObjectMountView_H__10C81D5E_BD16_435A_B5A3_F4825AAD2037__INCLUDED_)
