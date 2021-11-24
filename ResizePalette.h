#if !defined(AFX_RESIZEPALETTE_H__EB2980E6_BF80_11D2_B561_E4554702007F__INCLUDED_)
#define AFX_RESIZEPALETTE_H__EB2980E6_BF80_11D2_B561_E4554702007F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ResizePalette.h : header file
//
#include "Resource.h"
/////////////////////////////////////////////////////////////////////////////
// CResizePalette window

class CResizePalette : public CDialogBar
{
// Construction
public:
	CResizePalette();

	//{{AFX_DATA(CResizePalette)
	enum { IDD = IDD_RESIZE_PALETTE };
	//}}AFX_DATA

// Attributes
public:
	bool	m_bShowPos;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResizePalette)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL Create(CWnd * pParentWnd);
	virtual ~CResizePalette();

	// Generated message map functions
protected:
	void InvertResizeOpt();
	void OffsetWindow(UINT nID,int nDelta);
	afx_msg LRESULT OnMaximizeClick(WPARAM wParam,LPARAM lParam);
	//{{AFX_MSG(CResizePalette)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESIZEPALETTE_H__EB2980E6_BF80_11D2_B561_E4554702007F__INCLUDED_)
