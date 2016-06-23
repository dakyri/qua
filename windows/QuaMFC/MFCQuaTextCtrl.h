#pragma once

// base class for specialised text display controls
// MFCQuaTextCtrl

#include "StdDefs.h"

#include <string>
using namespace std;

#undef QUA_V_TE1
#ifdef QUA_V_TE1

class MFCQuaTextCtrl : public CStatic
{
	DECLARE_DYNAMIC(MFCQuaTextCtrl)

public:
	MFCQuaTextCtrl();
	virtual ~MFCQuaTextCtrl();

	CRect				bounds;
	CRect				textRect;
	static CFont		defaultDisplayFont;
	CFont				*displayFont;
	COLORREF			bgColor;
	COLORREF			tbgColor;
	COLORREF			textColor;
	bool				fillBg;

	string label;
	CFont *labelFont;

	bool				CreateTextCtrl(CRect &r, CWnd *w, UINT id, CFont *cf=NULL, CRect *tr=NULL, COLORREF *bgc=NULL, COLORREF *tbgc=NULL, COLORREF *tc=NULL);
	void				SetLabel(char *, CFont *lf=NULL);

	virtual void		DrawContents(CDC *);

	void				RedrawDisplay();
	virtual char		*GetText()=NULL;
	virtual void		SetText(char *)=NULL;

	void				SendVCMsg();
	void				SetVCMsgParams(uint32, WPARAM, LPARAM);
	uint32				vcMessid;
	WPARAM				vcWparam;
	LPARAM				vcLparam;

protected:
	//{{AFX_MSG(MFCQuaTextCtrl)
	afx_msg int			OnCreate(LPCREATESTRUCT lpCreateStruct );
	afx_msg void		OnPaint();
	afx_msg BOOL		OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void		OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL		OnEraseBkgnd(CDC* pDC);
	afx_msg void		OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void		OnMenuSelection(UINT nID);
	afx_msg void		OnSize(UINT nType, int cx, int cy);
	afx_msg void		OnSizing( UINT, LPRECT );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class MFCQuaTextCtrlEdit: public CEdit
{
public:
	DECLARE_DYNAMIC(MFCQuaTextCtrlEdit)

	MFCQuaTextCtrlEdit();
	virtual ~MFCQuaTextCtrlEdit();

	MFCQuaTextCtrl		*ctrl;	// probably parent

	int					CreateEdit(MFCQuaTextCtrl *);
	void				RemoveEdit();
protected:
	//{{AFX_MSG(MFCQuaTextCtrlEdit)
	afx_msg void		OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void		OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void		OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



#else
class MFCQuaTextCtrl : public CEdit
{
	DECLARE_DYNAMIC(MFCQuaTextCtrl)

public:
	MFCQuaTextCtrl();
	virtual ~MFCQuaTextCtrl();

	CRect				bounds;
	CRect				textRect;
	static CFont		defaultDisplayFont;
	CFont				*displayFont;
	COLORREF			bgColor;
	COLORREF			tbgColor;
	COLORREF			textColor;
	bool				fillBg;

	string label;
	CFont *labelFont;

	bool				CreateTextCtrl(CRect &r, CWnd *w, UINT id, CFont *cf=NULL, CRect *tr=NULL, COLORREF *bgc=NULL, COLORREF *tbgc=NULL, COLORREF *tc=NULL);
	void				SetLabel(char *, CFont *lf=NULL);

	virtual void		DrawContents(CDC *);

	void				RedrawDisplay();
	virtual char		*GetText()=NULL;
	virtual void		SetText(char *)=NULL;

	void				SendVCMsg();
	void				SetVCMsgParams(uint32, WPARAM, LPARAM);
	uint32				vcMessid;
	WPARAM				vcWparam;
	LPARAM				vcLparam;

protected:
	//{{AFX_MSG(MFCQuaTextCtrl)
	afx_msg int			OnCreate(LPCREATESTRUCT lpCreateStruct );
	afx_msg void		OnPaint();
	afx_msg BOOL		OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void		OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL		OnEraseBkgnd(CDC* pDC);
	afx_msg void		OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void		OnMenuSelection(UINT nID);
	afx_msg void		OnSize(UINT nType, int cx, int cy);
	afx_msg void		OnSizing( UINT, LPRECT );
	afx_msg HBRUSH		CtlColor ( CDC* pDC, UINT nCtlColor );
	afx_msg void		OnKillFocus(CWnd* pNewWnd);
	afx_msg void		OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif