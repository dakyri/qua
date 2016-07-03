
// MFCQuaObjectView.cpp : implementation file
//
// a lot of the fixed bits could/should be changed from being being dynamically allocated.
// however this has caused problems with 
//#define _AFXDLL
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE
#include "stdafx.h"
#include "qua_version.h"

#include "StdDefs.h"
#include "Colors.h"

#include "QuaMFC.h"
#include "MFCObjectView.h"
#include "MFCStreamDataEditor.h"
#include "MFCSampleDataEditor.h"
#include "MFCClipListView.h"
#include "MFCTakeListView.h"
#include "MFCBlockEdit.h"
#include "MFCSimpleTypeView.h"
#include "MFCQuaStateDisplay.h"
#include "MFCQuaButton.h"
#include "MFCQuaMessageId.h"
#include "MFCObjectMountView.h"
//#include "MFCQuaClipController.h"
//#include "MFCQuaChannelController.h"

#include "Sym.h"
#include "Clip.h"
#include "Voice.h"
#include "Sample.h"
#include "Lambda.h"
#include "Channel.h"
#include "VstPlugin.h"
#include "Block.h"
#include "Parse.h"

CFont	MFCObjectView::displayFont;
CFont	MFCStackFrameView::displayFont;
CFont	MFCQuaClipController::displayFont;
CFont	MFCQuaChannelController::displayFont;
CFont	MFCQuaVstProgramController::displayFont;
CFont	MFCQuaTimeController::displayFont;
CFont	MFCQuaIntController::displayFont;

#define INI_CHILDVIEW_HEIGHT	60
#define CHILDVIEW_MARGIN	2


#define OV_MARGIN	4
#define CLIP_POSX	OV_MARGIN
#define CLIP_POSY	17
//#define TAKE_POSX	104
//#define TAKE_POSY	17
#define EDIT_POSX	105
#define EDIT_POSY	OV_MARGIN
#define ROW1_BOT	100
#define ROW2_TOP	105
#define SIMPLE_T_POSX	OV_MARGIN
#define SIMPLE_T_POSY	ROW2_TOP
#define BLOCK_POSX	170
#define BLOCK_POSY	ROW2_TOP
#define BLOCK_STATE_H	12
#define ROW2_BOT	200
#define STREAMED_YSCALE_WID	15
#define CHANNEL_T_POSY	17
#define CHROW_BOT	112





IMPLEMENT_DYNCREATE(QuaPopupWnd, CWnd)

BEGIN_MESSAGE_MAP(QuaPopupWnd, CWnd)
END_MESSAGE_MAP()

QuaPopupWnd::QuaPopupWnd()
{
	;
}

QuaPopupWnd::~QuaPopupWnd()
{
	;
}
long
QuaPopupWnd::CreatePopup(char *nm, long x, long y, long w, long h, HWND parent)
{
	int capH = GetSystemMetrics(SM_CYCAPTION);
	return CreateEx(
		WS_EX_LEFT,
		quaPopupClassName,
		nm,
		WS_POPUP|WS_BORDER|WS_VISIBLE
			|WS_CAPTION
//			|WS_SYSMENU|WS_MINIMIZEBOX
			,
		x,y,w,h+capH,
		parent,
		NULL);
}

// MFCQuaObjectView

IMPLEMENT_DYNCREATE(MFCObjectView, CView)

MFCObjectView::MFCObjectView()
{
	SetBGColor(rgb_blue);

	minimizeButton = NULL;
}

MFCObjectView::~MFCObjectView()
{
}

BEGIN_MESSAGE_MAP(MFCObjectView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_MOVE()
END_MESSAGE_MAP()

void
MFCObjectView::SetBGColor(COLORREF bg)
{
	bgColor = bg;
}

MFCObjectView *
MFCObjectView::ToObjectView(QuaObjectRepresentation *or)
{
	if (or == NULL || or->symbol == NULL) {
		return NULL;
	}
	switch (or->symbol->type) {
		case TypedValue::S_VOICE: return (MFCVoiceObjectView *)or;
		case TypedValue::S_SAMPLE: return (MFCSampleObjectView *)or;
		case TypedValue::S_CHANNEL: return (MFCChannelObjectView *)or;
		case TypedValue::S_LAMBDA: return (MFCMethodObjectView *)or;
		case TypedValue::S_INSTANCE: return (MFCInstanceObjectView *)or;
	}
	return NULL;
}

// MFCObjectView drawing

void
MFCObjectView::OnDraw(CDC* pdc)
{
	RaisedBox(pdc, &bounds, bgColor, true);
}

// MFCObjectView diagnostics

#ifdef _DEBUG
void MFCObjectView::AssertValid() const
{
//	fprintf(stderr, "obj view valid\n");
	CView::AssertValid();
}

void MFCObjectView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG
MFCObjectMountView *
MFCObjectView::ObjectMountView()
{
	CWnd	*parent = GetParent();
	char	buf[40];
	while (parent != NULL) {
		parent->GetWindowText(buf, 40);
		if (strcmp(buf, "Qua Object View") != 0) {
			break;
		}
		parent = parent->GetParent();
	}
	if (parent) {
		parent->GetWindowText(buf, 40);
		if (strcmp(buf, "Qua Object Mount Rack") == 0) {
			parent = parent->GetParent();
		}
		return (MFCObjectMountView*)parent;
	}
	return NULL;
}



MFCObjectView *
MFCObjectView::ParentObjectView()
{
	CWnd	*parent = GetParent();
	if (parent) {
		char	buf[40];
		parent->GetWindowText(buf, 40);
		if (strcmp(buf, "ObjectView") == 0) {
			return (MFCObjectView *)parent;
		}
	}

	return NULL;
}

// MFCObjectView message handlers

afx_msg void
MFCObjectView::OnMove(int x, int y)
{
	fprintf(stderr, "MFCObjectView::OnMove() %d %d\n", x, y);
	frame.MoveToXY(x,y);
}

void
MFCObjectView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	if (bounds.right != cx) {
		SetWidth(cx);
	}
	bounds.bottom = cy;
	frame.right = frame.left+cx;
	frame.bottom = frame.top+cy;

	fprintf(stderr, "MFCObjectView::OnSize() %d %d, frame %d %d %d %d\n", cx, cy, frame.left, frame.top, frame.right, frame.bottom);
}

void
MFCObjectView::OnSizing( UINT which, LPRECT r)
{
	CView::OnSizing(which, r);
}

bool
MFCObjectView::CompileBlock(QuaPerceptualSet *quaLink, MFCBlockEdit *bed, MFCBlockEditCtrlStrip *ctl, char *blockName)
{
	if (quaLink && bed && bed->symbol && ctl && ctl->state.state == Q_PARSE_DEFER) {
		long len = bed->GetWindowTextLength();
		char	*buf = new char[len+1];
		bed->GetWindowText(buf, len+1);
		buf[len] = 0;
		fprintf(stderr, "%s<%s> len %d\n", blockName, buf, len);
		char nm[256];
		sprintf(nm, "%s %s", bed->symbol->UniqueName(), blockName);
		long state = quaLink->CompileBlock(bed->symbol, nm, buf, len);
		ctl->SetState(state);
		delete buf;
		return true;
	}
	return false;
}

bool
MFCObjectView::ParseBlock(QuaPerceptualSet *quaLink, MFCBlockEdit *bed, MFCBlockEditCtrlStrip *ctl, char *blockName)
{
	if (quaLink && bed && bed->symbol && ctl && ctl->state.state == Q_PARSE_DEFER) {
		long len = bed->GetWindowTextLength();
		char	*buf = new char[len+1];
		bed->GetWindowText(buf, len+1);
		buf[len] = 0;
		fprintf(stderr, "%s<%s> len %d\n", blockName, buf, len);
		char nm[256];
		sprintf(nm, "%s %s", bed->symbol->UniqueName(), blockName);
		long state = quaLink->ParseBlock(bed->symbol, nm, buf, len);
		delete buf;
		return true;
	}
	return false;
}

bool
MFCObjectView::RevertBlock(QuaPerceptualSet *quaLink, MFCBlockEdit *bed, MFCBlockEditCtrlStrip *ctl, char *blockName)
{
	if (quaLink && bed && bed->symbol && ctl && ctl->state.state == Q_PARSE_DEFER) {
	}
	return false;
}

BEGIN_MESSAGE_MAP(ObjectNameView, CStatic)
	ON_WM_SIZE()
END_MESSAGE_MAP()

ObjectNameView::ObjectNameView()
{
	;
}

BOOL
ObjectNameView::CreateObjectNameView(char *text, CRect &r, CWnd *w, CFont *f)
{
	BOOL	ret = Create(text, WS_CHILD|WS_VISIBLE|SS_OWNERDRAW, r, w);
	font = f;
	bounds = r;
	bounds.MoveToXY(0,0);

	return ret;
}

void
ObjectNameView::SetTo(char*txt)
{
	SetWindowText(txt);
}

void
ObjectNameView::OnSize(UINT nType, int cx, int cy)
{
	bounds.right = cx;
	bounds.bottom = cy;
}
void
ObjectNameView::DrawItem(LPDRAWITEMSTRUCT lpd)
{
	CDC	pdc;
	char	txt[128];
	GetWindowText(txt, 128);
	pdc.Attach(lpd->hDC);
	pdc.SelectObject(font);
	pdc.SetBkMode(TRANSPARENT);
	pdc.SetTextColor(RGB(0,0,0));
	fprintf(stderr, "text %s, bnd %d %d %d %d\n", txt, bounds.left, bounds.top, bounds.right, bounds.bottom);
	pdc.DrawText(txt, &bounds, DT_VCENTER|DT_LEFT);
}


//	CSize	sz = pdc->GetTextExtent();
//	CRect textRect(1,1,Min(sz.cx, bounds.right-1),15);

int
MFCObjectView::OnCreate(LPCREATESTRUCT lpCreateStruct )
{
	bounds.left = 0;
	bounds.top = 0;
	bounds.right = lpCreateStruct->cx;
	bounds.bottom = lpCreateStruct->cy;
	frame.left = lpCreateStruct->x;
	frame.top = lpCreateStruct->y;
	frame.right = frame.left+bounds.right; 
	frame.bottom = frame.top+bounds.bottom; 

	nameView.CreateObjectNameView("object view", CRect(1,1,2,2), this, &displayFont);

	return CView::OnCreate(lpCreateStruct);
}

void
MFCObjectView::ArrangeChildren()
{
	;
}

void
MFCObjectView::SetWidth(long cx)
{
	bounds.right = cx;
	ArrangeChildren();
}

/////////////////////////////////////////////////////////
// MFCChannelObjectView
////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(MFCChannelObjectView, MFCObjectView)

MFCChannelObjectView::MFCChannelObjectView()
{
	SetBGColor(rgb_orange);

	variableView = NULL;
	rxBlockEdit = NULL;
	txBlockEdit = NULL;
	rxBlkCtl = NULL;
	txBlkCtl = NULL;
}

MFCChannelObjectView::~MFCChannelObjectView()
{
}


void
MFCChannelObjectView::OnDraw(CDC* pdc)
{
	RaisedBox(pdc, &bounds, bgColor, true);
}

int
MFCChannelObjectView::OnCreate(LPCREATESTRUCT lpCreateStruct )
{
	int ret = MFCObjectView::OnCreate(lpCreateStruct);

	minimizeButton = new MFCSmallQuaMinBut;
	minimizeButton->CreateButton(NULL, CRect(1,1,12, 12), this, 6969);
	minimizeButton->SetLCMsgParams(QM_MINIMIZE_VIEW,NULL,NULL);

	nameView.SetWindowPos(&wndTop, minimizeButton->bounds.right+OV_MARGIN, 1, EDIT_POSX-2*OV_MARGIN-minimizeButton->bounds.right, 15, NULL);
	SetName();

	CRect		r;

	variableView = new MFCSimpleTypeView;
	variableView->CreateList(
		CRect(SIMPLE_T_POSX, CHANNEL_T_POSY,BLOCK_POSX-OV_MARGIN, CHROW_BOT),
		this, 6969);

	long	cx = (bounds.right - BLOCK_POSX - 2*OV_MARGIN)/2;
	rxBlkCtl = new MFCBlockEditCtrlStrip;
	rxBlkCtl->CreateCtrlStrip(
			"Input", CRect(BLOCK_POSX,CHANNEL_T_POSY, BLOCK_POSX+cx, CHANNEL_T_POSY+BLOCK_STATE_H+1),
			this, 6969);
	rxBlockEdit = new MFCEventBlockEdit;
	rxBlockEdit->CreateEditor(
			CRect(BLOCK_POSX,CHANNEL_T_POSY+BLOCK_STATE_H+1, BLOCK_POSX+cx, CHROW_BOT),
			this,
			6969,
			&rxBlkCtl->state,
			NULL
			);
	rxBlockEdit->SetPMsgParams(QM_BLOCK_PARSE, QCID_OV_RXBLOCK, (LPARAM)rxBlockEdit);
	rxBlockEdit->SetCMsgParams(QM_BLOCK_COMPILE, QCID_OV_RXBLOCK, (LPARAM)rxBlockEdit);
	rxBlkCtl->SetMessageParams(rxBlockEdit, QCID_OV_RXBLOCK);

	txBlkCtl = new MFCBlockEditCtrlStrip;
	txBlkCtl->CreateCtrlStrip(
			"Output",
			CRect(BLOCK_POSX+cx+OV_MARGIN,CHANNEL_T_POSY, bounds.right-OV_MARGIN, CHANNEL_T_POSY+BLOCK_STATE_H+1),
			this, 6969);
	txBlockEdit = new MFCExecutableBlockEdit;
	txBlockEdit->CreateEditor(
		CRect(BLOCK_POSX+cx+OV_MARGIN,CHANNEL_T_POSY+BLOCK_STATE_H+1, bounds.right-OV_MARGIN, CHROW_BOT),
		this, 6969, &txBlkCtl->state,NULL);
	txBlockEdit->SetPMsgParams(QM_BLOCK_PARSE, QCID_OV_TXBLOCK, (LPARAM)txBlockEdit);
	txBlockEdit->SetCMsgParams(QM_BLOCK_COMPILE, QCID_OV_TXBLOCK, (LPARAM)txBlockEdit);
	txBlkCtl->SetMessageParams(txBlockEdit, QCID_OV_TXBLOCK);

	Channel	*c = symbol->ChannelValue();
	if (c) {
		if (c->rx.block) {
			quaLink->RequestPopFrameRepresentation(
				this, c->rx.sym, NULL, NULL, true, true);
		}
		if (c->tx.block) {
			fprintf(stderr, "tx block add to channel\n");
			quaLink->RequestPopFrameRepresentation(
				this, c->tx.sym, NULL, NULL, true, true);
		}
	}

	ArrangeChildren();
	return ret;
}


void
MFCChannelObjectView::ArrangeChildren()
{
	fprintf(stderr, "arrange sample children\n");

	variableView->SetWindowPos(&wndTop, SIMPLE_T_POSX, CHANNEL_T_POSY, 0, 0, SWP_NOSIZE);

	long	cx = (bounds.right - BLOCK_POSX- 2 *OV_MARGIN)/2;
	rxBlkCtl->SetWindowPos(&wndTop, BLOCK_POSX,CHANNEL_T_POSY, 0, 0, SWP_NOSIZE);
	rxBlockEdit->SetWindowPos(&wndTop, BLOCK_POSX,CHANNEL_T_POSY+BLOCK_STATE_H+1, cx, CHROW_BOT-CHANNEL_T_POSY-BLOCK_STATE_H-1, NULL);
	txBlkCtl->SetWindowPos(&wndTop, BLOCK_POSX+cx+OV_MARGIN, CHANNEL_T_POSY, 0, 0, SWP_NOSIZE);
	txBlockEdit->SetWindowPos(&wndTop, BLOCK_POSX+cx+OV_MARGIN, CHANNEL_T_POSY+BLOCK_STATE_H+1, cx, CHROW_BOT-CHANNEL_T_POSY-BLOCK_STATE_H-1, NULL);

	bounds.bottom = CHROW_BOT+OV_MARGIN; // set so calling routine has correct dimensions from here

	for (short i=0; i<NCOR(); i++) {
		QuaObjectRepresentation	*or = COR(i);
		if (or != NULL) {
			MFCObjectView	*ov = ToObjectView(or);
			if (ov->bounds.right != bounds.right-2*CHILDVIEW_MARGIN) {
				ov->SetWidth(bounds.right-2*CHILDVIEW_MARGIN);
			}
			ov->SetWindowPos(&wndTop, 2, bounds.bottom, 0, 0, SWP_NOSIZE);
			bounds.bottom += ov->bounds.bottom;
		}
	}
	for (short i=0; i<NFR(); i++) {
		QuaFrameRepresentation	*or = FR(i);
		if (or != NULL && or->parent == NULL) {
			MFCStackFrameView	*ov = (MFCStackFrameView*)or;
			if (ov->wbounds.right != bounds.right-2*CHILDVIEW_MARGIN) {
				ov->SetWidth(bounds.right-2*CHILDVIEW_MARGIN);
			}
			ov->SetWindowPos(&wndTop, 2, bounds.bottom, 0, 0, SWP_NOSIZE);
			bounds.bottom += ov->wbounds.bottom;
		}
	}

	SetWindowPos(&wndTop, 0, 0,bounds.right, bounds.bottom+CHILDVIEW_MARGIN, SWP_NOMOVE);
}

