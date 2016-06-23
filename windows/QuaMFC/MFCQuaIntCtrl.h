#pragma once

#include "StdDefs.h"
#include "MFCQuaTextCtrl.h"
// MFCQuaTimeCtrl

#ifdef QUA_V_TE1

class MFCQuaIntEdit: public MFCQuaTextCtrlEdit
{
	DECLARE_DYNAMIC(MFCQuaIntEdit)
public:
	MFCQuaIntEdit();
	virtual ~MFCQuaIntEdit();

	//{{AFX_MSG(MFCQuaIntEdit)
	afx_msg void		OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void		OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class MFCQuaIntCtrl : public MFCQuaTextCtrl
{
	DECLARE_DYNAMIC(MFCQuaIntCtrl)

public:
	MFCQuaIntCtrl();
	virtual ~MFCQuaIntCtrl();

	long				value;
	long				min;
	long				mid;
	long				max;
	long				step;
	long				step_2;
	long				step_4;

	bool				CreateIntCtrl(CRect &r, CWnd *w, UINT id, CFont *cf=NULL, CRect *tr=NULL, COLORREF *bgc=NULL, COLORREF *tbgc=NULL, COLORREF *tc=NULL);
	virtual void		DrawContents(CDC *);

	void				SetValue(long);
	void				SetRange(long, long, long);
	void				SetSteps(long, long=0, long=0);

	MFCQuaIntEdit		edit;

	virtual char		*GetText();
	virtual void		SetText(char *);
protected:
	//{{AFX_MSG(MFCQuaIntCtrl)
	afx_msg void		OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void		OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#else
class MFCQuaIntCtrl : public MFCQuaTextCtrl
{
	DECLARE_DYNAMIC(MFCQuaIntCtrl)

public:
	MFCQuaIntCtrl();
	virtual ~MFCQuaIntCtrl();

	long				value;
	long				min;
	long				mid;
	long				max;
	long				step;
	long				step_2;
	long				step_4;

	bool				CreateIntCtrl(CRect &r, CWnd *w, UINT id, CFont *cf=NULL, CRect *tr=NULL, COLORREF *bgc=NULL, COLORREF *tbgc=NULL, COLORREF *tc=NULL);
	virtual void		DrawContents(CDC *);

	void				SetValue(long);
	void				SetRange(long, long, long);
	void				SetSteps(long, long=0, long=0);

	virtual char		*GetText();
	virtual void		SetText(char *);
protected:
	//{{AFX_MSG(MFCQuaIntCtrl)
	afx_msg void		OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void		OnSetFocus(CWnd* pOldWnd);
	afx_msg void		OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
