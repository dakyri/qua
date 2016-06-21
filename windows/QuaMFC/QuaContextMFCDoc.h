// QuaContextMFCDoc.h : interface of the CQuaCpntextMFCDoc class
//   this doesn't so much as represent a separate document, as a unifying
// perspective on all the currently open arrangements, CQuaMFCDoc, and
// their relationships to the global resources.
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CQuaContextMFCDoc_H__4643A572_A059_40A0_A0F0_C2827F59D67E__INCLUDED_)
#define AFX_CQuaContextMFCDoc_H__4643A572_A059_40A0_A0F0_C2827F59D67E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Qua;

class CQuaContextMFCDoc : public CDocument
{
protected: // create from serialization only
	CQuaContextMFCDoc();
	DECLARE_DYNCREATE(CQuaContextMFCDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQuaContextMFCDoc)
public:
	//}}AFX_VIRTUAL

// Implementation
public:
	// these look better in blue than gray ;)
	virtual void	OnCloseDocument();
	virtual BOOL	OnNewDocument();
	virtual BOOL	OnOpenDocument(LPCTSTR lpszPathName);
	virtual void	OnChangedViewList();
	virtual BOOL	OnSaveDocument(LPCTSTR lpszPathName);
	virtual void	Serialize(CArchive& ar);
	virtual void	PreCloseFrame(CFrameWnd* pFrame);
	virtual BOOL	CanCloseFrame(CFrameWnd* pFrame);
	virtual BOOL	SaveModified();

	virtual			~CQuaContextMFCDoc();
#ifdef _DEBUG
	virtual void	AssertValid() const;
	virtual void	Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CQuaContextMFCDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CQuaContextMFCDoc_H__4643A572_A059_40A0_A0F0_C2827F59D67E__INCLUDED_)
