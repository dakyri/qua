#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE
#include "stdafx.h"
#include "qua_version.h"
// MyPropertyPage1.cpp : implementation file
//
//#define _AFXDLL
#include "resource.h"

#include "StdDefs.h"

#ifdef QUA_V_AUDIO_ASIO
#include "QuaAsio.h"

#include "QuaAudio.h"
#endif
#include "MFCPropertyPages.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(MFCconfigPropertyPage, CPropertyPage)
IMPLEMENT_DYNCREATE(MFCvstPropertyPage, CPropertyPage)
IMPLEMENT_DYNCREATE(MFCmidiPropertyPage, CPropertyPage)
IMPLEMENT_DYNCREATE(MFCaudioPropertyPage, CPropertyPage)


/////////////////////////////////////////////////////////////////////////////
// CMyPropertyPage1 property page

MFCconfigPropertyPage::MFCconfigPropertyPage() : CPropertyPage(MFCconfigPropertyPage::IDD)
{
	//{{AFX_DATA_INIT(CMyPropertyPage1)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

MFCconfigPropertyPage::~MFCconfigPropertyPage()
{
}

void MFCconfigPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(MFCconfigPropertyPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BOOL
MFCconfigPropertyPage::OnInitDialog()
{
	return CPropertyPage::OnInitDialog();
}


BEGIN_MESSAGE_MAP(MFCconfigPropertyPage, CPropertyPage)
	//{{AFX_MSG_MAP(MFCconfigPropertyPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// MFCVSTPropertyPage property page

MFCvstPropertyPage::MFCvstPropertyPage() : CPropertyPage(MFCvstPropertyPage::IDD)
{
	//{{AFX_DATA_INIT(MFCvstPropertyPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

MFCvstPropertyPage::~MFCvstPropertyPage()
{
}

void MFCvstPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(MFCvstPropertyPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BOOL
MFCvstPropertyPage::OnInitDialog()
{
	return CPropertyPage::OnInitDialog();
}


BEGIN_MESSAGE_MAP(MFCvstPropertyPage, CPropertyPage)
	//{{AFX_MSG_MAP(MFCvstPropertyPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// MFCmidiPropertyPage property page

MFCmidiPropertyPage::MFCmidiPropertyPage() : CPropertyPage(MFCmidiPropertyPage::IDD)
{
	//{{AFX_DATA_INIT(MFCMIDIPropertyPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

MFCmidiPropertyPage::~MFCmidiPropertyPage()
{
}

void MFCmidiPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(MFCMIDIPropertyPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BOOL
MFCmidiPropertyPage::OnInitDialog()
{
	return CPropertyPage::OnInitDialog();
}


BEGIN_MESSAGE_MAP(MFCmidiPropertyPage, CPropertyPage)
	//{{AFX_MSG_MAP(MFCMIDIPropertyPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// MFCAudioPropertyPage property page

MFCaudioPropertyPage::MFCaudioPropertyPage() : CPropertyPage(MFCaudioPropertyPage::IDD)
{
	//{{AFX_DATA_INIT(MFCAudioPropertyPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

MFCaudioPropertyPage::~MFCaudioPropertyPage()
{
}

void MFCaudioPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(MFCaudioPropertyPage)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_ASIO_DRIVER_SELECT, asioDriverSelect);
}


BEGIN_MESSAGE_MAP(MFCaudioPropertyPage, CPropertyPage)
	//{{AFX_MSG_MAP(MFCaudioPropertyPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_ASIO_CTL, OnBnClickedAsioControl)
	ON_CBN_SELCHANGE(IDC_ASIO_DRIVER_SELECT, OnCbnSelchangeAsioDriverSelect)
END_MESSAGE_MAP()



void MFCaudioPropertyPage::OnBnClickedAsioControl()
{
#ifdef QUA_V_AUDIO_ASIO
	ASIOControlPanel();
#endif
}

BOOL
MFCaudioPropertyPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	
	asioDriverSelect.ResetContent();
#ifdef QUA_V_AUDIO_ASIO
	int32		na = QuaAudioManager::asio.nDrivers;
	char		*cd = QuaAudioManager::asio.CurrentDriver();
	for (long i=0; i<na; i++) {
		char	* p = QuaAudioManager::asio.DriverName(i);
		if (p && *p) {
			long ind = asioDriverSelect.AddString(p);
			if (ind >= 0) {
				asioDriverSelect.SetItemData(ind, i);
			}
		}
	}
	if (cd) {
		asioDriverSelect.SelectString(-1, cd);
	}
#endif
	return TRUE;
}

void MFCaudioPropertyPage::OnCbnSelchangeAsioDriverSelect()
{
#ifdef QUA_V_AUDIO_ASIO
	long sel = asioDriverSelect.GetCurSel();
	if (sel >= 0) {
		long dsel = asioDriverSelect.GetItemData(sel);

		char	*drp = QuaAudioManager::asio.DriverName(dsel);
		if (drp != NULL) {
			status_t err = QuaAudioManager::asio.LoadDriver(drp);
			if (err != ASE_OK) {
				// asio subsystem should give feasible enough error msg;
			} else {
				QuaAudioManager::asio.SetPreferredDriver(drp);
			}
		}
	}
#endif
}
