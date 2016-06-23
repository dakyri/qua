#pragma once

class MFCDestinationView;

// DeviceSelector

#include "BBitmap.h"

class MFCDeviceSelector : public CStatic
{
	DECLARE_DYNAMIC(MFCDeviceSelector)

public:
	CRect				bounds;
	CRect				textRect;
	bool				isInput;
	MFCDestinationView	*destinationView;

						MFCDeviceSelector();
	virtual				~MFCDeviceSelector();

	const char *		DeviceName(uchar df, uchar cf);
	long				DeviceType();

	static CFont		displayFont;

	static BBitmap		midiImg;
	static BBitmap		audioImg;
	static BBitmap		otherImg;
	static BBitmap		joyImg;

	void				CreateSelector(CRect &, MFCDestinationView *, UINT id, bool);

	virtual int			OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	afx_msg void		OnToolTipNotify(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL		OnToolTipNeedsTxt( UINT id, NMHDR * pTTTStruct, LRESULT * pResult );
	afx_msg void		OnInitMenuPopup(
								CMenu* pPopupMenu,
								UINT nIndex,
								BOOL bSysMenu 
								);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MFCDeviceSelector)
	protected:
	//}}AFX_VIRTUAL
protected:
	//{{AFX_MSG(MFCDeviceSelector)
	afx_msg int			OnCreate(LPCREATESTRUCT lpCreateStruct );
	afx_msg void		OnPaint();
	afx_msg BOOL		OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg BOOL		OnEraseBkgnd(CDC* pDC);
	afx_msg void		OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void		OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void		OnMenuSelection(UINT nID);
	afx_msg LRESULT		OnMenuCommand(WPARAM, LPARAM);
	afx_msg void		OnSize(UINT nType, int cx, int cy);
	afx_msg void		OnSizing( UINT, LPRECT );
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


