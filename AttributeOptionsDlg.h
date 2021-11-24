#if !defined(AFX_ATTRIBUTEOPTIONSDLG_H__708907F2_5AC9_11D2_B47D_C0AF247977E2__INCLUDED_)
#define AFX_ATTRIBUTEOPTIONSDLG_H__708907F2_5AC9_11D2_B47D_C0AF247977E2__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// AttributeOptionsDlg.h : header file
//
#include "FillWnd.h"
#include "LineStyleWnd.h"

/////////////////////////////////////////////////////////////////////////////
// CAttributeOptionsDlg dialog

class CPCDraftDoc;
struct ShapeAttributes;

class CAttributeOptionsDlg : public CDialog
{
// Construction
public:
	CAttributeOptionsDlg(CPCDraftDoc* pDoc,ShapeAttributes* pAttr,bool bApply=false,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAttributeOptionsDlg)
	enum { IDD = IDD_ATTRIBUTE_OPTIONS };
	BOOL	m_bUseFill;
	CFillWnd m_curFill;
	BOOL	m_bUsePen;
	CLineStyleWnd m_curPen;
	BOOL	m_bUseInfo;
	CString	m_sLabel1;
	CString	m_sValue1;
	CString	m_sLabel2;
	CString	m_sValue2;
	CString	m_sLabel3;
	CString	m_sValue3;
	CString	m_sLabel4;
	CString	m_sValue4;
	CString	m_sLabel5;
	CString	m_sValue5;
	BOOL	m_bUsePara;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAttributeOptionsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CPCDraftDoc* m_pDoc;
	ShapeAttributes* m_pAttr;
	bool m_bApply;

	// Generated message map functions
	//{{AFX_MSG(CAttributeOptionsDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATTRIBUTEOPTIONSDLG_H__708907F2_5AC9_11D2_B47D_C0AF247977E2__INCLUDED_)
