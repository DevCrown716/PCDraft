#if !defined(AFX_EDITLINESTYLEWND_H__8DA771D6_7C7A_11D2_B4D6_C6673FA1B664__INCLUDED_)
#define AFX_EDITLINESTYLEWND_H__8DA771D6_7C7A_11D2_B4D6_C6673FA1B664__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditLineStyleWnd.h : header file
//

class CEditLineStyleDlg;
/////////////////////////////////////////////////////////////////////////////
// CEditLineStyleWnd window

class CEditLineStyleWnd : public CWnd
{
// Construction
public:
	CEditLineStyleWnd();

// Attributes
public:
	bool m_bWhite;
	bool m_bTrack;
	unsigned char* m_Style;
	unsigned char m_EditStyle[16];
	int m_nPos,m_nEditPos;
	int m_nLength;
	CEditLineStyleDlg* m_pParentDlg;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditLineStyleWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetParentDlg(CEditLineStyleDlg* parentDlg);
	void SegAdd(unsigned char &seg,int add);
	void CalcLength();
	virtual ~CEditLineStyleWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditLineStyleWnd)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg UINT OnGetDlgCode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITLINESTYLEWND_H__8DA771D6_7C7A_11D2_B4D6_C6673FA1B664__INCLUDED_)
