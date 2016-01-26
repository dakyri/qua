#pragma once

#include "inx/QuaDisplay.h"

// QuaMainTransportView form view

class QuaMainTransportView : public CFormView, public QuaTransportPerspective
{
	DECLARE_DYNCREATE(QuaMainTransportView)

public:
	QuaMainTransportView();           // protected constructor used by dynamic creation
	virtual ~QuaMainTransportView();

public:
	enum { IDD = IDD_MAIN_TRANSPORT_BAR };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRecordButt();
	afx_msg void OnBnClickedStopButt();
	afx_msg void OnBnClickedPlayButt();
	afx_msg void OnBnClickedPauseButt();
	afx_msg void OnBnClickedRewindButt();
	afx_msg void OnBnClickedFfButt();
};


