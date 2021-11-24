#if !defined(AFX_DRAWINGSIZEDLG_H__3FD831D5_5191_11D2_9853_000000000000__INCLUDED_)
#define AFX_DRAWINGSIZEDLG_H__3FD831D5_5191_11D2_9853_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DrawingSizeDlg.h : header file
//

#include "DrawingSizeWnd.h"

/////////////////////////////////////////////////////////////////////////////
// CDrawingSizeDlg dialog

class CDrawingSizeDlg : public CDialog
{
// Construction
public:
	CDrawingSizeDlg(CWnd* pParent = NULL);   // standard constructor

	CDrawingSizeWnd m_wndDrawingSize;
// Dialog Data
	//{{AFX_DATA(CDrawingSizeDlg)
	enum { IDD = IDD_DRAWING_SIZE };
	CStatic	m_SizeInches;
	CStatic	m_SizeCM;
	int		m_nPageNum;
	BOOL	m_bLastPageFirst;
	BOOL	m_bRegistrationMarks;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDrawingSizeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	afx_msg LRESULT OnDrawingSizeChanged(WPARAM wParam,LPARAM lParam);

	// Generated message map functions
	//{{AFX_MSG(CDrawingSizeDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRAWINGSIZEDLG_H__3FD831D5_5191_11D2_9853_000000000000__INCLUDED_)
