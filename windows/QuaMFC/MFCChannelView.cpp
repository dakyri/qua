
// MFCChannelView.cpp : implementation file

#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include "qua_version.h"

#include "QuaMFC.h"
#include "MFCChannelView.h"
#include "MFCChannelMountView.h"
#include "MFCArrangeView.h"
#include "MFCDestinationView.h"
#include "MFCQuaMessageId.h"
#include "ChildFrm.h"

#include "Colors.h"

// Qua main includes
#include "Qua.h"
#include "QuaEnvironment.h"
#include "Channel.h"

#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define INI_DESTINATION_HEIGHT	16
#define INI_DESTINATION_WIDTH	80

/////////////////////////////////////////////////////////////////////////////
// MFCChannelView

CFont	MFCChannelView::displayFont;
BBitmap	MFCChannelView::inArrowImg;
BBitmap	MFCChannelView::outArrowImg;

IMPLEMENT_DYNCREATE(MFCChannelView, CView)

MFCChannelView::MFCChannelView()
{
	channel = NULL;
	selected = false;
}

MFCChannelView::~MFCChannelView()
{
}


BEGIN_MESSAGE_MAP(MFCChannelView, CView)
	//{{AFX_MSG_MAP(MFCChannelView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_ERASEBKGND()
	ON_WM_MOVE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MFCChannelView drawing
/////////////////////////////////////////////////////////////////////////////
void MFCChannelView::OnDraw(CDC* pdc)
{
	RaisedBox(pdc, &bounds, rgb_orange, true);
	CRect textRect(1,1,bounds.right-1,11);
	pdc->SelectObject(&displayFont);
	pdc->SetBkMode(TRANSPARENT);
	char	buf[512];
//	sprintf(buf, "%s (%d ins, %d outs)", channel->sym->name, channel->nAudioIns, channel->nAudioOuts);
	sprintf(buf, "%s", channel->sym->name.c_str(), channel->nAudioIns, channel->nAudioOuts);
	pdc->DrawText(buf, &textRect, DT_VCENTER|DT_LEFT);

	for (short i=0; i<NInR(); i++) {
		MFCInputView	*iv = (MFCInputView *)InR(i);
		inArrowImg.DrawTransparent(pdc->m_hDC, iv->frame.right+1,iv->frame.top+1, 0, 0);
	}
	for (short i=0; i<NOutR(); i++) {
		MFCOutputView	*ov = (MFCOutputView *)OutR(i);
		outArrowImg.DrawTransparent(pdc->m_hDC, ov->frame.right+1,ov->frame.top+1, 0, 0);
	}
}

void
MFCChannelView::NameChanged()
{
	Invalidate();
}

afx_msg BOOL
MFCChannelView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE; //CStatic::OnEraseBkgnd(pDC);
}


/////////////////////////////////////////////////////////////////////////////
// MFCChannelView diagnostics
/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void MFCChannelView::AssertValid() const
{
	CView::AssertValid();
}

void MFCChannelView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// MFCChannelView message handlers
/////////////////////////////////////////////////////////////////////////////

afx_msg void
MFCChannelView::OnMove(int x, int y)
{
#ifdef XXXX
	MFCChannelMountView	*cmv = (MFCChannelMountView*)GetParent();
	if (cmv) {
		MFCArrangeView	*arranger = cmv->arranger;
		if (arranger) {
			y += arranger->yScrollPos;
		}
	}
#endif
	frame.MoveToXY(x,y);
	cout << "channel view " << (channel ? channel->sym->name.c_str() : "?") << " move " << x << " " << y << " " << frame.left << " " << frame.top << endl;
}

void
MFCChannelView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	bounds.right = cx;
	bounds.bottom = cy;
	frame.right = frame.left + cx;
	frame.bottom = frame.top + cy;
	for (short i=0; i<NInR(); i++) {
		MFCInputView	*iv = (MFCInputView *)InR(i);
		iv->SetWindowPos(&wndTop, 0, 0, bounds.right-16, iv->bounds.bottom, SWP_NOMOVE);
	}
	for (short i=0; i<NOutR(); i++) {
		MFCOutputView	*ov = (MFCOutputView *)OutR(i);
		ov->SetWindowPos(&wndTop, 0, 0, bounds.right-16, ov->bounds.bottom, SWP_NOMOVE);
	}
	cout << "channel view " << (channel ? channel->sym->name.c_str() : "?") << " size "  << frame.left << " " << frame.top << endl;
}

