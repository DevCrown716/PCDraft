#if !defined(AFX_SYMBOLFRAME_H__6804A9A6_ED0C_11D2_993C_000000000000__INCLUDED_)
#define AFX_SYMBOLFRAME_H__6804A9A6_ED0C_11D2_993C_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SymbolFrame.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSymbolFrame frame

class CSymbolFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CSymbolFrame)
protected:
	CSymbolFrame();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSymbolFrame)
   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CSymbolFrame();

	// Generated message map functions
	//{{AFX_MSG(CSymbolFrame)
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct); // Interface Update - Infinisys Ltd.
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYMBOLFRAME_H__6804A9A6_ED0C_11D2_993C_000000000000__INCLUDED_)
