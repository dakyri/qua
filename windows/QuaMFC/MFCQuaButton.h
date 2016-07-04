#pragma once

#include "StdDefs.h"
#include "BBitmap.h"
#include "BRect.h"

// MFCQuaButton

enum {
	QUA_BPS_UP=0,
	QUA_BPS_OVER=1,
	QUA_BPS_DOWN=2
};


class MFCQuaButton : public CButton
{
	DECLARE_DYNAMIC(MFCQuaButton)

public:
	MFCQuaButton();
	virtual				~MFCQuaButton();

	virtual void		DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	virtual bool		CreateButton(const char *label, CRect &r, CWnd *w, UINT id);

	afx_msg	void		OnMouseMove(UINT nFlags,CPoint point);
	afx_msg void		OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void		OnLButtonUp(UINT nFlags,  CPoint point);
	afx_msg void		OnToolTipNotify(NMHDR *pNMHDR, LRESULT *pResult);

	virtual int			OnToolHitTest(CPoint point, TOOLINFO* pTI) const;

	void				SendLCMsg();
	void				SetLCMsgParams(uint32, WPARAM, LPARAM);
	void				SendShLCMsg();
	void				SetShLCMsgParams(uint32, WPARAM, LPARAM);
	void				SendCtlLCMsg();
	void				SetCtlLCMsgParams(uint32, WPARAM, LPARAM);
	void				SendCtlShLCMsg();
	void				SetCtlShLCMsgParams(uint32, WPARAM, LPARAM);
	uint32				lcMessid;
	WPARAM				lcWparam;
	LPARAM				lcLparam;
	uint32				clcMessid;
	WPARAM				clcWparam;
	LPARAM				clcLparam;
	uint32				slcMessid;
	WPARAM				slcWparam;
	LPARAM				slcLparam;
	uint32				cslcMessid;
	WPARAM				cslcWparam;
	LPARAM				cslcLparam;

	BBitmap				upBitmap;
	BBitmap				ovrBitmap;
	BBitmap				dnBitmap;

	BRect				bounds;
	uchar				buttonPressState;
	bool				mouseTracking;

	string tooltipText;

	static CFont displayFont;
protected:
	DECLARE_MESSAGE_MAP()
};

class MFCQuaChkButton : public MFCQuaButton
{
	DECLARE_DYNAMIC(MFCQuaChkButton)

public:
	MFCQuaChkButton();
	virtual				~MFCQuaChkButton();

	virtual void		DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	afx_msg void		OnLButtonUp(UINT nFlags,  CPoint point);

	virtual bool		CreateButton(char *label, CRect &r, CWnd *w, UINT id);
	void				SetCheckState(short);

	BBitmap				chkOBitmap;
	BBitmap				chkDBitmap;
	BBitmap				chkUBitmap;
	bool				checked;
	bool				enableChecking;

	static CFont		displayFont;
protected:
	DECLARE_MESSAGE_MAP()
};


class MFCSmallQuaChkBut : public MFCQuaChkButton
{
	DECLARE_DYNAMIC(MFCSmallQuaChkBut)

public:
	MFCSmallQuaChkBut();
	virtual				~MFCSmallQuaChkBut();

	virtual bool		CreateButton(const char *label, CRect &r, CWnd *w, UINT id);

	virtual void		DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	static CFont		displayFont;
protected:
	DECLARE_MESSAGE_MAP()
};


class MFCSmallQuaClsBut : public MFCQuaButton
{
	DECLARE_DYNAMIC(MFCSmallQuaClsBut)

public:
	MFCSmallQuaClsBut();
	virtual				~MFCSmallQuaClsBut();

	virtual bool		CreateButton(const char *label, CRect &r, CWnd *w, UINT id);

	virtual void		DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	static CFont		displayFont;
protected:
	DECLARE_MESSAGE_MAP()
};



class MFCSmallQuaMinBut : public MFCQuaChkButton
{
	DECLARE_DYNAMIC(MFCSmallQuaMinBut)

public:
	MFCSmallQuaMinBut();
	virtual				~MFCSmallQuaMinBut();

	virtual bool		CreateButton(const char *label, CRect &r, CWnd *w, UINT id);

	virtual void		DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	static CFont		displayFont;
protected:
	DECLARE_MESSAGE_MAP()
};

class MFCSmallQuaLetterBut : public MFCQuaChkButton
{
	DECLARE_DYNAMIC(MFCSmallQuaLetterBut)

public:
	MFCSmallQuaLetterBut(char c);
	virtual				~MFCSmallQuaLetterBut();

	virtual bool		CreateButton(const char *label, CRect &r, CWnd *w, UINT id);

	virtual void		DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	static CFont		displayFont;
	char				butChar;
protected:
	DECLARE_MESSAGE_MAP()
};