void
MFCChannelObjectView::ChildPopulate(StabEnt *sym)
{
	if (Symbol() == NULL) {
		fprintf(stderr, "Populate null channel\n");
		return;
	}
	switch (sym->type) {
		case TypedValue::S_CLIP:
//			clipsView->AddItem(sym);
			break;
		case TypedValue::S_TAKE:
//			clipsView->AddItem(sym);
			break;
		case TypedValue::S_EVENT: {
	// set main block for event handler
			if (strcmp(sym->name, "Wake") == 0) {
			} else if (strcmp(sym->name, "Sleep") == 0) {
			} else if (strcmp(sym->name, "Receive") == 0) {
			} else if (strcmp(sym->name, "Cue") == 0) {
			} else if (strcmp(sym->name, "Start") == 0) {
			} else if (strcmp(sym->name, "Stop") == 0) {
			} else if (strcmp(sym->name, "Record") == 0) {
			} else if (strcmp(sym->name, "Init") == 0) {
			} else {
			}
			break;
		}
		case TypedValue::S_LAMBDA:
		default:
			variableView->AddSym(sym);
			break;
	}
}


void
MFCChannelObjectView::AttributePopulate()
{
	if (Symbol() == NULL) {
		fprintf(stderr, "Populate null channel\n");
		return;
	}
	// set main block
	Channel	*channel = Symbol()->ChannelValue();
	if (channel == NULL) {
		fprintf(stderr, "Populate non channel\n");
		return;
	}
	// set main and rx block
	if (txBlockEdit) {
		fprintf(stderr, "set main block sym to %s\n", symbol->UniqueName());
		txBlockEdit->SetSymbol(symbol);
		// block edit to set text value
		txBlockEdit->SetValue(channel->tx.block);
	}
	if (rxBlockEdit) {
		fprintf(stderr, "set rx block sym to %s\n", symbol->UniqueName());
		rxBlockEdit->SetSymbol(symbol);
		// block edit to set text value
		rxBlockEdit->SetValue(channel->rx.block);
	}
}

void
MFCChannelObjectView::SetName()
{
	nameView.SetWindowText(symbol?symbol->UniqueName():"(nul channel)");
	CRect	r;
	nameView.GetWindowRect(&r);
	ScreenToClient(&r);
	InvalidateRect(&r);
	nameView.Invalidate(1);
}

void
MFCChannelObjectView::ChildNameChanged(StabEnt *sym)
{
	if (Symbol() == NULL) {
		fprintf(stderr, "Populate null channel\n");
		return;
	}
	if (Symbol() == sym->context) {
		long ind = variableView->ItemForSym(sym);
		if (ind >= 0) {
			variableView->GetListCtrl().SetItemText(ind, 0, sym->UniqueName());
		}
	}
}

QuaObjectRepresentation *
MFCChannelObjectView::AddChildRepresentation(StabEnt *s)
{
	if (s->type == TypedValue::S_LAMBDA) {
		MFCMethodObjectView *nv = new MFCMethodObjectView;
		nv->SetSymbol(s);
		nv->SetLinkage(quaLink);
		nv->Create(_T("STATIC"), "ObjectView", WS_CHILD | WS_VISIBLE,
				CRect(CHILDVIEW_MARGIN, 0, bounds.right-CHILDVIEW_MARGIN, INI_CHILDVIEW_HEIGHT), this, 1234, NULL);
		nv->Populate();
		AddCOR(nv);
		ArrangeChildren();

		MFCObjectMountView	*orac = ObjectMountView();
		if (orac) {
			orac->ArrangeChildren();
		}
		return nv;
	}
	return NULL;
}

StabEnt *
MFCChannelObjectView::Symbol()
{
	return symbol;
}

QuaPerceptualSet *
MFCChannelObjectView::QuaLink()
{
	return quaLink;
}


afx_msg LRESULT
MFCChannelObjectView::OnQuaBlockParse(WPARAM, LPARAM)
{
	return 0;
}

afx_msg LRESULT
MFCChannelObjectView::OnQuaBlockCompile(WPARAM, LPARAM)
{
	return 0;
}

afx_msg LRESULT
MFCChannelObjectView::OnQuaBlockRevert(WPARAM, LPARAM)
{
	return 0;
}

void
MFCChannelObjectView::UpdateControllerDisplay(StabEnt *stacker, QuasiStack *frame, StabEnt *sym)
{
	;
}


void
MFCChannelObjectView::UpdateVariableIndexDisplay()
{
	if (Symbol() == NULL) {
		fprintf(stderr, "Populate null channel\n");
		return;
	}
	StabEnt	*p = symbol->children;

	vector<StabEnt*> presentTakes;
	while (p != NULL) {
		switch (p->type) {
			case TypedValue::S_EVENT:
			case TypedValue::S_LAMBDA:
			case TypedValue::S_TAKE:
			case TypedValue::S_CLIP:
				break;
			default:{
				variableView->AddSym(p);
				presentTakes.push_back(p);
			}
		}
		p = p->sibling;
	}
	variableView->RemoveSymbolsNotIn(presentTakes);
}

#ifdef Q_FRAME_MAP
void
MFCChannelObjectView::PopFrameRepresentation(StabEnt *, QuasiStack *, QuasiStack *, frame_map_hdr *map, bool add_children)
{
	if (Symbol() == NULL) {
		fprintf(stderr, "Populate null channel\n");
		return;
	}
}
#else
void
MFCChannelObjectView::PopFrameRepresentation(
			StabEnt *sym, QuasiStack *parentStack,
			QuasiStack *stack, bool add_children)
{
	if (Symbol() == NULL) {
		fprintf(stderr, "Populate null channel\n");
		return;
	}

	MFCStackFrameView	*sfv = (MFCStackFrameView *)RepresentationFor(stack);
	if (sfv != NULL) {
		sfv->PopFrame(stack, add_children);
	} else {
		MFCStackFrameView	*pfv = NULL;
		CWnd				*parent = NULL;
		sfv = new MFCStackFrameView;
		if (parentStack != NULL) {
			pfv = (MFCStackFrameView *)RepresentationFor(parentStack);
		} else {
			;
		}
		fprintf(stderr, "ChannelView::PopFrameRepresentation: parent stack %x pfv %x nframe rep %d sym %s\n",
			parentStack, pfv, NFR(), sym?sym->UniqueName():"(nul)");
		sfv->CreateFrameView(bounds, sym?sym->UniqueName():"stack",
			this, this, pfv, stack, add_children);
//		sfv->SetSymbol(s);
//		sfv->SetLinkage(quaLink);
//		sfv->Populate();
//		AddFR(sfv);
		ArrangeChildren();
	}
}
#endif

void
MFCChannelObjectView::RefreshFrameRepresentation(StabEnt *, QuasiStack *)
{
	reportError("refresh unimp");
}

void
MFCChannelObjectView::HideFrameRepresentation(StabEnt *, QuasiStack *)
{
	if (Symbol() == NULL) {
		fprintf(stderr, "Populate null channel\n");
		return;
	}
}

void
MFCChannelObjectView::DeleteFrameRepresentation(StabEnt *sym, QuasiStack *stack)
{
	if (Symbol() == NULL || Symbol() != sym) {
		fprintf(stderr, "Delete frame representation:: null channel\n");
		return;
	}

	MFCStackFrameView	*sfv = (MFCStackFrameView *)RepresentationFor(stack);
	if (sfv != NULL) {
		sfv->DestroyWindow();
		RemFR(sfv);
		ArrangeChildren();
	}
}

BEGIN_MESSAGE_MAP(MFCChannelObjectView, MFCObjectView)
	ON_WM_CREATE()
	ON_MESSAGE(QM_BLOCK_PARSE, OnQuaBlockParse)
	ON_MESSAGE(QM_BLOCK_COMPILE, OnQuaBlockCompile)
	ON_MESSAGE(QM_BLOCK_REVERT, OnQuaBlockRevert)
END_MESSAGE_MAP()

MFCQCR::MFCQCR()
{
	;
}

MFCQCR::~MFCQCR()
{
	;
}

QuaControllerRepresentation *
MFCQCR::CreateControllerRepresentation(BRect &r, MFCStackFrameView *parv, StabEnt *sym, ulong id)
{
	return NULL;
}


/////////////////////////////////////////////
// time controller
/////////////////////////////////////////////
MFCQuaTimeController::MFCQuaTimeController()
{
	;
}

MFCQuaTimeController::~MFCQuaTimeController()
{
	;
}


QuaControllerRepresentation *
MFCQuaTimeController::CreateControllerRepresentation(BRect &r, MFCStackFrameView *parv, StabEnt *sym, uint32 id)
{
	bool	ret=false;
	Set(sym, parv);
	if (symbol) {
		QuaControllerRepresentation	*qvr = this;
		CRect	tr = InnerRect(r, 1);
		ret = CreateTimeCtrl(
				r, parv, id,
				&displayFont, &tr, NULL, &rgb_black, &rgb_red);
		SetVCMsgParams(QM_SYM_CTL_CHANGED, (WPARAM)symbol, (LPARAM)qvr);
//		SetMDMsgParams(QM_SYM_CTL_BEGIN_MOVE, (WPARAM)symbol, (LPARAM)qvr);
//		SetMUMsgParams(QM_SYM_CTL_END_MOVE, (WPARAM)symbol, (LPARAM)qvr);
	}
	return ret?this:NULL;
}

void
MFCQuaTimeController::SetSymValue()
{
	if (symbol != NULL && parent != NULL && parent->frame != NULL) {
		LValue	l;
		ResultValue	r;
		symbol->SetLValue(l, NULL,
			parent->frame->stacker,
			parent->frame->stackerSym,
			parent->frame);
		r = l.CurrentValue();
		UpdateDisplay(r);
	}
}

void
MFCQuaTimeController::UpdateDisplay(TypedValue &v)
{
	if (v.TimeValue() != NULL) {
		Time	t = *v.TimeValue();
		SetTime(t);
	}
}

TypedValue
MFCQuaTimeController::Value()
{
	TypedValue	v;
	v.Set(time.ticks, time.metric);
	return v;
}

BEGIN_MESSAGE_MAP(MFCQuaTimeController, MFCQuaTimeCtrl)
//	ON_WM_CREATE()
//	ON_MESSAGE(QM_BLOCK_PARSE, OnQuaBlockParse)
//	ON_MESSAGE(QM_BLOCK_COMPILE, OnQuaBlockCompile)
END_MESSAGE_MAP()

/////////////////////////////////////////////
// real controller
/////////////////////////////////////////////
MFCQuaRealController::MFCQuaRealController()
{
	;
}

MFCQuaRealController::~MFCQuaRealController()
{
}

QuaControllerRepresentation *
MFCQuaRealController::CreateControllerRepresentation(BRect &r, MFCStackFrameView *parv, StabEnt *sym, uint32 id)
{
	bool	ret=false;
	Set(sym, parv);
	if (symbol) {
		QuaControllerRepresentation	*qvr = this;
		r.Set(2, 2, 14, 14);
		ret = CreateRotor(
			NULL,
			r, parv, id,
			symbol->iniVal.FloatValue(NULL),
			symbol->minVal.FloatValue(NULL),
			symbol->iniVal.FloatValue(NULL),
			symbol->maxVal.FloatValue(NULL),
			30, 330);
		SetVCMsgParams(QM_SYM_CTL_CHANGED, (WPARAM)symbol, (LPARAM)qvr);
		SetMDMsgParams(QM_SYM_CTL_BEGIN_MOVE, (WPARAM)symbol, (LPARAM)qvr);
		SetMUMsgParams(QM_SYM_CTL_END_MOVE, (WPARAM)symbol, (LPARAM)qvr);
	}
	return ret?this:NULL;
}

void
MFCQuaRealController::SetSymValue()
{
	if (symbol != NULL && parent != NULL && parent->frame != NULL) {
		LValue	l;
		ResultValue	r;
		symbol->SetLValue(l, NULL,
			parent->frame->stacker,
			parent->frame->stackerSym,
			parent->frame);
		r = l.CurrentValue();
		UpdateDisplay(r);
	}
}

void
MFCQuaRealController::UpdateDisplay(TypedValue &)
{
	;
}

TypedValue
MFCQuaRealController::Value()
{
	if (symbol->type == TypedValue::S_DOUBLE) {
		return TypedValue::Double(0);
	}
	return TypedValue::Float(0);
}

BEGIN_MESSAGE_MAP(MFCQuaRealController, MFCQuaRotor)
//	ON_WM_CREATE()
//	ON_MESSAGE(QM_BLOCK_PARSE, OnQuaBlockParse)
//	ON_MESSAGE(QM_BLOCK_COMPILE, OnQuaBlockCompile)
END_MESSAGE_MAP()

/////////////////////////////////////////////
// int controller
/////////////////////////////////////////////
MFCQuaIntController::MFCQuaIntController()
{
	;
}

MFCQuaIntController::~MFCQuaIntController()
{
	;
}

QuaControllerRepresentation *
MFCQuaIntController::CreateControllerRepresentation(BRect &r, MFCStackFrameView *parv, StabEnt *sym, uint32 id)
{
	bool	ret=false;
	Set(sym, parv);
	if (symbol) {
		QuaControllerRepresentation	*qvr = this;
		CRect	tr = InnerRect(r, 1);
		ret = CreateIntCtrl(
				r, parv, id,
				&displayFont, &tr, NULL, &rgb_black, &rgb_red);
		SetRange(
			symbol->minVal.IntValue(NULL),
			symbol->iniVal.IntValue(NULL),
			symbol->maxVal.IntValue(NULL));
		SetValue(symbol->iniVal.IntValue(NULL));
		SetVCMsgParams(QM_SYM_CTL_CHANGED, (WPARAM)symbol, (LPARAM)qvr);
//		SetMDMsgParams(QM_SYM_CTL_BEGIN_MOVE, (WPARAM)symbol, (LPARAM)qvr);
//		SetMUMsgParams(QM_SYM_CTL_END_MOVE, (WPARAM)symbol, (LPARAM)qvr);
	}
	return ret?this:NULL;
}

void
MFCQuaIntController::SetSymValue()
{
	if (symbol != NULL && parent != NULL && parent->frame != NULL) {
		LValue	l;
		ResultValue	r;
		symbol->SetLValue(l, NULL,
			parent->frame->stacker,
			parent->frame->stackerSym,
			parent->frame);
		r = l.CurrentValue();
		UpdateDisplay(r);
	}
}

void
MFCQuaIntController::UpdateDisplay(TypedValue &)
{
	;
}


TypedValue
MFCQuaIntController::Value()
{
	if (symbol->type == TypedValue::S_BYTE) {
		return TypedValue::Byte(value);
	}
	if (symbol->type == TypedValue::S_SHORT) {
		return TypedValue::Short(value);
	}
	if (symbol->type == TypedValue::S_LONG) {
		return TypedValue::Long(value);
	}
	return TypedValue::Int(value);
}

BEGIN_MESSAGE_MAP(MFCQuaIntController, MFCQuaIntCtrl)
//	ON_WM_CREATE()
//	ON_MESSAGE(QM_BLOCK_PARSE, OnQuaBlockParse)
//	ON_MESSAGE(QM_BLOCK_COMPILE, OnQuaBlockCompile)
END_MESSAGE_MAP()



