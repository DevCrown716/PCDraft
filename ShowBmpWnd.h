#if !defined(AFX_SHOWBMPWND_H__4E0EEA8B_3519_402F_8817_48A725522184__INCLUDED_)
#define AFX_SHOWBMPWND_H__4E0EEA8B_3519_402F_8817_48A725522184__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ShowBmpWnd.h : header file
//

#include "LineWidthWnd.h"

/////////////////////////////////////////////////////////////////////////////
// CShowBmpWnd window
#define LEFTTOP_POSITION			0
#define CENTER_POSITION				1

extern UINT UM_SHOWBMP_UPDATE;
class CShowBmpWnd : public CLineWnd
{
// Construction
public:
	CShowBmpWnd(int iLimitHeight=-1, int nPositionType = LEFTTOP_POSITION);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShowBmpWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CShowBmpWnd();

	// Generated message map functions
protected:
	int m_nPositionType;
	CBitmap m_bitmapReceived;
   int      m_iLimitHeight;
	//{{AFX_MSG(CShowBmpWnd)
	afx_msg void OnPaint();
	//}}AFX_MSG
	afx_msg LPARAM OnReceiveBmp(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
private:
	//void DrawBorder(CDC *pDC, LPRECT pRect, LPRECT pOutClient, int nStyle = 0);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHOWBMPWND_H__4E0EEA8B_3519_402F_8817_48A725522184__INCLUDED_)
