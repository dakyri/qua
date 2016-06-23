#if !defined(AFX_MFCDATAEDITOR_H__10C81D5E_BD16_435A_B5A3_F4825AAD2037__INCLUDED_)
#define AFX_MFCDATAEDITOR_H__10C81D5E_BD16_435A_B5A3_F4825AAD2037__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MFCDataEditor.h : header file
//

#include "afxole.h"

#ifdef QUA_V_GDI_PLUS
#include <gdiplus.h>
using namespace Gdiplus;
#endif

#include "StdDefs.h"
#include "BRect.h"
#include "QuaDisplay.h"
#include "Time.h"

#include "QuaDrop.h"

#include <vector>

using namespace std;

class MFCObjectView;
class MFCDataEditor;
class MFCDataEditorTScale;
class MFCSequenceEditor;
class MFCEditorItemView;
class Take;
class Clip;
class StabEnt;

/////////////////////////////////////////////////////////////////////////////
// MFCDataEditor view

class MFCEditorItemView
{
public:
						MFCEditorItemView(MFCSequenceEditor *, short);
	virtual				~MFCEditorItemView();

#ifdef QUA_V_GDI_PLUS
	virtual void		Draw(Graphics *, CRect *);
#else
	virtual void		Draw(CDC *, CRect *);
#endif
	virtual void		CalculateBounds();
	virtual bool		Represents(void *)=NULL;
	virtual short		HitTest(CPoint &, UINT, void * & clkit)=NULL;

	void				Redraw(bool redraw=true);
	void				DrawMove();
	void				Select(bool);

	inline BRect &		BoundingBox() { return bounds; }

	MFCSequenceEditor	*editor;
	bool				selected;
	enum {
		NONE = 0,
		DISCRETE = 1,
		LIST = 2,
		ENV = 3,
		CLIP = 4,
		SYM = 5,
		INSTANCE=6
	};

	short				type;
protected:
	BRect				bounds;
};

class MFCClipItemView: public MFCEditorItemView
{
public:
						MFCClipItemView(MFCSequenceEditor *, Clip *);
	virtual				~MFCClipItemView();

#ifdef QUA_V_GDI_PLUS
	virtual void		Draw(Graphics *, CRect *);
#else
	virtual void		Draw(CDC *, CRect *);
#endif
	virtual void		CalculateBounds();
	virtual bool		Represents(void *);
	virtual short		HitTest(CPoint &, UINT, void * & clkit);

	void				SetClipStartTime(Time &st);
	void				SetClipEndTime(Time &et);

	Clip				*item;
};

class MFCSymItemView: public MFCEditorItemView
{
public:
						MFCSymItemView(MFCDataEditor *, StabEnt *);
	virtual				~MFCSymItemView();

#ifdef QUA_V_GDI_PLUS
	virtual void		Draw(Graphics *, CRect *);
#else
	virtual void		Draw(CDC *, CRect *);
#endif
	virtual void		CalculateBounds();
	virtual bool		Represents(void *);
	virtual short		HitTest(CPoint &, UINT, void * & clkit);

	StabEnt				*item;
};


class MFCSequenceEditor : public CScrollView
{
public:
	MFCSequenceEditor();           // protected constructor used by dynamic creation
	virtual						~MFCSequenceEditor();
	DECLARE_DYNCREATE(MFCSequenceEditor)

	virtual void				PostNcDestroy();

// Attributes
public:
	CRect						bounds;
	CRect						frame;

	Time						lastScheduledEvent;
//	long						currentTool;
	HCURSOR						originalCursor;
	COleDropTarget				target;

	QuaDrop						dragon;

	CWnd						*yscale;
	MFCDataEditorTScale			*xscale;

	virtual long				NHorizontalPix() { return 0;  };
	virtual long				HorizontalPix(long) { return 0; };
	virtual void				SetHVScroll() { };

	virtual void				EditorContextMenu(CPoint &p, UINT nf);
	void						TrackPopupMenu(CPoint &pos, UINT nMenuID);

	action_t					mouse_action;
	CPoint						mouse_click;
	CPoint						last_mouse_point;
	bool						mouse_captured;
	MFCEditorItemView			*mouse_item;
	CPoint						mouse_move_item_offset;

// children with a visual representation
	vector<MFCEditorItemView *> itemRepresentations;
	inline long NItemR()
		{ return itemRepresentations.size(); }
	inline MFCEditorItemView *ItemR(long i)
		{ return i >= 0 && ((unsigned)i)<itemRepresentations.size()? itemRepresentations[i]:nullptr; }
	inline void AddItemR(MFCEditorItemView *i)
		{ itemRepresentations.push_back(i); }
	inline bool	RemItemR(MFCEditorItemView *i) {
		for (auto it = itemRepresentations.begin(); it != itemRepresentations.end(); ++it) {
			if (*it == i) {
				itemRepresentations.erase(it);
				return true;
			}
		}
		return false;
	}