/////////////////////////////////////////////
// bool controller
/////////////////////////////////////////////
MFCQuaBoolController::MFCQuaBoolController()
{
	;
}

MFCQuaBoolController::~MFCQuaBoolController()
{
	;
}

QuaControllerRepresentation *
MFCQuaBoolController::CreateControllerRepresentation(BRect &r, MFCStackFrameView *parv, StabEnt *sym, uint32 id)
{
	bool	ret=false;
	Set(sym, parv);
	if (symbol) {
		QuaControllerRepresentation	*qvr = this;
		CRect	tr = InnerRect(r, 1);
		ret = CreateButton(symbol->name, r, parv, id);
		SetLCMsgParams(QM_SYM_CTL_CHANGED, (WPARAM)symbol, (LPARAM)qvr);
//		SetMDMsgParams(QM_SYM_CTL_BEGIN_MOVE, (WPARAM)symbol, (LPARAM)qvr);
//		SetMUMsgParams(QM_SYM_CTL_END_MOVE, (WPARAM)symbol, (LPARAM)qvr);
	}
	return ret?this:NULL;
}

void
MFCQuaBoolController::SetSymValue()
{
	if (symbol != NULL && parent != NULL && parent->frame != NULL) {
		LValue	l;
		ResultValue	r;
		symbol->SetLValue(l, NULL,
			parent->frame->stacker,
			parent->frame->stackerSym,
			parent->frame);
		r = l.CurrentValue();
		UpdateDisplay(r);
	}
}

void
MFCQuaBoolController::UpdateDisplay(TypedValue &)
{
	;
}


TypedValue
MFCQuaBoolController::Value()
{
	return TypedValue::Bool(GetState()&0x0003);
}

BEGIN_MESSAGE_MAP(MFCQuaBoolController, MFCSmallQuaChkBut)
//	ON_WM_CREATE()
//	ON_MESSAGE(QM_BLOCK_PARSE, OnQuaBlockParse)
//	ON_MESSAGE(QM_BLOCK_COMPILE, OnQuaBlockCompile)
END_MESSAGE_MAP()

/////////////////////////////////////////////
// clip controller
/////////////////////////////////////////////
MFCQuaClipController::MFCQuaClipController()
{
	;
}

MFCQuaClipController::~MFCQuaClipController()
{
	;
}

bool
MFCQuaClipController::SetSelectorValues()
{
	ClearValues();

	if (parent == NULL || parent->root == NULL || parent->root->symbol == NULL || parent->root->symbol->context == NULL) {
		return false;
	}
	StabEnt	*schedulableSym = parent->root->symbol->context;
	StabEnt	*p = schedulableSym->children;
	while (p != NULL) {
		if (p->type == TypedValue::S_CLIP) {
			AddValue(p, p->name);
		}
		p = p->sibling;
	}
	return true;
}

QuaControllerRepresentation *
MFCQuaClipController::CreateControllerRepresentation(BRect &r, MFCStackFrameView *parv, StabEnt *sym, uint32 id)
{
	bool	ret=false;
	Set(sym, parv);
	if (symbol) {
		QuaControllerRepresentation	*qvr = this;
		ret = CreateListBox(
				r, parv, id);
		SetFont(&displayFont);
		SetSelectorValues();
		SetVCMsgParams(QM_SYM_CTL_CHANGED, (WPARAM)symbol, (LPARAM)qvr);
//		SetMDMsgParams(QM_SYM_VAL_BEGIN_MOVE, (WPARAM)symbol, (LPARAM)qvr);
//		SetMUMsgParams(QM_SYM_VAL_END_MOVE, (WPARAM)symbol, (LPARAM)qvr);
	}
	return ret?this:NULL;
}


void
MFCQuaClipController::SetSymValue()
{
	if (symbol != NULL && parent != NULL && parent->frame != NULL) {
		LValue	l;
		ResultValue	r;
		symbol->SetLValue(l, NULL,
			parent->frame->stacker,
			parent->frame->stackerSym,
			parent->frame);
		r = l.CurrentValue();
		UpdateDisplay(r);
	}
}

void
MFCQuaClipController::UpdateDisplay(TypedValue &v)
{
	if (v.ClipValue(parent->frame) != NULL) {
		SelectValue(v.ClipValue(parent->frame)->sym);
	}
}

TypedValue
MFCQuaClipController::Value()
{
	int sel = GetCurSel();
	if (sel >= 0 && sel <GetCount()) {
		StabEnt *s = (StabEnt *)GetItemData(sel);
		return TypedValue::Pointer(TypedValue::S_CLIP, s->ClipValue(NULL));
	}
	return TypedValue::Pointer(TypedValue::S_STRANGE_POINTER, NULL);
}

BEGIN_MESSAGE_MAP(MFCQuaClipController, MFCQuaListBox)
//	ON_WM_CREATE()
//	ON_MESSAGE(QM_BLOCK_PARSE, OnQuaBlockParse)
//	ON_MESSAGE(QM_BLOCK_COMPILE, OnQuaBlockCompile)
END_MESSAGE_MAP()

/////////////////////////////////////////////
// channel controller
/////////////////////////////////////////////
MFCQuaChannelController::MFCQuaChannelController()
{
	;
}

MFCQuaChannelController::~MFCQuaChannelController()
{
}

bool
MFCQuaChannelController::SetSelectorValues(StabEnt *ds)
{
	ClearValues();

	if (parent == NULL) {
		return false;
	}
	if (parent->root == NULL) {
		return false;
	}
	if (parent->root->symbol == NULL) {
		return false;
	}
	if (parent->root->symbol->context == NULL) {
		return false;
	}
	if (parent->root->symbol->context->context == NULL) {
		return false;
	}
	StabEnt	*instSym = parent->root->symbol;
	StabEnt	*schSym = instSym->context;
	StabEnt	*qSym = schSym->context;
	StabEnt	*p = qSym->children;
	while (p != NULL) {
		if (p->type == TypedValue::S_CHANNEL && (ds == NULL || ds != p)) {
			AddValue(p, p->name);
		}
		p = p->sibling;
	}
	return true;
}

QuaControllerRepresentation *
MFCQuaChannelController::CreateControllerRepresentation(BRect &r, MFCStackFrameView *parv, StabEnt *sym, uint32 id)
{
	bool	ret=false;
	Set(sym, parv);
	if (symbol) {
		QuaControllerRepresentation	*qvr = this;
		ret = CreateListBox(
				r, parv, id);
		SetSelectorValues(NULL);
		SetFont(&displayFont);
		SetVCMsgParams(QM_SYM_CTL_CHANGED, (WPARAM)symbol, (LPARAM)qvr);

//		SetMDMsgParams(QM_SYM_VAL_BEGIN_MOVE, (WPARAM)symbol, (LPARAM)qvr);
//		SetMUMsgParams(QM_SYM_VAL_END_MOVE, (WPARAM)symbol, (LPARAM)qvr);
		SetSymValue();
	}
	return ret?this:NULL;
}

void
MFCQuaChannelController::SetSymValue()
{
	if (symbol != NULL && parent != NULL && parent->frame != NULL) {
		LValue	l;
		ResultValue	r;
		symbol->SetLValue(l, NULL,
			parent->frame->stacker,
			parent->frame->stackerSym,
			parent->frame);
		r = l.CurrentValue();
		UpdateDisplay(r);
	}
}

void
MFCQuaChannelController::UpdateDisplay(TypedValue &v)
{
	if (v.ChannelValue() != NULL) {
		SelectValue(v.ChannelValue()->sym);
	}
}

TypedValue
MFCQuaChannelController::Value()
{
	int sel = GetCurSel();
	if (sel >= 0 && sel <GetCount()) {
		StabEnt *s = (StabEnt *)GetItemData(sel);
		return TypedValue::Pointer(TypedValue::S_CHANNEL, s->ChannelValue());
	}
	return TypedValue::Pointer(TypedValue::S_STRANGE_POINTER, NULL);
}

BEGIN_MESSAGE_MAP(MFCQuaChannelController, MFCQuaListBox)
//	ON_WM_CREATE()
//	ON_MESSAGE(QM_BLOCK_PARSE, OnQuaBlockParse)
//	ON_MESSAGE(QM_BLOCK_COMPILE, OnQuaBlockCompile)
END_MESSAGE_MAP()


/////////////////////////////////////////////
// vst program controller
/////////////////////////////////////////////
MFCQuaVstProgramController::MFCQuaVstProgramController()
{
	;
}

MFCQuaVstProgramController::~MFCQuaVstProgramController()
{
}

bool
MFCQuaVstProgramController::SetSelectorValues()
{
	ClearValues();

	if (parent == NULL) {
		return false;
	}
	QuasiStack	*qs = parent->frame;
	if (qs == NULL) {
		return false;
	}
	Block		*b = qs->callingBlock;
	if (b == NULL) {
		return false;
	}
	VstPlugin	*vst = b->crap.call.crap.vstplugin;
#ifdef QUA_V_VST_HOST
	if (vst) {
		if (vst->status != VST_PLUG_LOADED) {
			return false;
		}
		AEffect	*afx = qs->stk.afx;
		for (short i=0; i<afx->numPrograms; i++) {
			char	*nm = vst->GetProgramName(afx, i);
			AddValue((void *)i, nm);
		}
	}
#endif
	return true;
}

QuaControllerRepresentation *
MFCQuaVstProgramController::CreateControllerRepresentation(BRect &r, MFCStackFrameView *parv, StabEnt *sym, uint32 id)
{
	bool	ret=false;
	Set(sym, parv);
	if (symbol) {
		QuaControllerRepresentation	*qvr = this;
		ret = CreateListBox(
				r, parv, id);
		SetSelectorValues();
		SetFont(&displayFont);
		SetVCMsgParams(QM_SYM_CTL_CHANGED, (WPARAM)symbol, (LPARAM)qvr);

//		SetMDMsgParams(QM_SYM_VAL_BEGIN_MOVE, (WPARAM)symbol, (LPARAM)qvr);
//		SetMUMsgParams(QM_SYM_VAL_END_MOVE, (WPARAM)symbol, (LPARAM)qvr);
		SetSymValue();
	}
	return ret?this:NULL;
}

void
MFCQuaVstProgramController::SetSymValue()
{
	if (symbol != NULL && parent != NULL && parent->frame != NULL) {
		Block		*b = parent->frame->callingBlock;
		if (b == NULL) {
			return;
		}
		VstPlugin	*vst = b->crap.call.crap.vstplugin;
#ifdef QUA_V_VST_HOST
		if (vst) {
			if (vst->status != VST_PLUG_LOADED) {
				return;
			}
			AEffect	*afx = parent->frame->stk.afx;
			int32 prog = vst->GetProgram(afx);
			TypedValue	r = TypedValue::VstProgram(prog);
			UpdateDisplay(r);
		}
#endif
	}
}

void
MFCQuaVstProgramController::UpdateDisplay(TypedValue &v)
{
#ifdef QUA_V_VST_HOST
	if (v.type == TypedValue::S_VST_PROGRAM) {
		SetCurSel(v.VstProgramValue());
	}
#endif
}

TypedValue
MFCQuaVstProgramController::Value()
{
	int sel = GetCurSel();
#ifdef QUA_V_VST_HOST
	if (sel >= 0 && sel <GetCount()) {
		return TypedValue::VstProgram(GetItemData(sel));
	}

#endif
	return TypedValue::Pointer(TypedValue::S_STRANGE_POINTER, NULL);
}

BEGIN_MESSAGE_MAP(MFCQuaVstProgramController, MFCQuaListBox)
//	ON_WM_CREATE()
//	ON_MESSAGE(QM_BLOCK_PARSE, OnQuaBlockParse)
//	ON_MESSAGE(QM_BLOCK_COMPILE, OnQuaBlockCompile)
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////
// MFCStackFrameView
////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(MFCStackFrameView, CWnd)

BEGIN_MESSAGE_MAP(MFCStackFrameView, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_MOVE()
	ON_WM_PAINT()
//	ON_WM_PARENTNOTIFY()
	ON_MESSAGE(QM_SYM_CTL_CHANGED, OnQuaCtlChanged)
	ON_MESSAGE(QM_SYM_CTL_BEGIN_MOVE, OnQuaCtlBeginMove)
	ON_MESSAGE(QM_SYM_CTL_END_MOVE, OnQuaCtlEndMove)
	ON_MESSAGE(QM_OPEN_EXT_ED, OnQuaExtEd)
	ON_MESSAGE(QM_CLOSE_EXT_ED, OnQuaExtEd)
	ON_MESSAGE(QM_EXT_ED_CLOSED, OnQuaExtEd)
END_MESSAGE_MAP()

afx_msg LRESULT
MFCStackFrameView::OnQuaCtlChanged(WPARAM wparam, LPARAM lparam)
{
	StabEnt		*symchanged = (StabEnt *)wparam;
	QuaControllerRepresentation *qcr = NULL;
	fprintf(stderr, "stack frame view ctl changed\n");
	TypedValue val;
	if (symchanged) {
		switch(symchanged->type) {
			case TypedValue::S_VST_PROGRAM: {
				qcr = (MFCQuaVstProgramController*)lparam;
				val = qcr->Value();
				if (popupEdit) {
					popupEdit->Invalidate(FALSE);
				}
				break;
			}
			case TypedValue::S_VST_PARAM: {
//				qcr = (MFCQuaVstProgramController*)lparam;
				if (popupEdit) {
					popupEdit->Invalidate(FALSE);
				}
				break;
			}

			case TypedValue::S_CLIP: {
				qcr = (MFCQuaClipController*)lparam;
				val = qcr->Value();
				break;
			}

			case TypedValue::S_TAKE: {
				break;
			}

			case TypedValue::S_BOOL: {
				qcr = (MFCQuaBoolController*)lparam;
				val = qcr->Value();
				break;
			}

			case TypedValue::S_BYTE:
			case TypedValue::S_SHORT:
			case TypedValue::S_LONG:
			case TypedValue::S_INT: {
				qcr = (MFCQuaIntController*)lparam;
				val = qcr->Value();
				break;
			}
			case TypedValue::S_DOUBLE:
			case TypedValue::S_FLOAT: {
				qcr = (MFCQuaRealController*)lparam;
				val = qcr->Value();
				break;
			}
			case TypedValue::S_CHANNEL: {
				qcr = (MFCQuaChannelController*)lparam;
				val = qcr->Value();
				break;
			}
			case TypedValue::S_TIME: {
				qcr = (MFCQuaTimeController*)lparam;
				val = qcr->Value();
				break;
			}
		}
		if (qcr != NULL) {
			root->quaLink->ControllerChanged(symchanged, frame, val);
		}
	}
	return true;
}

// mainly to check movement of continuous controllers for envelope edits
afx_msg LRESULT
MFCStackFrameView::OnQuaCtlBeginMove(WPARAM wparam, LPARAM lparam)
{
	return true;
}

afx_msg LRESULT
MFCStackFrameView::OnQuaCtlEndMove(WPARAM wparam, LPARAM lparam)
{
	return true;
}

afx_msg LRESULT
MFCStackFrameView::OnQuaExtEd(WPARAM wparam, LPARAM lparam)
{
#ifdef QUA_V_VST_HOST
	VstPlugin	*vst = (VstPlugin *)wparam;
	if (vst != NULL && frame != NULL) {
		AEffect	*afx = frame->stk.afx;
		CRect r;
#ifdef QUA_V_VST_HOST_GUI
		if (vst->EditorGetRect(afx, r)) {
			fprintf(stderr, "got rect %d %d\n", r.right, r.bottom);
			if (r.right < 10) r.right = 10;
			if (r.bottom < 10) r.bottom = 10;

// don't seem to be getting notifications from this child ... for the
// moment make 
			if (popupEdit) {
				popupEdit->PostMessage(WM_CLOSE);
				popupEdit = NULL;
			} else {
				popupEdit = new QuaPopupWnd;
				popupEdit->CreatePopup(
					vst->sym->name,
					200,200,r.right,r.bottom,
					m_hWnd);
				if (vst->EditorOpen(afx, popupEdit->m_hWnd)) {
					fprintf(stderr, "open editor succeeds\n");
					popupEdit->RedrawWindow();
				} else {
					fprintf(stderr, "open editor fails\n");
				}
			}
		} else {
			fprintf(stderr, "edit fails to get rect");
		}
#endif
	}
#endif
	return true;
}

void
MFCStackFrameView::OnParentNotify(UINT message, LPARAM lParam)
{
	fprintf(stderr, "on parent notify\n");
}


afx_msg void
MFCStackFrameView::OnMove(int x, int y)
{
	fprintf(stderr, "move MFCStackFrameView %d %d\n", x, y);
	wframe.MoveToXY(x,y);
}

void
MFCStackFrameView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	wbounds.right = cx;
	wbounds.bottom = cy;
	wframe.right = wframe.left+cx;
	wframe.bottom = wframe.top+cy;
	fprintf(stderr, "size MFCStackFrameView %d %d, fr %d %d %d %d\n", cx, cy, wframe.left, wframe.top, wframe.right, wframe.bottom);
}

