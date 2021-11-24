#if !defined(AFX_ENDMARKSWND_H__991174E6_071E_11D5_9C5F_000021D1C71F__INCLUDED_)
#define AFX_ENDMARKSWND_H__991174E6_071E_11D5_9C5F_000021D1C71F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EndmarksWnd.h : header file
//

#include "LineWidthWnd.h"

/////////////////////////////////////////////////////////////////////////////
// CEndmarksWnd window

class CEndmarksWnd : public CLineWnd // Opacity/Transparency - Infinisys Ltd
{
// Construction
public:
	CEndmarksWnd();

// Attributes
public:
	BYTE m_nCap1,m_nCap2;
	CBitmap m_bitmap;

// Operations
public:
	void SetCaps(BYTE nCap1,BYTE nCap2);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEndmarksWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	bool m_bSubClassed;
	virtual ~CEndmarksWnd();

	BOOL Create(CRect& rc,CWnd *pParent,UINT nID);
	BOOL Subclass(CWnd *pParent,UINT nID);

	// Generated message map functions
protected:
	//{{AFX_MSG(CEndmarksWnd)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	LRESULT OnSetCaps(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENDMARKSWND_H__991174E6_071E_11D5_9C5F_000021D1C71F__INCLUDED_)
