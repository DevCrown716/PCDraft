#if !defined(AFX_DRAWINGSIZEWND_H__3FD831D6_5191_11D2_9853_000000000000__INCLUDED_)
#define AFX_DRAWINGSIZEWND_H__3FD831D6_5191_11D2_9853_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DrawingSizeWnd.h : header file
//

#define DSN_SIZECHANGED        WM_USER + 1001        // Drawing Size Changed

/////////////////////////////////////////////////////////////////////////////
// CDrawingSizeWnd window

class CDrawingSizeWnd : public CWnd
{
// Construction
public:
	CDrawingSizeWnd();

// Attributes
public:
	CSize m_PageSize;
	CPoint m_NumPage;
	CPoint m_MinNumPage;
   CPoint m_MaxNumPage; // 453.39cm Drawing Size - Infinisys Ltd

// Operations
public:
	BOOL Create(CRect& rc,CWnd *pParent,UINT nID);
	CSize GetSize();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDrawingSizeWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDrawingSizeWnd();
	void SetNewDrawingSize(CPoint pt);

	// Generated message map functions
protected:
	CSize m_DevPageSize;
	bool m_bTrackSize;

	//{{AFX_MSG(CDrawingSizeWnd)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRAWINGSIZEWND_H__3FD831D6_5191_11D2_9853_000000000000__INCLUDED_)
