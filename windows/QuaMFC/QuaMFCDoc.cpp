////////////////////////////////////////////////////////////////////////
// QuaMFCDoc.cpp : implementation of the CQuaMFCDoc class
//  This contains the interface to the core functionality
// of a running project/sequence/sequencer
////////////////////////////////////////////////////////////////////////

//#define _AFXDLL
#include "stdafx.h"			// all the bog standard windows bits

#include "qua_version.h"	// file of basic versioning definitions

// the Gloubal headers ... all the useful bits I've got in libraries
#include "StdDefs.h"

// and the Qua headers ....
#include "QuasiStack.h"
#include "BaseVal.h"
#include "QuaDisplay.h"
#include "Qua.h"

// and all the MFC headers for the Qua MFC app ...
#include "QuaMFC.h"
#include "QuaMFCDoc.h"
#include "QuaMFCCCDialog.h"

#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQuaMFCDoc

IMPLEMENT_DYNCREATE(CQuaMFCDoc, CDocument)

BEGIN_MESSAGE_MAP(CQuaMFCDoc, CDocument)
	//{{AFX_MSG_MAP(CQuaMFCDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	ON_COMMAND(ID_EDIT_ADD_SAMPLE,OnAddSample)
	ON_COMMAND(ID_EDIT_ADD_VOICE,OnAddVoice)
	ON_COMMAND(ID_EDIT_ADD_METHOD,OnAddMethod)
	ON_COMMAND(ID_EDIT_ADD_CLIP,OnAddClip)
	ON_COMMAND(ID_EDIT_ADD_MARKER,OnAddMarker)
	ON_BN_CLICKED(ID_PLAY_BUTTON,OnPlayClick)
	ON_BN_CLICKED(ID_STOP_BUTTON,OnStopClick)
	ON_BN_CLICKED(ID_PAUSE_BUTTON,OnPauseClick)
	ON_BN_CLICKED(ID_FF_BUTTON,OnFFClick)
	ON_BN_CLICKED(ID_REWIND_BUTTON,OnRewClick)
	ON_BN_CLICKED(ID_RECORD_BUTTON,OnRecordClick)
	ON_BN_CLICKED(ID_FULL_FORWARD_BUTTON,OnJumpForwardClick)
	ON_BN_CLICKED(ID_FULL_REWIND_BUTTON,OnJumpBackClick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQuaMFCDoc construction/destruction

CQuaMFCDoc::CQuaMFCDoc()
{
	qua = NULL;
}

CQuaMFCDoc::~CQuaMFCDoc()
{
}


/////////////////////////////////////////////////////////////////////////////
// CQuaMFCDoc diagnostics

#ifdef _DEBUG
void CQuaMFCDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CQuaMFCDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CQuaMFCDoc virtual overrides
/////////////////////////////////////////////////////////////////////////////

void
CQuaMFCDoc::DeleteContents()
{
	;
}

BOOL
CQuaMFCDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
// careful here with OnNew called multiply ... but also with it being called if we use SDI
	if (qua == NULL) {
		qua = new Qua("Untitled", display, true);
		qua->PostCreationInit();
	}

	return true;
}

BOOL
CQuaMFCDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
//	if (!CDocument::OnOpenDocument(lpszPathName))
//		return FALSE;

	DeleteContents();
// as above ... careful if we use sdi ... pretty committed to mdi now, though
	if (qua != NULL) {
		return false;
	}

	string docPath(lpszPathName);
	string docext = getExt(docPath);
	cerr << "got doc " << docPath << ", docExt" << docext << endl;
	if (docext == "qs" || docext == "qua") {
		qua = Qua::loadScriptFile(docPath.c_str(), display);
		if (qua) {
			string s(lpszPathName);
			string snapshotPath = getParent(s)+"/"+getBase(s)+"."+"qx";
			qua->loadSnapshotFile(snapshotPath.c_str()); // which may or may not do anything depending on if it exists
		}
	}

	return true;
}

void
CQuaMFCDoc::OnCloseDocument()
{
	CDocument::OnCloseDocument();
}

void
CQuaMFCDoc::OnChangedViewList()
{
	CDocument::OnChangedViewList();
}

