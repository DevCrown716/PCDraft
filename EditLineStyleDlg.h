#if !defined(AFX_EDITLINESTYLEDLG_H__C4A92267_5961_11D2_B479_FED1ADE6F2A4__INCLUDED_)
#define AFX_EDITLINESTYLEDLG_H__C4A92267_5961_11D2_B479_FED1ADE6F2A4__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// EditLineStyleDlg.h : header file
//

#include "EditLineStyleWnd.h"

/////////////////////////////////////////////////////////////////////////////
// CEditLineStyleDlg dialog

class CEditLineStyleDlg : public CDialog
{
// Construction
public:
	void Update();
	unsigned char m_EditStyle[16+2];
	unsigned char* m_Style;
	CEditLineStyleDlg(unsigned char* style,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditLineStyleDlg)
	enum { IDD = IDD_EDIT_LINE_STYLE };
	CStatic	m_white;
	CStatic	m_draw;
	CStatic	m_black;
	CEditLineStyleWnd m_edit;
	CStatic	m_ruler;
	int		m_nUnits;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditLineStyleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditLineStyleDlg)
	afx_msg void OnUnits();
	virtual BOOL OnInitDialog();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRevert();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITLINESTYLEDLG_H__C4A92267_5961_11D2_B479_FED1ADE6F2A4__INCLUDED_)
