// MFCBlockEdit.cpp : implementation file
//
//#define _AFXDLL
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE
#include "stdafx.h"
#include "StdDefs.h"
#include "Colors.h"

#include "QuaMFC.h"
#include "MFCBlockEdit.h"
#include "MFCQuaStateDisplay.h"
#include "MFCObjectView.h"
#include "MFCQuaMessageId.h"

#include "Parse.h"
#include "Block.h"
#include "Sym.h"

#include <sstream>

//////////////////////////////////
// MFCBlockEdit
//////////////////////////////////

IMPLEMENT_DYNCREATE(MFCBlockEdit, CEditView)

BEGIN_MESSAGE_MAP(MFCBlockEdit, CEditView)
	ON_WM_CREATE()
	ON_WM_KEYDOWN( )
	ON_WM_KEYUP( )
	ON_WM_KILLFOCUS( )
	ON_WM_CONTEXTMENU()
	ON_CONTROL_REFLECT(EN_CHANGE, OnEditChange)
END_MESSAGE_MAP()

CFont	MFCBlockEdit::displayFont;

MFCBlockEdit::MFCBlockEdit()
{
	cMessid = 0;
	pMessid = 0;
	symbol = NULL;
	stateDisplay = NULL;
}

MFCBlockEdit::~MFCBlockEdit()
{
}

void
MFCBlockEdit::SetSymbol(StabEnt *sym)
{
	symbol = sym;
}

/////////////////////////////////////////////////////////////////////////////
// MFCBlockEdit dragndrop handlers (virtuals)
/////////////////////////////////////////////////////////////////////////////


int
MFCBlockEdit::OnCreate(LPCREATESTRUCT lpCreateStruct )
{
	target.Register(this);
	bool ret= CEditView::OnCreate(lpCreateStruct);
	if (!ret) {
		long bu = GetDialogBaseUnits();
		SetTabStops(4);
	}
	return ret;
}

DROPEFFECT
MFCBlockEdit::OnDragEnter(
			COleDataObject* object,
			DWORD keyState,
			CPoint point 
		)
{
	fprintf(stderr, "MFCArrangeView::on drag enter\n");
	dragon.SetTo(object, keyState);
	switch (dragon.type) {
		case QuaDrop::VSTPLUGIN:
		case QuaDrop::BUILTIN:
			break;
		default:
			dragon.dropEffect = DROPEFFECT_NONE;
			return DROPEFFECT_NONE;
	}
	return dragon.dropEffect != DROPEFFECT_NONE? dragon.dropEffect:DROPEFFECT_NONE;
}

DROPEFFECT
MFCBlockEdit::OnDragOver(
			COleDataObject* object,
			DWORD dwKeyState,
			CPoint point 
		)
{
//	fprintf(stderr, "MFCArrangeView::on drag over\n");
	return dragon.dropEffect != DROPEFFECT_NONE? dragon.dropEffect:DROPEFFECT_NONE;
}


void
MFCBlockEdit::OnDragLeave( )
{
	fprintf(stderr, "MFCArrangeView::on drag leave\n");
	dragon.Clear();
}

