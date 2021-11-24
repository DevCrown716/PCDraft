// ScaleUnitsDlg.h : header file
//
#if !defined(AFX_SCALEUNITSDLG_H__30AB8A53_4CD0_11D2_984C_000000000000__INCLUDED_)
#define AFX_SCALEUNITSDLG_H__30AB8A53_4CD0_11D2_984C_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "DimStructs.h"
#include "lpoint.h"
/////////////////////////////////////////////////////////////////////////////
// CScaleUnitsDlg dialog

class CPCDraftDoc;

class CScaleUnitsDlg : public CDialog
{
// Construction
public:
	CScaleUnitsDlg(CPCDraftDoc* pDoc,bool bAutoScale=false,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CScaleUnitsDlg)
	enum { IDD = IDD_SCALE_UNITS };
   CEdit	m_editNumerator;      // Free Scale -  Infinisys Ltd
   CEdit	m_editDenominator;      // Free Scale -  Infinisys Ltd
	CStatic	m_DimStatus;
	CComboBox	m_UnitsPlaces;
	CComboBox	m_DefaultScale;
	CComboBox	m_UnitsDisplay;
	int		m_nAnglePlaces;
	int		m_nPlaces;
	BOOL	m_bHideMetricUnits;
	int		m_nUserDefined;
   int		m_nUserDefinedNumerator;      // Free Scale -  Infinisys Ltd
   int		m_nUserDefinedDenominator;      // Free Scale -  Infinisys Ltd
	//}}AFX_DATA

   LongRatio m_longratioCustom;      // Free Scale -  Infinisys Ltd
   bool m_bCustomScale;      // Free Scale -  Infinisys Ltd
	int m_nUnit;
	int m_nAngleUnit;
	int m_nScaleIndex;
	bool m_bAutoScale;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScaleUnitsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void SetupAngleUnit();
	void SetupUnit(int nUnit);
#ifndef _PERSONAL_EDITION
	void UpdateDimStatus();
#endif

	CPCDraftDoc* m_pDoc;
	DimStandard m_DimStd;

   void UpdateUserDefinedEdit();      // Free Scale -  Infinisys Ltd

	// Generated message map functions
	//{{AFX_MSG(CScaleUnitsDlg)
	afx_msg void OnAngleDecimalDegrees();
	afx_msg void OnAngleDegreesMin();
	afx_msg void OnAngleDegreesMinSec();
	afx_msg void OnAngleUserDefined();
	afx_msg void OnUnitEnglish();
	afx_msg void OnUnitMetric();
   afx_msg void OnUserDefinedApply();      // Free Scale -  Infinisys Ltd
	afx_msg void OnSelchangeUnitsDisplay();
	virtual BOOL OnInitDialog();
	afx_msg void OnCustom();
	virtual void OnOK();
	afx_msg void OnSelchangeDefaultScale();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCALEUNITSDLG_H__30AB8A53_4CD0_11D2_984C_000000000000__INCLUDED_)
