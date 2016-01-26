// QuaMFCDoc.h : interface of the CQuaMFCDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUAMFCDOC_H__4643A572_A059_40A0_A0F0_C2827F59D67E__INCLUDED_)
#define AFX_QUAMFCDOC_H__4643A572_A059_40A0_A0F0_C2827F59D67E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Qua;
class QuaMFCCCDialog;

class CQuaMFCDoc : public CDocument
{
protected: // create from serialization only
	CQuaMFCDoc();
	DECLARE_DYNCREATE(CQuaMFCDoc)

// Attributes
public:
	Qua			*qua;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQuaMFCDoc)
	public:
	//}}AFX_VIRTUAL

// Implementation
	virtual void	OnCloseDocument();
	virtual BOOL	OnNewDocument();
	virtual BOOL	OnOpenDocument(LPCTSTR lpszPathName);
	virtual void	OnChangedViewList();
	virtual BOOL	OnSaveDocument(LPCTSTR lpszPathName);
	virtual void	Serialize(CArchive& ar);
	virtual void	PreCloseFrame(CFrameWnd* pFrame);
	virtual BOOL	CanCloseFrame(CFrameWnd* pFrame);
	virtual BOOL	SaveModified();
	virtual BOOL	IsModified();
	virtual void	DeleteContents();

public:
	virtual ~CQuaMFCDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
	afx_msg void			OnAddSample();
	afx_msg void			OnAddVoice();
	afx_msg void			OnAddMethod();
	afx_msg void			OnAddClip();
	afx_msg void			OnAddMarker();

	afx_msg void			OnPlayClick();
	afx_msg void			OnStopClick();
	afx_msg void			OnPauseClick();
	afx_msg void			OnFFClick();
	afx_msg void			OnRewClick();
	afx_msg void			OnJumpForwardClick();
	afx_msg void			OnJumpBackClick();
	afx_msg void			OnRecordClick();

protected:
	//{{AFX_MSG(CQuaMFCDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUAMFCDOC_H__4643A572_A059_40A0_A0F0_C2827F59D67E__INCLUDED_)
