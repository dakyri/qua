// QuaMainTransportView.cpp : implementation file
//

#include "stdafx.h"
#include "QuaMFC.h"
#include "QuaMainTransportView.h"


// QuaMainTransportView

IMPLEMENT_DYNCREATE(QuaMainTransportView, CFormView)

QuaMainTransportView::QuaMainTransportView()
	: CFormView(QuaMainTransportView::IDD)
{
}

QuaMainTransportView::~QuaMainTransportView()
{
}

void QuaMainTransportView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(QuaMainTransportView, CFormView)
	ON_BN_CLICKED(IDC_RECORD_BUTT, OnBnClickedRecordButt)
	ON_BN_CLICKED(IDC_STOP_BUTT, OnBnClickedStopButt)
	ON_BN_CLICKED(IDC_PLAY_BUTT, OnBnClickedPlayButt)
	ON_BN_CLICKED(IDC_PAUSE_BUTT, OnBnClickedPauseButt)
	ON_BN_CLICKED(IDC_REWIND_BUTT, OnBnClickedRewindButt)
	ON_BN_CLICKED(IDC_FF_BUTT, OnBnClickedFfButt)
END_MESSAGE_MAP()


// QuaMainTransportView diagnostics

#ifdef _DEBUG
void QuaMainTransportView::AssertValid() const
{
	CFormView::AssertValid();
}

void QuaMainTransportView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// QuaMainTransportView message handlers

void QuaMainTransportView::OnBnClickedRecordButt()
{
	// TODO: Add your control notification handler code here
}

void QuaMainTransportView::OnBnClickedStopButt()
{
	// TODO: Add your control notification handler code here
}

void QuaMainTransportView::OnBnClickedPlayButt()
{
	// TODO: Add your control notification handler code here
}

void QuaMainTransportView::OnBnClickedPauseButt()
{
	// TODO: Add your control notification handler code here
}

void QuaMainTransportView::OnBnClickedRewindButt()
{
	// TODO: Add your control notification handler code here
}

void QuaMainTransportView::OnBnClickedFfButt()
{
	// TODO: Add your control notification handler code here
}
