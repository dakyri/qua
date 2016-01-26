#include "qua_version.h"

// QuaMFC.cpp : Defines the class behaviors for the application.
//
// For a bog standard MDI version of Qua, register it as aCMultiDocTemplate:
//	* type IDR_QUAMFCTYPE,
//	* class for a Qua document: CQuaMFCDoc,
//	* class for the MDI child frame: QuaChildFrame
//  * class for the view on the document CQuaMFCView

#include "stdafx.h"
#ifdef QUA_V_GDI_PLUS
#include <gdiplus.h>
using namespace Gdiplus;
GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR gdiplusToken;
#endif

#include "DaBasicTypes.h"
#include "DaList.h"
#include "DaErrorCodes.h"
#include "DaKernel.h"
#include "DaPath.h"
#include "DaFile.h"
#include "DaMimeType.h"
#include "DaRect.h"
//#include "DaMessage.h"
//#include "DaApplication.h"
//#include "DaAudio.h"
//#include "DaPoint.h"
#include "QuaMFC.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "QuaMFCDoc.h"
#include "MFCArrangeView.h"
#include "QuaContextMFCFrame.h"
#include "QuaContextMFCDoc.h"
#include "MFCContextView.h"
#include "QuaMFCView.h"
#include "MFCDeviceSelector.h"
#include "MFCChannelView.h"
#include "MFCObjectView.h"
#include "MFCQuaButton.h"
#include "MFCQuaRotor.h"
#include "MFCQuaTransportBar.h"
#include "MFCQuaTimeCtrl.h"
#include "MFCQuaFloatCtrl.h"
#include "MFCBlockEdit.h"
#include "MFCQuaStateDisplay.h"
#include "MFCStreamDataEditor.h"
#include "MFCSampleDataEditor.h"
#include "MFCObjectView.h"
#include "Splash.h"
#include "QuaDrop.h"

////////////////////////////////////////
// Qua headers
////////////////////////////////////////


#include "inx/Qua.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

FILE *trace_fp;

HCURSOR pointCursor;
HCURSOR drawCursor;
HCURSOR pointCtlCursor;
HCURSOR drawCtlCursor;
HCURSOR regionCursor;
HCURSOR sliceCursor;
long	currentTool;
LPCSTR	quaPopupClassName;

class QuaCommandLineInfo: public CCommandLineInfo, public QuaCommandLine
{
public:
	QuaCommandLineInfo();

	virtual void ParseParam(
			const char* pszParam,
			BOOL bFlag,
			BOOL bLast
		);
	long	argCount;
};

////////////////////////////////////////
// Qua globals
////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CQuaMFCApp

BEGIN_MESSAGE_MAP(CQuaMFCApp, CWinApp)
	//{{AFX_MSG_MAP(CQuaMFCApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	ON_COMMAND(ID_SHOW_CONTEXT_WINDOW, OnShowContextWindow)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQuaMFCApp construction

CQuaMFCApp::CQuaMFCApp()
{
	trace_fp = fopen("dbfile", "w");
	setbuf(trace_fp, NULL);
	fprintf(trace_fp, "qua mfc 0.93 debug listing\n");
	pArrangementTemplate = NULL;
	pContextTemplate = NULL;
	contextDocument = NULL;
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
#ifdef QUA_V_MULTIMEDIA_TIMER
	TIMECAPS tc;

	if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR) {
		ReportError("Failed to access multimedia timer capabilities");
	}

	wTimerRes = min(max(tc.wPeriodMin, 1), tc.wPeriodMax);
	fprintf(stderr, "mm resolution %dms", wTimerRes);
	if (timeBeginPeriod(wTimerRes) != TIMERR_NOERROR) {
		ReportError("Failed to set requested timer resolution %dms", wTimerRes); 
	}
