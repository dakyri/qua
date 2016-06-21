#pragma once


// MFCQuaTimeCtrl
#include "DaBasicTypes.h"
#include "MFCQuaTextCtrl.h"
#include "inx/Time.h"

#ifdef QUA_V_TE1

class MFCQuaTimeEdit: public MFCQuaTextCtrlEdit
{
	DECLARE_DYNAMIC(MFCQuaTimeEdit)
public:
	MFCQuaTimeEdit();
	virtual ~MFCQuaTimeEdit();

	//{{AFX_MSG(MFCQuaTimeEdit)
	afx_msg void		OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void		OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void		OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void		OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class MFCQuaTimeCtrl : public MFCQuaTextCtrl
{
	DECLARE_DYNAMIC(MFCQuaTimeCtrl)

public:
	MFCQuaTimeCtrl();
	virtual ~MFCQuaTimeCtrl();


	bool				CreateTimeCtrl(CRect &r, CWnd *w, UINT id, CFont *cf=NULL, CRect *tr=NULL, COLORREF *bgc=NULL, COLORREF *tbgc=NULL, COLORREF *tc=NULL);
	virtual void		DrawContents(CDC *);

	virtual char		*GetText();
	virtual void		SetText(char *);

	void				SetTime(Time&t);
	void				GetTime(Time&t);

	MFCQuaTimeEdit		edit;
	Time				time;
protected:
	//{{AFX_MSG(MFCQuaTimeCtrl)
	afx_msg void		OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void		OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#else

class MFCQuaTimeCtrl : public MFCQuaTextCtrl
{
	DECLARE_DYNAMIC(MFCQuaTimeCtrl)

public:
	MFCQuaTimeCtrl();
	virtual ~MFCQuaTimeCtrl();


	bool				CreateTimeCtrl(CRect &r, CWnd *w, UINT id, CFont *cf=NULL, CRect *tr=NULL, COLORREF *bgc=NULL, COLORREF *tbgc=NULL, COLORREF *tc=NULL);
	virtual void		DrawContents(CDC *);

	virtual char		*GetText();
	virtual void		SetText(char *);

	void				SetTime(Time&t);
	void				GetTime(Time&t);

	Time				time;
protected:
	//{{AFX_MSG(MFCQuaTimeCtrl)
	afx_msg void		OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void		OnSetFocus(CWnd* pOldWnd);
	afx_msg void		OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif