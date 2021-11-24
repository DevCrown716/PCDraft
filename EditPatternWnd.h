#if !defined(AFX_EDITPATTERNWND_H__6F7389C5_A9FC_11D2_A021_00A0C9A0C319__INCLUDED_)
#define AFX_EDITPATTERNWND_H__6F7389C5_A9FC_11D2_A021_00A0C9A0C319__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditPatternWnd.h : header file
//

class CEditPatternDlg;
/////////////////////////////////////////////////////////////////////////////
// CEditPatternWnd window

class CEditPatternWnd : public CWnd
{
enum Constraints
{
	kNone=0,
	kTest,
	kVertical,
	kHorizontal
};

friend class CEditPatternDlg;

// Construction
public:
	CEditPatternWnd();

// Attributes
public:
	int m_nConstraint;
	COLORREF m_BlendColor;
	int m_nEditY;
	int m_nEditX;
	int m_nY;
	int m_nX;
	void UpdateEditInfo();
	int m_nEditSize;
	int m_nSide;
	bool m_bTrack;
	bool m_bToggle;
	HCURSOR m_hThumb;
	CEditPatternDlg* m_pParentDlg;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditPatternWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	void AverageColor(COLORREF src,COLORREF& dst);
	void UpdateColor();
	void UpdateThumbColor();
	virtual ~CEditPatternWnd();
	void SetParentDlg(CEditPatternDlg* parentDlg);
	bool ApplyTool();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditPatternWnd)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITPATTERNWND_H__6F7389C5_A9FC_11D2_A021_00A0C9A0C319__INCLUDED_)
