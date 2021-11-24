#if !defined(AFX_CIRCLEDUPDLG_H__2B192EA1_53E7_11D2_B468_CE80E887D7A4__INCLUDED_)
#define AFX_CIRCLEDUPDLG_H__2B192EA1_53E7_11D2_B468_CE80E887D7A4__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CircleDupDlg.h : header file
//

#include "NumberEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CCircleDupDlg dialog

class CPCDraftDoc;

class CCircleDupDlg : public CDialog
{
// Construction
public:
	CCircleDupDlg(CPCDraftDoc* pDoc,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCircleDupDlg)
	enum { IDD = IDD_CIRCLE_DUP };
	CNumberEdit	m_Angle;
	CNumberEdit	m_Radius;
	CNumberEdit	m_Shift;
	CNumberEdit	m_CentreY;
	CNumberEdit	m_CentreX;
	CButton	m_UseMouse;
	BOOL	m_bRotateCopies;
	int		m_nDistribute;
	int		m_nByObject;
	int		m_nCircles;
	int		m_nCopies;
	BOOL	m_bUseMouse;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCircleDupDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CPCDraftDoc* m_pDoc;

	// Generated message map functions
	//{{AFX_MSG(CCircleDupDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnUseMouseClick();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CIRCLEDUPDLG_H__2B192EA1_53E7_11D2_B468_CE80E887D7A4__INCLUDED_)
