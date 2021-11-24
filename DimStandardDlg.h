#if !defined(AFX_DIMSTANDARDDLG_H__24528AA4_78C9_11D2_B4C9_8A8CF35AB464__INCLUDED_)
#define AFX_DIMSTANDARDDLG_H__24528AA4_78C9_11D2_B4C9_8A8CF35AB464__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DimStandardDlg.h : header file
//
#include "DimStructs.h"
#include "NumberEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CDimStandardDlg dialog

class CDimStandardDlg : public CDialog
{
// Construction
public:
	CDimStandardDlg(DimStandard* curStandard,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDimStandardDlg)
	enum { IDD = IDD_DIM_STANDARD };
	CButton	m_Revert;
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
	//{{AFX_VIRTUAL(CDimStandardDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	DimStandard* m_pDimStd;
	DimStandard m_workStd,m_saveStd;

	void UpdateValues(bool bSaveAndValidate=true);

	// Generated message map functions
	//{{AFX_MSG(CDimStandardDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnRevert();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIMSTANDARDDLG_H__24528AA4_78C9_11D2_B4C9_8A8CF35AB464__INCLUDED_)
