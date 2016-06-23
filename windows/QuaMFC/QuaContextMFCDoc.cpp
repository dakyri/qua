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
#include "Qua.h"

// and all the MFC headers for the Qua MFC app ...
#include "QuaMFC.h"
#include "QuaContextMFCDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQuaMFCDoc

IMPLEMENT_DYNCREATE(CQuaContextMFCDoc, CDocument)

BEGIN_MESSAGE_MAP(CQuaContextMFCDoc, CDocument)
	//{{AFX_MSG_MAP(CQuaContextMFCDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQuaContextMFCDoc construction/destruction

CQuaContextMFCDoc::CQuaContextMFCDoc()
{

}

CQuaContextMFCDoc::~CQuaContextMFCDoc()
{
}




/////////////////////////////////////////////////////////////////////////////
// CQuaContextMFCDoc serialization

void
CQuaContextMFCDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CQuaMFCDoc diagnostics

#ifdef _DEBUG
void
CQuaContextMFCDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CQuaContextMFCDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CQuaContextMFCDoc virtual overrides
BOOL
CQuaContextMFCDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	return true;
}

BOOL
CQuaContextMFCDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	return true;
}

void
CQuaContextMFCDoc::OnCloseDocument()
{
	theApp.contextDocument = NULL;
	CDocument::OnCloseDocument();
}

void
CQuaContextMFCDoc::OnChangedViewList()
{
	CDocument::OnChangedViewList();
}

void
CQuaContextMFCDoc::PreCloseFrame(CFrameWnd* pFrame)
{
	CDocument::PreCloseFrame(pFrame);
}

BOOL
CQuaContextMFCDoc::CanCloseFrame(CFrameWnd* pFrame)
{
	return CDocument::CanCloseFrame(pFrame);
}

// save should make changes to arragement docs
BOOL
CQuaContextMFCDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	return CDocument::OnSaveDocument(lpszPathName);
}

BOOL
CQuaContextMFCDoc::SaveModified()
{
	return CDocument::SaveModified();
}
