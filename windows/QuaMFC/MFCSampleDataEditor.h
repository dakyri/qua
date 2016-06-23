#pragma once
#include <afxmt.h>
#include "MFCDataEditor.h"

class SampleTake;
class SampleFile;

#if QUA_V_SAMPLE_DRAW=='p'
//peak data stored in an array[1..nchannel][1..npoints]
struct peak_data
{
	float	hi;
	float	lo;
	bool	valid;
};
#endif

class MFCSampleDataEditor :	public MFCDataEditor
{
public:
	MFCSampleDataEditor(void);

	DECLARE_DYNCREATE(MFCSampleDataEditor)

	virtual					~MFCSampleDataEditor(void);

	virtual long			NHorizontalPix();
	virtual long			HorizontalPix(long);
	virtual void			SetHVScroll();
	virtual bool			SetToTake(Take *t);
	virtual void			EditorContextMenu(CPoint &p, UINT);

	SampleTake				*take;
//	long					SampleValHeight();
	long					SampleVal2Pix(float v, short c);
	float					Pix2SampleVal(long, short &c);

#if QUA_V_SAMPLE_DRAW=='p'
	enum {
		PEAKER_RAW_BUFSIZE = 4096
	};
	int32					Peakerator();
	static int32			PeakeratorWrapper(void *data);
	thread_id				peakingThread;
	CMutex					peakingMutx;
	peak_data				**peakCache;
	ulong					peakCacheLength;
	ushort					peakCacheNChannel;
	bool					stillPeaking;
	float					peakSampleBuffer[PEAKER_RAW_BUFSIZE];
	char					peakRawBuffer[PEAKER_RAW_BUFSIZE];
	long					peakBufferFrame;
	long					peakBufferNFrames;
	long					currentPeakIdx;
	long					firstUndrawnTick;
	long					lastUndrawnTick;
	bool					scheduledDraw;

	bool					PeakData(long idx, short channel, float &sample_hi, float &sample_lo);
	bool					SetupPeakCache(SampleTake *t);
	bool					CleanupPeakCache();
	bool					SchedulePeakRedraw(long from, long to);
	bool					SignalPeakRedraw(long from, long to);
#endif

	afx_msg void			OnDestroy();
	afx_msg void			OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void			OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void			OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void			OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void			OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void			OnMButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void			OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL			OnMouseWheel(UINT nFlags, short zDelta,	CPoint pt );
	afx_msg void			OnKeyDown(UINT nChar, UINT nRepCnt,	UINT nFlags	);
	afx_msg void			OnKeyUp(UINT nChar,	UINT nRepCnt, UINT nFlags );

	afx_msg void			OnAddController();

	virtual void			OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void			OnInitialUpdate();

	bool					CursorCheck(CPoint pt, bool isCtl);

	void					UpdateDisplayedItems();

	bool					DelItemView(MFCEditorItemView *);
	void					RemoveItemViewFor(void *i);
	void					MoveItemViewFor(void *i);
//	void					SetItemDuration(MFCEditorItemView *, dur_t dur);
//	MFCEditorItemView		*AddStreamItemView(StreamItem *i, CPoint *pt, bool redraw);
//	MFCEditorItemView		*RefreshControllerView(ctrl_t t, UINT py);

	bool					AddAllItemViews(bool redraw);

	DECLARE_MESSAGE_MAP()
};