void
MFCChannelView::OnSizing( UINT which, LPRECT r)
{
	CView::OnSizing(which, r);
}

int
MFCChannelView::OnCreate(LPCREATESTRUCT lpCreateStruct )
{
	bounds.left = 0;
	bounds.top = 0;
	bounds.right = lpCreateStruct->cx;
	bounds.bottom = lpCreateStruct->cy;
	frame.left = lpCreateStruct->x;
	frame.top = lpCreateStruct->y;
	frame.right = frame.left+bounds.right; 
	frame.bottom = frame.top+bounds.bottom; 

	cout << "channel view create "<< frame.left<<","<<frame.top<<endl;
	return CView::OnCreate(lpCreateStruct);
}


void
MFCChannelView::OnMouseMove(UINT nFlags, CPoint point) 
{
	/*
	// Create TRACKMOUSEEVENT structure
	TRACKMOUSEEVENT tmeMouseEvent;		
	
	// Initialize members of structure
	tmeMouseEvent.cbSize = sizeof(TRACKMOUSEEVENT);
	tmeMouseEvent.dwFlags = TME_LEAVE;
	tmeMouseEvent.hwndTrack = m_hWnd;
	
	// Track mouse leave event
	_TrackMouseEvent(&tmeMouseEvent);
	*/
	CView::OnMouseMove(nFlags, point);
} //End of OnMouseMove

void
MFCChannelView::ChannelMenu(CPoint popPt)
{
	cout << "channel menu\n";
	ClientToScreen(&popPt);
	QuaPort			*selPort;
	port_chan_id	selChannel;
	short			selMenu;

	PortPopup	pp;
	pp.MenuIndexItem(NULL, 0, 0); // adds an item  at 0, so menu can store true offsets
	CMenu		*inputPortMenu = pp.PortMenu(true);
	CMenu		*outputPortMenu = pp.PortMenu(false);

	CMenu		*chInputMenu = new CMenu;
	CMenu		*chOutputMenu = new CMenu;
	CMenu		*deviceMenu = new CMenu;

	deviceMenu->CreatePopupMenu();
	chInputMenu->CreatePopupMenu();
	chOutputMenu->CreatePopupMenu();

	short	i;
	char	buf[128];
	for (i=0; i<channel->inputs.size(); i++) {
		cout << "input menu item" << i << endl;
		sprintf(buf, "input %d (%s)", i+1, channel->inputs.Item(i)->Name(NMFMT_NAME, NMFMT_NUM));
		chInputMenu->AppendMenu(MF_STRING, pp.MenuIndexItem(NULL, i, 3), buf);
	}
	for (i=0; i<channel->outputs.size(); i++) {
		cout << "output menu item" << i << endl;
		sprintf(buf, "output %d (%s)", i+1, channel->outputs.Item(i)->Name(NMFMT_NAME, NMFMT_NUM));
		chOutputMenu->AppendMenu(MF_STRING, pp.MenuIndexItem(NULL, i, 2), buf);
	}

	deviceMenu->AppendMenu(MF_POPUP, (UINT) inputPortMenu->m_hMenu, "add input");
	deviceMenu->AppendMenu(MF_POPUP, (UINT) outputPortMenu->m_hMenu, "add output");
	deviceMenu->AppendMenu(MF_POPUP, (UINT) chInputMenu->m_hMenu, "remove input");
	deviceMenu->AppendMenu(MF_POPUP, (UINT) chOutputMenu->m_hMenu, "remove output");

	short ret = deviceMenu->TrackPopupMenuEx(
						TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RETURNCMD,
						popPt.x, popPt.y, this, NULL);
	cout << "Channel Menu::device Menu ... popup returns " << ret << endl;
	delete deviceMenu;

	if (ret == 0) {	// cancel
		return;
	}
	menu_idx_item	*p = pp.menuIdx[ret];

	if (p && channel) {
		selPort = p->port;
		selChannel = p->chan;
		selMenu = p->submenu;
		switch (selMenu) {
			case 0:
				channel->AddOutput("out", "*", selPort, selChannel, true);
				break;
			case 1:
				channel->AddInput("out", "*", selPort, selChannel, true);
				break;
			case 2:
				channel->RemoveOutput(channel->outputs.Item(selChannel));
				break;
			case 3:
				channel->RemoveInput(channel->inputs.Item(selChannel));
				break;
			default:
				// oopss!!!!!
				break;
		}
		/*
		bool sel=pp.DoPopup(this, popPt, selPort, selChannel, isInput);
		if (sel) {
			if (destinationView) {
				if (isInput) {
					((MFCInputView *)destinationView)->UpdateDestination(selPort, selChannel, 0);
				} else {
					((MFCOutputView *)destinationView)->UpdateDestination(selPort, selChannel, 0);
				}
			}
		}
		*/
	}

}

