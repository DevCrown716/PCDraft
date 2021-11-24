#if !defined(AFX_MACBUTTON_H__B203D341_B3A1_11D2_98E3_000000000000__INCLUDED_)
#define AFX_MACBUTTON_H__B203D341_B3A1_11D2_98E3_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MacButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMacButton window

class CMacButton : public CWnd
{
// Construction
public:
	CMacButton();

// Attributes
public:

// Operations
public:
	BOOL Create(LPCTSTR lpszCaption,DWORD dwStyle,const RECT& rect,CWnd* pParentWnd,UINT nID);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMacButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMacButton();

protected:
	bool	m_bPushed;
	bool	m_bClick;

	// Generated message map functions
protected:
	//{{AFX_MSG(CMacButton)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnEnable(BOOL bEnable);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MACBUTTON_H__B203D341_B3A1_11D2_98E3_000000000000__INCLUDED_)