void
MFCStackFrameView::OnSizing( UINT which, LPRECT r)
{
	CWnd::OnSizing(which, r);
}

int
MFCStackFrameView::OnCreate(LPCREATESTRUCT lpCreateStruct )
{
	wbounds.left = 0;
	wbounds.top = 0;
	wbounds.right = lpCreateStruct->cx;
	wbounds.bottom = lpCreateStruct->cy;
	wframe.left = lpCreateStruct->x;
	wframe.top = lpCreateStruct->y;
	wframe.right = wframe.left+wbounds.right; 
	wframe.bottom = wframe.top+wbounds.bottom; 
	int ret = CWnd::OnCreate(lpCreateStruct);
//	EnableWindow(TRUE);
	return ret;
}
void
MFCStackFrameView::SetWidth(long cx)
{
	wbounds.right = cx;
	ArrangeChildren();
}

MFCStackFrameView::MFCStackFrameView()
{
	eButton = NULL;
	popupEdit = NULL;
	popupInfo = NULL;
}

MFCStackFrameView::~MFCStackFrameView()
{
	;
}



#ifdef Q_FRAME_MAP

inline StabEnt *
map_controller(frame_map_hdr *map, long i)
{
	return (i>=0&&i<map->n_controller)?	
		((StabEnt **)(((char *)map)+sizeof(frame_map_hdr)))[i]:NULL;
}

inline QuasiStack *
map_frame(frame_map_hdr *map, long i)
{
	return (i>=0&&i<map->n_children)?
		((QuasiStack **)
			(((char *)map)
				+sizeof(frame_map_hdr)
				+(map->n_controller*sizeof(StabEnt *))
				)
			)[i]:NULL;
}

bool
MFCStackFrameView::CreateFrameView(
	CRect &r,
	char *lbl,
	MFCInstanceObjectView *rv,
	MFCStackFrameView *pv,
	QuasiStack *fr,
	frame_map_hdr * map,
	bool add_children)
{
	fprintf(stderr, "create frame view for %x -> %x\n", fr, this);
	Set(rv, pv, fr, map->n_children > 0);
	CWnd	*wpv = rv;
	if (pv) {
		wpv = pv;
	}
	if (lbl) {
		label = lbl;
	} else {
		label = "stack";
	}
	long	ret=Create(NULL, "FrameView", WS_CHILD|WS_VISIBLE, r, wpv, NULL);
	if (ret && frame != NULL && frame->callingBlock != NULL) {
		if (root) {
			root->AddFR(this);
		}
		if (frame->callingBlock->type == C_VST) {
			VstPlugin	*vst = frame->callingBlock->crap.call.crap.vstplugin;
			reportError("vst");
			if (vst->hasEditor) {
				reportError("has editor");
				eButton = new MFCSmallQuaLetterBut;
				eButton->CreateButton(NULL, CRect(1,1,12, 12), this, 6969);
				eButton->SetLCMsgParams(QM_MINIMIZE_VIEW,vst,(LPARAM)this);
			}
		}
		short i;
		fprintf(stderr, "\tadding %d controllers\n",map->n_controller); 
		for (i=0; i<map->n_controller; i++) {
			fprintf(stderr, "\tadding %x\n",map_controller(map, i)); 
			if (map_controller(map, i)) {
				AddController(map_controller(map, i));
			}
		}
		if (add_children) {
			fprintf(stderr, "\tadding %d children\n",map->n_children); 
			for (i=0; i<map->n_children; i++) {
				fprintf(stderr, "\add %x\n",map_controller(map, i));
				if (map_frame(map, i)) {
					AddChildFrame(map_frame(map, i));
				}
			}
		}
		ArrangeChildren();
		return true;
	}
	return false;
}

void
MFCStackFrameView::PopFrame(frame_map_hdr *map, bool add_children)
{
	;
}

void
MFCStackFrameView::RefreshFrame(frame_map_hdr *map, bool add_children)
{
	;
}
#else
bool
MFCStackFrameView::CreateFrameView(
	CRect &r,
	char *lbl,
	CWnd *wnd,
	QuaInstanceObjectRepresentation *rv,
	MFCStackFrameView *pv,
	QuasiStack *fr,
	bool add_children)
{
	frame = fr;
	fprintf(stderr, "MFCStackFrameView::CreateFrameView() for %x -> %x inst rep %x parent %x\n", fr, this, pv, rv);
	if (fr != NULL && fr->stackable != NULL) {
		Set(rv, pv, fr, fr->countFrames() > 0);
		CWnd	*wpv = wnd;
		if (pv) {
			wpv = pv;
		}
		if (lbl) {
			label = lbl;
		} else {
			label = "stack";
		}
		long	ret=Create(NULL, "FrameView", WS_CHILD|WS_VISIBLE, r, wpv, NULL);
		if (ret) {
			if (root) {
				root->AddFR(this);
			}
			fprintf(stderr, "frame cb %x %d", frame->callingBlock, frame->callingBlock?frame->callingBlock->type:-1);
			if (frame->callingBlock != NULL && frame->callingBlock->type == Block::C_VST) {
				VstPlugin	*vst = frame->callingBlock->crap.call.crap.vstplugin;
				if (vst->hasEditor) {
					eButton = new MFCSmallQuaLetterBut('E');
					eButton->CreateButton(NULL, CRect(1,1,12, 12), this, 6969);
					eButton->SetLCMsgParams(QM_OPEN_EXT_ED,(WPARAM)vst,(LPARAM)this);
				}
			}
			short i;
			for (i=0; i<fr->stackable->countControllers(); i++) {
				AddController(fr->stackable->controller(i));
			}
			if (add_children) {
				for (i=0; i<fr->countFrames(); i++) {
					AddChildFrame(fr->frameAt(i));
				}
			}
			ArrangeChildren();
			return true;
		}
	}
	return false;
}

void
MFCStackFrameView::PopFrame(QuasiStack *qs, bool add_children)
{
	;
}

void
MFCStackFrameView::RefreshFrame(QuasiStack *qs, bool add_children)
{
	;
}
#endif

void
MFCStackFrameView::HideFrame()
{
	;
}

void
MFCStackFrameView::AddChildFrame(QuasiStack *s)
{
	if (s && root) {
//		fprintf(stderr, "Add child for %x %s %s\n", s,
//			s->context?s->context->UniqueName():"(null context)",
//			s->stackerSym?s->stackerSym->UniqueName():"(null stacker)");
		QuaFrameRepresentation		*r = root->RepresentationFor(s);
		if (r == NULL) {
			root->quaLink->RequestPopFrameRepresentation(
				root, s->context, frame, s, true, false);
			((MFCInstanceObjectView*)root)->ArrangeChildren();
			MFCObjectMountView *orac = ((MFCInstanceObjectView*)root)->ObjectMountView();
			if (orac) {
				orac->ArrangeChildren();
			}
		}
	}
}

#define TIME_DISPLAY_WIDTH	55
#define TIME_DISPLAY_HEIGHT	12
#define INT_DISPLAY_WIDTH	55
#define INT_DISPLAY_HEIGHT	12
void
MFCStackFrameView::AddController(StabEnt *c)
{
	if (c != NULL && frame != NULL) {
		BRect	r(0,0, 100, 20);
		fprintf(stderr, "Add controller for %x %s\n", c, c->UniqueName(), c->type);
		switch (c->type) {
			case TypedValue::S_VST_PARAM: {
				break;
			}
				
			case TypedValue::S_VST_PROGRAM: {
				MFCQuaVstProgramController	*qcv = new MFCQuaVstProgramController;
				qcv->CreateControllerRepresentation(r, this, c, 6969);
				qcv->SetSymValue();
				AddCR(qcv);
				break;
			}
				
			case TypedValue::S_CLIP: {
				MFCQuaClipController	*qcv = new MFCQuaClipController;
				qcv->CreateControllerRepresentation(r, this, c, 6969);
				AddCR(qcv);
				break;
			}

			case TypedValue::S_TAKE: {
				r.Set(0,0, 100, 20);
				break;
			}

			case TypedValue::S_BOOL: {
				r.Set(0,0, 100, 20);
				MFCQuaBoolController	*qcv = new MFCQuaBoolController;
				qcv->CreateControllerRepresentation(r, this, c, 6969);
				qcv->SetSymValue();
				AddCR(qcv);
				break;
			}

			case TypedValue::S_BYTE:
			case TypedValue::S_SHORT:
			case TypedValue::S_LONG:
			case TypedValue::S_INT: {
				r.Set(0, 0, INT_DISPLAY_WIDTH, INT_DISPLAY_HEIGHT);
				MFCQuaIntController	*qcv = new MFCQuaIntController;
				qcv->CreateControllerRepresentation(r, this, c, 6969);
				qcv->SetSymValue();
				AddCR(qcv);
				break;
			}
			case TypedValue::S_DOUBLE:
			case TypedValue::S_FLOAT: {
				MFCQuaRealController	*qcv = new MFCQuaRealController;
				qcv->CreateControllerRepresentation(r, this, c, 6969);
				if (root) {
					MFCObjectView *ov;
					if (root->symbol->type == TypedValue::S_CHANNEL) {
						ov = (MFCChannelObjectView *)root;
					} else {
						ov = (MFCInstanceObjectView *)root;
					}
					qcv->bgColor = ov->bgColor;
				}
				qcv->SetSymValue();
				AddCR(qcv);
				break;
			}
			case TypedValue::S_CHANNEL: {
				MFCQuaChannelController	*qcv = new MFCQuaChannelController;
				qcv->CreateControllerRepresentation(r, this, c, 6969);
				AddCR(qcv);
				break;
			}
			case TypedValue::S_TIME: {
				MFCQuaTimeController	*tcv = new MFCQuaTimeController;
				r.Set(0, 0, TIME_DISPLAY_WIDTH, TIME_DISPLAY_HEIGHT);
				fprintf(stderr, "\t added time controller %x\n", tcv);
				tcv->CreateControllerRepresentation(r, this, c, 6969);
				tcv->SetSymValue();
				AddCR(tcv);
				break;
			}
		}
	}
}

void
MFCStackFrameView::UpdateChannelIndexDisplay(StabEnt *ds)
{
	for (short i=0; i<NCR(); i++) {
		QuaControllerRepresentation	*cr = CR(i);
		if (cr && cr->symbol) {
			if (cr->symbol->type == TypedValue::S_CHANNEL) {
				MFCQuaChannelController	*qcv = (MFCQuaChannelController *)cr;
				qcv->SetSelectorValues(ds);
}
		}
	}
}

void
MFCStackFrameView::updateClipIndexDisplay()
{
	for (short i=0; i<NCR(); i++) {
		QuaControllerRepresentation	*cr = CR(i);
		if (cr && cr->symbol) {
			if (cr->symbol->type == TypedValue::S_CLIP) {
				MFCQuaClipController	*qcv = (MFCQuaClipController *)cr;
				qcv->SetSelectorValues();
			}
		}
	}
}

#define CTL_X_GAP	10
void
MFCStackFrameView::ArrangeChildren()
{
	long	sx=0;
	long	sy;
	StringExtent(((char *)label.c_str()), &displayFont, this, sx, sy);
	long	atX = sx+10;
	long	atY = 0;
	if (eButton != NULL) {
		eButton->SetWindowPos(&wndTop, atX,atY+2, 0, 0, SWP_NOSIZE);
		atX += eButton->bounds.right+CTL_X_GAP;
	}
	for (short i=0; i<NCR(); i++) {
		QuaControllerRepresentation	*cr = CR(i);
		if (cr && cr->symbol) {
			StringExtent(cr->symbol->UniqueName(), &displayFont, this, sx, sy);
			switch (cr->symbol->type) {
				case TypedValue::S_CLIP: {
					MFCQuaClipController	*qcv = (MFCQuaClipController *)cr;
					qcv->labelFrame.Set(atX, atY, atX+sx, atY+sy);
					atX += sx+2;
					fprintf(stderr, "arrange clip %x\n", qcv);
					qcv->SetWindowPos(&wndTop, atX,atY+2, 0, 0, SWP_NOSIZE);
					fprintf(stderr, "moved %x\n", qcv);
					atX += qcv->bounds.right+CTL_X_GAP;
					break;
				}

				case TypedValue::S_TAKE: {
					break;
				}

				case TypedValue::S_BOOL: {
					MFCQuaBoolController	*qcv = (MFCQuaBoolController *)cr;
					qcv->labelFrame.Set(atX, atY, atX+sx, atY+sy);
					atX += sx+2;
					qcv->SetWindowPos(&wndTop, atX,atY+2, 0, 0, SWP_NOSIZE);
					atX += qcv->bounds.right+CTL_X_GAP;
					break;
				}

				case TypedValue::S_BYTE:
				case TypedValue::S_SHORT:
				case TypedValue::S_LONG:
				case TypedValue::S_INT: {
					MFCQuaIntController	*qcv = (MFCQuaIntController *)cr;
					qcv->labelFrame.Set(atX, atY, atX+sx, atY+sy);
					atX += sx+2;
					fprintf(stderr, "arrange int %x\n", qcv);
					qcv->SetWindowPos(&wndTop, atX,atY+2, 0, 0, SWP_NOSIZE);
					atX += qcv->bounds.right+CTL_X_GAP;
					break;
				}
				case TypedValue::S_DOUBLE:
				case TypedValue::S_FLOAT: {
					MFCQuaRealController	*qcv = (MFCQuaRealController *)cr;
					qcv->labelFrame.Set(atX, atY, atX+sx, atY+sy);
					atX += sx+2;
					fprintf(stderr, "arrange real %x\n", qcv);
					qcv->SetWindowPos(&wndTop, atX,atY+3, 0, 0, SWP_NOSIZE);
					atX += qcv->bounds.right+CTL_X_GAP;
					break;
				}
				case TypedValue::S_CHANNEL: {
					MFCQuaChannelController	*qcv = (MFCQuaChannelController *)cr;
					qcv->labelFrame.Set(atX, atY, atX+sx, atY+sy);
					atX += sx+2;
					qcv->SetWindowPos(&wndTop, atX,atY+2, 0, 0, SWP_NOSIZE);
					atX += qcv->bounds.right+CTL_X_GAP;
					break;
				}
				case TypedValue::S_VST_PROGRAM: {
					MFCQuaVstProgramController	*qcv = (MFCQuaVstProgramController *)cr;
					qcv->labelFrame.Set(0,0,0,0);
//					atX += sx+2;
					qcv->SetWindowPos(&wndTop, atX,atY+2, 0, 0, SWP_NOSIZE);
					atX += qcv->bounds.right+CTL_X_GAP;
					break;
				}
				case TypedValue::S_TIME: {
					MFCQuaTimeController	*qcv = (MFCQuaTimeController *)cr;
					qcv->labelFrame.Set(atX, atY, atX+sx, atY+sy);
					atX += sx+2;
					fprintf(stderr, "arrange time %x\n", qcv);
					qcv->SetWindowPos(&wndTop, atX,atY+2, 0, 0, SWP_NOSIZE);
					atX += qcv->bounds.right+CTL_X_GAP;
					break;
				}
			}
		}
	}
	atY = 20;
	atX = 0;
	if (root) {
		for (short i=0; i<root->NFR(); i++) {
			QuaFrameRepresentation	*fr = root->FR(i);
			QuaFrameRepresentation	*mfr = (QuaFrameRepresentation *)this;
			fprintf(stderr, "MFCStackFrameView::ArrangeChildren() this=%x, root child %d parent %x\n", mfr, i, fr->parent);
			if (fr->parent == mfr) {
				MFCStackFrameView	*sfv = (MFCStackFrameView *)fr;
				sfv->ArrangeChildren();
				fprintf(stderr, "MFCStackFrameView::ArrangeChildren() this=%x::arrange move child %x to %d %d child bot %d\n", this, sfv, atX, atY, sfv->wbounds.bottom);
				sfv->SetWindowPos(NULL, atX,atY, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
				atY += sfv->wbounds.bottom;
			}
		}
	}
	fprintf(stderr, "MFCStackFrameView::ArrangeChildren() done atY %d\n", atY);
	wbounds.bottom = atY;
	SetWindowPos(NULL, 0, 0, wbounds.right, wbounds.bottom, SWP_NOMOVE|SWP_NOZORDER);
}

afx_msg void
MFCStackFrameView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
//	BRect	r(0,0,10,wbounds.bottom);
//	RaisedBox(&dc, &r, rgb_purple, true);
	dc.SetBkMode(TRANSPARENT);
	if (label.size() > 0) {
		dc.SelectObject(&displayFont);
		CSize	s = dc.GetTextExtent(label.c_str());
		CRect	tr(0,0, s.cx, wbounds.bottom);
		dc.DrawText(label.c_str(), -1, &tr, DT_LEFT|DT_VCENTER);
	}
	for (short i=0; i<NCR(); i++) {
		QuaControllerRepresentation	*cr = CR(i);
		if (cr && cr->symbol) {
			switch (cr->symbol->type) {
				case TypedValue::S_CLIP: {
					MFCQuaClipController	*qcv = (MFCQuaClipController *)cr;
					dc.DrawText(cr->symbol->UniqueName(), -1, &qcv->labelFrame, DT_LEFT|DT_VCENTER);
					break;
				}

				case TypedValue::S_TAKE: {
					break;
				}

				case TypedValue::S_BOOL: {
					break;
				}

				case TypedValue::S_BYTE:
				case TypedValue::S_SHORT:
				case TypedValue::S_LONG:
				case TypedValue::S_INT: {
					MFCQuaIntController	*qcv = (MFCQuaIntController *)cr;
					dc.DrawText(cr->symbol->UniqueName(), -1, &qcv->labelFrame, DT_LEFT|DT_VCENTER);
					break;
				}
				case TypedValue::S_DOUBLE:
				case TypedValue::S_FLOAT: {
					MFCQuaRealController	*qcv = (MFCQuaRealController *)cr;
					dc.DrawText(cr->symbol->UniqueName(), -1, &qcv->labelFrame, DT_LEFT|DT_VCENTER);
					break;
				}
				case TypedValue::S_CHANNEL: {
					MFCQuaChannelController	*qcv = (MFCQuaChannelController *)cr;
					dc.DrawText(cr->symbol->UniqueName(), -1, &qcv->labelFrame, DT_LEFT|DT_VCENTER);
					break;
				}
				case TypedValue::S_TIME: {
					MFCQuaTimeController	*qcv = (MFCQuaTimeController *)cr;
					dc.DrawText(cr->symbol->UniqueName(), -1, &qcv->labelFrame, DT_LEFT|DT_VCENTER);
					break;
				}
			}
		}
	}
//	dc.ExcludeClipRect(0,0,100, wbounds.bottom);
//	PaintRotor(&dc);
}


