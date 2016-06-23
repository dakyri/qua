#if !defined(AFX_MFCCHANNELVIEW_H__E95E43F7_980D_439B_B3C3_7EFDD8682EBE__INCLUDED_)
#define AFX_MFCCHANNELVIEW_H__E95E43F7_980D_439B_B3C3_7EFDD8682EBE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class MFCOutputView;
class MFCInputView;

class Input;
class Output;
class Channel;

#include "QuaDisplay.h"
#include "BBitmap.h"
#include <vector>
using namespace std;

/////////////////////////////////////////////////////////////////////////////
// MFCChannelView view
/////////////////////////////////////////////////////////////////////////////
class MFCChannelView : public CView, public QuaChannelRepresentation
{
public:
	MFCChannelView();           // protected constructor used by dynamic creation
	virtual				~MFCChannelView();
	DECLARE_DYNCREATE(MFCChannelView)

// Attributes
public:
	CRect				bounds;
	CRect				frame;
	static CFont		displayFont;

	static BBitmap		inArrowImg;
	static BBitmap		outArrowImg;

	bool				selected;

// Operations
public:
	afx_msg void		OnSize(UINT nType, int cx, int cy);
	afx_msg void		OnSizing( UINT, LPRECT );
	afx_msg int			OnCreate(LPCREATESTRUCT lpCreateStruct );
	afx_msg void		OnMove(int x, int y);
	afx_msg BOOL		OnEraseBkgnd(CDC* pDC);
	afx_msg void		OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void		OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void		OnMouseMove(UINT nFlags, CPoint point);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MFCChannelView)
	protected:
	virtual void		OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL


// Implementation
public:

	virtual void		AddInputRepresentation(Input *d);
	virtual void		AddOutputRepresentation(Output *d);
	virtual void		RemoveInputRepresentation(Input *d);
	virtual void		RemoveOutputRepresentation(Output *d);
	virtual void		NameChanged();
	void				ArrangeChildren();

	void				ChannelMenu(CPoint p);

protected:
#ifdef _DEBUG
	virtual void		AssertValid() const;
	virtual void		Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(MFCChannelView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

struct menu_idx_item
{
	menu_idx_item(QuaPort *p, long c, short sub)
	{
		port = p;
		chan = c;
		submenu = sub;
	}

	QuaPort			*port;
	long			chan;
	short			submenu;
};

class PortPopup
{
public:
						PortPopup();
						~PortPopup();

	vector<menu_idx_item*> menuIdx;

	void				ClearMenuIndex();
	long				MenuIndexItem(QuaPort *, long, short);

	CMenu				*PortMenu(bool isinp);
	CMenu				*AudioMenu(bool isinp, short);
	CMenu				*MidiMenu(bool isinp);
	CMenu				*JoyMenu(bool isinp);
	CMenu				*ChannelMenu(bool isinp);
	bool				DoPopup(CWnd *, CPoint &scrPt, QuaPort *&p, port_chan_id &cp, bool isinp);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#endif // !defined(AFX_MFCCHANNELVIEW_H__E95E43F7_980D_439B_B3C3_7EFDD8682EBE__INCLUDED_)
