#pragma once


// MFCClipListView view

#include "afxole.h"

class MFCObjectView;
class MFCDataEditor;
class StabEnt;
class QuaObjectRepresentation;

#include <vector>
using namespace std;

class MFCClipListView : public CTreeView
{
public:
	DECLARE_DYNCREATE(MFCClipListView)
	MFCClipListView();           // protected constructor used by dynamic creation
	virtual					~MFCClipListView();

	int						CreateList(CRect &r, MFCObjectView *, QuaObjectRepresentation *, MFCDataEditor *, UINT);

	afx_msg int				OnCreate(LPCREATESTRUCT lpCreateStruct );
	afx_msg void			OnSize(UINT nType, int cx, int cy);
	afx_msg void			OnSizing( UINT, LPRECT );

	afx_msg void			OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void			OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void			OnRButtonUp(UINT nFlags, CPoint point);

	afx_msg void			OnBeginDrag(NMHDR *pnmh, LRESULT * bHandled);
	afx_msg void			OnNodeSelect(NMHDR *pNotifyStruct,LRESULT *result);
	afx_msg void			OnNodeExpand(NMHDR *pNotifyStruct,LRESULT *result);
	afx_msg void			OnEndLabelEdit(NMHDR *pNotifyStruct,LRESULT *result);
	afx_msg void			OnBeginLabelEdit(NMHDR *pNotifyStruct,LRESULT *result);
	afx_msg void			OnKeyDown(NMHDR *pNotifyStruct,LRESULT *result);
	afx_msg void			OnRightClick(NMHDR *pNotifyStruct,LRESULT *result);

	afx_msg void			OnPopupDeleteClip();
	afx_msg void			OnPopupEditClip();
	afx_msg void			OnPopupCreateClip();
	afx_msg void			OnPopupDeleteTake();
	afx_msg void			OnPopupEditTake();
	afx_msg void			OnPopupCreateStream();
	afx_msg void			OnPopupLoadSample();
	afx_msg void			OnSysColorChange();

	BOOL					DoPopupMenu(UINT nMenuID, bool return_cmd);

	virtual DROPEFFECT		OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL			OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	virtual DROPEFFECT		OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);

//	HTREEITEM				AddClipItem(char *, LPARAM, int);
	HTREEITEM				ItemFor(StabEnt *s);
	HTREEITEM				ItemFor(StabEnt *s, HTREEITEM parent);
	HTREEITEM				ItemAtPoint(CPoint &);
	HTREEITEM				AddItem(StabEnt *s);
	HTREEITEM				AddItem(const char *s, LPARAM type, HTREEITEM parent, int img);
	bool					RemoveItem(StabEnt *s);
	bool					SelectClip(StabEnt *s, bool);

	bool					RemoveClipsNotIn(vector<StabEnt*> &present);
	bool					RemoveTakesNotIn(vector<StabEnt*> &present);

	CImageList				images;

	QuaObjectRepresentation	*parent;
	MFCDataEditor			*editor;

//	TVITEM					selected_item;
	HTREEITEM				selectedItem;

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


