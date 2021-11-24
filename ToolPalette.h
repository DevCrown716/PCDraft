#if !defined(AFX_TOOLPALETTE_H__7B40C603_5CF2_11D2_9FBB_00A0C9A0C319__INCLUDED_)
#define AFX_TOOLPALETTE_H__7B40C603_5CF2_11D2_9FBB_00A0C9A0C319__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ToolPalette.h : header file
//
#define	kNumToolButtons	20

/////////////////////////////////////////////////////////////////////////////
// CToolPalette window

class CToolPalette : public CToolBar
{
// Construction
public:
	CToolPalette();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolPalette)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SelectButtonWithId(UINT nId);
	CPoint m_ptHit;
	UINT m_nHitFlags;
	int m_nHitID;
	BOOL Create( CWnd* pParentWnd, bool bVisible = true);
	virtual ~CToolPalette();
   CSize CalcDynamicLayout(int nLength,DWORD dwMode);
	// Generated message map functions
protected:
	//{{AFX_MSG(CToolPalette)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	CBitmap m_bitmaps[kNumToolButtons];
	int m_base[kNumToolButtons];
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLPALETTE_H__7B40C603_5CF2_11D2_9FBB_00A0C9A0C319__INCLUDED_)
