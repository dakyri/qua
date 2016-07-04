#pragma once


// MFCSimpleTypeView view

#include "afxole.h"

class MFCObjectView;
class StabEnt;

#include <vector>
using namespace std;

class MFCSimpleTypeView : public CListView
{
public:
	DECLARE_DYNCREATE(MFCSimpleTypeView)
	MFCSimpleTypeView();           // protected constructor used by dynamic creation
	virtual					~MFCSimpleTypeView();

	int						CreateList(CRect &r, MFCObjectView *, UINT);

	afx_msg int				OnCreate(LPCREATESTRUCT lpCreateStruct );
	afx_msg void			OnSize(UINT nType, int cx, int cy);
	afx_msg void			OnSizing( UINT, LPRECT );

	afx_msg void			OnLButtonDown(UINT nFlags, CPoint point	);
	afx_msg void			OnLButtonUp(UINT nFlags, CPoint point );
	afx_msg void			OnLButtonDblClk(UINT nFlags, CPoint point );
	afx_msg void			OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void			OnMButtonUp(UINT nFlags, CPoint point );
	afx_msg void			OnMButtonDblClk(UINT nFlags, CPoint point	);
	afx_msg void			OnRButtonDown(UINT nFlags, CPoint point	);
	afx_msg void			OnRButtonUp(UINT nFlags, CPoint point	);
	afx_msg void			OnRButtonDblClk(UINT nFlags,	CPoint point);
	afx_msg void			OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL			OnMouseWheel(UINT nFlags, short zDelta,	CPoint pt );
	afx_msg void			OnKeyDown(UINT nChar, UINT nRepCnt,	UINT nFlags	);
	afx_msg void			OnKeyUp(UINT nChar,	UINT nRepCnt, UINT nFlags );

	afx_msg void			OnBeginDrag(LPNMHDR pnmhdr, LRESULT *pResult);
	afx_msg void			OnSelect(LPNMHDR pnmhdr, LRESULT *pResult);
	afx_msg void			OnBeginLabelEdit(LPNMHDR pnmhdr, LRESULT *pResult);
	afx_msg void			OnEndLabelEdit(LPNMHDR pnmhdr, LRESULT *pResult);
	afx_msg void			OnKeyDown(NMHDR *pNotifyStruct,LRESULT *result);

	virtual DROPEFFECT		OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL			OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	virtual DROPEFFECT		OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);

	virtual void			DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	long					AddSymItem(const char *, LPARAM, int);
	long					ItemForSym(StabEnt *s);
	StabEnt *				SymForItem(long);
	long					ItemAtPoint(CPoint &);
	long					AddSym(StabEnt *s);
	bool					RemoveSym(StabEnt *s);
	bool					SelectSym(StabEnt *s, bool);
	bool					RemoveSymbolsNotIn(vector<StabEnt*> &list);

	void					ContextMenu(UINT nFlags, CPoint point);

	CImageList				images;

	MFCObjectView			*parent;

	TVITEM					selected_item;

	CRect					bounds;
	CRect					frame;

protected:

public:
#ifdef _DEBUG
	virtual void			AssertValid() const;
	virtual void			Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
};


