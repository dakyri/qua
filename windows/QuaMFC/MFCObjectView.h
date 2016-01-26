#pragma once

#include "inx/QuaDisplay.h"

class MFCDataEditor;
class MFCStreamDataEditor;
class MFCSampleDataEditor;
class MFCClipListView;
class MFCTakeListView;
class MFCBlockEdit;
class MFCExecutableBlockEdit;
class MFCEventBlockEdit;
class MFCSimpleTypeView;
class MFCQuaStateDisplay;
class MFCQuaParseStateDisplay;
class MFCInstanceObjectView;
class MFCStackFrameView;
class MFCStreamEditorYScale;
class MFCSmallQuaClsBut;
class MFCSmallQuaMinBut;

struct frame_map_hdr;

#include "DString.h"
#include "DaRect.h"

#include "MFCQuaIntCtrl.h"
#include "MFCQuaFloatCtrl.h"
#include "MFCQuaTimeCtrl.h"
#include "MFCQuaRotor.h"
//#include "MFCQuaChannelController.h"
#include "MFCQuaListBox.h"
#include "MFCQuaButton.h"

class MFCStackFrameView;
class MFCObjectView;
class MFCBlockEditCtrlStrip;

#include "inx/BaseVal.h"

class QuaPopupWnd : public CWnd
{
public:
	QuaPopupWnd();           // protected constructor used by dynamic creation
	virtual				~QuaPopupWnd();
	DECLARE_DYNCREATE(QuaPopupWnd)

	long				CreatePopup(char * nm, long x, long y, long w, long h, HWND parent);

public:

protected:
	DECLARE_MESSAGE_MAP()
};


class MFCQCR:  public QuaControllerRepresentation
{
public:
	MFCQCR();
	~MFCQCR();
//	afx_msg int					OnCreate(LPCREATESTRUCT lpCreateStruct );
//	afx_msg void				OnMove(int x, int y);
//	afx_msg void				OnSize(UINT nType, int cx, int cy);
//	afx_msg void				OnSizing( UINT, LPRECT );

	virtual QuaControllerRepresentation*
						CreateControllerRepresentation(BRect &r, MFCStackFrameView *, StabEnt *, ulong);

	BRect						labelFrame;
};

class MFCQuaTimeController: public MFCQCR, public MFCQuaTimeCtrl
{
public:
	MFCQuaTimeController();
	~MFCQuaTimeController();

	virtual void				UpdateDisplay(TypedValue &);
	virtual void				SetSymValue();
	virtual TypedValue			Value();

	static CFont				displayFont;
	virtual QuaControllerRepresentation*
						CreateControllerRepresentation(BRect &r, MFCStackFrameView *, StabEnt *, uint32);
protected:
	DECLARE_MESSAGE_MAP()
};

class MFCQuaClipController: public MFCQCR, public MFCQuaSymListBox
{
public:
	MFCQuaClipController();
	~MFCQuaClipController();

	virtual void				UpdateDisplay(TypedValue &);
	virtual void				SetSymValue();
	virtual TypedValue			Value();

	bool						SetSelectorValues();

	static CFont				displayFont;

	virtual QuaControllerRepresentation*
						CreateControllerRepresentation(BRect &r, MFCStackFrameView *, StabEnt *, uint32);
protected:
	DECLARE_MESSAGE_MAP()
};

class MFCQuaIntController: public MFCQCR, public MFCQuaIntCtrl
{
public:
	MFCQuaIntController();
	~MFCQuaIntController();

	virtual void				UpdateDisplay(TypedValue &);
	virtual void				SetSymValue();
	virtual TypedValue			Value();

	static CFont				displayFont;
	virtual QuaControllerRepresentation*
						CreateControllerRepresentation(BRect &r, MFCStackFrameView *, StabEnt *, uint32);
protected:
	DECLARE_MESSAGE_MAP()
};


class MFCQuaRealController: public MFCQCR, public MFCQuaRotor
{
public:
	MFCQuaRealController();
	~MFCQuaRealController();