void
MFCChannelView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	cout << "lb down!\n";
//	ChannelMenu(point);
} //End of OnLButtonDown

void
MFCChannelView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	ChannelMenu(point);
} //End of OnRButtonDown

/////////////////////////////////////////////////////////////////////////////
// MFCChannelView methods
/////////////////////////////////////////////////////////////////////////////

void
MFCChannelView::AddInputRepresentation(Input *d)
{
	MFCInputView *dv = new MFCInputView;
	dv->Create(_T("STATIC"), "Input", WS_CHILD | WS_VISIBLE,
       CRect(0, 0, INI_DESTINATION_WIDTH, INI_DESTINATION_HEIGHT), this, 1234, NULL);
	dv->SetInput(d);
	AddInR(dv);

	ArrangeChildren();
}

void
MFCChannelView::AddOutputRepresentation(Output *d)
{
	MFCOutputView *dv = new MFCOutputView;
	dv->Create(_T("STATIC"), "Output", WS_CHILD | WS_VISIBLE,
       CRect(0, 0, INI_DESTINATION_WIDTH, INI_DESTINATION_HEIGHT), this, 1234, NULL);
	dv->SetOutput(d);
	AddOutR(dv);

	ArrangeChildren();
}

void
MFCChannelView::RemoveInputRepresentation(Input *d)
{
	MFCInputView *dv = (MFCInputView *)InputRepresentationFor(d);
	RemoveInR(dv);
	dv->DestroyWindow();
	ArrangeChildren();
}

void
MFCChannelView::RemoveOutputRepresentation(Output *d)
{
	MFCOutputView *dv = (MFCOutputView *)OutputRepresentationFor(d);
	RemoveOutR(dv);
	dv->DestroyWindow();
	ArrangeChildren();
}


void
MFCChannelView::ArrangeChildren()
{
	cout << "cv: arrange children()\n";
	long	atY = 12;
	for (short i=0; i<NInR(); i++) {
		MFCInputView	*iv = (MFCInputView *)InR(i);
		iv->SetWindowPos(&wndTop, 1, atY, 0, 0, SWP_NOSIZE);
		atY += iv->bounds.bottom+1;
	}
	atY += 3;
	for (short i=0; i<NOutR(); i++) {
		MFCOutputView	*ov = (MFCOutputView *)OutR(i);
		ov->SetWindowPos(&wndTop, 1, atY, 0, 0, SWP_NOSIZE);
		atY += ov->bounds.bottom+1;
	}
	bool	adjustMounty = false;
	if (atY != bounds.bottom) {
		adjustMounty = true;
	}
	SetWindowPos(&wndTop, 0, 0, bounds.right, atY, SWP_NOMOVE);
	if (adjustMounty) {
		CWnd *w = GetParent(), *ww=NULL;
		if (w && w->IsWindowVisible() && (ww=w->GetParent())) {
			long ret = ww->PostMessage(QM_ARRANGE_VIEW, 0, 0);
			CFrameWnd	*pfw = GetParentFrame();
			if (pfw) {
				QuaChildFrame	*qcfw = (QuaChildFrame *)pfw;
				qcfw->arranger->Invalidate();
			}
		}
	}
}

