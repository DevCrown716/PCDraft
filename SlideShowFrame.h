#if !defined(AFX_SLIDESHOWFRAME_H__C21BD301_0149_11D3_91F5_000000000000__INCLUDED_)
#define AFX_SLIDESHOWFRAME_H__C21BD301_0149_11D3_91F5_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SlideShowFrame.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSlideShowFrame frame

class CSlideShowFrame : public CFrameWnd
{
	DECLARE_DYNCREATE(CSlideShowFrame)
protected:
	CSlideShowFrame();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSlideShowFrame)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CSlideShowFrame();

	// Generated message map functions
	//{{AFX_MSG(CSlideShowFrame)
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SLIDESHOWFRAME_H__C21BD301_0149_11D3_91F5_000000000000__INCLUDED_)