void
CQuaMFCDoc::PreCloseFrame(CFrameWnd* pFrame)
{
	CDocument::PreCloseFrame(pFrame);
}

BOOL
CQuaMFCDoc::CanCloseFrame(CFrameWnd* pFrame)
{
	return CDocument::CanCloseFrame(pFrame);
}

BOOL
CQuaMFCDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	if (qua == NULL) {
		return FALSE;
	}
	status_t err = qua->DoSave(lpszPathName);
// this makes the calls via Object::Serialize
//	return CDocument::OnSaveDocument(lpszPathName);
	return err == B_OK;
}

BOOL
CQuaMFCDoc::SaveModified()
{
	return CDocument::SaveModified();
}

// at the moment we are avoiding this, and using the core qua hooks from
// save and open ....
void
CQuaMFCDoc::Serialize(CArchive& ar)
{
	reportError("Serialize");
	if (ar.IsStoring())	{
		// TODO: add storing code here
	} else {
		// TODO: add loading code here
	}
}

void CQuaMFCDoc::OnAddSample()
{
	if (qua) {
		display.CreateSample("New", vector<string>());
		cerr << "unimplemented OnAddSample()" << endl;
	}
}

void CQuaMFCDoc::OnAddVoice()
{
	if (qua) {
		display.CreateVoice("New", vector<string>());
		cerr << "unimplemented OnAddVoice()" << endl;
	}
}

void CQuaMFCDoc::OnAddMethod()
{
	if (qua) {
		display.CreateMethod(NULL, qua->sym);
	}
}

void CQuaMFCDoc::OnAddClip()
{
	if (qua) {
		Time	start;
		Time	dur;
		start = qua->theTime;
		dur.Set("1:0.0");
		display.CreateClip("region", &start, &dur);
	}
}

void CQuaMFCDoc::OnAddMarker()
{
	if (qua) {
		Time	start;
		Time	dur;
		start = qua->theTime;
		dur.ticks = 0;
		display.CreateClip("marker", &start, &dur);
	}
}


void CQuaMFCDoc::OnPlayClick()
{
	if (qua) {
		cerr << "Playing ...\n";
		qua->Start();
	} else {
		cerr << "Play pressed but sequencer is blank ...\n";
	}
}

void CQuaMFCDoc::OnStopClick()
{
	if (qua) {
		cerr <<  "Stopping ...\n";
		qua->Stop();
	} else {
		cerr <<  "Stop pressed but sequencer is blank ...\n";
	}
}

void CQuaMFCDoc::OnPauseClick()
{
	if (qua) {
		cerr <<  "Pausing ...\n";
		qua->Stop();
	} else {
		cerr <<  "Pause pressed but sequencer is blank ...\n";
	}
}

void CQuaMFCDoc::OnFFClick()
{
	if (qua) {
		cerr <<  "Fast forwarding ...\n";
		qua->FastForward();
	} else {
		cerr <<  "Fast forward pressed but sequencer is blank ...\n";
	}
}

void CQuaMFCDoc::OnRewClick()
{
	if (qua) {
		cerr <<  "Rewinding ...\n";
		qua->Rewind();
	} else {
		cerr <<  "Rewind pressed but sequencer is blank ...\n";
	}
}

void CQuaMFCDoc::OnRecordClick()
{
	if (qua) {
		cerr <<  "Recording ...\n";
		qua->StartRecording();
	} else {
		cerr <<  "Record pressed but sequencer is blank ...\n";
	}
}

void CQuaMFCDoc::OnJumpBackClick()
{
	if (qua) {
		cerr <<  "Jump back ...\n";
		qua->ToPreviousMarker();
	} else {
		cerr <<  "Full Rewind pressed but sequencer is blank ...\n";
	}
}

void CQuaMFCDoc::OnJumpForwardClick()
{
	if (qua) {
		cerr <<  "Jump forward ...\n";
		qua->ToNextMarker();
	} else {
		cerr <<  "Full Forward pressed but sequencer is blank ...\n";
	}
}

BOOL
CQuaMFCDoc::IsModified()
{
	return TRUE;
}
