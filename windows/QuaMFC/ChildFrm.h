// ChildFrm.h : interface of the QuaChildFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHILDFRM_H__DDDA867C_3C23_4697_8C17_AF56C91DDD23__INCLUDED_)
#define AFX_CHILDFRM_H__DDDA867C_3C23_4697_8C17_AF56C91DDD23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class MFCArrangeView;
class MFCQuaSymbolIndexView;
class MFCObjectMountView;
class MFCChannelMountView;

#include "MFCQuaTransportBar.h"
#include "MFCArrangerToolBar.h"

class QuaPerceptualSet;
class CQuaMFCDoc;

class QuaChildFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(QuaChildFrame)
public:
	QuaChildFrame();

// Attributes

public:
	CSplitterWnd			topSplitter;
	CSplitterWnd			sideSplitter;

// toolbars on the childframe
	MFCQuaTransportBar		transportBar;
	MFCArrangerToolBar		arrangerToolBar;

// these are typed references to the panes... the splitters create them dynamically as panes
	MFCArrangeView			*arranger;
	MFCChannelMountView		*channeler;
	MFCQuaSymbolIndexView	*indexer;
	MFCObjectMountView		*objectifier;
/*
	BList			channels;
*/

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(QuaChildFrame)
	public:
	virtual BOOL			PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL			OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	afx_msg BOOL			OnViewToolBar(UINT nID);
	afx_msg BOOL			OnSelectArrangerTool(UINT nID);
	afx_msg void			OnUpdateUIArrangerTools(CCmdUI* pCmdUI);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~QuaChildFrame();
#ifdef _DEBUG
	virtual void			AssertValid() const;
	virtual void			Dump(CDumpContext& dc) const;
#endif

	afx_msg void			OnSize(UINT nType, int cx, int cy);
	afx_msg void			OnSizing( UINT, LPRECT );

	virtual BOOL			OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL			OnNotify(WPARAM wParam, LPARAM lParam,	LRESULT* pResult);

	afx_msg BOOL			OnToolTipNeedsTxt( UINT id, NMHDR * pTTTStruct, LRESULT * pResult );

	afx_msg void			OnEditCut();
	afx_msg void			OnEditCopy();
	afx_msg void			OnEditPaste();
	afx_msg void			OnEditUndo();

	int						width;
	int						height;
	CQuaMFCDoc				*document;
	QuaPerceptualSet		*quaLink;
// Generated message map functions
protected:
	//{{AFX_MSG(QuaChildFrame)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDFRM_H__DDDA867C_3C23_4697_8C17_AF56C91DDD23__INCLUDED_)
