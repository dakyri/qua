#ifndef _MFCQUAROTOR
#define _MFCQUAROTOR


#include "BBitmap.h"
#include "BRect.h"

class MFCQuaRotor : public CSliderCtrl
{
	DECLARE_DYNCREATE(MFCQuaRotor)
public:
	bool				SetKnobBitmap(HBITMAP hbmp);
	bool				SetKnobResource(uint32);
	HBITMAP				GetKnobBitmap();
	bool				SetDotBitmap(HBITMAP hbmp);
	bool				SetDotResource(uint32 hbmp);
	HBITMAP				GetDotBitmap();
	bool				SetScaleBitmap(HBITMAP hbmp);
	bool				SetScaleResource(uint32 hbmp);
	HBITMAP				GetScaleBitmap();

	void				SetKnobParams();

	void				PaintRotor(CPaintDC *);
	void				PaintLabel(CPaintDC *);

	virtual void		SetKnobRadius(int nKnobRadius);
	virtual int			GetKnobRadius() const;

	virtual void		SetZero(int nZero);
	virtual int			GetZero() const;

	bool				CreateRotor(char *label, CRect &r, CWnd *w, UINT id, float ini=1, float min=0, float mid=0.5, float max=1, int ami=30, int amx=330, int lWidth=-1);

						MFCQuaRotor();
	virtual				~MFCQuaRotor();

	BRect bounds;
	string label;

	void				SetFloatRange(float, float, float);
	bool				SetFloatValue(float);
	float				fMin;
	float				fMid;
	float				fMax;
	float				fValue;

	void				SetAngleRange(long, long);
	long				angMin;
	long				angMax;

	void				SetPixelScaleLength(long);
	long				pixelScaleLength;

	void				SendVCMsg();
	void				SetVCMsgParams(uint32, WPARAM, LPARAM);
	uint32				vcMessid;
	WPARAM				vcWparam;
	LPARAM				vcLparam;
	void				SendMDMsg();
	void				SetMDMsgParams(uint32, WPARAM, LPARAM);
	uint32				mdMessid;
	WPARAM				mdWparam;
	LPARAM				mdLparam;
	void				SendMUMsg();
	void				SetMUMsgParams(uint32, WPARAM, LPARAM);
	uint32				muMessid;
	WPARAM				muWparam;
	LPARAM				muLparam;

	COLORREF			bgColor;
	void				RedrawRotor();

	static CFont		displayFont;

	virtual int			OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	afx_msg void		OnToolTipNotify(NMHDR *pNMHDR, LRESULT *pResult);

	//{{AFX_VIRTUAL(MFCQuaRotor)
	protected:
	virtual void PreSubclassWindow();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

protected:
//	virtual bool		SetSliderCtrlPos(int nPos);
	virtual bool		PixelDelta(int nDeltaPos, int deltaScale);

	//{{AFX_MSG(MFCQuaRotor)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnToolTipNeedsTxt( UINT id, NMHDR * pTTTStruct, LRESULT * pResult );
	//}}AFX_MSG


protected:
	DECLARE_MESSAGE_MAP()
	int					zeroMark; // not yet implemented
	int					knobRadius;
	int					labelWidth;

	bool				isDragging;

	BBitmap				dotBitmap;
	BBitmap				knobBitmap;
	BBitmap				scaleBitmap;
	POINT				ptDot;
	POINT				ptKnob;
	POINT				ptCenter;
	POINT				ptScale;
	POINT				ptHit;
};

class MFCSmallQuaRotor: public MFCQuaRotor
{
	DECLARE_DYNCREATE(MFCSmallQuaRotor)
public:
	MFCSmallQuaRotor();
	virtual	 ~MFCSmallQuaRotor();

	bool CreateRotor(char *label, CRect &r, CWnd *w,
							UINT id, char *shortlabel, float ini=1, float min=0, float mid=0.5, float max=1, int ami=30, int amx=330, int lWidth=-1);

	string shortLabel;
	afx_msg	void OnPaint();

	static CFont displayFont;
protected:
	DECLARE_MESSAGE_MAP()
};

#endif