#if !defined(AFX_REPORTPREFRAME_H__CA1A8615_5722_11D3_99BC_000000000000__INCLUDED_)
#define AFX_REPORTPREFRAME_H__CA1A8615_5722_11D3_99BC_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ReportPreFrame.h : header file
//

#include "MacButton.h"

/////////////////////////////////////////////////////////////////////////////
// CReportPreFrame frame

class CReportPreFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CReportPreFrame)
protected:
	CReportPreFrame();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:
	CScrollBar* GetScrollBar(int nBar) const;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReportPreFrame)
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
	CMacButton m_wndPrint;
	CMacButton m_wndPrintSetup;
	CMacButton m_wndUpdate;
	int	m_cxReserved;

	virtual ~CReportPreFrame();

	afx_msg void OnPrint();
	afx_msg void OnPrintSetup();
	afx_msg void OnUpdate();
	afx_msg LRESULT OnDelaySize(WPARAM wParam, LPARAM lParam);
	// Generated message map functions
	//{{AFX_MSG(CReportPreFrame)
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

#endif // !defined(AFX_REPORTPREFRAME_H__CA1A8615_5722_11D3_99BC_000000000000__INCLUDED_)
