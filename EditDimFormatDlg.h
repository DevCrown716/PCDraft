#if !defined(AFX_EDITDIMFORMATDLG_H__24528AA5_78C9_11D2_B4C9_8A8CF35AB464__INCLUDED_)
#define AFX_EDITDIMFORMATDLG_H__24528AA5_78C9_11D2_B4C9_8A8CF35AB464__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditDimFormatDlg.h : header file
//

#include "DimStructs.h"
#include "NumberEdit.h"

class CPCDraftDoc;
/////////////////////////////////////////////////////////////////////////////
// CEditDimFormatDlg dialog

class CEditDimFormatDlg : public CDialog
{
// Construction
public:
	CEditDimFormatDlg(DimFormat* curFormat,DimStandard* curStandard,CPCDraftDoc* pDoc,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditDimFormatDlg)
	enum { IDD = IDD_EDIT_DIM_FORMAT };
	CEdit	m_Label;
	CComboBox	m_DimRoundoff;
	CButton	m_DimLabel;
	CButton	m_TextCentered;
	CButton	m_TextFramed;
	CButton	m_ArrowsInside;
	CButton	m_WitnessLines;
	CComboBox	m_TextDisplay;
	CComboBox	m_Leader;
	CComboBox	m_RDSymbol;
	CComboBox	m_Tolerance;
	CNumberEdit	m_Tol2;
	CNumberEdit	m_Tol1;
	CString	m_sDimType;
	CString	m_sTol1Measure;
	CString	m_sTol2Measure;
	//}}AFX_DATA
	short m_nChanges;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditDimFormatDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	DimFormat* m_pDimFmt;
	DimFormat m_workFmt;
	DimStandard* m_pDimStd;
	DimStandard m_workStd;
	CPCDraftDoc* m_pDoc;

	void UpdateValues(bool bSaveAndValidate=true);

	// Generated message map functions
	//{{AFX_MSG(CEditDimFormatDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnStandards();
	afx_msg void OnSelchangeTolerance();
	virtual void OnOK();
	afx_msg void OnSelchangeLeader();
	afx_msg void OnSelchangeTextDisplay();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITDIMFORMATDLG_H__24528AA5_78C9_11D2_B4C9_8A8CF35AB464__INCLUDED_)
