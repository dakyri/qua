// QuaMFC.h : main header file for the QUAMFC application
//



#if !defined(AFX_QUAMFC_H__8524E1C5_0286_4D77_B895_3FEF72594F85__INCLUDED_)
#define AFX_QUAMFC_H__8524E1C5_0286_4D77_B895_3FEF72594F85__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CQuaMFCApp:
// See QuaMFC.cpp for the implementation of this class
//

class CQuaMFCApp : public CWinApp
{
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CQuaMFCApp();

	CMultiDocTemplate*		pArrangementTemplate;
	CMultiDocTemplate*		pContextTemplate;
	CDocument*				contextDocument;

#ifdef QUA_V_MULTIMEDIA_TIMER
	UINT					wTimerRes;
#endif

	afx_msg void			OnShowContextWindow();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQuaMFCApp)
	public:
	virtual BOOL			InitInstance();
	virtual int				ExitInstance( );
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CQuaMFCApp)
	afx_msg void			OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CQuaMFCApp theApp;

extern HCURSOR pointCursor;
extern HCURSOR drawCursor;
extern HCURSOR pointCtlCursor;
extern HCURSOR drawCtlCursor;
extern HCURSOR regionCursor;
extern HCURSOR sliceCursor;
extern long currentTool;
extern LPCTSTR quaPopupClassName;


#ifdef WIN32
extern HINSTANCE quaAppInstance;
extern HMODULE quaAppModule;
#endif

#include <string>
using namespace std;

extern void ReportError(string fmt, ...);

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUAMFC_H__8524E1C5_0286_4D77_B895_3FEF72594F85__INCLUDED_)
