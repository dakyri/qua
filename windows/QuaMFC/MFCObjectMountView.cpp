
// MFCChannelMountView.cpp : implementation file
//
#define _AFXDLL
#include "stdafx.h"
#include "qua_version.h"
#include "QuaMFC.h"
#include "QuaMFCDoc.h"
#include "MFCObjectMountView.h"
#include "MFCObjectView.h"
#include "MFCQuaMessageId.h"


#include "StdDefs.h"

#include "Qua.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define INI_CHANNEL_HEIGHT	60

/////////////////////////////////////////////////////////////////////////////
// MFCChannelMountView
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(MFCObjectMountView, CScrollView)

MFCObjectMountView::MFCObjectMountView()
{
}

MFCObjectMountView::~MFCObjectMountView()
{
}


BEGIN_MESSAGE_MAP(MFCObjectMountView, CScrollView)
	//{{AFX_MSG_MAP(MFCObjectMountView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_SETFOCUS()
	ON_MESSAGE(QM_CLOSE_VIEW, OnQuaCloseView)
	ON_MESSAGE(QM_MINIMIZE_VIEW, OnQuaMinView)
	ON_MESSAGE(QM_MAXIMIZE_VIEW, OnQuaMaxView)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MFCObjectMountView drawing

void MFCObjectMountView::OnDraw(CDC* pdc)
{
	;
}


void
MFCObjectMountView::displayArrangementTitle(const char *nm)
{
	;
}

afx_msg LRESULT
MFCObjectMountView::OnQuaCloseView(WPARAM wparam, LPARAM lparam)
{
	QuaObjectRepresentation	*nv = NULL;
	switch (wparam) {
		case QCID_OV_SAMPLE_ED: {
			MFCSampleObjectView	*vov = (MFCSampleObjectView *)lparam;
			nv = vov;
			RemOR(nv);
			vov->DestroyWindow();
			ArrangeChildren();
			break;
		}
		case QCID_OV_VOICE_ED: {
			MFCVoiceObjectView	*vov = (MFCVoiceObjectView *)lparam;
			nv = vov;
			RemOR(nv);
			vov->DestroyWindow();
			ArrangeChildren();
			break;
		}
		case QCID_OV_METHOD_ED: {
			MFCMethodObjectView	*vov = (MFCMethodObjectView *)lparam;
			nv = vov;
			RemOR(nv);
			vov->DestroyWindow();
			ArrangeChildren();
			break;
		}
		case QCID_OV_CHANNEL_ED: {
			MFCChannelObjectView	*vov = (MFCChannelObjectView *)lparam;
			nv = vov;
			RemOR(nv);
			vov->DestroyWindow();
			ArrangeChildren();
			break;
		}
		case QCID_OV_INSTANCE_CT: {
			MFCInstanceObjectView	*vov = (MFCInstanceObjectView *)lparam;
			nv = vov;
			RemOR(nv);
			vov->DestroyWindow();
			ArrangeChildren();
			break;
		}
	}
	return 0;
}

afx_msg LRESULT
MFCObjectMountView::OnQuaMinView(WPARAM wparam, LPARAM lparam)
{
	QuaObjectRepresentation	*nv = NULL;
	switch (wparam) {
		case QCID_OV_SAMPLE_ED: {
			MFCSampleObjectView	*vov = (MFCSampleObjectView *)lparam;
			nv = vov;
			ArrangeChildren();
			break;
		}
		case QCID_OV_VOICE_ED: {
			MFCVoiceObjectView	*vov = (MFCVoiceObjectView *)lparam;
			nv = vov;
			ArrangeChildren();
			break;
		}
		case QCID_OV_METHOD_ED: {
			MFCMethodObjectView	*vov = (MFCMethodObjectView *)lparam;
			nv = vov;
			ArrangeChildren();
			break;
		}
		case QCID_OV_CHANNEL_ED: {
			MFCChannelObjectView	*vov = (MFCChannelObjectView *)lparam;
			nv = vov;
			ArrangeChildren();
			break;
		}
		case QCID_OV_INSTANCE_CT: {
			MFCInstanceObjectView	*vov = (MFCInstanceObjectView *)lparam;
			nv = vov;
			ArrangeChildren();
			break;
		}
	}
	return 0;
}


