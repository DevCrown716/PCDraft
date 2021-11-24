#if !defined(AFX_LAYERSETUPDLG_H__DBCA0446_5C8B_11D2_9863_000000000000__INCLUDED_)
#define AFX_LAYERSETUPDLG_H__DBCA0446_5C8B_11D2_9863_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// LayerSetupDlg.h : header file
//

#include "LayerListBox.h"

/////////////////////////////////////////////////////////////////////////////
// CLayerSetupDlg dialog

class CPCDraftDoc;

class CLayerSetupDlg : public CDialog
{
// Construction
public:
	CLayerSetupDlg(CWnd* pParent = NULL);   // standard constructor
	void DoIdleUpdate();

// Dialog Data
	//{{AFX_DATA(CLayerSetupDlg)
	enum { IDD = IDD_LAYER_SETUP };
	CEdit	m_LayerName;
	CComboBox	m_Scale;
	CLayerListBox	m_LayerList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLayerSetupDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CPCDraftDoc* m_pDoc;
	bool m_bIgnoreActivate;
	bool m_bLastDocument;
	int m_nLastScale;

	void Refresh();
	void LoadListBox(bool bCheck=false);

	// Generated message map functions
	//{{AFX_MSG(CLayerSetupDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeLayerList();
	afx_msg void OnAdd();
	afx_msg void OnRename();
	afx_msg void OnDelete();
	afx_msg void OnMerge();
	afx_msg void OnGrayed();
	afx_msg void OnHidden();
	afx_msg void OnEditLayer();
	afx_msg void OnDblclkLayerList();
	afx_msg void OnArrange();
	afx_msg void OnSelchangeScale();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LAYERSETUPDLG_H__DBCA0446_5C8B_11D2_9863_000000000000__INCLUDED_)
