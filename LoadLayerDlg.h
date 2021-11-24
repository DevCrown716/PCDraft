#if !defined(AFX_LOADLAYERDLG_H__E00EE256_089E_11D3_9960_000000000000__INCLUDED_)
#define AFX_LOADLAYERDLG_H__E00EE256_089E_11D3_9960_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LoadLayerDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLoadLayerDlg dialog

class CPCDraftDoc;

class CLoadLayerDlg : public CDialog
{
// Construction
public:
	CLoadLayerDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLoadLayerDlg)
	enum { IDD = IDD_LOAD_LAYER };
	CListBox	m_LayerList;
	int		m_nLayerIndex;
	//}}AFX_DATA
	CPCDraftDoc* m_pDoc;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLoadLayerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLoadLayerDlg)
	afx_msg void OnSelchangeLayerList();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOADLAYERDLG_H__E00EE256_089E_11D3_9960_000000000000__INCLUDED_)
