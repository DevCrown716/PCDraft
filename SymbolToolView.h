#if !defined(AFX_SYMBOLTOOLVIEW_H__18A9E214_D954_11D3_AFA6_000000000000__INCLUDED_)
#define AFX_SYMBOLTOOLVIEW_H__18A9E214_D954_11D3_AFA6_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SymbolToolView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSymbolToolView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CSymbolLibraryDoc;

class CSymbolToolView : public CFormView
{
protected:
	CSymbolToolView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CSymbolToolView)

// Form Data
public:
	//{{AFX_DATA(CSymbolToolView)
	enum { IDD = IDD_SYMBOL_TOOL_FORM };
	CButton	m_Unscaled;
	CStatic	m_ScaleUnits;
	CButton	m_Place;
	CComboBox	m_List;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	CSymbolLibraryDoc* GetDocument();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSymbolToolView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CSymbolToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CSymbolToolView)
	afx_msg void OnResizeMore();
	afx_msg void OnSelchangeList();
	afx_msg void OnPlace();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYMBOLTOOLVIEW_H__18A9E214_D954_11D3_AFA6_000000000000__INCLUDED_)
