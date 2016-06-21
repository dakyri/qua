// MyPropertyPage1.h : header file
//

#ifndef __MYPROPERTYPAGE1_H__
#define __MYPROPERTYPAGE1_H__
#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
// MFCConfigPropertyPage dialog

class MFCconfigPropertyPage : public CPropertyPage
{
	DECLARE_DYNCREATE(MFCconfigPropertyPage)

// Construction
public:
	MFCconfigPropertyPage();
	~MFCconfigPropertyPage();

// Dialog Data
	//{{AFX_DATA(MFCconfigPropertyPage)
	enum { IDD = IDD_PROP_CONFIG };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(MFCconfigPropertyPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(MFCconfigPropertyPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////
// MFCvstPropertyPage dialog

class MFCvstPropertyPage : public CPropertyPage
{
	DECLARE_DYNCREATE(MFCvstPropertyPage)

// Construction
public:
	MFCvstPropertyPage();
	~MFCvstPropertyPage();

// Dialog Data
	//{{AFX_DATA(MFCvstPropertyPage)
	enum { IDD = IDD_PROP_VST };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(MFCvstPropertyPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(MFCvstPropertyPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////
// MFCmidiPropertyPage dialog

class MFCmidiPropertyPage : public CPropertyPage
{
	DECLARE_DYNCREATE(MFCmidiPropertyPage)

// Construction
public:
	MFCmidiPropertyPage();
	~MFCmidiPropertyPage();

// Dialog Data
	//{{AFX_DATA(MFCmidiPropertyPage)
	enum { IDD = IDD_PROP_MIDI };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(MFCmidiPropertyPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(MFCmidiPropertyPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////
// MFCaudioPropertyPage dialog

class MFCaudioPropertyPage : public CPropertyPage
{
	DECLARE_DYNCREATE(MFCaudioPropertyPage)

// Construction
public:
	MFCaudioPropertyPage();
	~MFCaudioPropertyPage();

// Dialog Data
	//{{AFX_DATA(MFCaudioPropertyPage)
	enum { IDD = IDD_PROP_AUDIO };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(MFCaudioPropertyPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(MFCaudioPropertyPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedAsioControl();
	CComboBox asioDriverSelect;
	afx_msg void OnCbnSelchangeAsioDriverSelect();
};



#endif // __MYPROPERTYPAGE1_H__
