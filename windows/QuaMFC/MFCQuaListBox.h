#pragma once

#include "DaBasicTypes.h"

// MFCQuaListBox

class StabEnt;
#include "DaList.h"

class MFCQuaListBox : public CListBox
{
	DECLARE_DYNAMIC(MFCQuaListBox)

public:
	MFCQuaListBox();
	virtual ~MFCQuaListBox();

	bool						CreateListBox(CRect &r, CWnd *, uint32);
//	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
//	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
//	virtual int CompareItem(LPCOMPAREITEMSTRUCT lpCIS);
	afx_msg void				OnSize(UINT nType, int cx, int cy);
	afx_msg void				OnSizing( UINT, LPRECT );
	afx_msg int					OnCreate(LPCREATESTRUCT lpCreateStruct );
	afx_msg void				OnMove(int x, int y);
	afx_msg void				OnSelChanged();
	afx_msg void				OnDblClk();

	void						SendVCMsg();
	void						SetVCMsgParams(uint32, WPARAM, LPARAM);
	uint32						vcMessid;
	WPARAM						vcWparam;
	LPARAM						vcLparam;

	void						SendDCMsg();
	void						SetDCMsgParams(uint32, WPARAM, LPARAM);
	uint32						dcMessid;
	WPARAM						dcWparam;
	LPARAM						dcLparam;

	CRect						bounds;
	CRect						frame;

	void						AddValue(void *, char *);
	void						SelectValue(void *);
	void						*Value(long);
	void						ClearValues();
protected:
	DECLARE_MESSAGE_MAP()
};

class MFCQuaSymListBox: public MFCQuaListBox
{
	DECLARE_DYNAMIC(MFCQuaSymListBox)
public:
	void						AddValue(StabEnt *, char *);
	void						SelectValue(StabEnt *);
	StabEnt						*Value(long);
	char						*ValueStr(long);
};