BOOL
MFCBlockEdit::OnDrop(
			COleDataObject* object,
			DROPEFFECT dropEffect,
			CPoint point 
		)
{
	fprintf(stderr, "MFCArrangeView::on drop\n");
/*	point += GetScrollPosition();*/
	switch (dragon.type) {
		case QuaDrop::FILES:
		case QuaDrop::AUDIOFILES:
		case QuaDrop::MIDIFILES: {
/*
			Time	at_time;
			short	at_channel;
			Pix2Time(point.x, at_time);
			at_channel = Pix2Channel(point.y);
//			int		bar, barbeat, beattick;
//			at_time.GetBBQValue(bar, barbeat, beattick);
//			fprintf(stderr, "drag files is dropped: on point %d %d, channel %d, ticks %d t %d:%d.%d\n", point.x, point.y, at_channel, at_time.ticks, bar, barbeat, beattick);
			short	i;
			bool	drop_sample_file=false;
			bool	drop_midi_file=false;
			bool	drop_qua_file=false;
			for (i=0; i<dragon.count; i++) {
				BMimeType	mime_t;
				if (Qua::IdentifyFile(&dragon.data.filePathList[i], &mime_t) == B_OK) {
					fprintf(stderr, "%s (%s)\n", dragon.data.filePathList[i].Path(), mime_t.Type());
					if (strcmp(mime_t.Type(), "audio/x-midi") == 0) {
						drop_midi_file = true;
						break;
					} else if (	strcmp(mime_t.Type(), "audio/x-wav") == 0 ||
								strcmp(mime_t.Type(), "audio/x-raw") == 0 ||
								strcmp(mime_t.Type(), "audio/x-aiff") == 0) {
						drop_sample_file = true;
						break;
					} else if (	strcmp(mime_t.Type(), "audio/x-quascript") == 0) {
						drop_qua_file = true;
						break;
					}
				} else {
//					fprintf(stderr, "dragged file %d unidentifiable\n", i);
				}
			}
			if (i == dragon.count) { // didn't find something useable in the drop
				return FALSE;
			}
			if (drop_sample_file) {
				quaLink->CreateSample(&dragon.data.filePathList[i], dragon.count-i, at_channel, &at_time, NULL);
			} else if (drop_midi_file) {
				;
			} else if (drop_qua_file) {
				;
			} else {
				return FALSE;
			}
			return TRUE;
			*/
			return FALSE;
		}

		case QuaDrop::VOICE: 
		case QuaDrop::SAMPLE: 
		case QuaDrop::SCHEDULABLE: {
			/*
			Time	at_time;
			short	at_channel;
			Pix2Time(point.x, at_time);
			at_channel = Pix2Channel(point.y);
//			int		bar, barbeat, beattick;
//			at_time.GetBBQValue(bar, barbeat, beattick);
//			fprintf(stderr, "drop schedulable %s: on point %d %d, channel %d, ticks %d t %d:%d.%d\n", dragon.data.symbol->name, point.x, point.y, at_channel, at_time.ticks, bar, barbeat, beattick);

			quaLink->CreateInstance(dragon.data.symbol, at_channel, &at_time, NULL);*/
			return FALSE;
		}
			
		case QuaDrop::INSTANCE: {
			/*
			Time	at_time;
			short	at_channel;
			Pix2Time(point.x, at_time);
			at_channel = Pix2Channel(point.y);
//			int		bar, barbeat, beattick;
//			at_time.GetBBQValue(bar, barbeat, beattick);
//			fprintf(stderr, "drop instance %s: on point %d %d, channel %d, ticks %d t %d:%d.%d\n", dragon.data.symbol->name, point.x, point.y, at_channel, at_time.ticks, bar, barbeat, beattick);

			quaLink->MoveInstance(dragon.data.symbol, at_channel, &at_time, NULL);
			*/
			return FALSE;
		}

		case QuaDrop::VSTPLUGIN:
		case QuaDrop::BUILTIN: {
			long pos = GetEditCtrl().CharFromPos(point);
			fprintf(stderr, "pos = %d\n", pos);
			StabEnt	*sym = dragon.data.symbol;
			const char	*sym_nm = sym->uniqueName();
			long buflen = GetWindowTextLength();
			char	*buf = new char[buflen+1];
			char	*nbuf = new char[strlen(sym_nm)+buflen+3];
			GetWindowText(buf, buflen);
			if (pos >= 0 && (pos <=buflen || buflen == 0)) {
				if (buflen == 0) {
					strcpy(nbuf, sym_nm);
				} else if (pos == buflen) {
					strcpy(nbuf, buf);
					nbuf[pos] = ' ';
					strcpy(nbuf+pos+1, sym_nm);
				} else {
					bool space_before = false;
					bool space_aftr  = false;
					if (isalpha(buf[pos])) {
						space_before = true;
					}
					if (pos < buflen-1 && isalpha(buf[pos+1])) {
						space_aftr = true;
					}
					char	*dp = nbuf;
					memcpy(dp, buf, pos);
					dp += pos;
					if (space_before) {
						*dp++ = ' ';
					}
					strcpy(dp, sym_nm);
					dp += pos;
					if (space_aftr) {
						*dp++ = ' ';
					}
					if (pos < buflen) {
						memcpy(dp, buf+pos, buflen-pos);
						dp += buflen-pos;
					}
					*dp = '\0';
				}
				fprintf(stderr, "dropped %s, text is now %s\n", sym_nm, nbuf);
				SetWindowText(nbuf);
				SendCMsg();
			} else {
				fprintf(stderr, "not doing it\n");
			}
			delete buf;
			delete nbuf;
			break;
		}

		case QuaDrop::APPLICATION:
		case QuaDrop::PORT:
		case QuaDrop::TEMPLATE:
			switch (dropEffect) {
				case DROPEFFECT_COPY:
					fprintf(stderr, "left click drag: on drop copy %d %d\n", point.x, point.y);
					break;
				case DROPEFFECT_MOVE:
					fprintf(stderr, "left click drag: on drop move %d %d\n", point.x, point.y);
					break;
				case DROPEFFECT_LINK:
					fprintf(stderr, "left click drag: on drop link %d %d\n", point.x, point.y);
					break;
			}
			break;
	}
	return FALSE;
}