afx_msg LRESULT
MFCObjectMountView::OnQuaMaxView(WPARAM wparam, LPARAM lparam)
{
	QuaObjectRepresentation	*nv = NULL;
	switch (wparam) {
		case QCID_OV_SAMPLE_ED: {
			MFCSampleObjectView	*vov = (MFCSampleObjectView *)lparam;
			nv = vov;
			ArrangeChildren();
			break;
		}
		case QCID_OV_VOICE_ED: {
			MFCVoiceObjectView	*vov = (MFCVoiceObjectView *)lparam;
			nv = vov;
			ArrangeChildren();
			break;
		}
		case QCID_OV_METHOD_ED: {
			MFCMethodObjectView	*vov = (MFCMethodObjectView *)lparam;
			nv = vov;
			ArrangeChildren();
			break;
		}
		case QCID_OV_CHANNEL_ED: {
			MFCChannelObjectView	*vov = (MFCChannelObjectView *)lparam;
			nv = vov;
			ArrangeChildren();
			break;
		}
		case QCID_OV_INSTANCE_CT: {
			MFCInstanceObjectView	*vov = (MFCInstanceObjectView *)lparam;
			nv = vov;
			ArrangeChildren();
			break;
		}
	}
	return 0;
}


afx_msg BOOL
MFCObjectMountView::OnToolTipNeedsTxt( UINT id, NMHDR * pTTTStruct, LRESULT * pResult )
{
	NMTTDISPINFO *ttip = ((NMTTDISPINFO*)pTTTStruct);
//	wsprintf(ttip->lpszText,"hmmm");
//	ttip->hinst = quaAppInstance;
//	strcpy(((NMTTDISPINFO*)pTTTStruct)->szText, "hmmm");
	return false;
}

afx_msg void
MFCObjectMountView::OnSetFocus(CWnd* pOldWnd)
{
	fprintf(stderr, "object mount on set focus\n");
	if (pOldWnd != NULL) {
//		pOldWnd->SetFocus();
	}
	CScrollView::OnSetFocus(pOldWnd);
}

