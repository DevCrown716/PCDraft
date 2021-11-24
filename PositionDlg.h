#if !defined(AFX_POSITIONDLG_H__53D43F41_5316_11D2_B466_B32F0398D7A4__INCLUDED_)
#define AFX_POSITIONDLG_H__53D43F41_5316_11D2_B466_B32F0398D7A4__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PositionDlg.h : header file
//

#include "lpoint.h"
#include "NumberEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CPositionDlg dialog

class CPCDraftDoc;

class CPositionDlg : public CDialog
{
// Construction
public:
	CPositionDlg(CPCDraftDoc *pDoc, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPositionDlg)
	enum { IDD = IDD_POSITION };
	CStatic	m_MeasureY;
	CStatic	m_MeasureX;
	CNumberEdit	m_PositionY;
	CNumberEdit	m_PositionX;
	//}}AFX_DATA

	LongPoint m_Delta;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPositionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CPCDraftDoc *m_pDoc;
	double m_unitFactor;
	// Generated message map functions
	//{{AFX_MSG(CPositionDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POSITIONDLG_H__53D43F41_5316_11D2_B466_B32F0398D7A4__INCLUDED_)