#endif
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CQuaMFCApp object
CQuaMFCApp theApp;
extern void * hInstance;
/////////////////////////////////////////////////////////////////////////////
// CQuaMFCApp initialization
// Place all significant initialization in InitInstance
BOOL
CQuaMFCApp::InitInstance()
{
	quaAppInstance = m_hInstance;
	hInstance = m_hInstance;
	// CG: The following block was added by the Splash Screen component.
#ifdef QUA_V_DEBUG_CONSOLE
	AllocConsole();
	freopen("conin$", "r", stdin); 
	freopen("conout$", "w", stdout); 
	freopen("conout$", "w", stderr); 

#endif
	QuaCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	cmdInfo.ListingCommands();
	CSplashWnd::EnableSplashScreen(cmdInfo.m_bShowSplash);
	AfxEnableControlContainer();

////////////////////////////////////////////////////////
// global initializations for Qua
	define_global_symbols();
//	VstPlugin::LoadTest(
//		"F:/progs/VstPlugins/vb1.dll"
//		"F:/progs/VstPlugins/Native Instruments/Absynth 3.dll"
//		);
//	exit(0);
	context.Setup();	// some app setup may rely on command line
	context.SetupDevices();
////////////////////////////////////////////////////////

#ifdef QUA_V_GDI_PLUS
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
#endif

	if (!QuaDrop::Initialize()) {
		ReportError("AfxOleInit failed.  Could not initialized OLE 2; Ole2View cannot run.") ;
		return FALSE;
	}


	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	SetRegistryKey(_T("Maya Swall Digital Media Qua"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

////////////////////////////////////////////////////
// set default fonts                              //
////////////////////////////////////////////////////

	if (!MFCChannelView::displayFont.CreatePointFont(70, "Arial")) {
		ReportError("can't make channel view font");
	}
	if (!MFCObjectView::displayFont.CreatePointFont(90, "Arial")) {
		ReportError("can't make object view font");
	}
	if (!MFCStackFrameView::displayFont.CreatePointFont(90, "Arial")) {
		ReportError("can't make frame view font");
	}
	if (!MFCDeviceSelector::displayFont.CreatePointFont(70, "Arial")) {
		ReportError("can't make selector font");
	}
	if (!MFCQuaButton::displayFont.CreatePointFont(120, "Arial")) {
		ReportError("can't make button font");
	}
	if (!MFCQuaChkButton::displayFont.CreatePointFont(120, "Arial")) {
		ReportError("can't make check button font");
	}
	if (!MFCSmallQuaChkBut::displayFont.CreatePointFont(70, "Arial")) {
		ReportError("can't make small button font");
	}
	if (!MFCQuaRotor::displayFont.CreatePointFont(120, "Arial")) {
		ReportError("can't make large rotor font");
	}
	if (!MFCSmallQuaRotor::displayFont.CreatePointFont(70, "Arial")) {
		ReportError("can't make small rotor font");
	}
	if (!MFCQuaTransportBar::displayFont.CreatePointFont(80, "Arial")) {
		ReportError("can't make time ctrl font");
	}
	if (!MFCQuaTransportBar::labelFont.CreatePointFont(90, "Arial")) {
		ReportError("can't make time ctrl font");
	}
	if (!MFCQuaTextCtrl::defaultDisplayFont.CreatePointFont(90, "Arial")) {
		ReportError("can't make time ctrl font");
	}
	if (!MFCBlockEdit::displayFont.CreatePointFont(90, "Arial")) {
		ReportError("can't make block ctrl font");
	}
	if (!MFCQuaStateDisplay::displayFont.CreatePointFont(70, "Arial")) {
		ReportError("can't make state display font");
	}
	if (!MFCQuaClipController::displayFont.CreatePointFont(80, "Arial")) {
		ReportError("can't make state display font");
	}
	if (!MFCQuaChannelController::displayFont.CreatePointFont(80, "Arial")) {
		ReportError("can't make state display font");
	}
	if (!MFCQuaVstProgramController::displayFont.CreatePointFont(80, "Arial")) {
		ReportError("can't make state display font");
	}
	if (!MFCQuaTimeController::displayFont.CreatePointFont(70, "Arial")) {
		ReportError("can't make time display font");
	}
	if (!MFCQuaIntController::displayFont.CreatePointFont(70, "Arial")) {
		ReportError("can't make int display font");
	}
	if (!MFCStreamEditorYScale::displayFont.CreatePointFont(60, "Arial")) {
		ReportError("can't make editor x scale display font");
	}
	if (!MFCDataEditorTScale::displayFont.CreatePointFont(60, "Arial")) {
		ReportError("can't make editor t scale display font");
	}
	if (!MFCSmallQuaLetterBut::displayFont.CreatePointFont(60, "Arial")) {
		ReportError("can't make small button display font");
	}

////////////////////////////////////////////////////
// load default bitmaps from resources            //
////////////////////////////////////////////////////

	if (!MFCDeviceSelector::midiImg.LoadBitmap(IDB_ICO_SM_MDI)) {
		ReportError("Can't load small midi bitmap");
	}
	if (!MFCDeviceSelector::audioImg.LoadBitmap(IDB_ICO_SM_AUD)) {
		ReportError("Can't load small audio bitmap");
	}
	if (!MFCDeviceSelector::joyImg.LoadBitmap(IDB_ICO_SM_JOY)) {
		ReportError("Can't load small joystick bitmap");
	}
	if (!MFCDeviceSelector::otherImg.LoadBitmap(IDB_ICO_SM_NON)) {
		ReportError("Can't load small null device bitmap");
	}
	if (!MFCChannelView::inArrowImg.LoadBitmap(IDB_ARROW_R_SM)) {
		ReportError("Can't load small arrow bitmap");
	}
	if (!MFCChannelView::outArrowImg.LoadBitmap(IDB_ARROW_L_SM)) {
		ReportError("Can't load small arrow bitmap");
	}

////////////////////////////////////////////////////
// load specific cursors from resources           //
////////////////////////////////////////////////////
	if ((pointCursor=LoadCursor(IDC_ARTOOL_POINT)) == NULL) {
		ReportError("Can't load custom point cursor");
	}
	if ((drawCursor=LoadCursor(IDC_ARTOOL_DRAW)) == NULL) {
		ReportError("Can't load custom draw cursor");
	}
	if ((pointCtlCursor=LoadCursor(IDC_ARTOOL_POINT_CTL)) == NULL) {
		ReportError("Can't load custom point control cursor");
	}
	if ((drawCtlCursor=LoadCursor(IDC_ARTOOL_DRAW_CTL)) == NULL) {
		ReportError("Can't load custom draw control cursor");
	}
	if ((regionCursor=LoadCursor(IDC_ARTOOL_REGION)) == NULL) {
		ReportError("Can't load custom region cursor");
	}
	if ((sliceCursor=LoadCursor(IDC_ARTOOL_SLICE)) == NULL) {
		ReportError("Can't load custom slice cursor");
	}
	quaPopupClassName = AfxRegisterWndClass(0, 0, (HBRUSH)(COLOR_INFOBK + 1), 0);             // [1]

// initializations for other bits of MFC stuff

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	pArrangementTemplate = new CMultiDocTemplate(
		IDR_QUAMFCTYPE,
		RUNTIME_CLASS(CQuaMFCDoc),
		RUNTIME_CLASS(QuaChildFrame), // custom MDI child frame
		RUNTIME_CLASS(MFCArrangeView));
	AddDocTemplate(pArrangementTemplate);

	pContextTemplate = new CMultiDocTemplate(
		IDR_QUACONTEXTTYPE,
		RUNTIME_CLASS(CQuaContextMFCDoc),
		RUNTIME_CLASS(QuaContextFrame), // custom MDI child frame
		RUNTIME_CLASS(MFCContextView));
	AddDocTemplate(pContextTemplate);

	// create main MDI Frame window
	QuaMainFrame* pMainFrame = new QuaMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

int
CQuaMFCApp::ExitInstance( )
{
#ifdef QUA_V_GDI_PLUS
	GdiplusShutdown(gdiplusToken);
#endif
#ifdef QUA_V_MULTIMEDIA_TIMER
	if (timeEndPeriod(wTimerRes) != TIMERR_NOERROR) {
		ReportError("Failed to multimedia timer period"); 
	}
#endif
	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void
CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void
CQuaMFCApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CQuaMFCApp message handlers


BOOL
CQuaMFCApp::PreTranslateMessage(MSG* pMsg)
{
	// CG: The following lines were added by the Splash Screen component.
	if (CSplashWnd::PreTranslateAppMessage(pMsg))
		return TRUE;
//	CWnd	* pWnd = CWnd::FromHandle(pMsg->hwnd);
//	if (pWnd) {
//		fprintf(stderr, "tm hw %x %d\n", pWnd->m_hWnd, ::IsWindow(pWnd->m_hWnd));
//		CWnd	*ppWnd = pWnd->GetTopLevelParent();
//		if (ppWnd) {
//			fprintf(stderr, "top level parent of tm hw %x %d\n", ppWnd->m_hWnd, ::IsWindow(pWnd->m_hWnd));
//		}
//	}
	return CWinApp::PreTranslateMessage(pMsg);
}

void
CQuaMFCApp::OnShowContextWindow()
{
// lot of trouble just to keep a single view up for the global context ... maybe multiple views
// on it would be good anyway.
	FILE *dbf = fopen("crap", "a");
	if (contextDocument == NULL) {
		if (pContextTemplate) {
			contextDocument = pContextTemplate->OpenDocumentFile(NULL, true);
		}
	} else {
		POSITION	pos = contextDocument->GetFirstViewPosition();
		CView		*ctxtView = contextDocument->GetNextView(pos);
		if (ctxtView == NULL) {
			if (pContextTemplate) {
				CFrameWnd *f = pContextTemplate->CreateNewFrame(contextDocument, NULL);
				f->ActivateFrame();
			}
		} else {
			// it should be there somewhere ... there can be only one ... 
		}
	}
	fclose(dbf);
}

QuaCommandLineInfo::QuaCommandLineInfo()
{
	argCount = 0;
}

void
QuaCommandLineInfo::ParseParam(
			const char* pszParam,
			BOOL bFlag,
			BOOL bLast
		)
{
	if (!ProcessCommandLineWord(argCount, (char *)pszParam, bFlag)) {
		CCommandLineInfo::ParseParam(pszParam, bFlag, bLast);
	}
	argCount++;
}