/////////////////////////////////////////////////////////
// MFCCInstanceObjectView
////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(MFCInstanceObjectView, MFCObjectView)

MFCInstanceObjectView::MFCInstanceObjectView()
{
	SetBGColor(rgb_lavender);
}

MFCInstanceObjectView::~MFCInstanceObjectView()
{
}


StabEnt *
MFCInstanceObjectView::Symbol()
{
	return symbol;
}


QuaPerceptualSet *
MFCInstanceObjectView::QuaLink()
{
	return quaLink;
}

void
MFCInstanceObjectView::SetName()
{
	nameView.SetWindowText(symbol?symbol->UniqueName():"(nul inst)");
}

void
MFCInstanceObjectView::OnDraw(CDC* pdc)
{
	RaisedBox(pdc, &bounds, bgColor, true);
	CRect textRect(1,1,bounds.right-1,15);
	pdc->SelectObject(&displayFont);
	pdc->SetBkMode(TRANSPARENT);
	if (symbol) {
//		pdc->DrawText(symbol->UniqueName(), &textRect, DT_VCENTER|DT_LEFT);
	}
}


int
MFCInstanceObjectView::OnCreate(LPCREATESTRUCT lpCreateStruct )
{
	int ret = MFCObjectView::OnCreate(lpCreateStruct);
	quaLink->RequestPopFrameRepresentation(this, symbol, NULL, NULL, true, true);

//	EnableWindow(TRUE);
	return ret;
}

/*
void
MFCInstanceObjectView::Pop(stack_img)
{
	frame handle (quasistack)
	state
	array of controller symbols

stack_img
	frame parent
	

	find parent frame
	;
}*/

void
MFCInstanceObjectView::ArrangeChildren()
{
	long atY = CHILDVIEW_MARGIN;
	for (short i=0; i<NFR(); i++) {
		QuaFrameRepresentation	*or = FR(i);
		if (or != NULL && or->parent == NULL) {
			MFCStackFrameView	*ov = (MFCStackFrameView*)or;
			if (ov->wbounds.right != bounds.right-2*CHILDVIEW_MARGIN) {
				ov->SetWidth(bounds.right-2*CHILDVIEW_MARGIN);
			}
			ov->ArrangeChildren();
			fprintf(stderr, "MFCInstanceObjectView::ArrangeChildren()  %d/%x ay %d parent %x\n", i, ov, atY, ov->parent);
			ov->SetWindowPos(&wndTop, 2, atY, 0, 0, SWP_NOSIZE);
			atY += ov->wbounds.bottom;
		}
	}
	bounds.bottom = atY+CHILDVIEW_MARGIN;
	fprintf(stderr, "MFCInstanceObjectView::ArrangeChildren() done bottom is %d\n", bounds.bottom);
	SetWindowPos(&wndTop, 0, 0,bounds.right, bounds.bottom, SWP_NOMOVE);
}

// fill in the instance variables
void
MFCInstanceObjectView::ChildPopulate(StabEnt *sym)
{
	;
}

// fill in the stack frames
void
MFCInstanceObjectView::AttributePopulate()
{
	;
}

void
MFCInstanceObjectView::ChildNameChanged(StabEnt *sym)
{
	if (Symbol() == sym->context) {
//		long ind = variableView->ItemForSym(sym);
//		if (ind >= 0) {
//			variableView->GetListCtrl().SetItemText(ind, 0, sym->UniqueName());
//		}
	}
}

QuaObjectRepresentation *
MFCInstanceObjectView::AddChildRepresentation(StabEnt *c)
{
	return NULL;
}

void
MFCInstanceObjectView::UpdateControllerDisplay(StabEnt *stacker, QuasiStack *frame, StabEnt *sym)
{
//	fprintf(stderr, "%s update controller display for %s,  stack %x\n", stacker->name, sym->name, frame);
	QuaFrameRepresentation	*fr = RepresentationFor(frame);
	if (fr != NULL) {
		QuaControllerRepresentation	*cr = fr->RepresentationFor(sym);
		if (cr != NULL) {
			cr->SetSymValue();
		}
	}
}


// pop a frame representation.
//  this will add
#ifdef Q_FRAME_MAP
void
MFCInstanceObjectView::PopFrameRepresentation(StabEnt *sym, QuasiStack *parentStack, QuasiStack *stack, frame_map_hdr *map, bool add_children)
{
	StabEnt			*s;
	frame_map_hdr	*p;

	MFCStackFrameView	*sfv = (MFCStackFrameView *)RepresentationFor(stack);
	if (sfv != NULL) {
		sfv->PopFrame(map, add_children);
	} else {
		MFCStackFrameView	*pfv = NULL;
		CWnd				*parent = NULL;
		sfv = new MFCStackFrameView;
		if (parentStack != NULL) {
			pfv = (MFCStackFrameView *)RepresentationFor(parentStack);
		} else {
			;
		}
		fprintf(stderr, "PopFrameRepresentation: parent stack %x pfv %x nframe rep %d sym %s\n",
			parentStack, pfv, NFR(), sym?sym->UniqueName():"(nul)");
		sfv->CreateFrameView(
			bounds, sym?sym->UniqueName():"stack", this, pfv,
			stack, map, add_children);
	}
	if (map) {
		delete map;
	}
}
#else
void
MFCInstanceObjectView::PopFrameRepresentation(
	StabEnt *sym, QuasiStack *parentStack,
	QuasiStack *stack, bool add_children)
{
	StabEnt			*s;

	MFCStackFrameView	*sfv = (MFCStackFrameView *)RepresentationFor(stack);
	if (sfv != NULL) {
		sfv->PopFrame(stack, add_children);
	} else {
		MFCStackFrameView	*pfv = NULL;
		CWnd				*parent = NULL;
		sfv = new MFCStackFrameView;
		if (parentStack != NULL) {
			pfv = (MFCStackFrameView *)RepresentationFor(parentStack);
		} else {
			;
		}
		fprintf(stderr, "PopFrameRepresentation: parent stack %x pfv %x nframe rep %d sym %s\n",
			parentStack, pfv, NFR(), sym?sym->UniqueName():"(nul)");
		sfv->CreateFrameView(bounds, sym?sym->UniqueName():"stack",
			this, this, pfv, stack, add_children);
	}
}
#endif

void
MFCInstanceObjectView::RefreshFrameRepresentation(StabEnt *, QuasiStack *)
{
	reportError("refresh unimp");
}

void
MFCInstanceObjectView::HideFrameRepresentation(StabEnt *, QuasiStack *stack)
{
	reportError("hide unimp");
}

void
MFCInstanceObjectView::DeleteFrameRepresentation(StabEnt *sym, QuasiStack *stack)
{
	if (Symbol() == NULL || Symbol() != sym) {
		fprintf(stderr, "Delete frame representation:: null instance\n");
		return;
	}
	StabEnt			*s;

	MFCStackFrameView	*sfv = (MFCStackFrameView *)RepresentationFor(stack);
	if (sfv != NULL) {
		sfv->DestroyWindow();
		QuaFrameRepresentation	*fr = (QuaFrameRepresentation *)sfv;
		RemFR(sfv);
		ArrangeChildren();
	}
}

BEGIN_MESSAGE_MAP(MFCInstanceObjectView, MFCObjectView)
	ON_WM_CREATE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////
// MFCSampleObjectView
////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(MFCSampleObjectView, MFCObjectView)

MFCSampleObjectView::MFCSampleObjectView()
{
	SetBGColor(rgb_aqua);

	clipsView = NULL;
	variableView = NULL;
	rxBlockEdit = NULL;
	mainBlockEdit = NULL;
	rxBlkCtl = NULL;
	mainBlkCtl = NULL;
}

MFCSampleObjectView::~MFCSampleObjectView()
{
	fprintf(stderr, "delete sample object view %x\n", this);
}


void
MFCSampleObjectView::OnDraw(CDC* pdc)
{
	RaisedBox(pdc, &bounds, bgColor, true);
}

int
MFCSampleObjectView::OnCreate(LPCREATESTRUCT lpCreateStruct )
{
	fprintf(stderr, "create sample object view %x\n", this);
	minimizeButton = new MFCSmallQuaMinBut;
	minimizeButton->CreateButton(NULL, CRect(1,1,12, 12), this, 6969);
	minimizeButton->SetLCMsgParams(QM_MINIMIZE_VIEW,NULL,NULL);

	int ret = MFCObjectView::OnCreate(lpCreateStruct);
	nameView.SetWindowPos(&wndTop, minimizeButton->bounds.right+OV_MARGIN, 1, EDIT_POSX-2*OV_MARGIN-minimizeButton->bounds.right, 15, NULL);
	SetName();

	CRect		r;
	dataEditor = new MFCSampleDataEditor;
	dataEditor->CreateEditor(
		CRect(EDIT_POSX,EDIT_POSY,bounds.right-OV_MARGIN, ROW1_BOT),
		this, 6969);

	clipsView = new MFCClipListView;
	clipsView->CreateList(
		CRect(CLIP_POSX,CLIP_POSY,EDIT_POSX-OV_MARGIN, ROW1_BOT),
		this, this, dataEditor, 6969);

	variableView = new MFCSimpleTypeView;
	variableView->CreateList(
		CRect(SIMPLE_T_POSX, SIMPLE_T_POSY,BLOCK_POSX-OV_MARGIN, ROW2_BOT),
		this, 6969);

	long	cx = (bounds.right - BLOCK_POSX - 2*OV_MARGIN)/2;
	rxBlkCtl = new MFCBlockEditCtrlStrip;
	rxBlkCtl->CreateCtrlStrip(
			"Input", CRect(BLOCK_POSX,BLOCK_POSY, BLOCK_POSX+cx, BLOCK_POSY+BLOCK_STATE_H+1),
			this, 6969);
	rxBlockEdit = new MFCEventBlockEdit;
	rxBlockEdit->CreateEditor(
			CRect(BLOCK_POSX,BLOCK_POSY+BLOCK_STATE_H+1, BLOCK_POSX+cx, ROW2_BOT),
			this,
			6969,
			&rxBlkCtl->state,
			NULL
			);
	rxBlockEdit->SetPMsgParams(QM_BLOCK_PARSE, QCID_OV_RXBLOCK, (LPARAM)rxBlockEdit);
	rxBlockEdit->SetCMsgParams(QM_BLOCK_COMPILE, QCID_OV_RXBLOCK, (LPARAM)rxBlockEdit);
	rxBlkCtl->SetMessageParams(rxBlockEdit, QCID_OV_RXBLOCK);

	mainBlkCtl = new MFCBlockEditCtrlStrip;
	mainBlkCtl->CreateCtrlStrip(
			"Output",
			CRect(BLOCK_POSX+cx+OV_MARGIN,BLOCK_POSY, bounds.right-OV_MARGIN, BLOCK_POSY+BLOCK_STATE_H+1),
			this, 6969);
	mainBlockEdit = new MFCExecutableBlockEdit;
	mainBlockEdit->CreateEditor(
		CRect(BLOCK_POSX+cx+OV_MARGIN,BLOCK_POSY+BLOCK_STATE_H+1, bounds.right-OV_MARGIN, ROW2_BOT),
		this, 6969, &mainBlkCtl->state,NULL);
	mainBlockEdit->SetPMsgParams(QM_BLOCK_PARSE, QCID_OV_MAINBLOCK, (LPARAM)mainBlockEdit);
	mainBlockEdit->SetCMsgParams(QM_BLOCK_COMPILE, QCID_OV_MAINBLOCK, (LPARAM)mainBlockEdit);
	mainBlkCtl->SetMessageParams(mainBlockEdit, QCID_OV_MAINBLOCK);

	ArrangeChildren();
	return ret;
}

void
MFCSampleObjectView::ArrangeChildren()
{
	fprintf(stderr, "arrange sample children\n");
	clipsView->SetWindowPos(&wndTop, CLIP_POSX, CLIP_POSY, 0, 0, SWP_NOSIZE);
//	takesView->SetWindowPos(&wndTop, TAKE_POSX, TAKE_POSY, 0, 0, SWP_NOSIZE);
	dataEditor->SetWindowPos(&wndTop, EDIT_POSX,EDIT_POSY,bounds.right-OV_MARGIN-EDIT_POSX, 100-EDIT_POSY, NULL);
	variableView->SetWindowPos(&wndTop, SIMPLE_T_POSX, SIMPLE_T_POSY, 0, 0, SWP_NOSIZE);

	long	cx = (bounds.right - BLOCK_POSX- 2 *OV_MARGIN)/2;
	rxBlkCtl->SetWindowPos(&wndTop, BLOCK_POSX,BLOCK_POSY, 0, 0, SWP_NOSIZE);
	rxBlockEdit->SetWindowPos(&wndTop, BLOCK_POSX,BLOCK_POSY+BLOCK_STATE_H+1, cx, ROW2_BOT-BLOCK_POSY-BLOCK_STATE_H-1, NULL);
	mainBlkCtl->SetWindowPos(&wndTop, BLOCK_POSX+cx+OV_MARGIN, BLOCK_POSY, 0, 0, SWP_NOSIZE);
	mainBlockEdit->SetWindowPos(&wndTop, BLOCK_POSX+cx+OV_MARGIN, BLOCK_POSY+BLOCK_STATE_H+1, cx, ROW2_BOT-BLOCK_POSY-BLOCK_STATE_H-1, NULL);

	bounds.bottom = ROW2_BOT+OV_MARGIN; // set so calling routine has correct dimensions from here

	for (short i=0; i<NCOR(); i++) {
		QuaObjectRepresentation	*or = COR(i);
		if (or != NULL) {
			MFCObjectView	*ov = ToObjectView(or);
			if (ov->bounds.right != bounds.right-2*CHILDVIEW_MARGIN) {
				ov->SetWidth(bounds.right-2*CHILDVIEW_MARGIN);
			}
			ov->SetWindowPos(&wndTop, 2, bounds.bottom, 0, 0, SWP_NOSIZE);
			bounds.bottom += ov->bounds.bottom;
		}
	}

	SetWindowPos(&wndTop, 0, 0,bounds.right, bounds.bottom+CHILDVIEW_MARGIN, SWP_NOMOVE);
}