PortPopup::PortPopup()
{
	;
}

PortPopup::~PortPopup()
{
	ClearMenuIndex();
}

CMenu *
PortPopup::AudioMenu(bool isInput, short nch)
{
	CMenu	*menu = new CMenu;
	menu->CreatePopupMenu();
	short n_p = environment.quaAudio.countPorts();
	for (short i=0; i<n_p; i++) {
		QuaAudioPort *p = environment.quaAudio.port(i);
		CMenu	*chmenu = new CMenu;
		chmenu->CreatePopupMenu();
		if (isInput) {
			short n_c = p->NInputs();
			for (short j=0; (j+nch-1)<n_c; j+=nch) {
				chmenu->AppendMenu(MF_STRING, MenuIndexItem(p, j, isInput), p->InputName(j).c_str());
			}
		} else {
			short n_c = p->NOutputs();
			for (short j=0; (j+nch-1)<n_c; j+=nch) {
				chmenu->AppendMenu(MF_STRING, MenuIndexItem(p, j, isInput), p->OutputName(j).c_str());
			}
		}
		menu->AppendMenu(MF_POPUP, (UINT) chmenu->m_hMenu, p->name(NMFMT_NAME));
	}
	return menu;
}

CMenu *
PortPopup::MidiMenu(bool isInput)
{
	CMenu	*menu = new CMenu;
	menu->CreatePopupMenu();
	short n_p = environment.quaMidi.countPorts();
	for (short i=0; i<n_p; i++) {
		QuaMidiPort *p = environment.quaMidi.port(i);
		if (	(isInput && (p->mode & QUA_PORT_IN)) ||
				(!isInput && (p->mode & QUA_PORT_OUT))) {
			CMenu	*chmenu = new CMenu;
			chmenu->CreatePopupMenu();
			static char	buf[16];
			if (isInput) {
				for (short j=1; j<=16; j++) {
					sprintf(buf, "Ch %d", j);
					chmenu->AppendMenu(MF_STRING, MenuIndexItem(p, j, isInput), buf);
				}
			} else {
				for (short j=1; j<=16; j++) {
					sprintf(buf, "Ch %d", j);
					chmenu->AppendMenu(MF_STRING, MenuIndexItem(p, j, isInput), buf);
				}
			}
			menu->AppendMenu(MF_POPUP, (UINT) chmenu->m_hMenu, p->name(NMFMT_NAME));
		}
	}
	return menu;
}

CMenu *
PortPopup::JoyMenu(bool isInput)
{
	CMenu	*menu = new CMenu;
	menu->CreatePopupMenu();
#ifdef QUA_V_JOYSTICK
	short n_p = environment.quaJoystick.countPorts();
	for (short i=0; i<n_p; i++) {
		QuaJoystickPort *p = environment.quaJoystick.port(i);
		CMenu	*chmenu = new CMenu;
		chmenu->CreatePopupMenu();
		menu->AppendMenu(MF_POPUP, (UINT) chmenu->m_hMenu, p->name(NMFMT_NAME));
	}
#endif
	return menu;
}

CMenu *
PortPopup::ChannelMenu(bool isInput)
{
	CMenu	*menu = new CMenu;
	menu->CreatePopupMenu();
	return menu;
}

