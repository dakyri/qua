#pragma once

#include "StdDefs.h"
#include "BRect.h"
#include "BBitmap.h"

// MFCQuaStateDisplay

class MFCQuaStateDisplay : public CStatic
{
public:
	MFCQuaStateDisplay();
	virtual ~MFCQuaStateDisplay();

	DECLARE_DYNAMIC(MFCQuaStateDisplay)

public:
	long				nState;
	long				state;
	BBitmap				*stateBmp;
	long				*states;

	virtual void		DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	long				StateInd(long);
	void				SetStates(long ns, long *st);
	void				SetStateBMP(long s, long);

	void				SetState(long);

	long				CreateStateDisplay(char *label, CRect &r, CWnd *w, long id, short ns, long *sta);

	static CFont		displayFont;

public:
	DECLARE_MESSAGE_MAP()
};

class MFCQuaParseStateDisplay : public MFCQuaStateDisplay
{
public:
	MFCQuaParseStateDisplay();
	virtual ~MFCQuaParseStateDisplay();

	DECLARE_DYNAMIC(MFCQuaParseStateDisplay)
	long				CreateStateDisplay(char *label, CRect &r, CWnd *w, long id);

	DECLARE_MESSAGE_MAP()
};

