#pragma once

#include "MFCDataEditor.h"
#include "Note.h"

class MFCStreamDataEditor;
class Stream;
class StreamItem;
class StreamBend;
class StreamProg;
class StreamCtrl;

class MFCStreamEditorYScale: public CWnd
{
public:
						MFCStreamEditorYScale();
	DECLARE_DYNCREATE(MFCStreamEditorYScale)
	virtual				~MFCStreamEditorYScale();

	static CFont		displayFont;

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct );
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing( UINT, LPRECT );
	afx_msg void OnMove(int x, int y);

	bool				CreateYScale(CRect &r, CWnd *p, UINT id, MFCStreamDataEditor *);

	CRect				bounds;
	MFCStreamDataEditor	*editor;
	long				keylen;
	DECLARE_MESSAGE_MAP()
};


// note a square block the length of it's duration
// sysx. definitely a chunk of sysx data.
// sysc? could be one way or the other...
class MFCStreamItemView: public MFCEditorItemView
{
public:
						MFCStreamItemView(MFCStreamDataEditor *, StreamItem *);
	virtual				~MFCStreamItemView();

#ifdef QUA_V_GDI_PLUS
	virtual void		Draw(Graphics *, CRect *);
#else
	virtual void		Draw(CDC *, CRect *);
#endif
	virtual void		CalculateBounds();
	virtual bool		Represents(void *);
	virtual short		HitTest(CPoint &, UINT, void * & clkit);

	StreamItem			*item;
};

// ctrl and bend ... collated and shown as an envelope.
// progs collated, a list of program nos.
// likewise values ....
class MFCStreamItemListView: public MFCEditorItemView
{
public:
						MFCStreamItemListView(MFCStreamDataEditor *, CPoint *, short, short);
	virtual				~MFCStreamItemListView();
#ifdef QUA_V_GDI_PLUS
	virtual void		Draw(Graphics *, CRect *);
#else
	virtual void		Draw(CDC *, CRect *);
#endif
	virtual void		CalculateBounds();
	virtual bool		Represents(void *);
	virtual short		HitTest(CPoint &, UINT, void *&clkit);

	short				listitemType;
	short				listitemData1;

	void				MoveToY(long);
	void				ResizeToY(long);

	bool				SetSubItemTime(StreamItem *, Time &);
	bool				SetSubItemValue(StreamItem *, float);
	bool				SetSubItemValue(StreamItem *, int8, int8, int8);
	bool				DelSubItem(StreamItem *);

	long				Val2Pix(StreamCtrl *);
	long				Val2Pix(StreamBend *);
	float				Pix2Val(long);

//	BList				items;
//	inline long			CountItems() { return items.CountItems(); }
//	inline StreamItem	*Item(long i) { return (StreamItem *)items.ItemAt(i); }
//	inline bool			HasItem(StreamItem *i) { return items.HasItem(i); }
//	inline long			IndexOf(StreamItem *i) { return items.IndexOf(i); }
//	inline bool			AddItem(StreamItem *i) { return items.AddItem(i); }
//	inline bool			RemoveItem(StreamItem * i) { return items.RemoveItem(i); }
//	inline StreamItem	*RemoveItemAt(int32 i) { return (StreamItem *)items.RemoveItem(i); }
};

class MFCEnvelopeView: public MFCEditorItemView
{
public:
#ifdef QUA_V_GDI_PLUS
	virtual void		Draw(Graphics *, CRect *);
#else
	virtual void		Draw(CDC *, CRect *);
#endif
	virtual void		CalculateBounds();
	virtual bool		Represents(void *);
	virtual short		HitTest(CPoint &, UINT, void * & clkit);
};

class MFCStreamDataEditor :	public MFCDataEditor
{
public:
	MFCStreamDataEditor(void);
	DECLARE_DYNCREATE(MFCStreamDataEditor)
	virtual ~MFCStreamDataEditor(void);

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt );
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags	);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags );

	afx_msg void			OnAddController();

	virtual void			OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void			OnInitialUpdate();

	virtual long			NHorizontalPix();
	virtual long			HorizontalPix(long);
	virtual void			SetHVScroll();
	virtual bool			SetToTake(Take *t);
	virtual void			EditorContextMenu(CPoint &p, UINT);

	bool					CursorCheck(CPoint pt, bool isCtl);

//	void					SetStream(Stream *);
	void					UpdateDisplayedItems();

	MFCStreamItemListView	*ItemListViewFor(StreamItem *);
	MFCEditorItemView		*AddStreamItemView(StreamItem *i, CPoint *pt, bool redraw);
	bool					DelItemView(MFCEditorItemView *);
	MFCEditorItemView		*RefreshControllerView(ctrl_t t, UINT py);
	void					RemoveItemViewFor(void *i);
	void					MoveItemViewFor(void *i);
	void					SetItemDuration(MFCEditorItemView *, dur_t dur);
	void					SetStartTimeMidiParams(MFCEditorItemView *iv, Time &at, int8);

	bool					AddAllItemViews(bool redraw);

	Stream *stream;
	StreamTake *take;

	StreamItem *mouse_sub_item;
	float last_controller_value;

	long NoteHeight();
	long Note2Pix(long c);
	long Pix2Note(long);

	DECLARE_MESSAGE_MAP()
};
