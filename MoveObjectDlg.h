#if !defined(AFX_MOVEOBJECTDLG_H__7F1F8052_524B_11D2_B464_E13F5088D6A4__INCLUDED_)
#define AFX_MOVEOBJECTDLG_H__7F1F8052_524B_11D2_B464_E13F5088D6A4__INCLUDED_

#include "PCDraftDoc.h"	// Added by ClassView
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MoveObjectDlg.h : header file
//

#include "NumberEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CMoveObjectDlg dialog
class CPCDraftView;

class CMoveObjectDlg : public CDialog
{
// Construction
public:
	int m_nToLayer;
	CMoveObjectDlg(CPCDraftView* pView,CWnd* pParent = NULL);   // standard constructor

	LongPoint m_lptMove;
// Dialog Data
	//{{AFX_DATA(CMoveObjectDlg)
	enum { IDD = IDD_MOVE_OBJECT };
	CStatic	m_ToLayer;
	CNumberEdit	m_Value2;
	CNumberEdit	m_Value1;
	CStatic	m_label2;
	CStatic	m_label1;
	BOOL	m_bMoveCopy;
	int		m_nAbsRel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMoveObjectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CPCDraftView* m_pView;
	CMenu m_LayerPopup;

	void CalcMoveParams();
	// Generated message map functions
	//{{AFX_MSG(CMoveObjectDlg)
	afx_msg void OnAbsolute();
	afx_msg void OnRelative();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnTolayer();
	afx_msg void OnApply();
	afx_msg void OnApplyReverse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOVEOBJECTDLG_H__7F1F8052_524B_11D2_B464_E13F5088D6A4__INCLUDED_)
