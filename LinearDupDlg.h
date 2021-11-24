#if !defined(AFX_LINEARDUPDLG_H__56712DE1_50CA_11D2_B460_A69CD8EB62A4__INCLUDED_)
#define AFX_LINEARDUPDLG_H__56712DE1_50CA_11D2_B460_A69CD8EB62A4__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// LinearDupDlg.h : header file
//

#include "NumberEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CLinearDupDlg dialog

class CPCDraftDoc;

class CLinearDupDlg : public CDialog
{
// Construction
public:
	CLinearDupDlg(CPCDraftDoc* pDoc,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLinearDupDlg)
	enum { IDD = IDD_LINEAR_DUP };
	CNumberEdit	m_Dir2Value2;
	CNumberEdit	m_Dir2Value1;
	CNumberEdit	m_Dir1Value2;
	CNumberEdit	m_Dir1Value1;
	CStatic	m_dir1label2;
	CStatic	m_dir1label1;
	CStatic	m_dir2label2;
	CStatic	m_dir2label1;
	int		m_nDupType;
	int		m_nDir2ParamType;
	int		m_nDir1ParamType;
	int		m_Dir1Copies;
	int		m_Dir2Copies;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLinearDupDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CPCDraftDoc* m_pDoc;

	// Generated message map functions
	//{{AFX_MSG(CLinearDupDlg)
	afx_msg void On2direction();
	afx_msg void OnDirection();
	afx_msg void OnDir1DistanceAngle();
	afx_msg void OnDir1Offset();
	afx_msg void OnDir2DistanceAngle();
	afx_msg void OnDir2Offset();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void UpdateControls();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LINEARDUPDLG_H__56712DE1_50CA_11D2_B460_A69CD8EB62A4__INCLUDED_)
