#if !defined(AFX_DWGUNITSDLG_H__CDBD050C_9104_4E6E_9CCD_795CCF205C36__INCLUDED_)
#define AFX_DWGUNITSDLG_H__CDBD050C_9104_4E6E_9CCD_795CCF205C36__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DWGUnitsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDWGUnitsDlg dialog

class CPCDraftDoc;

class CDWGUnitsDlg : public CDialog
{
// Construction
public:
	CDWGUnitsDlg(CPCDraftDoc* pDoc, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDWGUnitsDlg)
	enum { IDD = IDD_DWG_UNITS };
	double	m_Units;
	int		m_nMetric;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDWGUnitsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CPCDraftDoc* m_pDoc;
	// Generated message map functions
	//{{AFX_MSG(CDWGUnitsDlg)
	afx_msg void OnDwgFeet();
	afx_msg void OnDwgMetre();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DWGUNITSDLG_H__CDBD050C_9104_4E6E_9CCD_795CCF205C36__INCLUDED_)
