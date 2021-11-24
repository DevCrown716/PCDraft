#if !defined(AFX_LINEWIDTHWND_H__FC5A2301_D756_4085_9E2F_99015D21F1CE__INCLUDED_)
#define AFX_LINEWIDTHWND_H__FC5A2301_D756_4085_9E2F_99015D21F1CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LineWidthWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLineWidthWnd window
extern UINT UM_LINEWIDTH_UPDATE;


// Gradient/Opacity - Infinisys Ltd
class CSepWnd: public CStatic
{
public:
   CSepWnd();

   virtual ~CSepWnd();


   afx_msg void OnPaint();
   DECLARE_MESSAGE_MAP()
};

class CLineWnd: public CWnd
{
public:
   CLineWnd();

   virtual ~CLineWnd();

   
   void DrawNonClient(CDC *pDC,CRect * pRect,CRect * pOutClient,COLORREF crBorder,COLORREF crBk);
};

class CLineWidthWnd : public CLineWnd
{
// Construction
public:
	CLineWidthWnd();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLineWidthWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLineWidthWnd();

	// Generated message map functions
protected:
	BOOL m_bFlag;
	int m_nWidthRegim;
	//{{AFX_MSG(CLineWidthWnd)
	afx_msg void OnPaint();
	//}}AFX_MSG
	afx_msg LPARAM OnNewLine(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LINEWIDTHWND_H__FC5A2301_D756_4085_9E2F_99015D21F1CE__INCLUDED_)
