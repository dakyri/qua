#pragma once

#include "StdDefs.h"

#include "MFCQuaTextCtrl.h"
// MFCQuaTimeCtrl

#ifdef QUA_V_TE1

class MFCQuaFloatEdit: public MFCQuaTextCtrlEdit
{
	DECLARE_DYNAMIC(MFCQuaFloatEdit)
public:
	MFCQuaFloatEdit();
	virtual ~MFCQuaFloatEdit();

	//{{AFX_MSG(MFCQuaFloatEdit)
	afx_msg void		OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void		OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void		OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class MFCQuaFloatCtrl : public MFCQuaTextCtrl
{
	DECLARE_DYNAMIC(MFCQuaFloatCtrl)

public:
	MFCQuaFloatCtrl();
	virtual ~MFCQuaFloatCtrl();

	float				value;
	float				min;
	float				mid;
	float				max;
	float				step;
	float				step_2;
	float				step_4;

	bool				CreateFloatCtrl(CRect &r, CWnd *w, UINT id, CFont *cf=NULL, CRect *tr=NULL, COLORREF *bgc=NULL, COLORREF *tbgc=NULL, COLORREF *tc=NULL);
	virtual void		DrawContents(CDC *);

	void				SetValue(float val);
	void				SetRange(float min, float ini, float max);
	void				SetSteps(float step1, float step2=0, float step3=0);

	MFCQuaFloatEdit		edit;

	virtual char		*GetText();
	virtual void		SetText(char *);
protected:
	//{{AFX_MSG(MFCQuaFloatEdit)
	afx_msg void		OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void		OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#else
class MFCQuaFloatCtrl : public MFCQuaTextCtrl
{
	DECLARE_DYNAMIC(MFCQuaFloatCtrl)

public:
	MFCQuaFloatCtrl();
	virtual ~MFCQuaFloatCtrl();

	float				value;
	float				min;
	float				mid;
	float				max;
	float				step;
	float				step_2;
	float				step_4;

	bool				CreateFloatCtrl(CRect &r, CWnd *w, UINT id, CFont *cf=NULL, CRect *tr=NULL, COLORREF *bgc=NULL, COLORREF *tbgc=NULL, COLORREF *tc=NULL);
	virtual void		DrawContents(CDC *);

	void				SetValue(float val);
	void				SetRange(float min, float ini, float max);
	void				SetSteps(float step1, float step2=0, float step3=0);

	virtual string getText() override;
	virtual void setText(const char *) override;
protected:
	//{{AFX_MSG(MFCQuaFloatEdit)
	afx_msg void		OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void		OnSetFocus(CWnd* pOldWnd);
	afx_msg void		OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
