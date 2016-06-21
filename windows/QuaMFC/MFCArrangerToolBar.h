#pragma once


// MFCArrangerToolBar

class MFCArrangerToolBar : public CToolBar
{
	DECLARE_DYNAMIC(MFCArrangerToolBar)

public:
	MFCArrangerToolBar();
	virtual ~MFCArrangerToolBar();

	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
protected:
	DECLARE_MESSAGE_MAP()
};
