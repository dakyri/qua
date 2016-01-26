#pragma once

#include "inx/QuaDisplay.h"
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

	virtual void			DisplayArrangementTitle(const char *);

	afx_msg int				OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT			OnQuaCtlChanged(WPARAM wparam, LPARAM lparam);

	virtual void			DisplayCurrentTime(Time &t, bool async);	// ie update time cursor
	virtual void			DisplayTempo(float t, bool async);
	virtual void			DisplayMetric(Metric *m, bool async);	// ie update time cursor
protected:
	DECLARE_MESSAGE_MAP()
};


// control id's
#define QCID_TB_TIME	1
#define QCID_TB_TEMPO	2


