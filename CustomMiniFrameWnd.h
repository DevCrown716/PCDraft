#pragma once

/////////////////////////////////////////////////////////////////////////////
// CCustomMiniFrameWnd frame - Interface Update - Infinisys Ltd.

extern UINT UM_MAXIMIZE_CLICK;

class CCustomMiniFrameWnd : public CMiniFrameWnd
{
	DECLARE_DYNCREATE(CCustomMiniFrameWnd)
protected:
	CCustomMiniFrameWnd();           // protected constructor used by dynamic creation

// Attributes
public:
	bool GetMaximized() { return m_bMaximized; }
	void SetMaximized(bool bMaximized);
	void InvertMaximized();
	void ShowMaximizeBox(bool bShowMaximizeBox);

// Operations
public:
   bool     m_bResizable;
   bool     m_bResizeGrip;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomMiniFrameWnd)
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool m_bInButton;
	bool m_bTracking;
	bool m_bMaximized;
	bool m_bShowMaximizeBox;
	virtual ~CCustomMiniFrameWnd();
	void CalcButtonRect(CRect& rc);
	void DrawButton();
   bool is_narrow_window();
   // Generated message map functions
	//{{AFX_MSG(CCustomMiniFrameWnd)
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
   afx_msg LRESULT OnNcHitTest(CPoint mouse); // Interface Update - Infinisys Ltd.
   afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
};


