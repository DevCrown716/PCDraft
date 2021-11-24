#if !defined(AFX_REPORTPREVIEW_H__CCD3F5A3_CF2E_11D2_990A_000000000000__INCLUDED_)
#define AFX_REPORTPREVIEW_H__CCD3F5A3_CF2E_11D2_990A_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ReportPreView.h : header file
//
#include "pcdraftdoc.h"

/////////////////////////////////////////////////////////////////////////////
// CReportPreView view

class CReportPreView : public CScrollView
{
protected:
	CReportPreView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CReportPreView)

// Attributes
public:
	CMDReport*	m_pReport;
	int	m_nRowHeight;
	int m_nNumRowInPage;
	int m_nPrintPageNo;
	DWORD m_nReportTag;

// Operations
public:
	CPCDraftDoc* GetDocument() { return (CPCDraftDoc*)CScrollView::GetDocument(); }
	void SetReportSize();
	void Calculate();
	void DoUpdate();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReportPreView)
	public:
	virtual CScrollBar* GetScrollBarCtrl(int nBar) const;
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
	CFont	m_ReportFont;

	virtual ~CReportPreView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CReportPreView)
	afx_msg void OnUpdateDataFindReplace(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REPORTPREVIEW_H__CCD3F5A3_CF2E_11D2_990A_000000000000__INCLUDED_)
