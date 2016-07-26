#if !defined(AFX_MFCARRANGEVIEW_H__10C81D5E_BD16_435A_B5A3_F4825AAD2037__INCLUDED_)
#define AFX_MFCARRANGEVIEW_H__10C81D5E_BD16_435A_B5A3_F4825AAD2037__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MFCArrangeView.h : header file
//

#include "afxole.h"

#ifdef QUA_V_GDI_PLUS
#include <gdiplus.h>
using namespace Gdiplus;
#endif

#include "StdDefs.h"
#include "QuaDisplay.h"
#include "Time.h"

#include "QuaDrop.h"

#include "BRect.h"

class Channel;
class MFCArrangeView;
class MFCChannelMountView;

#include "MFCDataEditor.h"

// not a cview/cwnd! just a container for a crect and wrapper round drawing code
//   ... should it be or not?
class MFCInstanceView: public QuaInstanceRepresentation
//	, public MFCEditorItemView
{
public:
	MFCInstanceView(Instance *i, MFCArrangeView *);
	~MFCInstanceView();


#ifdef QUA_V_GDI_PLUS
	static Font			labelFont;
	void				Draw(Graphics &g, CRect &r);
#else
	void				Draw(CDC *, CRect &r);
#endif
	void Redraw();
	void DrawMove();
	void CalculateBounds();
	void Select(bool);

//	virtual bool		Represents(void *);
//	virtual short		HitTest(CPoint &, UINT, void * & clkit);

	BRect bounds;
	bool selected;
	MFCArrangeView *arranger;
};

/////////////////////////////////////////////////////////////////////////////
// MFCArrangeView view

class MFCArrangeView : 
		public MFCSequenceEditor,
		public QuaArrangerPerspective
{
public:
	MFCArrangeView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(MFCArrangeView)

// Attributes
public:
	MFCChannelMountView*channeler;
	virtual void SetHVScroll();

//	long						currentTool;

	CPoint mouse_move_inst_offset;
	MFCInstanceView *mouse_instance;

// Operations
public:
	virtual void EditorContextMenu(CPoint &p, UINT nf);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct );
	afx_msg void OnDestroy();

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point	);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point );
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point );
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point );
	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point	);

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta,	CPoint pt );
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt,	UINT nFlags	);
	afx_msg void OnKeyUp(UINT nChar,	UINT nRepCnt, UINT nFlags );

//	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MFCArrangeView)

protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();

	virtual DROPEFFECT OnDragEnter( COleDataObject* pDataObject, DWORD dwKeyState, CPoint point  );
	virtual void OnDragLeave( );
	virtual DROPEFFECT OnDragOver( COleDataObject* pDataObject, DWORD dwKeyState, CPoint point  );
	virtual BOOL OnDrop( COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point  );
	virtual DROPEFFECT OnDropEx( COleDataObject* pDataObject, DROPEFFECT dropDefault,
			DROPEFFECT dropList, CPoint point  );
	virtual BOOL OnScroll( UINT nScrollCode, UINT nPos, BOOL bDoScroll = TRUE );
	virtual BOOL OnScrollBy( CSize sizeScroll, BOOL bDoScroll = TRUE  );
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~MFCArrangeView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	virtual void AddInstanceRepresentation(Instance *i);
	virtual void MoveInstanceRepresentation(Instance *i);
	virtual void RemoveInstanceRepresentation(Instance *i);

	virtual void displayArrangementTitle(const char *);
	virtual void DisplayChannelMetric(Channel *c, Metric *m, bool async);

	virtual void DisplayCurrentTime(Time &t, bool async);	// ie update time cursor
	virtual void DisplayTempo(float t, bool async);
	virtual void DisplayMetric(Metric *m, bool async);	// ie update time cursor

	virtual void updateClipIndexDisplay();

	virtual void UpdateControllerDisplay(StabEnt *, QuasiStack *, StabEnt *);
	virtual void symbolNameChanged(StabEnt *s);

	MFCInstanceView *InstanceViewAtPoint(CPoint &);
	void DeselectAll();
	void ChangeSelection(BRect &r);
	bool AddAllItemViews(bool redraw);

	MFCDataEditorTScale			tScale;

// pixel translation
	long ChannelHeight(int c);
	long Channel2Pix(int c);
	short Pix2Channel(long);

	virtual long NHorizontalPix() { return 1; }
	virtual long HorizontalPix(long) { return 1; }

	virtual long Time2Pix(Time &t);
	virtual void Pix2Time(long, Time &t);

	Time cursorTime;
	long cursorPx;
	void SetCursor(Time &t);

// drawing
#ifdef QUA_V_GDI_PLUS
	void DrawGridGraphics(Graphics &g, CRect &r);
	void DrawCursor(Graphics &g, CRect &r);
#else
	void DrawCursor(CDC *pdc, CRect *);
#endif
	void DrawGrid(CDC *pdc, CRect *);
	// Generated message map functions
protected:
	//{{AFX_MSG(MFCArrangeView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCARRANGEVIEW_H__10C81D5E_BD16_435A_B5A3_F4825AAD2037__INCLUDED_)
