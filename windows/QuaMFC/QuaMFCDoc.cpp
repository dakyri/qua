////////////////////////////////////////////////////////////////////////
// QuaMFCDoc.cpp : implementation of the CQuaMFCDoc class
//  This contains the interface to the core functionality
// of a running project/sequence/sequencer
////////////////////////////////////////////////////////////////////////

#define _AFXDLL
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
		qua = new Qua("Untitled", true);
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
	if (docext == "qs" || docext == "qua") {
		qua = Qua::LoadScriptFile(docPath.c_str());
		if (qua) {
			string s(lpszPathName);
			string snapshotPath = getParent(s)+"/"+getBase(s)+"."+"qx";
			qua->LoadSnapshotFile(snapshotPath.c_str());
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
	ReportError("Serialize");
	if (ar.IsStoring())	{
		// TODO: add storing code here
	} else {
		// TODO: add loading code here
	}
}

void CQuaMFCDoc::OnAddSample()
{
	if (qua && qua->bridge.display) {
//		qua->bridge.display->CreateSample();
		cerr << "unimplemented OnAddSample()" << endl;
	}
}

void CQuaMFCDoc::OnAddVoice()
{
	if (qua && qua->bridge.display) {
//		qua->bridge.display->CreateVoice();
		cerr << "unimplemented OnAddVoice()" << endl;
	}
}

void CQuaMFCDoc::OnAddMethod()
{
	if (qua && qua->bridge.display) {
		qua->bridge.display->CreateMethod(NULL, qua->sym);
	}
}

void CQuaMFCDoc::OnAddClip()
{
	if (qua && qua->bridge.display) {
		Time	start;
		Time	dur;
		start = qua->theTime;
		dur.Set("1:0.0");
		qua->bridge.display->CreateClip("region", &start, &dur);
	}
}

void CQuaMFCDoc::OnAddMarker()
{
	if (qua && qua->bridge.display) {
		Time	start;
		Time	dur;
		start = qua->theTime;
		dur.ticks = 0;
		qua->bridge.display->CreateClip("marker", &start, &dur);
	}
}


void CQuaMFCDoc::OnPlayClick()
{
	if (qua) {
		fprintf(stderr, "Playing ...\n");
		qua->Start();
	} else {
		fprintf(stderr, "Play pressed but sequencer is blank ...\n");
	}
}

void CQuaMFCDoc::OnStopClick()
{
	if (qua) {
		fprintf(stderr, "Stopping ...\n");
		qua->Stop();
	} else {
		fprintf(stderr, "Stop pressed but sequencer is blank ...\n");
	}
}

void CQuaMFCDoc::OnPauseClick()
{
	if (qua) {
		fprintf(stderr, "Pausing ...\n");
		qua->Stop();
	} else {
		fprintf(stderr, "Pause pressed but sequencer is blank ...\n");
	}
}

void CQuaMFCDoc::OnFFClick()
{
	if (qua) {
		fprintf(stderr, "Fast forwarding ...\n");
		qua->FastForward();
	} else {
		fprintf(stderr, "Fast forward pressed but sequencer is blank ...\n");
	}
}

void CQuaMFCDoc::OnRewClick()
{
	if (qua) {
		fprintf(stderr, "Rewinding ...\n");
		qua->Rewind();
	} else {
		fprintf(stderr, "Rewind pressed but sequencer is blank ...\n");
	}
}

void CQuaMFCDoc::OnRecordClick()
{
	if (qua) {
		fprintf(stderr, "Recording ...\n");
		qua->StartRecording();
	} else {
		fprintf(stderr, "Record pressed but sequencer is blank ...\n");
	}
}

void CQuaMFCDoc::OnJumpBackClick()
{
	if (qua) {
		fprintf(stderr, "Jump back ...\n");
		qua->ToPreviousMarker();
	} else {
		fprintf(stderr, "Full Rewind pressed but sequencer is blank ...\n");
	}
}

void CQuaMFCDoc::OnJumpForwardClick()
{
	if (qua) {
		fprintf(stderr, "Jump forward ...\n");
		qua->ToNextMarker();
	} else {
		fprintf(stderr, "Full Forward pressed but sequencer is blank ...\n");
	}
}

BOOL
CQuaMFCDoc::IsModified()
{
	return TRUE;
}
