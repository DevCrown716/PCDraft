#if !defined(AFX_EDITDIMSTANDARDDLG_H__24528AA6_78C9_11D2_B4C9_8A8CF35AB464__INCLUDED_)
#define AFX_EDITDIMSTANDARDDLG_H__24528AA6_78C9_11D2_B4C9_8A8CF35AB464__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditDimStandardDlg.h : header file
//
#include "DimStructs.h"
#include "NumberEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CEditDimStandardDlg dialog

class CEditDimStandardDlg : public CDialog
{
// Construction
public:
	CEditDimStandardDlg(DimStandard* curStandard,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditDimStandardDlg)
	enum { IDD = IDD_EDIT_DIM_STANDARD };
	CNumberEdit	m_WitExt;
	CNumberEdit	m_OutsideLineLen;
	CNumberEdit	m_CircleCenterExt;
	CNumberEdit	m_CircleCenterGap;
	CNumberEdit	m_CircleCenterLen;
	CNumberEdit	m_DimTextGap;
	CNumberEdit	m_LeaderLen;
	CNumberEdit	m_TolSpaceScale;
	CNumberEdit	m_TolTextScale;
	CNumberEdit	m_WitLineGap;
	CString	m_Units;
	CString	m_Standard;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditDimStandardDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	DimStandard* m_pDimStd;
	DimStandard m_workStd;

	void UpdateValues(bool bSaveAndValidate=true);

	// Generated message map functions
	//{{AFX_MSG(CEditDimStandardDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITDIMSTANDARDDLG_H__24528AA6_78C9_11D2_B4C9_8A8CF35AB464__INCLUDED_)
