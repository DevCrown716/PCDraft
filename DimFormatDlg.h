#if !defined(AFX_DIMFORMATDLG_H__24528AA3_78C9_11D2_B4C9_8A8CF35AB464__INCLUDED_)
#define AFX_DIMFORMATDLG_H__24528AA3_78C9_11D2_B4C9_8A8CF35AB464__INCLUDED_

#include "DimStructs.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DimFormatDlg.h : header file
//

#include "DimStructs.h"
#include "NumberEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CDimFormatDlg dialog

class CDimFormatDlg : public CDialog
{
// Construction
public:
	void OnChangeControls();
	CDimFormatDlg(int nDSType,int startType,DimFormat* curFormats,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDimFormatDlg)
	enum { IDD = IDD_DIM_FORMAT };
	CButton	m_Revert;
	CComboBox	m_DimType;
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
	CStatic	m_Tol1Measure;
	CStatic	m_Tol2Measure;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDimFormatDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_bReactOnChanges;
	int m_nDSType;
	int m_nType;
	DimFormat* m_pDimFmts;
	DimFormat m_workFmts[3],m_saveFmt;

	void UpdateValues(bool bSaveAndValidate=true);
	void UpdateControls();

	// Generated message map functions
	//{{AFX_MSG(CDimFormatDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnRevert();
	afx_msg void OnSelchangeDimType();
	afx_msg void OnSelchangeTolerance();
	virtual void OnOK();
	afx_msg void OnSelchangeLeader();
	afx_msg void OnSelchangeTextDisplay();
	afx_msg void OnWitnessLines();
	afx_msg void OnArrowsInside();
	afx_msg void OnTextCentered();
	afx_msg void OnTextFramed();
	afx_msg void OnChangeTol1();
	afx_msg void OnChangeTol2();
	afx_msg void OnSelchangeRdsymbol();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIMFORMATDLG_H__24528AA3_78C9_11D2_B4C9_8A8CF35AB464__INCLUDED_)