void
MFCObjectMountView::OnInitialUpdate()
{
	CQuaMFCDoc	*qdoc = (CQuaMFCDoc *)GetDocument();
	if (qdoc == NULL) {
		ReportError("initial update of object mount finds a null sequencer document");
	} else if (qdoc->qua == NULL) {
		ReportError("initial update finds a null sequencer");
	} else {	// set qua up with our hooks
		SetLinkage(qdoc->qua->bridge.display);
		quaLink->AddObjectRack(this);
//		display->AddChannelRepresentations(this);
		StabEnt	*pars = qdoc->qua->sym;
		bool	add_initial_views=true;
		if (add_initial_views && pars) {
			StabEnt	*sibp = pars->children;
			while (sibp != NULL) {
				switch (sibp->type) {
				case TypedValue::S_SAMPLE:
					case TypedValue::S_VOICE: {
		//				Clip	*c = sibp->ClipValue(NULL);
		//				StabEnt		*ts = c->media->sym;
						quaLink->ShowObjectRepresentation(sibp);

						// add instances
						StabEnt	*childp = sibp->children;
						while (childp != NULL) {
							switch (childp->type) {
								case TypedValue::S_INSTANCE: {
									quaLink->ShowObjectRepresentation(childp);
									break;
								}
							}
							childp = childp->sibling;
						}

						break;
					}
				}
				sibp = sibp->sibling;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// MFCArrangeView diagnostics
/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void MFCObjectMountView::AssertValid() const
{
//	fprintf(stderr, "obj mount view assert valid\n");
	CScrollView::AssertValid();
}

void MFCObjectMountView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// MFCObjectMountView message handlers
/////////////////////////////////////////////////////////////////////////////

void
MFCObjectMountView::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);
	bounds.right = cx;
	bounds.bottom = cy;
	frame.right = frame.left + cx;
	frame.bottom = frame.top + cy;

	CPoint p = GetScrollPosition();

	fprintf(stderr, "size sc pos %d %d\n", p.x, p.y);
	
	ArrangeChildren();
}

void
MFCObjectMountView::OnSizing( UINT which, LPRECT r)
{
	CScrollView::OnSizing(which, r);
}

int
MFCObjectMountView::OnCreate(LPCREATESTRUCT lpCreateStruct )
{
	bounds.left = 0;
	bounds.top = 0;
	bounds.right = lpCreateStruct->cx;
	bounds.bottom = lpCreateStruct->cy;
	frame.left = lpCreateStruct->x;
	frame.top = lpCreateStruct->y;
	frame.right = frame.left+bounds.right; 
	frame.bottom = frame.top+bounds.bottom; 
 	SetScrollSizes(MM_TEXT, CSize(bounds.right, bounds.bottom));
	if (!rack.Create(_T("STATIC"), "Qua Object Mount Rack", WS_CHILD | WS_VISIBLE,
				CRect(0, 0, bounds.right, bounds.bottom), this, 1234, NULL)) {
		fprintf(stderr, "create failed\n");
	}
	EnableWindow();
	rack.EnableWindow();
	return CScrollView::OnCreate(lpCreateStruct);
}


void
MFCObjectMountView::OnDestroy()
{
	long cnt = NOR();
	for (short i=0; i<cnt; i++) {
		fprintf(stderr, "bout to delete object rep %x\n", this);
		DeleteObjectRepresentation(OR(0));
		fprintf(stderr, "deleted object rep %x\n", this);
	}
	CScrollView::OnDestroy();
}


/////////////////////////////////////////////////////////////////////////////
// MFCObjectMountView methods
/////////////////////////////////////////////////////////////////////////////

bool
MFCObjectMountView::AddObjectRepresentation(StabEnt *s)
{
	CCreateContext	ctxt;
	ctxt.m_pCurrentDoc = GetDocument();
	ctxt.m_pCurrentFrame = GetParentFrame();
	ctxt.m_pLastView = NULL;
	ctxt.m_pNewDocTemplate = NULL;
	ctxt.m_pNewViewClass = NULL;

	switch (s->type) {
		case TypedValue::S_CHANNEL: {
			MFCChannelObjectView *nv = new MFCChannelObjectView;
			nv->SetSymbol(s);
			nv->SetLinkage(quaLink);
			nv->Create(_T("STATIC"), "Qua Object View", WS_CHILD | WS_VISIBLE,
				CRect(0, 0, bounds.right, INI_CHANNEL_HEIGHT), &rack, 1234, &ctxt);
			nv->Populate();
			AddOR(nv);
			break;
		}
		case TypedValue::S_VOICE: {
			MFCVoiceObjectView *nv = new MFCVoiceObjectView;
			nv->SetSymbol(s);
			nv->SetLinkage(quaLink);
			nv->Create(_T("STATIC"), "Qua Object View", WS_CHILD | WS_VISIBLE,
				CRect(0, 0, bounds.right, INI_CHANNEL_HEIGHT), &rack, 1234, &ctxt);
			nv->Populate();
			AddOR(nv);
			break;
		}
		case TypedValue::S_SAMPLE: {
			MFCSampleObjectView *nv = new MFCSampleObjectView;
			nv->SetSymbol(s);
			nv->SetLinkage(quaLink);
			nv->Create(_T("STATIC"), "Qua Object View", WS_CHILD | WS_VISIBLE,
				CRect(0, 0, bounds.right, INI_CHANNEL_HEIGHT), &rack, 1234, &ctxt);
			nv->Populate();
			AddOR(nv);
			break;
		}
		case TypedValue::S_METHOD: {
			MFCMethodObjectView *nv = new MFCMethodObjectView;
			nv->SetSymbol(s);
			nv->SetLinkage(quaLink);
			nv->Create(_T("STATIC"), "Qua Object View", WS_CHILD | WS_VISIBLE,
				CRect(0, 0, bounds.right, INI_CHANNEL_HEIGHT), &rack, 1234, &ctxt);
			nv->Populate();
			AddOR(nv);
			break;
		}
		case TypedValue::S_INSTANCE: {
			StabEnt	*pars = s->context;
			QuaObjectRepresentation *pr = RepresentationFor(pars);
			if (pr != NULL) {
				QuaObjectRepresentation *cr = pr->AddChildRepresentation(s);
			}
			break;
		}
	}
	ArrangeChildren();
	return true;
}


bool
MFCObjectMountView::DeleteObjectRepresentation(QuaObjectRepresentation	*nv)
{
	if (nv && nv->symbol) {
		RemOR(nv);
		if (nv->symbol->context) {
			QuaObjectRepresentation *pr = RepresentationFor(nv->symbol->context);
			if (pr) {
				pr->RemCOR(nv);
			}
		}
		switch (nv->symbol->type) {
			case TypedValue::S_SAMPLE: {
				MFCSampleObjectView	*mv = (MFCSampleObjectView*)nv;
				mv->DestroyWindow();
				ArrangeChildren();
// dirty work done by delete this in OnNcDelete() via DestroyWindow()
//				delete mv;
				break;
			}
			case TypedValue::S_VOICE: {
				MFCVoiceObjectView	*mv = (MFCVoiceObjectView*)nv;
				mv->DestroyWindow();
				ArrangeChildren();
//				delete mv;
				break;
			}
			case TypedValue::S_CHANNEL: {
				MFCChannelObjectView	*mv = (MFCChannelObjectView*)nv;
				mv->DestroyWindow();
				ArrangeChildren();
//				delete mv;
				break;
			}
			case TypedValue::S_INSTANCE: {
				MFCInstanceObjectView	*mv = (MFCInstanceObjectView*)nv;
				mv->DestroyWindow();
				ArrangeChildren();
//				delete mv;
				break;
			}
			case TypedValue::S_METHOD: {
				MFCMethodObjectView	*mv = (MFCMethodObjectView*)nv;
				mv->DestroyWindow();
				ArrangeChildren();
//				delete mv;
				break;
			}
			default: {
				ReportError("Deleting view for unknown symbol type %d", nv->symbol->type);
				return false;
				break;
			}
		}
		return true;
	}
	return false;
}

void
MFCObjectMountView::RemoveObjectRepresentation(StabEnt *s)
{
	QuaObjectRepresentation	*nv = this->RepresentationFor(s);
	if (s) {
		DeleteObjectRepresentation(nv);
	}
}

bool
MFCObjectMountView::CanShow(StabEnt *sym)
{
	return true;
}

// MFCObjectView doesn't inherit directly from QuaObjectRepresentation
// but is the generall class that holds all the common bits and pieces
// need to be a bit careful casting it ... probably there's a better
// way of doing this.
MFCObjectView *
MFCObjectMountView::MFCOV(long i)
{
	QuaObjectRepresentation	*nv = OR(i);
	if (nv) {
		switch (nv->symbol->type) {
			case TypedValue::S_SAMPLE: {
				MFCSampleObjectView	*mv = (MFCSampleObjectView*)nv;
				return (MFCObjectView*)mv;
			}
			case TypedValue::S_VOICE: {
				MFCVoiceObjectView	*mv = (MFCVoiceObjectView*)nv;
				return (MFCObjectView*)mv;
			}
			case TypedValue::S_CHANNEL: {
				MFCChannelObjectView	*mv = (MFCChannelObjectView*)nv;
				return (MFCObjectView*)mv;
			}
			case TypedValue::S_INSTANCE: {
				MFCInstanceObjectView	*mv = (MFCInstanceObjectView*)nv;
				return (MFCObjectView*)mv;
			}
			case TypedValue::S_METHOD: {
				MFCMethodObjectView	*mv = (MFCMethodObjectView*)nv;
				return (MFCObjectView*)mv;
			}
		}
	}
	return NULL;
}

void
MFCObjectMountView::ArrangeChildren()
{
	fprintf(stderr, "omv: arranging children\n");

	int n = NOR();
	long	atY = 0;
	for (int i=0; i<n; i++) {
		MFCObjectView			*ov = MFCOV(i);
		if (ov) {
			ov->SetWidth(bounds.right);
			ov->SetWindowPos(&wndTop, 0, atY, bounds.right, ov->bounds.bottom, SWP_NOSIZE);
			atY += ov->bounds.bottom;
		}
	}
	rack.SetWindowPos(&wndTop, 0, 0, bounds.right, atY, SWP_NOMOVE);
	SetHVScroll();
}



void
MFCObjectMountView::SetHVScroll()
{
	fprintf(stderr, "set arranger scroll\n");
	long	atY=0;
	MFCObjectView *ev = MFCOV(NOR()-1);
	if (ev && ev->frame.bottom > 0 && ev->frame.bottom > bounds.bottom) {
		SetScrollSizes(MM_TEXT, CSize(ev->frame.right, ev->frame.bottom));
		fprintf(stderr, "ev frb %d %d\n", ev->frame.bottom, bounds.bottom);
	} else {
		SetScrollSizes(MM_TEXT, CSize(0, 0));
	}
}