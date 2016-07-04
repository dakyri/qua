#pragma once


// MFCTakeListView view


#include "afxole.h"

class MFCObjectView;
class MFCDataEditor;
class StabEnt;

class MFCTakeListView : public CListView
{
	DECLARE_DYNCREATE(MFCTakeListView)
public:
	MFCTakeListView();           // protected constructor used by dynamic creation
	virtual ~MFCTakeListView();

	int						CreateList(CRect &r, MFCObjectView *, MFCDataEditor *, UINT);

	afx_msg int				OnCreate(LPCREATESTRUCT lpCreateStruct );
	afx_msg void			OnSize(UINT nType, int cx, int cy);
	afx_msg void			OnSizing( UINT, LPRECT );

	afx_msg void			OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void			OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void			OnRButtonUp(UINT nFlags, CPoint point);

	afx_msg void			OnBeginDrag(LPNMHDR pnmhdr, LRESULT *pResult);
	afx_msg void			OnSelect(LPNMHDR pnmhdr, LRESULT *pResult);
	afx_msg void			OnBeginLabelEdit(LPNMHDR pnmhdr, LRESULT *pResult);
	afx_msg void			OnEndLabelEdit(LPNMHDR pnmhdr, LRESULT *pResult);
	afx_msg void			OnKeyDown(NMHDR *pNotifyStruct,LRESULT *result);

	virtual DROPEFFECT		OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL			OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	virtual DROPEFFECT		OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);

	long					AddTakeItem(const char *, LPARAM, int);
	long					ItemForTake(StabEnt *s);
	long					ItemAtPoint(CPoint &);
	long					AddTake(StabEnt *s);
	bool					RemoveTake(StabEnt *s);
	bool					SelectTake(StabEnt *s, bool);

	CImageList				images;

	MFCObjectView			*parent;
	MFCDataEditor			*editor;

	CRect					bounds;
	CRect					frame;
public:
#ifdef _DEBUG
	virtual void			AssertValid() const;
	virtual void			Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
};


