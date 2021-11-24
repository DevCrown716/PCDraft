#if !defined(AFX_SYMBOLTOOLFRAME_H__18A9E213_D954_11D3_AFA6_000000000000__INCLUDED_)
#define AFX_SYMBOLTOOLFRAME_H__18A9E213_D954_11D3_AFA6_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SymbolToolFrame.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSymbolToolFrame frame

class CSymbolToolFrame : public CMiniFrameWnd
{
	DECLARE_DYNCREATE(CSymbolToolFrame)
protected:
	CSymbolToolFrame();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSymbolToolFrame)
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CSymbolToolFrame();

	// Generated message map functions
	//{{AFX_MSG(CSymbolToolFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYMBOLTOOLFRAME_H__18A9E213_D954_11D3_AFA6_000000000000__INCLUDED_)
