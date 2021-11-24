#if !defined(AFX_APPLICATIONDEFAULTSDLG_H__E2778595_141A_11D2_980C_000000000000__INCLUDED_)
#define AFX_APPLICATIONDEFAULTSDLG_H__E2778595_141A_11D2_980C_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ApplicationDefaultsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CApplicationDefaultsDlg dialog

class CApplicationDefaultsDlg : public CDialog
{
// Construction
public:
	CApplicationDefaultsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CApplicationDefaultsDlg)
	enum { IDD = IDD_APPLICATION_DEFAULTS };
	BOOL	m_bCrossCursor;
	int		m_nWindowAtStartUp;
	BOOL	m_bHideLineStyle;
	BOOL	m_bShowCursorPosition;
	BOOL	m_bToolDisplay;
	BOOL	m_bToolSaveLocation;
	BOOL	m_bZoomLineWeights;
	BOOL	m_bLinkedDimensions;
	BOOL	m_bRemoveLinks;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CApplicationDefaultsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CApplicationDefaultsDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_APPLICATIONDEFAULTSDLG_H__E2778595_141A_11D2_980C_000000000000__INCLUDED_)