CMenu	*
PortPopup::PortMenu(bool isInput)
{
	CMenu	*deviceMenu = new CMenu;
	CMenu	*midiMenu  = MidiMenu(isInput);
	CMenu	*audio1Menu  = AudioMenu(isInput, 1);
	CMenu	*audio2Menu  = AudioMenu(isInput, 2);
	CMenu	*audio4Menu  = AudioMenu(isInput, 4);
	CMenu	*joyMenu  = JoyMenu(isInput);

    deviceMenu->CreatePopupMenu();

	if (isInput) {
//		audioPSMenu->AppendMenu(MF_STRING, WM_MDS_AUDIO_DEVICE_PAIR, "pair mono audio dev");
//		joyMenu->AppendMenu(MF_STRING, WM_MDS_JOYSTICK_DEVICE, "joystick dev");

		deviceMenu->AppendMenu(MF_POPUP, (UINT) midiMenu->m_hMenu, "midi input");
		deviceMenu->AppendMenu(MF_POPUP, (UINT) audio1Menu->m_hMenu, "audio 1ch input");
		deviceMenu->AppendMenu(MF_POPUP, (UINT) audio2Menu->m_hMenu, "audio 2ch input");
		deviceMenu->AppendMenu(MF_POPUP, (UINT) joyMenu->m_hMenu, "joy input");
	} else {
//		channelStreamMenu->AppendMenu(MF_STRING, WM_MDS_CHANNEL_STREAM, "stream to channel X");
//		channelAudioMenu->AppendMenu(MF_STRING, WM_MDS_CHANNEL_AUDIO, "audio to channe X");
//		parallelMenu->AppendMenu(MF_STRING, WM_MDS_MIDI_DEVICE, "parallel dev");

		CMenu	*channelAudioMenu  = ChannelMenu(isInput);
		CMenu	*channelStreamMenu  = ChannelMenu(isInput);

//		channelStreamMenu->AppendMenu(MF_STRING, 2, "stream to channel X");
//		joyMenu->AppendMenu(MF_POPUP, (UINT)channelStreamMenu->m_hMenu, "test");

		deviceMenu->AppendMenu(MF_POPUP, (UINT) midiMenu->m_hMenu, "midi output");
		deviceMenu->AppendMenu(MF_POPUP, (UINT) audio1Menu->m_hMenu, "audio 1ch output");
		deviceMenu->AppendMenu(MF_POPUP, (UINT) audio2Menu->m_hMenu, "audio 2ch output");
		deviceMenu->AppendMenu(MF_POPUP, (UINT) joyMenu->m_hMenu, "joy output");
		deviceMenu->AppendMenu(MF_POPUP, (UINT) channelStreamMenu->m_hMenu, "channel stream output");
		deviceMenu->AppendMenu(MF_POPUP, (UINT) channelAudioMenu->m_hMenu, "channel audio output");
	}
	MENUINFO	menuInfo;
	menuInfo.fMask = MIM_APPLYTOSUBMENUS|MIM_STYLE;
	menuInfo.dwStyle = 
		MNS_MODELESS
		|MNS_AUTODISMISS
//		MNS_NOTIFYBYPOS
		;
	deviceMenu->SetMenuInfo(&menuInfo);

	return deviceMenu;
// Remove and destroy old menu
//SetMenu(NULL);
//CMenu* old_menu = CMenu::FromHandle(m_hMenuDefault);
//old_menu->DestroyMenu();

// Add new menu.
//SetMenu(&m_NewMenu);

// Assign default menu
//m_hMenuDefault = m_NewMenu.m_hMenu;
}

void
PortPopup::ClearMenuIndex()
{
	for (auto it: menuIdx) {
		delete it;
	}
	menuIdx.clear();
}

long
PortPopup::MenuIndexItem(QuaPort *qp, long r, short sub)
{
	struct menu_idx_item *p = new menu_idx_item(qp, r, sub);
	menuIdx.push_back(p);
	return menuIdx.size();
}

bool
PortPopup::DoPopup(CWnd *wnd, CPoint &scrPt, QuaPort *&port, port_chan_id &chan, bool isInput)
{
	ClearMenuIndex();
	MenuIndexItem(NULL, 0, 0); // adds an item  at 0, so menu can store true offsets

	CMenu	*deviceMenu = PortMenu(isInput);
	short ret = deviceMenu->TrackPopupMenuEx(
						TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RETURNCMD,
						scrPt.x, scrPt.y, wnd, NULL);
	cout << "PortPopup::DoPopup:: track popup returns " << ret << endl;
	delete deviceMenu;

	if (ret == 0) {	// cancel
		ClearMenuIndex();
		return false;
	}
	menu_idx_item	*p = menuIdx[ret];

	if (p) {
		port = p->port;
		chan = p->chan;
	}

	ClearMenuIndex();

	return true;
}