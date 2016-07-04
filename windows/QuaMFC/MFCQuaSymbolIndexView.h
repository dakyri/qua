#if !defined(AFX_MFCINDEXVIEW_H__10C81D5E_BD16_435A_B5A3_F4825AAD2037__INCLUDED_)
#define AFX_MFCINDEXVIEW_H__10C81D5E_BD16_435A_B5A3_F4825AAD2037__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MFCQuaSymbolIndexView.h : header file
//

#include "afxcview.h"
#include "QuaDisplay.h"

/////////////////////////////////////////////////////////////////////////////
// MFCQuaSymbolIndexView

class MFCQuaSymbolIndexView : public CTreeView, QuaIndexPerspective
{
public:
	MFCQuaSymbolIndexView();           // protected constructor used by dynamic creation

	virtual void				displayArrangementTitle(const char *);

	virtual void				addToSymbolIndex(StabEnt *s);
	virtual void				removeFromSymbolIndex(StabEnt *s);
	virtual void				symbolNameChanged(StabEnt *s);

	virtual void				updateClipIndexDisplay();

	virtual void				OnInitialUpdate();

	HTREEITEM					AddToSymbolIndex(StabEnt *s, HTREEITEM iti);
	HTREEITEM					AddTopLevelClass(char *, LPARAM, HTREEITEM parent, int);
	HTREEITEM					AddIndexItem(const char *, LPARAM, HTREEITEM parent, int);
	void						AddAllIndexItems();
	void						AddMethodIndexItems(StabEnt *, HTREEITEM parit);
	void						AddInstanceIndexItems(StabEnt *, HTREEITEM parit);
	HTREEITEM					IndexItemFor(StabEnt *, HTREEITEM parit);
	HTREEITEM					IndexItemFor(StabEnt *);

	HTREEITEM					top;
	HTREEITEM					channels;
	HTREEITEM					samples;
	HTREEITEM					voices;
	HTREEITEM					methods;
	HTREEITEM					regions;
	HTREEITEM					markers;

	CImageList					images;

	DECLARE_DYNCREATE(MFCQuaSymbolIndexView)

// Attributes
public:
	HTREEITEM					draggedItem;
	BOOL						draggingNow;
	CImageList					*dragImageList;

	HTREEITEM					selectedItem;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MFCQuaSymbolIndexView)
	protected:
//	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~MFCQuaSymbolIndexView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(MFCQuaSymbolIndexView)
		// NOTE - the ClassWizard will add and remove member functions here.
	afx_msg void	OnBeginDrag(NMHDR *pnmh, LRESULT * bHandled);
	afx_msg void	OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void	OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnNodeSelect(NMHDR *pNotifyStruct,LRESULT *result);
	afx_msg void	OnNodeExpand(NMHDR *pNotifyStruct,LRESULT *result);
	afx_msg void	OnEndLabelEdit(NMHDR *pNotifyStruct,LRESULT *result);
	afx_msg void	OnBeginLabelEdit(NMHDR *pNotifyStruct,LRESULT *result);
	afx_msg void	OnKeyDown(NMHDR *pNotifyStruct,LRESULT *result);
	afx_msg void	OnRightClick(NMHDR *pNotifyStruct,LRESULT *result);
	afx_msg void	OnDoubleClick(NMHDR *pNotifyStruct,LRESULT *result);
	afx_msg void	OnRightDoubleClick(NMHDR *pNotifyStruct,LRESULT *result);
	afx_msg void	DoPopupMenu(UINT nMenuID);
	afx_msg void	OnPopupDelete();
	afx_msg void	OnPopupAddMethod();
	afx_msg void	OnPopupControl();
	afx_msg void	OnPopupEdit();
	afx_msg void	OnPopupSelectClip();
	afx_msg void	OnPopupGotoStartClip();
	afx_msg void	OnPopupGotoEndClip();
	afx_msg void	OnSysColorChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCINDEXVIEW_H__10C81D5E_BD16_435A_B5A3_F4825AAD2037__INCLUDED_)