	MFCEditorItemView			*ItemViewFor(void *i);
	MFCEditorItemView			*ItemViewAtPoint(CPoint &p, UINT flags, short &hitType, void *&clkit);

	MFCEditorItemView *			AddClipItemView(Clip*clip);
	bool						DelClipItemView(MFCEditorItemView *itemview);
	bool						RemoveItemView(MFCEditorItemView *itemview);
	bool						ClearAllItemViews(bool rdw);

	bool						RemoveClipsNotIn(vector<StabEnt*> &);

// Operations
public:
	afx_msg void				OnSize(UINT nType, int cx, int cy);
	afx_msg void				OnSizing( UINT, LPRECT );
	afx_msg int					OnCreate(LPCREATESTRUCT lpCreateStruct );

	afx_msg void				OnLButtonDown(UINT nFlags, CPoint point	);
	afx_msg void				OnLButtonUp(UINT nFlags, CPoint point );
	afx_msg void				OnLButtonDblClk(UINT nFlags, CPoint point );
	afx_msg void				OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void				OnMButtonUp(UINT nFlags, CPoint point );
	afx_msg void				OnMButtonDblClk(UINT nFlags, CPoint point	);
	afx_msg void				OnRButtonDown(UINT nFlags, CPoint point	);
	afx_msg void				OnRButtonUp(UINT nFlags, CPoint point	);
	afx_msg void				OnRButtonDblClk(UINT nFlags,	CPoint point);
	afx_msg void				OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL				OnMouseWheel(UINT nFlags, short zDelta,	CPoint pt );
	afx_msg void				OnKeyDown(UINT nChar, UINT nRepCnt,	UINT nFlags	);
	afx_msg void				OnKeyUp(UINT nChar,	UINT nRepCnt, UINT nFlags );

//	afx_msg void				OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void				OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MFCSequenceEditor)
protected:
	virtual void	OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void	OnInitialUpdate();

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
	virtual BOOL OnScroll(
			UINT nScrollCode,
			UINT nPos,
			BOOL bDoScroll = TRUE 
		);
	virtual BOOL OnScrollBy(
			CSize sizeScroll,
			BOOL bDoScroll = TRUE 
		);
	//}}AFX_VIRTUAL

// Implementation
protected:
#ifdef _DEBUG
	virtual void				AssertValid() const;
	virtual void				Dump(CDumpContext& dc) const;
#endif

public:
	void						DeselectAll();
	void						ChangeSelection(BRect &r);

// pixel translation
	virtual long				Time2Pix(Time &t);
	virtual void				Pix2Time(long, Time &t);

	long						ticksPerNotch; // 1 = maximum res
	short						pixPerNotch; // 2 = minimum spread between grid lines
	Metric						*displayMetric;
// drawing
#ifdef QUA_V_GDI_PLUS
	void						DrawGridGraphics(Graphics *g, CRect *);
	void						DrawCursor(Graphics *g, CRect *);
#else
	void						DrawCursor(CDC *pdc, CRect *);
#endif
	void						DrawGrid(CDC *pdc, CRect *);
	// Generated message map functions
protected:
	//{{AFX_MSG(MFCSequenceEditor)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class MFCDataEditor: public MFCSequenceEditor
{
public:
	MFCObjectView				*parent;

	bool 						CreateEditor(CRect &, MFCObjectView *, UINT);

	virtual bool				SetToTake(Take *t)=0;
};

class MFCDataEditorTScale: public CWnd
{
public:
						MFCDataEditorTScale();
	DECLARE_DYNCREATE(MFCDataEditorTScale)
	virtual				~MFCDataEditorTScale();

	static CFont		displayFont;
	afx_msg BOOL		OnEraseBkgnd(CDC* pDC);
	afx_msg void		OnPaint();
	afx_msg int			OnCreate(LPCREATESTRUCT lpCreateStruct );
	afx_msg void		OnSize(UINT nType, int cx, int cy);
	afx_msg void		OnSizing( UINT, LPRECT );
	afx_msg void		OnMove(int x, int y);
	afx_msg void		OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	bool				CreateTScale(CRect &r, CWnd *p, UINT id, MFCSequenceEditor *);

	CRect				bounds;
	MFCSequenceEditor	*editor;

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCDATAEDITOR_H__10C81D5E_BD16_435A_B5A3_F4825AAD2037__INCLUDED_)