DROPEFFECT
MFCBlockEdit::OnDropEx(
			COleDataObject* object,
			DROPEFFECT dropDefault,
			DROPEFFECT dropList,
			CPoint point 
		)
{
	if (dragon.keyState & MK_RBUTTON) {
		fprintf(stderr, "right click drag: on drop ex %d %d\n", point.x, point.y);

		return OnDrop(object, dropDefault, point)? dragon.dropEffect:DROPEFFECT_NONE;
	}
	return -1;
}


//////////////////////////////////
// MFCBlockEdit diagnostics
//////////////////////////////////

#ifdef _DEBUG
void
MFCBlockEdit::AssertValid() const
{
	CEditView::AssertValid();
}

void
MFCBlockEdit::Dump(CDumpContext& dc) const
{
	CEditView::Dump(dc);
}
#endif //_DEBUG

//////////////////////////////////
// MFCBlockEdit message handlers
//////////////////////////////////


afx_msg void
MFCBlockEdit::OnContextMenu(CWnd* pWnd, CPoint pos)
{
	CEditView::OnContextMenu(pWnd, pos);
}

afx_msg void
MFCBlockEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch(nChar) {
		case VK_HOME: {	// parse test
			SendPMsg();
			break;
		}
		case VK_END: {	// compile and store current value
			SendCMsg();
			break;
		}
		default: {
			if (stateDisplay) {
				stateDisplay->SetState(Q_PARSE_DEFER);
			}
			CEditView::OnKeyDown(nChar, nRepCnt, nFlags);
			break;
		}
	}
}

afx_msg void
MFCBlockEdit::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CEditView::OnKeyUp(nChar, nRepCnt, nFlags);
}

afx_msg void
MFCBlockEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEditView::OnKillFocus(pNewWnd);
}

// null handler needed to prevent this calling up a null document
// on changes (as this class is used not in a normal CView manner)
afx_msg void
MFCBlockEdit::OnEditChange()
{
	;
}

//////////////////////////////////
// MFCBlockEdit members
//////////////////////////////////
int
MFCBlockEdit::CreateEditor(
		CRect &r,
		MFCObjectView *v,
		UINT id,
		MFCQuaParseStateDisplay *sd,
		Block *b
	)
{
	int ret = Create(NULL, "",
					WS_CHILD|WS_VISIBLE|dwStyleDefault,
					r, v, id, NULL);
	SetFont(&displayFont);
	parent = v;
	stateDisplay = sd;

	SetValue(b);

	return ret;
}

void
MFCBlockEdit::SetValue(Block *b)
{
	if (b != NULL) {
		string os;
		stringstream ssos(os);
		if (Esrap(b, ssos, true, 0, true)) {
			SetWindowText(os.c_str());
		} else {
			fprintf(stderr, "MFCBlockEdit:: esrap fails to generate readable form\n");
		}
	}
}


void
MFCBlockEdit::SendCMsg()
{
	if (GetParent() && cMessid) {
		GetParent()->SendMessage(cMessid, cWparam, cLparam);
	}
}

void
MFCBlockEdit::SetCMsgParams(uint32 mid, WPARAM wp, LPARAM lp)
{
	cMessid = mid;
	cWparam = wp;
	cLparam = lp;
}


void
MFCBlockEdit::SendPMsg()
{
	if (GetParent() && pMessid) {
		GetParent()->SendMessage(pMessid, pWparam, pLparam);
	}
}

void
MFCBlockEdit::SetPMsgParams(uint32 mid, WPARAM wp, LPARAM lp)
{
	pMessid = mid;
	pWparam = wp;
	pLparam = lp;
}

//////////////////////////////////
// MFCExecutableBlockEdit
//////////////////////////////////
IMPLEMENT_DYNCREATE(MFCExecutableBlockEdit, MFCBlockEdit)

MFCExecutableBlockEdit::MFCExecutableBlockEdit()
{
}

MFCExecutableBlockEdit::~MFCExecutableBlockEdit()
{
}

BEGIN_MESSAGE_MAP(MFCExecutableBlockEdit, MFCBlockEdit)
END_MESSAGE_MAP()


////////////////////////////
// MFCEventBlockEdit
////////////////////////////
IMPLEMENT_DYNCREATE(MFCEventBlockEdit, MFCBlockEdit)

MFCEventBlockEdit::MFCEventBlockEdit()
{
}

