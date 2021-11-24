#if !defined(AFX_CLONEFTINUNITDLG_H__9E1C1582_7CB3_11D3_A119_00A0C9A0C319__INCLUDED_)
#define AFX_CLONEFTINUNITDLG_H__9E1C1582_7CB3_11D3_A119_00A0C9A0C319__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CloneFtInUnitDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCloneFtInUnitDlg dialog

class CCloneFtInUnitDlg : public CDialog
{
// Construction
public:
	CCloneFtInUnitDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCloneFtInUnitDlg)
	enum { IDD = IDD_CLONE_FT_IN_UNIT };
	int		m_nUnits;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCloneFtInUnitDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCloneFtInUnitDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLONEFTINUNITDLG_H__9E1C1582_7CB3_11D3_A119_00A0C9A0C319__INCLUDED_)
