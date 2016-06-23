#if !defined(AFX_MFCDESTINATIONVIEW_H__E95E43F7_980D_439B_B3C3_7EFDD8682EBE__INCLUDED_)
#define AFX_MFCDESTINATIONVIEW_H__E95E43F7_980D_439B_B3C3_7EFDD8682EBE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MFCDeviceSelector.h"
#include "MFCQuaButton.h"
#include "MFCQuaRotor.h"
#include "QuaDisplay.h"

class Output;
class Input;

/////////////////////////////////////////////////////////////////////////////
// MFCDestinationView view
/////////////////////////////////////////////////////////////////////////////
class MFCDestinationView : public CView
{
public:
	MFCDestinationView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(MFCDestinationView)

// Attributes
public:
	CRect				bounds;
	CRect				frame;

// Operations
public:
	afx_msg void		OnSize(UINT nType, int cx, int cy);
	afx_msg void		OnSizing( UINT, LPRECT );
	afx_msg void		OnMove(int x, int y);
	afx_msg int			OnCreate(LPCREATESTRUCT lpCreateStruct );
	afx_msg BOOL		OnToolTipNeedsTxt( UINT id, NMHDR * pTTTStruct, LRESULT * pResult );
	afx_msg BOOL		OnEraseBkgnd(CDC* pDC);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MFCDestinationView)
	protected:
	virtual void		OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void		OnInitialUpdate();
	//}}AFX_VIRTUAL


// Implementation
public:
	MFCDeviceSelector	selector;
	MFCSmallQuaChkBut	enablor;
	MFCSmallQuaRotor	gainor;
	MFCSmallQuaRotor	panor;

	void				ArrangeChildren();

protected:
	virtual				~MFCDestinationView();
#ifdef _DEBUG
	virtual void		AssertValid() const;
	virtual void		Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(MFCDestinationView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class MFCInputView: public MFCDestinationView, public QuaInputRepresentation
{
public:
	MFCInputView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(MFCInputView)

	void						ArrangeChildren();

	afx_msg int					OnCreate(LPCREATESTRUCT lpCreateStruct );
	afx_msg LRESULT				OnQuaCtlChanged(WPARAM, LPARAM);
	afx_msg LRESULT				OnQuaCtlBeginMove(WPARAM, LPARAM);
	afx_msg LRESULT				OnQuaCtlEndMove(WPARAM, LPARAM);

	virtual void				OnInitialUpdate();

	virtual void				DisplayDestination();
	virtual void				DisplayEnable();
	virtual void				DisplayPan();
	virtual void				DisplayGain();

	virtual void				NameChanged();
protected:
	//{{AFX_MSG(MFCInputView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


class MFCOutputView: public MFCDestinationView, public QuaOutputRepresentation
{
public:
	MFCOutputView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(MFCOutputView)

	void						ArrangeChildren();

	afx_msg int					OnCreate(LPCREATESTRUCT lpCreateStruct );
	afx_msg LRESULT				OnQuaCtlChanged(WPARAM, LPARAM);
	afx_msg LRESULT				OnQuaCtlBeginMove(WPARAM, LPARAM);
	afx_msg LRESULT				OnQuaCtlEndMove(WPARAM, LPARAM);

	virtual void				OnInitialUpdate();

	virtual void				DisplayDestination();
	virtual void				DisplayEnable();
	virtual void				DisplayPan();
	virtual void				DisplayGain();

	virtual void				NameChanged();
protected:
	//{{AFX_MSG(MFCOutputView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#define QCID_DV_GAIN	1
#define QCID_DV_PAN		2
#define QCID_DV_SELECT	3
#define QCID_DV_ENABLE	4

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCDESTINATIONVIEW_H__E95E43F7_980D_439B_B3C3_7EFDD8682EBE__INCLUDED_)