StabEnt *
MFCSampleObjectView::Symbol()
{
	return symbol;
}

QuaPerceptualSet *
MFCSampleObjectView::QuaLink()
{
	return quaLink;
}

void
MFCSampleObjectView::SetName()
{
	nameView.SetWindowText(symbol?symbol->UniqueName():"(nul sam)");
	CRect	r;
	nameView.GetWindowRect(&r);
	ScreenToClient(&r);
	InvalidateRect(&r);
	nameView.Invalidate(1);
}

void
MFCSampleObjectView::OnDestroy()
{
	dataEditor->DestroyWindow();
	clipsView->DestroyWindow();
//	takesView->DestroyWindow();
	variableView->DestroyWindow();
	mainBlockEdit->DestroyWindow();
	rxBlockEdit->DestroyWindow();
	mainBlkCtl->DestroyWindow();
	rxBlkCtl->DestroyWindow();
	MFCObjectView::OnDestroy();
}

void
MFCSampleObjectView::updateClipIndexDisplay()
{
	vector<StabEnt *> presentClips;
	if (Symbol() == NULL) {
		fprintf(stderr, "Update null sample\n");
		return;
	}
/*
	StabEnt	*p = symbol->children;
	while (p != NULL) {
		if (p->type == S_CLIP) {
			clipsView->AddItem(p);
			presentClips.AddItem(p);
			Clip	*c = p->ClipValue(NULL);
			StabEnt		*ts = c->media->sym;
			Take		*tk = ts->TakeValue();
			if (dataEditor->take == tk) {
				dataEditor->AddClipItemView(c);
			}
		}
		p = p->sibling;
	}*/
	Sample	*v = symbol->SampleValue();
	if (v) {
		for (short i=0; i<v->nClip(); i++) {
			Clip	*c = v->sampleClip(i);
			if (c) {
				clipsView->AddItem(c->sym);
				presentClips.push_back(c->sym);
				StabEnt		*ts = c->media->sym;
				Take		*tk = ts->TakeValue();
				if (dataEditor->take == tk) {
					dataEditor->AddClipItemView(c);
				}
			}
		}
	}
	dataEditor->RemoveClipsNotIn(presentClips);
	clipsView->RemoveClipsNotIn(presentClips);
}

void
MFCSampleObjectView::UpdateTakeIndexDisplay()
{
	if (Symbol() == NULL) {
		fprintf(stderr, "Update null sample\n");
		return;
	}
	StabEnt	*p = symbol->children;

	vector<StabEnt*> presentTakes;
	while (p != NULL) {
		if (p->type == TypedValue::S_TAKE) {
			clipsView->AddItem(p);
			presentTakes.push_back(p);
		}
		p = p->sibling;
	}
	clipsView->RemoveTakesNotIn(presentTakes);
}

void
MFCSampleObjectView::UpdateVariableIndexDisplay()
{
	if (Symbol() == NULL) {
		fprintf(stderr, "Update null sample\n");
		return;
	}
	StabEnt	*p = symbol->children;

	vector<StabEnt*> presentTakes;
	while (p != NULL) {
		switch (p->type) {
			case TypedValue::S_EVENT:
			case TypedValue::S_LAMBDA:
			case TypedValue::S_TAKE:
			case TypedValue::S_CLIP:
				break;
			default:{
				variableView->AddSym(p);
				presentTakes.push_back(p);
			}
		}
		p = p->sibling;
	}
	variableView->RemoveSymbolsNotIn(presentTakes);
}

void
MFCSampleObjectView::ChildPopulate(StabEnt *sym)
{
	if (Symbol() == NULL) {
		fprintf(stderr, "Populate null sample\n");
		return;
	}
	switch (sym->type) {
		case TypedValue::S_CLIP:
			clipsView->AddItem(sym);
			break;
		case TypedValue::S_TAKE:
//			takesView->AddTake(sym);
			clipsView->AddItem(sym);
			break;
		case TypedValue::S_EVENT: {
	// set main block for event handler
			if (strcmp(sym->name, "Wake") == 0) {
				;
			} else if (strcmp(sym->name, "Sleep") == 0) {
				;
			} else if (strcmp(sym->name, "Receive") == 0) {
				if (rxBlockEdit) {
					fprintf(stderr, "set rx block sym to %s\n", sym->UniqueName());
					rxBlockEdit->SetSymbol(sym);
					// and set value to text value of block
				}
			} else if (strcmp(sym->name, "Cue") == 0) {
			} else if (strcmp(sym->name, "Start") == 0) {
			} else if (strcmp(sym->name, "Stop") == 0) {
			} else if (strcmp(sym->name, "Record") == 0) {
			} else if (strcmp(sym->name, "Init") == 0) {
			} else {
			}
			break;
		}

		case TypedValue::S_LAMBDA:
			AddChildRepresentation(sym);
			variableView->AddSym(sym);
			break;

		default:
			variableView->AddSym(sym);
			break;
	}
}

void
MFCSampleObjectView::AttributePopulate()
{
	if (Symbol() == NULL) {
		fprintf(stderr, "Populate null sample\n");
		return;
	}
	// set main block
	Sample	*sample = symbol->SampleValue();
	if (sample == NULL) {
		fprintf(stderr, "Populate non sample\n");
		return;
	}
	// set main and rx block
	if (mainBlockEdit) {
		fprintf(stderr, "set main block sym to %s\n", symbol->UniqueName());
		mainBlockEdit->SetSymbol(symbol);
		// block edit to set text value
		mainBlockEdit->SetValue(sample->mainBlock);
	}
	if (rxBlockEdit) {
		fprintf(stderr, "set rx block sym to %s\n", symbol->UniqueName());
		rxBlockEdit->SetSymbol(symbol);
		// block edit to set text value
		rxBlockEdit->SetValue(sample->rx.block);
	}
}

void
MFCSampleObjectView::UpdateControllerDisplay(StabEnt *stacker, QuasiStack *frame, StabEnt *sym)
{
	if (Symbol() == NULL) {
		fprintf(stderr, "Populate null sample\n");
		return;
	}
}

void
MFCSampleObjectView::ChildNameChanged(StabEnt *sym)
{
	if (Symbol() == NULL) {
		fprintf(stderr, "Populate null sample\n");
		return;
	}
	if (Symbol() == sym->context) {
		long ind = variableView->ItemForSym(sym);
		if (ind >= 0) {
			variableView->GetListCtrl().SetItemText(ind, 0, sym->UniqueName());
		}
	}
}

QuaObjectRepresentation *
MFCSampleObjectView::AddChildRepresentation(StabEnt *s)
{
	CCreateContext	ctxt;
	ctxt.m_pCurrentDoc = GetDocument();
	ctxt.m_pCurrentFrame = GetParentFrame();
	ctxt.m_pLastView = NULL;
	ctxt.m_pNewDocTemplate = NULL;
	ctxt.m_pNewViewClass = NULL;
	if (s->type == TypedValue::S_LAMBDA) {
		MFCMethodObjectView *nv = new MFCMethodObjectView;
		nv->SetSymbol(s);
		nv->SetLinkage(quaLink);
		nv->Create(_T("STATIC"), "Qua Child Lambda View", WS_CHILD | WS_VISIBLE,
				CRect(CHILDVIEW_MARGIN, 0, bounds.right-CHILDVIEW_MARGIN, INI_CHILDVIEW_HEIGHT), this, 1234, &ctxt);
		nv->Populate();
		AddCOR(nv);
		ArrangeChildren();

		MFCObjectMountView	*orac = ObjectMountView();
		if (orac) {
			orac->ArrangeChildren();
		}
		return nv;
	} else if (s->type == TypedValue::S_INSTANCE) {
		MFCInstanceObjectView *nv = new MFCInstanceObjectView;
		nv->SetSymbol(s);
		nv->SetLinkage(quaLink);
		nv->Create(_T("STATIC"), "Qua Instance View", WS_CHILD | WS_VISIBLE,
					CRect(CHILDVIEW_MARGIN, 0, bounds.right-CHILDVIEW_MARGIN, INI_CHILDVIEW_HEIGHT), this, 1234, &ctxt);
		nv->Populate();
		AddCOR(nv);
		ArrangeChildren();

		MFCObjectMountView	*orac = ObjectMountView();
		if (orac) {
			orac->ArrangeChildren();
		}
		return nv;
	}
	return NULL;
}


afx_msg LRESULT
MFCSampleObjectView::OnQuaCloseView(WPARAM wparam, LPARAM lparam)
{
	MFCObjectMountView *orac = ObjectMountView();
	if (orac) {
		orac->PostMessage(QM_CLOSE_VIEW, QCID_OV_SAMPLE_ED, (LPARAM)this);
	} else {
		reportError("trying to close parentless window. unimp ...");
		// we've probably popped up a standalone window. we just close it, i think
	}
	return 0;
}

afx_msg LRESULT
MFCSampleObjectView::OnQuaMinView(WPARAM wparam, LPARAM lparam)
{
	if (Symbol() && (Symbol()->context == NULL || Symbol()->context->type == TypedValue::S_QUA)) {
		MFCObjectMountView *orac = ObjectMountView();
		if (orac) {
			orac->PostMessage(QM_MINIMIZE_VIEW, QCID_OV_SAMPLE_ED, (LPARAM)this);
		} else {
			reportError("trying to minimize parentless window. unimp ...");
			// we've probably popped up a standalone window. we just close it, i think
		}
	}
	return 0;
}

afx_msg LRESULT
MFCSampleObjectView::OnQuaMaxView(WPARAM wparam, LPARAM lparam)
{
	if (Symbol() && (Symbol()->context == NULL || Symbol()->context->type == TypedValue::S_QUA)) {
		MFCObjectMountView *orac = ObjectMountView();
		if (orac) {
			orac->PostMessage(QM_MAXIMIZE_VIEW, QCID_OV_SAMPLE_ED, (LPARAM)this);
		} else {
			reportError("trying to maximize parentless window. unimp ...");
			// we've probably popped up a standalone window. we just close it, i think
		}
	}
	return 0;
}
afx_msg LRESULT
MFCSampleObjectView::OnQuaBlockParse(WPARAM wparam, LPARAM lparam)
{
	MFCBlockEdit	*bed = (MFCBlockEdit *)lparam;
	switch (wparam) {
		case QCID_OV_MAINBLOCK: {
			ParseBlock(quaLink, bed, mainBlkCtl, "main");
			break;
		}
		case QCID_OV_RXBLOCK: {
			if (quaLink && bed && bed->symbol) {
				long len = bed->GetWindowTextLength();
				char	*buf = new char[len+1];
				bed->GetWindowText(buf, len+1);
				buf[len] = 0;
				char nm[256];
				fprintf(stderr, "rx<%s>\n", buf);
				sprintf(nm, "%s receive", bed->symbol->UniqueName());
				long state = quaLink->ParseBlock(bed->symbol, nm, buf, len);
				rxBlkCtl->SetState(state);
				delete buf;
			}
			break;
		}
		case QCID_OV_TXBLOCK: {
			break;
		}
		case QCID_OV_WAKEBLOCK: {
			break;
		}
		case QCID_OV_SLEEPBLOCK: {
			break;
		}
		case QCID_OV_STARTBLOCK: {
			break;
		}
		case QCID_OV_STOPBLOCK: {
			break;
		}
		case QCID_OV_RECORDBLOCK: {
			break;
		}
		case QCID_OV_CUEBLOCK: {
			break;
		}
		case QCID_OV_INITBLOCK: {
			break;
		}
	}
	return 0;
}

afx_msg LRESULT
MFCSampleObjectView::OnQuaBlockCompile(WPARAM wparam, LPARAM lparam)
{
	MFCBlockEdit	*bed = (MFCBlockEdit *)lparam;
	switch (wparam) {
		case QCID_OV_MAINBLOCK: {
			CompileBlock(quaLink, bed, mainBlkCtl, "main");
			break;
		}
		case QCID_OV_RXBLOCK: {
			CompileBlock(quaLink, bed, rxBlkCtl, "receive");
			break;
		}
		case QCID_OV_TXBLOCK: {
			break;
		}
		case QCID_OV_WAKEBLOCK: {
			break;
		}
		case QCID_OV_SLEEPBLOCK: {
			break;
		}
		case QCID_OV_STARTBLOCK: {
			break;
		}
		case QCID_OV_STOPBLOCK: {
			break;
		}
		case QCID_OV_RECORDBLOCK: {
			break;
		}
		case QCID_OV_CUEBLOCK: {
			break;
		}
		case QCID_OV_INITBLOCK: {
			break;
		}
	}
	return 0;
}

afx_msg LRESULT
MFCSampleObjectView::OnQuaBlockRevert(WPARAM wparam, LPARAM lparam)
{
	MFCBlockEdit	*bed = (MFCBlockEdit *)lparam;
	switch (wparam) {
		case QCID_OV_MAINBLOCK: {
			RevertBlock(quaLink, bed, mainBlkCtl, "main");
			break;
		}
		case QCID_OV_RXBLOCK: {
			RevertBlock(quaLink, bed, rxBlkCtl, "receive");
			break;
		}
		case QCID_OV_TXBLOCK: {
			break;
		}
		case QCID_OV_WAKEBLOCK: {
			break;
		}
		case QCID_OV_SLEEPBLOCK: {
			break;
		}
		case QCID_OV_STARTBLOCK: {
			break;
		}
		case QCID_OV_STOPBLOCK: {
			break;
		}
		case QCID_OV_RECORDBLOCK: {
			break;
		}
		case QCID_OV_CUEBLOCK: {
			break;
		}
		case QCID_OV_INITBLOCK: {
			break;
		}
	}
	return 0;
}

