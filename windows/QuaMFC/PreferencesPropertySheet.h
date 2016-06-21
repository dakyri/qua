// PreferencesPropertySheet.h : header file
//
// PreferencesPropertySheet is a modeless property sheet that is 
// created once and not destroyed until the application
// closes.  It is initialized and controlled from
// CPropertyFrame.
 // PreferencesPropertySheet has been customized to include
// a preview window.
 
#ifndef __PREFERENCESPROPERTYSHEET_H__
#define __PREFERENCESPROPERTYSHEET_H__

#include "MFCPropertyPages.h"
#include "PreviewWnd.h"

/////////////////////////////////////////////////////////////////////////////
// PreferencesPropertySheet

class PreferencesPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(PreferencesPropertySheet)

// Construction
public:
	PreferencesPropertySheet(CWnd* pWndParent = NULL);

// Attributes
public:
	MFCconfigPropertyPage	m_Page1;
	MFCvstPropertyPage		m_Page2;
	MFCmidiPropertyPage		m_Page3;
	MFCaudioPropertyPage	m_Page4;
	CPreviewWnd				m_wndPreview;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PreferencesPropertySheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~PreferencesPropertySheet();
		 virtual BOOL OnInitDialog();
	 virtual void PostNcDestroy();

// Generated message map functions
protected:
	//{{AFX_MSG(PreferencesPropertySheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif	// __PREFERENCESPROPERTYSHEET_H__
