#if !defined(AFX_MFCCTXTINDEXVIEW_H__10C81D5E_BD16_435A_B5A3_F4825AAD2037__INCLUDED_)
#define AFX_MFCCTXTINDEXVIEW_H__10C81D5E_BD16_435A_B5A3_F4825AAD2037__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MFCQuaContextIndexView.h : header file
//

#include "afxcview.h"
#include "QuaDisplay.h"

/////////////////////////////////////////////////////////////////////////////
// MFCQuaSymbolIndexView

class MFCQuaContextIndexView : public CTreeView, QuaGlobalIndexPerspective
{
public:
	MFCQuaContextIndexView();           // protected constructor used by dynamic creation

	virtual void				addToSymbolIndex(StabEnt *s);
	virtual void				removeFromSymbolIndex(StabEnt *s);
	virtual void				symbolNameChanged(StabEnt *s);
	virtual void				displayArrangementTitle(const char *);

	virtual void				OnInitialUpdate();

	HTREEITEM					AddTopLevelClass(char *, LPARAM, HTREEITEM parent, int img);
	HTREEITEM					AddIndexItem(const char *, LPARAM, HTREEITEM parent, int img);
	void						AddAllIndexItems();
	HTREEITEM					IndexItemFor(StabEnt *, HTREEITEM parit);
	HTREEITEM					IndexItemFor(StabEnt *);

	CImageList					images;

	HTREEITEM					top;
	HTREEITEM					builtins;
	HTREEITEM					vstplugins;
	HTREEITEM					quas;
	HTREEITEM					methods;
	HTREEITEM					templates;
	HTREEITEM					ports;

	DECLARE_DYNCREATE(MFCQuaContextIndexView)

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
	//{{AFX_VIRTUAL(MFCQuaContextIndexView)
	protected:
//	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	//}}AFX_VIRTUAL
public:
// Implementation

protected:
	virtual ~MFCQuaContextIndexView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(MFCQuaContextIndexView)
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
	afx_msg void	DoPopupMenu(UINT nMenuID);
	afx_msg void	OnPopupDelete();
	afx_msg void	OnPopupControl();
	afx_msg void	OnPopupEdit();
	afx_msg void	OnSysColorChange();
	afx_msg void	OnVstPluginView();
	afx_msg void	OnVstPluginSave();
	afx_msg void	OnVstPluginReload();
	afx_msg void	OnVstPluginDisable();
	afx_msg void	OnVstPluginSetDirectory();
	afx_msg void	OnVstPluginAddDirectory();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCCTXTINDEXVIEW_H__10C81D5E_BD16_435A_B5A3_F4825AAD2037__INCLUDED_)