	virtual void				UpdateDisplay(TypedValue &);
	virtual void				SetSymValue();
	virtual TypedValue			Value();

	virtual QuaControllerRepresentation*
						CreateControllerRepresentation(BRect &r, MFCStackFrameView *, StabEnt *, uint32);
protected:
	DECLARE_MESSAGE_MAP()
};

class MFCQuaChannelController: public MFCQCR, public MFCQuaSymListBox
{
public:
	MFCQuaChannelController();
	~MFCQuaChannelController();

	virtual void				UpdateDisplay(TypedValue &);
	virtual void				SetSymValue();
	virtual TypedValue			Value();

	virtual QuaControllerRepresentation*
						CreateControllerRepresentation(BRect &r, MFCStackFrameView *, StabEnt *, uint32);

	bool						SetSelectorValues(StabEnt *ds);
	static CFont				displayFont;
protected:
	DECLARE_MESSAGE_MAP()
};

class MFCQuaVstProgramController: public MFCQCR, public MFCQuaListBox
{
public:
	MFCQuaVstProgramController();
	~MFCQuaVstProgramController();

	virtual void				UpdateDisplay(TypedValue &);
	virtual void				SetSymValue();
	virtual TypedValue			Value();

	virtual QuaControllerRepresentation*
						CreateControllerRepresentation(BRect &r, MFCStackFrameView *, StabEnt *, uint32);

	bool						SetSelectorValues();
	static CFont				displayFont;
protected:
	DECLARE_MESSAGE_MAP()
};

class MFCQuaBoolController: public MFCQCR, public MFCSmallQuaChkBut

{
public:
	MFCQuaBoolController();
	~MFCQuaBoolController();

	virtual void				UpdateDisplay(TypedValue &);
	virtual void				SetSymValue();
	virtual TypedValue			Value();

	virtual QuaControllerRepresentation*
						CreateControllerRepresentation(BRect &r, MFCStackFrameView *, StabEnt *, uint32);

	static CFont				displayFont;
protected:
	DECLARE_MESSAGE_MAP()
};



// MFCQuaObjectView view
class MFCStackFrameView: public CWnd, public QuaFrameRepresentation
{
	DECLARE_DYNCREATE(MFCStackFrameView)
public:
	MFCStackFrameView();
	virtual ~MFCStackFrameView();

public:
#ifdef Q_FRAME_MAP
	bool						CreateFrameView(
									CRect &r,
									char *lbl,
									CWnd *pWindow,
									QuaInstanceObjectRepresentation *root,
									MFCStackFrameView *parent,
									QuasiStack *,
									frame_map_hdr * map, bool);
	virtual void				RefreshFrame(frame_map_hdr *, bool);
	virtual void				PopFrame(frame_map_hdr *, bool);
#else
	bool						CreateFrameView(
									CRect &r,
									char *lbl,
									CWnd *pWindow,
									QuaInstanceObjectRepresentation *root,
									MFCStackFrameView *parent,
									QuasiStack *stack, bool);
	virtual void				RefreshFrame(QuasiStack *, bool);
	virtual void				PopFrame(QuasiStack *, bool);
#endif
	virtual void				UpdateClipIndexDisplay();
	virtual void				UpdateChannelIndexDisplay(StabEnt *ds);

	CRect						wbounds;
	CRect						wframe;
	static CFont				displayFont;
	DString						label;

	MFCQuaChkButton				*eButton;
	QuaPopupWnd					*popupEdit;
	QuaPopupWnd					*popupInfo;

	virtual void				ArrangeChildren();

	void						SetWidth(long cx);

