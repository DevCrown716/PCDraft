#if !defined(AFX_CUSTOMMINIDOCKFRAMEWND_H__4C68FF90_0B74_4A08_8CB6_A24B6554E038__INCLUDED_)
#define AFX_CUSTOMMINIDOCKFRAMEWND_H__4C68FF90_0B74_4A08_8CB6_A24B6554E038__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CCustomMiniDockFrameWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCustomMiniDockFrameWnd frame

extern UINT UM_MAXIMIZE_CLICK;

class CCustomMiniDockFrameWnd : public CMiniDockFrameWnd
{
	DECLARE_DYNCREATE(CCustomMiniDockFrameWnd)
protected:
	CCustomMiniDockFrameWnd();           // protected constructor used by dynamic creation

// Attributes
public:
	bool GetMaximized() { return m_bMaximized; }
	void SetMaximized(bool bMaximized);
	void InvertMaximized();
	void ShowMaximizeBox(bool bShowMaximizeBox);
   bool is_narrow_window();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomMiniDockFrameWnd)
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool m_bInButton;
	bool m_bTracking;
	bool m_bMaximized;
	bool m_bShowMaximizeBox;
	virtual ~CCustomMiniDockFrameWnd();
	void CalcButtonRect(CRect& rc);
	void DrawButton();

	// Generated message map functions
	//{{AFX_MSG(CCustomMiniDockFrameWnd)
	//afx_msg void OnNcPaint();
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
   afx_msg void OnPaint(); // Interface Update - Infinisys Ltd.
   afx_msg LRESULT OnDwm(WPARAM wparam,LPARAM lparam); // Interface Update - Infinisys Ltd.
   afx_msg void OnNcCalcSize(BOOL bCalcValidRects,NCCALCSIZE_PARAMS* lpncsp); // Interface Update - Infinisys Ltd.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnActivate(UINT nState,CWnd* pWndOther,BOOL bMinimized); // Interface Update - Infinisys Ltd.
   virtual void RecalcLayout(BOOL bNotify); // Interface Update - Infinisys Ltd.
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct); // Interface Update - Infinisys Ltd.
   afx_msg void OnLButtonDown(UINT nFlags,CPoint point); // Interface Update - Infinisys Ltd.
   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUSTOMMINIDOCKFRAMEWND_H__4C68FF90_0B74_4A08_8CB6_A24B6554E038__INCLUDED_)
