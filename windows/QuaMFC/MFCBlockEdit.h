#pragma once

#include "QuaDrop.h"
#include "MFCQuaStateDisplay.h"
#include "MFCQuaButton.h"

// MFCBlockEdit view

class MFCObjectView;
class Block;

class MFCBlockEdit : public CEditView
{
	DECLARE_DYNCREATE(MFCBlockEdit)

public:
	MFCBlockEdit();           // protected constructor used by dynamic creation
	virtual					~MFCBlockEdit();

	int						CreateEditor(
								CRect &r,
								MFCObjectView *,
								UINT,
								MFCQuaParseStateDisplay *,
								Block *iniVal);

	MFCObjectView			*parent;

	void					SetSymbol(StabEnt *sym);
	StabEnt					*symbol;

	void					SetValue(Block *b);

	MFCQuaParseStateDisplay	*stateDisplay;

	QuaDrop					dragon;
	COleDropTarget			target;
	static CFont			displayFont;

	void					SendCMsg();
	void					SetCMsgParams(uint32, WPARAM, LPARAM);
	uint32					cMessid;
	WPARAM					cWparam;
	LPARAM					cLparam;

	void					SendPMsg();
	void					SetPMsgParams(uint32, WPARAM, LPARAM);
	uint32					pMessid;
	WPARAM					pWparam;
	LPARAM					pLparam;
public:
	afx_msg int				OnCreate(LPCREATESTRUCT lpCreateStruct );
	afx_msg void			OnContextMenu(CWnd* pWnd, CPoint pos);
	afx_msg void			OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void			OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void			OnKillFocus(CWnd* pNewWnd);
	afx_msg void			OnEditChange();
#ifdef _DEBUG
	virtual void			AssertValid() const;
	virtual void			Dump(CDumpContext& dc) const;
#endif

protected:
	virtual DROPEFFECT OnDragEnter(
			COleDataObject* pDataObject,
			DWORD dwKeyState,
			CPoint point 
		);
	virtual void OnDragLeave( );
	virtual DROPEFFECT OnDragOver(
			COleDataObject* pDataObject,
			DWORD dwKeyState,
			CPoint point 
		);
	virtual BOOL OnDrop(
			COleDataObject* pDataObject,
			DROPEFFECT dropEffect,
			CPoint point 
		);
	virtual DROPEFFECT OnDropEx(
			COleDataObject* pDataObject,
			DROPEFFECT dropDefault,
			DROPEFFECT dropList,
			CPoint point 
		);
	DECLARE_MESSAGE_MAP()
};

class MFCExecutableBlockEdit: public MFCBlockEdit
{
public:
	MFCExecutableBlockEdit();
	virtual ~MFCExecutableBlockEdit();

	DECLARE_DYNCREATE(MFCExecutableBlockEdit)

protected:
	DECLARE_MESSAGE_MAP()
};


class MFCEventBlockEdit: public MFCBlockEdit
{
public:
	MFCEventBlockEdit();
	virtual ~MFCEventBlockEdit();

	DECLARE_DYNCREATE(MFCEventBlockEdit)

protected:
	DECLARE_MESSAGE_MAP()
};

class MFCBlockEditCtrlStrip: public CWnd
{
public:
	MFCBlockEditCtrlStrip();
	virtual ~MFCBlockEditCtrlStrip();

	int				CreateCtrlStrip(char *label, CRect &r, CWnd *w, long id);
	inline void		SetState(long s) { state.SetState(s); }
	void			SetMessageParams(MFCBlockEdit *ctl, long id);

	MFCQuaParseStateDisplay		state;
	MFCSmallQuaLetterBut		compile;
	MFCSmallQuaLetterBut		parse;
	MFCSmallQuaLetterBut		revert;

	MFCBlockEdit				*blockEdit;
	afx_msg LRESULT				OnQuaBlockEditCompile(WPARAM, LPARAM);
	afx_msg LRESULT				OnQuaBlockEditParse(WPARAM, LPARAM);
	afx_msg LRESULT				OnQuaBlockEditRevert(WPARAM, LPARAM);

	DECLARE_DYNCREATE(MFCBlockEditCtrlStrip)

protected:
	DECLARE_MESSAGE_MAP()
};


