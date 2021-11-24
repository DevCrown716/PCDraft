#if !defined(AFX_OBJECTINFODLG_H__53D43F42_5316_11D2_B466_B32F0398D7A4__INCLUDED_)
#define AFX_OBJECTINFODLG_H__53D43F42_5316_11D2_B466_B32F0398D7A4__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ObjectInfoDlg.h : header file
//

#include "MDShape.h"

/////////////////////////////////////////////////////////////////////////////
// CObjectInfoDlg dialog

class CPCDraftDoc;

class CObjectInfoDlg : public CDialog
{
// Construction
public:
	CObjectInfoDlg(CPCDraftDoc* pDoc,CMDShape* pSingleShape,CWnd* pParent = NULL);
	CObjectInfoDlg(CPCDraftDoc* pDoc,CMDShapeList* pShapes,CWnd* pParent = NULL);

	CMDShapeList* m_pShapes;
	int m_nCount;
	CMDShape* m_pSingleShape;

// Dialog Data
	//{{AFX_DATA(CObjectInfoDlg)
	enum { IDD = IDD_OBJECT_INFO };
	CString	m_sLabel1;
	CString	m_sLabel2;
	CString	m_sLabel3;
	CString	m_sLabel4;
	CString	m_sLabel5;
	CString	m_sValue1;
	CString	m_sValue2;
	CString	m_sValue3;
	CString	m_sValue4;
	CString	m_sValue5;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CObjectInfoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CObjectInfoDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTINFODLG_H__53D43F42_5316_11D2_B466_B32F0398D7A4__INCLUDED_)
