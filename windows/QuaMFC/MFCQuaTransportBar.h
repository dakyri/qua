#pragma once

#include "QuaDisplay.h"
#include "MFCQuaTimeCtrl.h"
#include "MFCQuaFloatCtrl.h"
// MFCQuaTransportBar

class MFCQuaTransportBar : public CToolBar, public QuaTransportPerspective
{
	DECLARE_DYNAMIC(MFCQuaTransportBar)

public:
	MFCQuaTransportBar();
	virtual ~MFCQuaTransportBar();

	static CFont			displayFont;
	static CFont			labelFont;
	CStatic					tymLabel;
	MFCQuaTimeCtrl			pleitym;
	CStatic					tempoLabel;
	MFCQuaFloatCtrl			tempeh;

	virtual void			displayArrangementTitle(const char *);

	afx_msg int				OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT			OnQuaCtlChanged(WPARAM wparam, LPARAM lparam);

	virtual void			displayCurrentTime(Time &t, bool async);	// ie update time cursor
	virtual void			displayTempo(float t, bool async);
	virtual void			displayMetric(Metric *m, bool async);	// ie update time cursor
protected:
	DECLARE_MESSAGE_MAP()
};


// control id's
#define QCID_TB_TIME	1
#define QCID_TB_TEMPO	2