MFCEventBlockEdit::~MFCEventBlockEdit()
{
}

BEGIN_MESSAGE_MAP(MFCEventBlockEdit, MFCBlockEdit)
END_MESSAGE_MAP()


#include "MFCErrorViewer.h"
CDialog	errd;

MFCErrorViewer::MFCErrorViewer()
{
	err_chars = 0;
}

MFCErrorViewer::~MFCErrorViewer()
{
	Clear();
	errd.DestroyWindow();
}

void
MFCErrorViewer::Clear()
{
	err_lines.clear();
	err_chars = 0;
}

void
MFCErrorViewer::AddLine(string line)
{
	err_lines.push_back(line);
	err_chars += line.size();
}

void
MFCErrorViewer::Show()
{
//	errd.ShowWindow(FALSE);
	errd.DestroyWindow();
	errd.Create(IDD_PARSE_ERROR);
	CWnd	*txtv=errd.GetDlgItem(IDC_PARSE_ERR_TXT);
	txtv->SetWindowText("");


	if (err_lines.size() > 0) {
		string buf;
		for (auto it: err_lines) {
			buf += it;
			buf.push_back('\r');
			buf.push_back('\n');
		}
		reportError(buf.c_str());
//		txtv->SetWindowText(buf);
//		errd.ShowWindow(TRUE);
	}
}



IMPLEMENT_DYNCREATE(MFCBlockEditCtrlStrip, CWnd)


BEGIN_MESSAGE_MAP(MFCBlockEditCtrlStrip, CWnd)
	ON_MESSAGE(QM_BLOCK_COMPILE, OnQuaBlockEditCompile)
	ON_MESSAGE(QM_BLOCK_PARSE, OnQuaBlockEditParse)
	ON_MESSAGE(QM_BLOCK_REVERT, OnQuaBlockEditRevert)
END_MESSAGE_MAP()

MFCBlockEditCtrlStrip::MFCBlockEditCtrlStrip():
	compile('C'),
	parse('P'),
	revert('R')
{
	compile.enableChecking = false;
	parse.enableChecking = false;
	revert.enableChecking = false;
}

MFCBlockEditCtrlStrip::~MFCBlockEditCtrlStrip()
{
	;
}

int
MFCBlockEditCtrlStrip::CreateCtrlStrip(char *label, CRect &r, CWnd *w, long id)
{
	int ret = Create(NULL, label, WS_CHILD | WS_VISIBLE |WS_CLIPCHILDREN,
       r, w, id, NULL);
	state.CreateStateDisplay(label, CRect(0,0,r.right,r.bottom), this, id);
	long	sx=0;
	long	sy;
	StringExtent(label, &state.displayFont, &state, sx, sy);
	long	atX = sx+20;
	compile.CreateButton("Compile", CRect(atX, 0, atX+12, 11), this, id);
	atX += 20;
	parse.CreateButton("Parse", CRect(atX, 0, atX+12, 11), this, id);
	atX += 20;
	revert.CreateButton("Revert", CRect(atX, 0, atX+12, 11), this, id);
	atX += 20;
	return ret;
}


void
MFCBlockEditCtrlStrip::SetMessageParams(MFCBlockEdit *ctl, long id)
{
	blockEdit = ctl;
	compile.SetLCMsgParams(QM_BLOCK_COMPILE,id,(LPARAM)ctl);
	parse.SetLCMsgParams(QM_BLOCK_PARSE,id,(LPARAM)ctl);
	revert.SetLCMsgParams(QM_BLOCK_REVERT,id,(LPARAM)ctl);
}


afx_msg LRESULT
MFCBlockEditCtrlStrip::OnQuaBlockEditCompile(WPARAM wp, LPARAM lp)
{
	if (GetParent() && state.state == Q_PARSE_DEFER) {
		GetParent()->SendMessage(QM_BLOCK_COMPILE, wp, lp);
	}
	return 0;
}

afx_msg LRESULT
MFCBlockEditCtrlStrip::OnQuaBlockEditParse(WPARAM wp, LPARAM lp)
{
	if (GetParent() && state.state == Q_PARSE_DEFER) {
		GetParent()->SendMessage(QM_BLOCK_PARSE, wp, lp);
	}
	return 0;
}

afx_msg LRESULT
MFCBlockEditCtrlStrip::OnQuaBlockEditRevert(WPARAM wp, LPARAM lp)
{
	if (GetParent() && state.state == Q_PARSE_DEFER) {
		GetParent()->SendMessage(QM_BLOCK_REVERT, wp, lp);
	}
	return 0;
}