	afx_msg void				OnSize(UINT nType, int cx, int cy);
	afx_msg void				OnSizing( UINT, LPRECT );
	afx_msg int					OnCreate(LPCREATESTRUCT lpCreateStruct );
	afx_msg void				OnMove(int x, int y);
	afx_msg void				OnPaint();
	afx_msg LRESULT				OnQuaCtlChanged(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT				OnQuaCtlBeginMove(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT				OnQuaCtlEndMove(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT				OnQuaExtEd(WPARAM wparam, LPARAM lparam);
	afx_msg void				OnParentNotify(UINT message, LPARAM lParam);
	virtual void				HideFrame();

	virtual void				AddChildFrame(QuasiStack *);
	virtual void				AddController(StabEnt *);

protected:
	DECLARE_MESSAGE_MAP()
};

class ObjectNameView: public CStatic
{
public:
//	DECLARE_DYNCREATE(ObjectNameView)
	ObjectNameView();
	BOOL				CreateObjectNameView(char *, CRect &, CWnd *, CFont *);
	virtual void		DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void		OnSize(UINT nType, int cx, int cy);
	void				SetTo(char *);
	CFont				*font;
	CRect				bounds;

protected:
	DECLARE_MESSAGE_MAP()
};

class MFCObjectView : public CView
{
public:
	MFCObjectView();           // protected constructor used by dynamic creation
	virtual ~MFCObjectView();
//	DECLARE_DYNCREATE(MFCObjectView)

	CRect						bounds;
	CRect						frame;
	static CFont				displayFont;

	ObjectNameView				nameView;

	void						SetBGColor(COLORREF bg);
	COLORREF					bgColor;

	virtual void				ArrangeChildren();
	virtual StabEnt				*Symbol()=NULL;
	virtual QuaPerceptualSet	*QuaLink()=NULL;
//	virtual void				ReSized(int, int)=NULL;

	static MFCObjectView		*ToObjectView(QuaObjectRepresentation *);
	class MFCObjectMountView	*ObjectMountView();
	class MFCObjectView			*ParentObjectView();

	bool						CompileBlock(QuaPerceptualSet *, MFCBlockEdit *, MFCBlockEditCtrlStrip *, char *);
	bool						ParseBlock(QuaPerceptualSet *, MFCBlockEdit *, MFCBlockEditCtrlStrip *, char *);
	bool						RevertBlock(QuaPerceptualSet *, MFCBlockEdit *, MFCBlockEditCtrlStrip *, char *);

	void						SetWidth(long cx);
public:
	afx_msg void				OnSize(UINT nType, int cx, int cy);
	afx_msg void				OnSizing( UINT, LPRECT );
	afx_msg int					OnCreate(LPCREATESTRUCT lpCreateStruct );
	afx_msg void				OnMove(int x, int y);
	afx_msg	void				OnPaint();

	MFCSmallQuaMinBut			*minimizeButton;
protected:

public:
	virtual void				OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void				AssertValid() const;
	virtual void				Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
};



class MFCInstanceObjectView : public MFCObjectView, public QuaInstanceObjectRepresentation
{
public:
	MFCInstanceObjectView();           // protected constructor used by dynamic creation
	virtual						~MFCInstanceObjectView();
	DECLARE_DYNCREATE(MFCInstanceObjectView)

	virtual void				ArrangeChildren();
	virtual StabEnt				*Symbol();
	virtual QuaPerceptualSet	*QuaLink();
	virtual void				ChildPopulate(StabEnt *sym);
	virtual void				AttributePopulate();
	virtual QuaObjectRepresentation	*AddChildRepresentation(StabEnt *c);
//	virtual void				ReSized(int, int);
	virtual void				SetName();
	virtual void				ChildNameChanged(StabEnt *);

#ifdef Q_FRAME_MAP
	virtual void				PopFrameRepresentation(StabEnt *, QuasiStack *, QuasiStack *, frame_map_hdr *, bool);
	virtual void				RefreshFrameRepresentation(StabEnt *, QuasiStack *, frame_map_hdr *);
#else
	virtual void				PopFrameRepresentation(StabEnt *, QuasiStack *, QuasiStack *,bool);
	virtual void				RefreshFrameRepresentation(StabEnt *, QuasiStack *);
#endif
	virtual void				HideFrameRepresentation(StabEnt *, QuasiStack *);
	virtual void				DeleteFrameRepresentation(StabEnt *, QuasiStack *);

	virtual void				UpdateControllerDisplay(StabEnt *, QuasiStack *, StabEnt *);
public:
// channel
// start time
// duration
// control panels for
//		main block
//		event handlers
//		lower stack frames
	afx_msg int					OnCreate(LPCREATESTRUCT lpCreateStruct );
protected:

public:
	virtual void				OnDraw(CDC* pDC);      // overridden to draw this view

protected:
	DECLARE_MESSAGE_MAP()
};

class MFCChannelObjectView : public MFCObjectView, public QuaChannelObjectRepresentation
{
public:
	MFCChannelObjectView();           // protected constructor used by dynamic creation
	virtual						~MFCChannelObjectView();
	DECLARE_DYNCREATE(MFCChannelObjectView)

	virtual void				ArrangeChildren();
	virtual StabEnt				*Symbol();
	virtual QuaPerceptualSet	*QuaLink();
	virtual void				ChildPopulate(StabEnt *sym);
	virtual void				AttributePopulate();
	virtual QuaObjectRepresentation	*AddChildRepresentation(StabEnt *c);
//	virtual void				ReSized(int, int);
	virtual void				SetName();
	virtual void				ChildNameChanged(StabEnt *);

	virtual void				UpdateControllerDisplay(StabEnt *, QuasiStack *, StabEnt *);
	virtual void				UpdateVariableIndexDisplay();

	MFCSimpleTypeView			*variableView;

#ifdef Q_FRAME_MAP
	virtual void				PopFrameRepresentation(StabEnt *, QuasiStack *, QuasiStack *, frame_map_hdr *, bool);
	virtual void				RefreshFrameRepresentation(StabEnt *, QuasiStack *, frame_map_hdr *);
	virtual void				HideFrameRepresentation(StabEnt *, QuasiStack *);
	virtual void				DeleteFrameRepresentation(StabEnt *, QuasiStack *);
#else
	virtual void				PopFrameRepresentation(StabEnt *, QuasiStack *, QuasiStack *, bool);
	virtual void				RefreshFrameRepresentation(StabEnt *, QuasiStack *);
	virtual void				HideFrameRepresentation(StabEnt *, QuasiStack *);
	virtual void				DeleteFrameRepresentation(StabEnt *, QuasiStack *);
#endif

public:
// inputs
// outputs
// event handlers
	MFCExecutableBlockEdit		*txBlockEdit;
	MFCBlockEditCtrlStrip		*txBlkCtl;
	MFCEventBlockEdit			*rxBlockEdit;
	MFCBlockEditCtrlStrip		*rxBlkCtl;
//  child methods
//  top level child data
// control panels for
//		main block
//		event handlers
//		lower stack frames
	afx_msg int					OnCreate(LPCREATESTRUCT lpCreateStruct );
	afx_msg LRESULT				OnQuaBlockParse(WPARAM, LPARAM);
	afx_msg LRESULT				OnQuaBlockCompile(WPARAM, LPARAM);
	afx_msg LRESULT				OnQuaBlockRevert(WPARAM, LPARAM);

protected:

public:
	virtual void				OnDraw(CDC* pDC);      // overridden to draw this view

	inline bool					RemIOV(MFCInstanceObjectView *iov)
			{ return instOViews.RemoveItem(iov); }
	inline long					AddIOV(MFCInstanceObjectView *iov)
			{ return instOViews.AddItem(iov); }
	inline MFCInstanceObjectView *IOV(long i)
			{ return (MFCInstanceObjectView *) instOViews.ItemAt(i); }
	inline long					CountIOV() { return instOViews.CountItems(); }
	BList						instOViews;

protected:
	DECLARE_MESSAGE_MAP()
};


class CDView: public CWnd
{
public:
	CDView() { }
	void OnDraw(CDC *p) { }
};

class MFCSampleObjectView : public MFCObjectView, public QuaSampleObjectRepresentation
{
public:
	MFCSampleObjectView();           // protected constructor used by dynamic creation
	virtual						~MFCSampleObjectView();
	DECLARE_DYNCREATE(MFCSampleObjectView)

// take data (samples)
// clip data (relates to the take data)
// main block
// event handlers
//		receive
//		wake
//		sleep
//		start
//		stop
//		record
//		cue
//		init
//  child methods
//  top level child data
	MFCSampleDataEditor			*dataEditor;
	MFCClipListView				*clipsView;
	MFCSimpleTypeView			*variableView;
	MFCExecutableBlockEdit		*mainBlockEdit;
	MFCBlockEditCtrlStrip		*mainBlkCtl;
	MFCEventBlockEdit			*rxBlockEdit;
	MFCBlockEditCtrlStrip		*rxBlkCtl;

	afx_msg int					OnCreate(LPCREATESTRUCT lpCreateStruct );
	afx_msg void				OnDestroy();
	afx_msg LRESULT				OnQuaBlockParse(WPARAM, LPARAM);
	afx_msg LRESULT				OnQuaBlockCompile(WPARAM, LPARAM);
	afx_msg LRESULT				OnQuaBlockRevert(WPARAM, LPARAM);
	afx_msg LRESULT				OnQuaCloseView(WPARAM, LPARAM);
	afx_msg LRESULT				OnQuaMinView(WPARAM, LPARAM);
	afx_msg LRESULT				OnQuaMaxView(WPARAM, LPARAM);

	virtual void				ArrangeChildren();
	virtual StabEnt				*Symbol();
	virtual QuaPerceptualSet	*QuaLink();
	virtual void				ChildPopulate(StabEnt *sym);
	virtual void				AttributePopulate();
	virtual QuaObjectRepresentation	*AddChildRepresentation(StabEnt *c);
//	virtual void				ReSized(int, int);
	virtual void				SetName();
	virtual void				ChildNameChanged(StabEnt *);

	virtual void				UpdateControllerDisplay(StabEnt *, QuasiStack *, StabEnt *);
	virtual void				UpdateClipIndexDisplay();
	virtual void				UpdateTakeIndexDisplay();
	virtual void				UpdateVariableIndexDisplay();
public:

protected:

public:
	virtual void				OnDraw(CDC* pDC);      // overridden to draw this view

protected:
	DECLARE_MESSAGE_MAP()
};

class MFCVoiceObjectView : public MFCObjectView, public QuaVoiceObjectRepresentation
{
public:
	MFCVoiceObjectView();           // protected constructor used by dynamic creation
	virtual						~MFCVoiceObjectView();
	DECLARE_DYNCREATE(MFCVoiceObjectView)

// take data (streams)
// clip data (relates to the take data)
// main block
// event handlers
//		receive
//		wake
//		sleep
//		start
//		stop
//		record
//		cue
//		init
//  child methods
//  top level child data
	CWnd						keyboardRack;
	MFCStreamEditorYScale		*yscale;
	MFCStreamDataEditor			*dataEditor;
	MFCClipListView				*clipsView;
	MFCSimpleTypeView			*variableView;
	MFCExecutableBlockEdit		*mainBlockEdit;
	MFCBlockEditCtrlStrip		*mainBlkCtl;
	MFCEventBlockEdit			*rxBlockEdit;
	MFCBlockEditCtrlStrip		*rxBlkCtl;

	virtual void				ArrangeChildren();
	virtual StabEnt				*Symbol();
	virtual QuaPerceptualSet	*QuaLink();
	virtual void				ChildPopulate(StabEnt *sym);
	virtual void				AttributePopulate();
	virtual QuaObjectRepresentation	*AddChildRepresentation(StabEnt *c);
//	virtual void				ReSized(int, int);
	virtual void				SetName();
	virtual void				ChildNameChanged(StabEnt *);

	virtual void				UpdateControllerDisplay(StabEnt *, QuasiStack *, StabEnt *);
	virtual void				UpdateVariableIndexDisplay();
	virtual void				UpdateClipIndexDisplay();
	virtual void				UpdateTakeIndexDisplay();
public:
	afx_msg int					OnCreate(LPCREATESTRUCT lpCreateStruct );
	afx_msg void				OnDestroy();
	afx_msg LRESULT				OnQuaBlockParse(WPARAM, LPARAM);
	afx_msg LRESULT				OnQuaBlockCompile(WPARAM, LPARAM);
	afx_msg LRESULT				OnQuaBlockRevert(WPARAM, LPARAM);
	afx_msg LRESULT				OnQuaCtlChanged(WPARAM, LPARAM);
	afx_msg LRESULT				OnQuaCloseView(WPARAM, LPARAM);
	afx_msg LRESULT				OnQuaMinView(WPARAM, LPARAM);
	afx_msg LRESULT				OnQuaMaxView(WPARAM, LPARAM);
protected:

public:
	virtual void				OnDraw(CDC* pDC);      // overridden to draw this view

	inline bool					RemIOV(MFCInstanceObjectView *iov)
			{ return instOViews.RemoveItem(iov); }
	inline long					AddIOV(MFCInstanceObjectView *iov)
			{ return instOViews.AddItem(iov); }
	inline MFCInstanceObjectView *IOV(long i)
			{ return (MFCInstanceObjectView *) instOViews.ItemAt(i); }
	inline long					CountIOV() { return instOViews.CountItems(); }
	BList						instOViews;

protected:
	DECLARE_MESSAGE_MAP()
};

class MFCMethodObjectView : public MFCObjectView, public QuaMethodObjectRepresentation
{
public:
	MFCMethodObjectView();           // protected constructor used by dynamic creation
	virtual						~MFCMethodObjectView();
	DECLARE_DYNCREATE(MFCMethodObjectView)

	virtual void				ArrangeChildren();
	virtual StabEnt				*Symbol();
	virtual QuaPerceptualSet	*QuaLink();
	virtual void				ChildPopulate(StabEnt *sym);
	virtual void				AttributePopulate();
	virtual QuaObjectRepresentation	*AddChildRepresentation(StabEnt *c);
//	virtual void				ReSized(int, int);
	virtual void				SetName();
	virtual void				ChildNameChanged(StabEnt *);

	virtual void				UpdateControllerDisplay(StabEnt *, QuasiStack *, StabEnt *);
	virtual void				UpdateVariableIndexDisplay();

	MFCBlockEditCtrlStrip		*mainBlkCtl;
	MFCExecutableBlockEdit		*mainBlockEdit;
	MFCSimpleTypeView			*variableView;
public:
// take data (streams)
// clip data (relates to the take data)
// main block
// event handlers
//		receive
//		wake
//		sleep
//		start
//		stop
//		record
//		cue
//		init
//  child methods
//  top level child data
	afx_msg void				OnDestroy();
	afx_msg int					OnCreate(LPCREATESTRUCT lpCreateStruct );
	afx_msg LRESULT				OnQuaBlockParse(WPARAM, LPARAM);
	afx_msg LRESULT				OnQuaBlockCompile(WPARAM, LPARAM);
	afx_msg LRESULT				OnQuaBlockRevert(WPARAM, LPARAM);
	afx_msg LRESULT				OnQuaCloseView(WPARAM, LPARAM);
	afx_msg LRESULT				OnQuaMinView(WPARAM, LPARAM);
	afx_msg LRESULT				OnQuaMaxView(WPARAM, LPARAM);
protected:

public:
	virtual void				OnDraw(CDC* pDC);      // overridden to draw this view

protected:
	DECLARE_MESSAGE_MAP()
};

#define QCID_OV_RXBLOCK	1
#define QCID_OV_TXBLOCK	2
#define QCID_OV_MAINBLOCK	3
#define QCID_OV_WAKEBLOCK	4
#define QCID_OV_SLEEPBLOCK	5
#define QCID_OV_STARTBLOCK	6
#define QCID_OV_STOPBLOCK	7
#define QCID_OV_RECORDBLOCK	8
#define QCID_OV_CUEBLOCK	9
#define QCID_OV_INITBLOCK	10
#define QCID_OV_SAMPLE_ED	11
#define QCID_OV_VOICE_ED	12
#define QCID_OV_METHOD_ED	13
#define QCID_OV_CHANNEL_ED	14
#define QCID_OV_INSTANCE_CT	15
