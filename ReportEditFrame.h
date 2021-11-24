#if !defined(AFX_REPORTEDITFRAME_H__EED4ECC6_1207_11D3_996F_000000000000__INCLUDED_)
#define AFX_REPORTEDITFRAME_H__EED4ECC6_1207_11D3_996F_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ReportEditFrame.h : header file
//

#include "MacButton.h"

/////////////////////////////////////////////////////////////////////////////
// CReportEditFrame frame

class CReportEditFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CReportEditFrame)
protected:
	CReportEditFrame();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:
	CScrollBar* GetScrollBar(int nBar) const;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReportEditFrame)
	public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	CScrollBar m_wndSBHorz;			 // horizontal scroll bar..
	CScrollBar m_wndSBVert;			 // ... vertical
	CScrollBar m_wndSBBox;			 // ... and box
	CMacButton m_wndSelInfo;
	int	m_cxReserved;

	virtual ~CReportEditFrame();

	afx_msg void OnSelInfo();
	afx_msg LRESULT OnDelaySize(WPARAM wParam, LPARAM lParam);
	// Generated message map functions
	//{{AFX_MSG(CReportEditFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REPORTEDITFRAME_H__EED4ECC6_1207_11D3_996F_000000000000__INCLUDED_)