BEGIN_MESSAGE_MAP(MFCSampleObjectView, MFCObjectView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_MESSAGE(QM_BLOCK_PARSE, OnQuaBlockParse)
	ON_MESSAGE(QM_BLOCK_COMPILE, OnQuaBlockCompile)
	ON_MESSAGE(QM_BLOCK_REVERT, OnQuaBlockRevert)
	ON_MESSAGE(QM_CLOSE_VIEW, OnQuaCloseView)
	ON_MESSAGE(QM_MINIMIZE_VIEW, OnQuaMinView)
	ON_MESSAGE(QM_MAXIMIZE_VIEW, OnQuaMaxView)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////
// MFCVoiceObjectView
////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(MFCVoiceObjectView, MFCObjectView)

MFCVoiceObjectView::MFCVoiceObjectView()
{
	SetBGColor(rgb_red);

	clipsView = NULL;
	variableView = NULL;
	rxBlockEdit = NULL;
	mainBlockEdit = NULL;
	rxBlkCtl = NULL;
	mainBlkCtl = NULL;
}

MFCVoiceObjectView::~MFCVoiceObjectView()
{
}


void MFCVoiceObjectView::OnDraw(CDC* pdc)
{
	RaisedBox(pdc, &bounds, bgColor, true);
}


int
MFCVoiceObjectView::OnCreate(LPCREATESTRUCT lpCreateStruct )
{
	fprintf(stderr, "create sample object view %x\n", this);
	int ret = MFCObjectView::OnCreate(lpCreateStruct);
	CRect		r;

	minimizeButton = new MFCSmallQuaMinBut;
	minimizeButton->CreateButton(NULL, CRect(1,1,12, 12), this, 6969);
	minimizeButton->SetLCMsgParams(QM_MINIMIZE_VIEW,NULL,NULL);

	nameView.SetWindowPos(&wndTop, minimizeButton->bounds.right+OV_MARGIN, 1, EDIT_POSX-2*OV_MARGIN-minimizeButton->bounds.right, 15, NULL);
	SetName();

	dataEditor = new MFCStreamDataEditor;
	dataEditor->CreateEditor(CRect(EDIT_POSX+STREAMED_YSCALE_WID+1,EDIT_POSY,bounds.right-OV_MARGIN, ROW1_BOT), this, 6969);
	yscale = new MFCStreamEditorYScale;
//	yscale->CreateYScale(CRect(EDIT_POSX,EDIT_POSY,EDIT_POSX+STREAMED_YSCALE_WID, ROW1_BOT), this, 6969, dataEditor);
	dataEditor->yscale = &keyboardRack;
	keyboardRack.Create(_T("STATIC"), "", WS_CHILD | WS_VISIBLE|WS_CLIPSIBLINGS,
       CRect(EDIT_POSX,EDIT_POSY,EDIT_POSX+STREAMED_YSCALE_WID, ROW1_BOT), this, 1234, NULL);
	yscale->CreateYScale(CRect(0,0,STREAMED_YSCALE_WID,128*4), &keyboardRack, 6969, dataEditor);

	clipsView = new MFCClipListView;
	clipsView->CreateList(CRect(CLIP_POSX,CLIP_POSY,EDIT_POSX-OV_MARGIN, ROW1_BOT), this, this, dataEditor, 6969);

//	takesView = new MFCTakeListView;
//	takesView->CreateList(CRect(TAKE_POSX,TAKE_POSY,EDIT_POSX-OV_MARGIN, ROW1_BOT), this, dataEditor, 6969);

	variableView = new MFCSimpleTypeView;
	variableView->CreateList(CRect(SIMPLE_T_POSX, SIMPLE_T_POSY,BLOCK_POSX-OV_MARGIN, ROW2_BOT), this, 6969);

	long	cx = (bounds.right - BLOCK_POSX - 2*OV_MARGIN)/2;
	rxBlkCtl = new MFCBlockEditCtrlStrip;
	rxBlkCtl->CreateCtrlStrip(
			"Input", CRect(BLOCK_POSX,BLOCK_POSY, BLOCK_POSX+cx, BLOCK_POSY+BLOCK_STATE_H+1),
			this, 6969);
	rxBlockEdit = new MFCEventBlockEdit;
	rxBlockEdit->CreateEditor(
			CRect(BLOCK_POSX,BLOCK_POSY+BLOCK_STATE_H+1, BLOCK_POSX+cx, ROW2_BOT),
			this, 6969, &rxBlkCtl->state, NULL);
	rxBlockEdit->SetPMsgParams(QM_BLOCK_PARSE, QCID_OV_RXBLOCK, (LPARAM)rxBlockEdit);
	rxBlockEdit->SetCMsgParams(QM_BLOCK_COMPILE, QCID_OV_RXBLOCK, (LPARAM)rxBlockEdit);
	rxBlkCtl->SetMessageParams(rxBlockEdit, QCID_OV_RXBLOCK);

	mainBlkCtl = new MFCBlockEditCtrlStrip;
	mainBlkCtl->CreateCtrlStrip(
			"Output", CRect(BLOCK_POSX+cx+OV_MARGIN,BLOCK_POSY, bounds.right-OV_MARGIN, BLOCK_POSY+BLOCK_STATE_H+1),
			this, 6969);
	mainBlockEdit = new MFCExecutableBlockEdit;
	mainBlockEdit->CreateEditor(
			CRect(BLOCK_POSX+cx+OV_MARGIN,BLOCK_POSY+BLOCK_STATE_H+1, bounds.right-OV_MARGIN, ROW2_BOT),
			this, 6969, &mainBlkCtl->state, NULL);
	mainBlockEdit->SetPMsgParams(QM_BLOCK_PARSE, QCID_OV_MAINBLOCK, (LPARAM)mainBlockEdit);
	mainBlockEdit->SetCMsgParams(QM_BLOCK_COMPILE, QCID_OV_MAINBLOCK, (LPARAM)mainBlockEdit);
	mainBlkCtl->SetMessageParams(mainBlockEdit, QCID_OV_MAINBLOCK);

//	EnableWindow(TRUE);

	ArrangeChildren();
	return ret;
}


void
MFCVoiceObjectView::OnDestroy()
{
	dataEditor->DestroyWindow();
	clipsView->DestroyWindow();
//	yscale->DestroyWindow();
//	takesView->DestroyWindow();
	variableView->DestroyWindow();
	mainBlockEdit->DestroyWindow();
	rxBlockEdit->DestroyWindow();
	mainBlkCtl->DestroyWindow();
	rxBlkCtl->DestroyWindow();
	MFCObjectView::OnDestroy();
}

void
MFCVoiceObjectView::ArrangeChildren()
{
	fprintf(stderr, "arrange voice children\n");
	clipsView->SetWindowPos(&wndTop, CLIP_POSX, CLIP_POSY, 0, 0, SWP_NOSIZE);
//	takesView->SetWindowPos(&wndTop, TAKE_POSX, TAKE_POSY, 0, 0, SWP_NOSIZE);
//	yscale->SetWindowPos(&wndTop, EDIT_POSX,EDIT_POSY,0,0, SWP_NOSIZE);
	dataEditor->SetWindowPos(&wndTop, EDIT_POSX+STREAMED_YSCALE_WID+1,EDIT_POSY,bounds.right-OV_MARGIN-EDIT_POSX-STREAMED_YSCALE_WID-1, 100-EDIT_POSY, NULL);
	variableView->SetWindowPos(&wndTop, SIMPLE_T_POSX, SIMPLE_T_POSY, 0, 0, SWP_NOSIZE);

	long	cx = (bounds.right - BLOCK_POSX- 2 *OV_MARGIN)/2;
	rxBlkCtl->SetWindowPos(&wndTop, BLOCK_POSX,BLOCK_POSY, 0, 0, SWP_NOSIZE);
	rxBlockEdit->SetWindowPos(&wndTop, BLOCK_POSX,BLOCK_POSY+BLOCK_STATE_H+1, cx, ROW2_BOT-BLOCK_POSY-BLOCK_STATE_H-1, NULL);
	mainBlkCtl->SetWindowPos(&wndTop, BLOCK_POSX+cx+OV_MARGIN, BLOCK_POSY, 0, 0, SWP_NOSIZE);
	mainBlockEdit->SetWindowPos(&wndTop, BLOCK_POSX+cx+OV_MARGIN, BLOCK_POSY+BLOCK_STATE_H+1, cx, ROW2_BOT-BLOCK_POSY-BLOCK_STATE_H-1, NULL);
	bounds.bottom = ROW2_BOT+OV_MARGIN; // set so calling routine has correct dimensions from here

	for (short i=0; i<NCOR(); i++) {
		QuaObjectRepresentation	*or = COR(i);
		if (or != NULL) {
			MFCObjectView	*ov = ToObjectView(or);
			if (ov->bounds.right != bounds.right-2*CHILDVIEW_MARGIN) {
				ov->SetWidth(bounds.right-2*CHILDVIEW_MARGIN);
			}
			ov->ArrangeChildren();
			ov->SetWindowPos(&wndTop, 2, bounds.bottom, 0, 0, SWP_NOSIZE);
			bounds.bottom += ov->bounds.bottom;
		}
	}

	SetWindowPos(&wndTop, 0, 0,bounds.right, bounds.bottom+CHILDVIEW_MARGIN, SWP_NOMOVE);
}

StabEnt *
MFCVoiceObjectView::Symbol()
{
	return symbol;
}

QuaPerceptualSet *
MFCVoiceObjectView::QuaLink()
{
	return quaLink;
}

void
MFCVoiceObjectView::SetName()
{
	nameView.SetWindowText(symbol?symbol->UniqueName():"(nul vox)");
	CRect	r;
	nameView.GetWindowRect(&r);
	ScreenToClient(&r);
	InvalidateRect(&r);
	nameView.Invalidate(1);
}

void
MFCVoiceObjectView::ChildPopulate(StabEnt *sym)
{
	if (Symbol() == NULL) {
		fprintf(stderr, "Populate null voice\n");
		return;
	}
	switch (sym->type) {
		case TypedValue::S_CLIP:
			clipsView->AddItem(sym);
			break;
		case TypedValue::S_TAKE:
//			takesView->AddTake(sym);
			clipsView->AddItem(sym);
			break;
		case TypedValue::S_EVENT: {
	// set main block for event handler
			if (strcmp(sym->name, "Wake") == 0) {
				;
			} else if (strcmp(sym->name, "Sleep") == 0) {
				;
			} else if (strcmp(sym->name, "Receive") == 0) {
				if (rxBlockEdit) {
					fprintf(stderr, "set rx block sym to %s\n", sym->UniqueName());
					rxBlockEdit->SetSymbol(sym);
					// and set value to text value of block
				}
			} else if (strcmp(sym->name, "Cue") == 0) {
			} else if (strcmp(sym->name, "Start") == 0) {
			} else if (strcmp(sym->name, "Stop") == 0) {
			} else if (strcmp(sym->name, "Record") == 0) {
			} else if (strcmp(sym->name, "Init") == 0) {
			} else {
			}
			break;
		}
		case TypedValue::S_LAMBDA:
			AddChildRepresentation(sym);
			variableView->AddSym(sym);
			break;

		default:
			variableView->AddSym(sym);
			break;
	}
}

void
MFCVoiceObjectView::AttributePopulate()
{
	if (symbol == NULL) {
		fprintf(stderr, "Populate null voice\n");
		return;
	}
	Voice	*voice = symbol->VoiceValue();
	if (voice == NULL) {
		fprintf(stderr, "Populate non voice\n");
		return;
	}
	// set main block
	if (mainBlockEdit) {
		fprintf(stderr, "set main block sym to %s\n", symbol->UniqueName());
		mainBlockEdit->SetSymbol(symbol);
		// block edit to set text value
		mainBlockEdit->SetValue(voice->mainBlock);
	}
	if (rxBlockEdit) {
		fprintf(stderr, "set rx block sym to %s\n", symbol->UniqueName());
		rxBlockEdit->SetSymbol(symbol);
		// block edit to set text value
		rxBlockEdit->SetValue(voice->rx.block);
	}
}

void
MFCVoiceObjectView::UpdateControllerDisplay(StabEnt *stacker, QuasiStack *frame, StabEnt *sym)
{
	if (Symbol() == NULL) {
		fprintf(stderr, "Populate null voice\n");
		return;
	}
}

void
MFCVoiceObjectView::ChildNameChanged(StabEnt *sym)
{
	if (Symbol() == NULL) {
		fprintf(stderr, "Populate null voice\n");
		return;
	}
	if (Symbol() == sym->context) {
		long ind = variableView->ItemForSym(sym);
		if (ind >= 0) {
			variableView->GetListCtrl().SetItemText(ind, 0, sym->UniqueName());
		}
	}
}

QuaObjectRepresentation *
MFCVoiceObjectView::AddChildRepresentation(StabEnt *s)
{
	CCreateContext	ctxt;
	ctxt.m_pCurrentDoc = GetDocument();
	ctxt.m_pCurrentFrame = GetParentFrame();
	ctxt.m_pLastView = NULL;
	ctxt.m_pNewDocTemplate = NULL;
	ctxt.m_pNewViewClass = NULL;
	if (s->type == TypedValue::S_LAMBDA) {
		MFCMethodObjectView *nv = new MFCMethodObjectView;
		nv->SetSymbol(s);
		nv->SetLinkage(quaLink);
		nv->Create(_T("STATIC"), "Qua Child Lambda View", WS_CHILD | WS_VISIBLE,
				CRect(CHILDVIEW_MARGIN, 0, bounds.right-CHILDVIEW_MARGIN, INI_CHILDVIEW_HEIGHT), this, 1234, &ctxt);
		nv->Populate();
		AddCOR(nv);
		ArrangeChildren();

		MFCObjectMountView	*orac = ObjectMountView();
		if (orac) {
			orac->ArrangeChildren();
		}
		return nv;
	} else if (s->type == TypedValue::S_INSTANCE) {
		MFCInstanceObjectView *nv = new MFCInstanceObjectView;
		nv->SetSymbol(s);
		nv->SetLinkage(quaLink);
		nv->Create(_T("STATIC"), "Qua Instance View", WS_CHILD | WS_VISIBLE,
					CRect(CHILDVIEW_MARGIN, 0, bounds.right-CHILDVIEW_MARGIN, INI_CHILDVIEW_HEIGHT),
					this, 1234, &ctxt);
		nv->Populate();
		AddCOR(nv);
		ArrangeChildren();

		MFCObjectMountView	*orac = ObjectMountView();
		if (orac) {
			orac->ArrangeChildren();
		}
		return nv;
	}
	return NULL;
}

afx_msg LRESULT
MFCVoiceObjectView::OnQuaCloseView(WPARAM wparam, LPARAM lparam)
{
	MFCObjectMountView *orac = ObjectMountView();
	if (orac) {
		orac->PostMessage(QM_CLOSE_VIEW, QCID_OV_VOICE_ED, (LPARAM)this);
	} else {
		reportError("trying to close parentless window. unimp ...");
		// we've probably popped up a standalone window. we just close it, i think
	}
	return 0;
}

afx_msg LRESULT
MFCVoiceObjectView::OnQuaMinView(WPARAM wparam, LPARAM lparam)
{
	if (Symbol() && (Symbol()->context == NULL || Symbol()->context->type == TypedValue::S_QUA)) {
		MFCObjectMountView *orac = ObjectMountView();
		if (orac) {
			orac->PostMessage(QM_MINIMIZE_VIEW, QCID_OV_VOICE_ED, (LPARAM)this);
		} else {
			reportError("trying to minimize parentless window. unimp ...");
			// we've probably popped up a standalone window. we just close it, i think
		}
	}
	return 0;
}

afx_msg LRESULT
MFCVoiceObjectView::OnQuaMaxView(WPARAM wparam, LPARAM lparam)
{
	if (Symbol() && (Symbol()->context == NULL || Symbol()->context->type == TypedValue::S_QUA)) {
		MFCObjectMountView *orac = ObjectMountView();
		if (orac) {
			orac->PostMessage(QM_MAXIMIZE_VIEW, QCID_OV_VOICE_ED, (LPARAM)this);
		} else {
			reportError("trying to maximize parentless window. unimp ...");
			// we've probably popped up a standalone window. we just close it, i think
		}
	}
	return 0;
}


afx_msg LRESULT
MFCVoiceObjectView::OnQuaBlockParse(WPARAM wparam, LPARAM lparam)
{
	MFCBlockEdit	*bed = (MFCBlockEdit *)lparam;
	switch (wparam) {
		case QCID_OV_MAINBLOCK: {
			ParseBlock(quaLink, bed, mainBlkCtl, "main");
			break;
		}
		case QCID_OV_RXBLOCK: {
			ParseBlock(quaLink, bed, rxBlkCtl, "receive");
			break;
		}
		case QCID_OV_TXBLOCK: {
			break;
		}
		case QCID_OV_WAKEBLOCK: {
			break;
		}
		case QCID_OV_SLEEPBLOCK: {
			break;
		}
		case QCID_OV_STARTBLOCK: {
			break;
		}
		case QCID_OV_STOPBLOCK: {
			break;
		}
		case QCID_OV_RECORDBLOCK: {
			break;
		}
		case QCID_OV_CUEBLOCK: {
			break;
		}
		case QCID_OV_INITBLOCK: {
			break;
		}
	}
	return 0;
}

void
MFCVoiceObjectView::updateClipIndexDisplay()
{
	if (Symbol() == NULL) {
		fprintf(stderr, "Populate null voice\n");
		return;
	}
	vector<StabEnt*> presentClips;
	StabEnt	*p = symbol->children;

/*
	while (p != NULL) {
		if (p->type == S_CLIP && !p->isDeleted) {
			clipsView->AddItem(p);
			presentClips.AddItem(p);
			Clip	*c = p->ClipValue(NULL);
			StabEnt		*ts = c->media->sym;
			Take		*tk = ts->TakeValue();
			if (dataEditor->take == tk) {
				dataEditor->AddClipItemView(c);
			}
		}
		p = p->sibling;
	}*/
	Voice	*v = symbol->VoiceValue();
	if (v) {
		for (short i=0; i<v->nClip(); i++) {
			Clip	*c = v->streamClip(i);
			if (c) {
				clipsView->AddItem(c->sym);
				presentClips.push_back(c->sym);
				StabEnt		*ts = c->media->sym;
				Take		*tk = ts->TakeValue();
				if (dataEditor->take == tk) {
					dataEditor->AddClipItemView(c);
				}
			}
		}
	}
	dataEditor->RemoveClipsNotIn(presentClips);
	clipsView->RemoveClipsNotIn(presentClips);
}

void
MFCVoiceObjectView::UpdateTakeIndexDisplay()
{
	if (Symbol() == NULL) {
		fprintf(stderr, "Populate null voice\n");
		return;
	}
	StabEnt	*p = symbol->children;

	vector<StabEnt*> presentTakes;
	while (p != NULL) {
		if (p->type == TypedValue::S_TAKE) {
			clipsView->AddItem(p);
			presentTakes.push_back(p);
		}
		p = p->sibling;
	}
	clipsView->RemoveTakesNotIn(presentTakes);
}

void
MFCVoiceObjectView::UpdateVariableIndexDisplay()
{
	if (Symbol() == NULL) {
		fprintf(stderr, "Populate null voice\n");
		return;
	}
	StabEnt	*p = symbol->children;

	vector<StabEnt*> presentTakes;
	while (p != NULL) {
		switch (p->type) {
			case TypedValue::S_EVENT:
			case TypedValue::S_LAMBDA:
			case TypedValue::S_TAKE:
			case TypedValue::S_CLIP:
				break;
			default:{
				variableView->AddSym(p);
				presentTakes.push_back(p);
			}
		}
		p = p->sibling;
	}
	variableView->RemoveSymbolsNotIn(presentTakes);
}

afx_msg LRESULT
MFCVoiceObjectView::OnQuaBlockRevert(WPARAM wparam, LPARAM lparam)
{
	MFCBlockEdit	*bed = (MFCBlockEdit *)lparam;
	switch (wparam) {
		case QCID_OV_MAINBLOCK: {
			RevertBlock(quaLink, bed, mainBlkCtl, "main");
			break;
		}
		case QCID_OV_RXBLOCK: {
			RevertBlock(quaLink, bed, rxBlkCtl, "receive");
			break;
		}
		case QCID_OV_TXBLOCK: {
			break;
		}
		case QCID_OV_WAKEBLOCK: {
			break;
		}
		case QCID_OV_SLEEPBLOCK: {
			break;
		}
		case QCID_OV_STARTBLOCK: {
			break;
		}
		case QCID_OV_STOPBLOCK: {
			break;
		}
		case QCID_OV_RECORDBLOCK: {
			break;
		}
		case QCID_OV_CUEBLOCK: {
			break;
		}
		case QCID_OV_INITBLOCK: {
			break;
		}
	}
	return 0;
}

afx_msg LRESULT
MFCVoiceObjectView::OnQuaBlockCompile(WPARAM wparam, LPARAM lparam)
{
	MFCBlockEdit	*bed = (MFCBlockEdit *)lparam;
	switch (wparam) {
		case QCID_OV_MAINBLOCK: {
			CompileBlock(quaLink, bed, mainBlkCtl, "main");
			break;
		}
		case QCID_OV_RXBLOCK: {
			CompileBlock(quaLink, bed, rxBlkCtl, "receive");
			break;
		}
		case QCID_OV_TXBLOCK: {
			break;
		}
		case QCID_OV_WAKEBLOCK: {
			break;
		}
		case QCID_OV_SLEEPBLOCK: {
			break;
		}
		case QCID_OV_STARTBLOCK: {
			break;
		}
		case QCID_OV_STOPBLOCK: {
			break;
		}
		case QCID_OV_RECORDBLOCK: {
			break;
		}
		case QCID_OV_CUEBLOCK: {
			break;
		}
		case QCID_OV_INITBLOCK: {
			break;
		}
	}
	return 0;
}

BEGIN_MESSAGE_MAP(MFCVoiceObjectView, MFCObjectView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_MESSAGE(QM_BLOCK_PARSE, OnQuaBlockParse)
	ON_MESSAGE(QM_BLOCK_COMPILE, OnQuaBlockCompile)
	ON_MESSAGE(QM_BLOCK_REVERT, OnQuaBlockRevert)
	ON_MESSAGE(QM_CLOSE_VIEW, OnQuaCloseView)
	ON_MESSAGE(QM_MINIMIZE_VIEW, OnQuaMinView)
	ON_MESSAGE(QM_MAXIMIZE_VIEW, OnQuaMaxView)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////
// MFCVoiceObjectView
////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(MFCMethodObjectView, MFCObjectView)

MFCMethodObjectView::MFCMethodObjectView()
{
	SetBGColor(rgb_purple);

	mainBlockEdit = NULL;
	mainBlkCtl = NULL;
	variableView = NULL;
}

MFCMethodObjectView::~MFCMethodObjectView()
{
}


void MFCMethodObjectView::OnDraw(CDC* pdc)
{
	RaisedBox(pdc, &bounds, bgColor, true);
}


int
MFCMethodObjectView::OnCreate(LPCREATESTRUCT lpCreateStruct )
{
	int ret = MFCObjectView::OnCreate(lpCreateStruct);
	CRect		r;

	nameView.SetWindowPos(&wndTop, minimizeButton?minimizeButton->bounds.right+1:1, 1, EDIT_POSX-2*OV_MARGIN-(minimizeButton?minimizeButton->bounds.right:0), 15, NULL);
	SetName();

	variableView = new MFCSimpleTypeView;
	variableView->CreateList(CRect(SIMPLE_T_POSX, 15,BLOCK_POSX-OV_MARGIN, ROW1_BOT), this, 6969);

	mainBlkCtl = new MFCBlockEditCtrlStrip;
	mainBlkCtl->CreateCtrlStrip(
			"Main Script", CRect(BLOCK_POSX+OV_MARGIN,2, bounds.right-OV_MARGIN, BLOCK_STATE_H+2+1),
			this, 6969);
	mainBlockEdit = new MFCExecutableBlockEdit;
	mainBlockEdit->CreateEditor(
			CRect(BLOCK_POSX+OV_MARGIN,BLOCK_STATE_H+3, bounds.right-2*OV_MARGIN, ROW1_BOT),
			this, 6969, &mainBlkCtl->state, NULL);
	mainBlockEdit->SetPMsgParams(QM_BLOCK_PARSE, QCID_OV_MAINBLOCK, (LPARAM)mainBlockEdit);
	mainBlockEdit->SetCMsgParams(QM_BLOCK_COMPILE, QCID_OV_MAINBLOCK, (LPARAM)mainBlockEdit);
	mainBlkCtl->SetMessageParams(mainBlockEdit, QCID_OV_MAINBLOCK);

	ArrangeChildren();
	return ret;
}


void
MFCMethodObjectView::ArrangeChildren()
{
	fprintf(stderr, "arrange voice children\n");
	variableView->SetWindowPos(&wndTop, SIMPLE_T_POSX, 15, 0, 0, SWP_NOSIZE);
	mainBlkCtl->SetWindowPos(&wndTop, BLOCK_POSX+OV_MARGIN, 2, 0, 0, SWP_NOSIZE);
	mainBlockEdit->SetWindowPos(&wndTop, BLOCK_POSX+OV_MARGIN, BLOCK_STATE_H+3, bounds.right-2*OV_MARGIN-BLOCK_POSX, ROW1_BOT-BLOCK_STATE_H-3, NULL);
	bounds.bottom = ROW1_BOT+OV_MARGIN; // set so calling routine has correct dimensions from here

	for (short i=0; i<NCOR(); i++) {
		QuaObjectRepresentation	*or = COR(i);
		if (or != NULL) {
			MFCObjectView	*ov = ToObjectView(or);
			if (ov->bounds.right != bounds.right-2*CHILDVIEW_MARGIN) {
				ov->SetWidth(bounds.right-2*CHILDVIEW_MARGIN);
			}
			ov->SetWindowPos(&wndTop, 2, bounds.bottom, 0, 0, SWP_NOSIZE);
			bounds.bottom += ov->bounds.bottom;
		}
	}

	SetWindowPos(&wndTop, 0, 0,bounds.right, bounds.bottom, SWP_NOMOVE);
}

StabEnt *
MFCMethodObjectView::Symbol()
{
	return symbol;
}


QuaPerceptualSet *
MFCMethodObjectView::QuaLink()
{
	return quaLink;
}

void
MFCMethodObjectView::SetName()
{
	nameView.SetWindowText(symbol?symbol->UniqueName():"(nul act)");
	CRect	r;
	nameView.GetWindowRect(&r);
	ScreenToClient(&r);
	InvalidateRect(&r);
	nameView.Invalidate(1);
}

void
MFCMethodObjectView::ChildPopulate(StabEnt *sym)
{
	if (Symbol() == NULL) {
		fprintf(stderr, "Populate null method\n");
		return;
	}
	switch (sym->type) {
		case TypedValue::S_CLIP:
//			clipsView->AddItem(sym);
			break;
		case TypedValue::S_TAKE:
//			clipsView->AddItem(sym);
			break;
		case TypedValue::S_EVENT: {
	// set main block for event handler
			break;
		}
		case TypedValue::S_LAMBDA:
			AddChildRepresentation(sym);
			variableView->AddSym(sym);
			break;

		default:
			variableView->AddSym(sym);
			break;
	}
}

void
MFCMethodObjectView::AttributePopulate()
{
	if (Symbol() == NULL) {
		fprintf(stderr, "Populate null method\n");
		return;
	}
	Lambda	*lambda = symbol->LambdaValue();
	if (lambda == NULL) {
		fprintf(stderr, "Populate non method\n");
		return;
	}
	// set main and rx block
	if (mainBlockEdit) {
		fprintf(stderr, "set main block sym to %s\n", symbol->UniqueName());
		mainBlockEdit->SetSymbol(symbol);
		// block edit to set text value
		mainBlockEdit->SetValue(lambda->mainBlock);
	}
}

void
MFCMethodObjectView::UpdateControllerDisplay(StabEnt *stacker, QuasiStack *frame, StabEnt *sym)
{
	;
}

void
MFCMethodObjectView::ChildNameChanged(StabEnt *sym)
{
	if (Symbol() == NULL) {
		fprintf(stderr, "Populate null method\n");
		return;
	}
	if (Symbol() == sym->context) {
		long ind = variableView->ItemForSym(sym);
		if (ind >= 0) {
			variableView->GetListCtrl().SetItemText(ind, 0, sym->UniqueName());
		}
	}
}

QuaObjectRepresentation *
MFCMethodObjectView::AddChildRepresentation(StabEnt *s)
{
	if (s->type == TypedValue::S_LAMBDA) {
		MFCMethodObjectView *nv = new MFCMethodObjectView;
		nv->SetSymbol(s);
		nv->SetLinkage(quaLink);
		nv->Create(_T("STATIC"), "ObjectView", WS_CHILD | WS_VISIBLE,
				CRect(CHILDVIEW_MARGIN, 0, bounds.right-CHILDVIEW_MARGIN, INI_CHILDVIEW_HEIGHT), this, 1234, NULL);
		nv->Populate();
		AddCOR(nv);
		ArrangeChildren();

		MFCObjectView *p = ParentObjectView();
		while (p != NULL) {
			p->ArrangeChildren();
			p = p->ParentObjectView();
		}
		MFCObjectMountView	*orac = ObjectMountView();
		if (orac) {
			orac->ArrangeChildren();
		}
		return nv;
	}
	return NULL;
}

void
MFCMethodObjectView::UpdateVariableIndexDisplay()
{
	if (Symbol() == NULL) {
		fprintf(stderr, "Populate null method\n");
		return;
	}
	StabEnt	*p = symbol->children;

	vector<StabEnt*> presentTakes;
	while (p != NULL) {
		switch (p->type) {
			case TypedValue::S_EVENT:
			case TypedValue::S_LAMBDA:
			case TypedValue::S_TAKE:
			case TypedValue::S_CLIP:
				break;
			default:{
				variableView->AddSym(p);
				presentTakes.push_back(p);
			}
		}
		p = p->sibling;
	}
	variableView->RemoveSymbolsNotIn(presentTakes);
}


afx_msg LRESULT
MFCMethodObjectView::OnQuaCloseView(WPARAM wparam, LPARAM lparam)
{
	if (Symbol() && (Symbol()->context == NULL || Symbol()->context->type == TypedValue::S_QUA)) {
		MFCObjectMountView *orac = ObjectMountView();
		if (orac) {
			orac->PostMessage(QM_CLOSE_VIEW, QCID_OV_METHOD_ED, (LPARAM)this);
		} else {
			reportError("trying to close parentless window. unimp ...");
			// we've probably popped up a standalone window. we just close it, i think
		}
	}
	return 0;
}

afx_msg LRESULT
MFCMethodObjectView::OnQuaMinView(WPARAM wparam, LPARAM lparam)
{
	if (Symbol() && (Symbol()->context == NULL || Symbol()->context->type == TypedValue::S_QUA)) {
		MFCObjectMountView *orac = ObjectMountView();
		if (orac) {
			orac->PostMessage(QM_MINIMIZE_VIEW, QCID_OV_METHOD_ED, (LPARAM)this);
		} else {
			reportError("trying to minimize parentless window. unimp ...");
			// we've probably popped up a standalone window. we just close it, i think
		}
	}
	return 0;
}

afx_msg LRESULT
MFCMethodObjectView::OnQuaMaxView(WPARAM wparam, LPARAM lparam)
{
	if (Symbol() && (Symbol()->context == NULL || Symbol()->context->type == TypedValue::S_QUA)) {
		MFCObjectMountView *orac = ObjectMountView();
		if (orac) {
			orac->PostMessage(QM_MAXIMIZE_VIEW, QCID_OV_METHOD_ED, (LPARAM)this);
		} else {
			reportError("trying to maximize parentless window. unimp ...");
			// we've probably popped up a standalone window. we just close it, i think
		}
	}
	return 0;
}


afx_msg LRESULT
MFCMethodObjectView::OnQuaBlockParse(WPARAM wparam, LPARAM lparam)
{
	MFCBlockEdit	*bed = (MFCBlockEdit *)lparam;
	switch (wparam) {
		case QCID_OV_MAINBLOCK: {
			ParseBlock(quaLink, bed, mainBlkCtl, "main");
			break;
		}
	}
	return 0;
}

afx_msg LRESULT
MFCMethodObjectView::OnQuaBlockCompile(WPARAM wparam, LPARAM lparam)
{
	MFCBlockEdit	*bed = (MFCBlockEdit *)lparam;
	switch (wparam) {
		case QCID_OV_MAINBLOCK: {
			CompileBlock(quaLink, bed, mainBlkCtl, "main");
			break;
		}
	}
	return 0;
}

afx_msg LRESULT
MFCMethodObjectView::OnQuaBlockRevert(WPARAM wparam, LPARAM lparam)
{
	MFCBlockEdit	*bed = (MFCBlockEdit *)lparam;
	switch (wparam) {
		case QCID_OV_MAINBLOCK: {
			RevertBlock(quaLink, bed, mainBlkCtl, "main");
			break;
		}
	}
	return 0;
}


void
MFCMethodObjectView::OnDestroy()
{
	variableView->DestroyWindow();
	mainBlockEdit->DestroyWindow();
	mainBlkCtl->DestroyWindow();
	MFCObjectView::OnDestroy();
}

BEGIN_MESSAGE_MAP(MFCMethodObjectView, MFCObjectView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_MESSAGE(QM_BLOCK_PARSE, OnQuaBlockParse)
	ON_MESSAGE(QM_BLOCK_COMPILE, OnQuaBlockCompile)
	ON_MESSAGE(QM_BLOCK_REVERT, OnQuaBlockRevert)
	ON_MESSAGE(QM_CLOSE_VIEW, OnQuaCloseView)
	ON_MESSAGE(QM_MINIMIZE_VIEW, OnQuaMinView)
	ON_MESSAGE(QM_MAXIMIZE_VIEW, OnQuaMaxView)
END_MESSAGE_MAP()
