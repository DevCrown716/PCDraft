#if !defined(AFX_DOCDEFAULTSDLG_H__C4A92264_5961_11D2_B479_FED1ADE6F2A4__INCLUDED_)
#define AFX_DOCDEFAULTSDLG_H__C4A92264_5961_11D2_B479_FED1ADE6F2A4__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DocDefaultsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDocDefaultsDlg dialog

class CDocDefaultsDlg : public CDialog
{
// Construction
public:
	void EnableOK();
	CDocDefaultsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDocDefaultsDlg)
	enum { IDD = IDD_NEWDOC_DEFAULTS };
	BOOL	m_bDimEndMarks;
	BOOL	m_bDimFormats;
	BOOL	m_bDimStandards;
	BOOL	m_bDrawAndPageSetup;
	BOOL	m_bFieldNames;
	BOOL	m_bFillPatternsColors;
	BOOL	m_bHideGridLines;
	BOOL	m_bHidePageBreaks;
	BOOL	m_bLineSettings;
	BOOL	m_bOpenAtCenter;
	BOOL	m_bScaleAndLayerSetup;
	BOOL	m_bShowRulers;
	BOOL	m_bSnapToObject;
	BOOL	m_bTextSettings;
	//}}AFX_DATA
	BOOL m_bEnableOK;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDocDefaultsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDocDefaultsDlg)
	afx_msg void OnAppDefaults();
	afx_msg void OnSaveSettingsChanged();
	afx_msg void OnSettingsChanged();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DOCDEFAULTSDLG_H__C4A92264_5961_11D2_B479_FED1ADE6F2A4__INCLUDED_)
