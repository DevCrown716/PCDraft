#if !defined(AFX_REPORTEDITVIEW_H__01490657_BEA2_11D2_98F2_000000000000__INCLUDED_)
#define AFX_REPORTEDITVIEW_H__01490657_BEA2_11D2_98F2_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ReportEditView.h : header file
//
#include "pcdraftdoc.h"
#include "ishapeselectednotify.h"

/////////////////////////////////////////////////////////////////////////////
// CReportEditView view

class CReportEditView : public CScrollView, public IShapeSelectedNotify
{
protected:
	CReportEditView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CReportEditView)

// Attributes
public:
	CMDReport*	m_pReport;
	int	m_nRowHeight;
	DWORD m_nReportTag;

// Operations
public:
	void CreateReportFont();
	CPCDraftDoc* GetDocument() { return (CPCDraftDoc*)CScrollView::GetDocument(); }
	void SetReportSize();
	void DoSelInfo();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReportEditView)
	public:
	virtual CScrollBar* GetScrollBarCtrl(int nBar) const;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	CBrush	m_DotBrush;
	CFont	m_SysFont;
	CFont	m_ReportFont;
	int		m_nActiveRow;
	int		m_nActiveCol;
	int		m_nStartSelectedRow;
	int		m_nEndSelectedRow;
	int		m_nStartSelectedCol;
	int		m_nEndSelectedCol;
	CEdit	m_wndInplaceEdit;
	
	bool SetRowType(int nIndex,short nRowType,short nFunction);
	bool SetFieldRefNum(int row,int col,short nFieldRefNum);
	void ShowInplaceEdit();
	void AdjustInplaceEdit();
	void SetTextJustify(int nJust);
	CString GetFunctionName(short nFunction);
	CString GetFieldRefNumName(short nFieldRefNum);
	CString GetCriteriaName();
	void InternalDraw(CDC* pDC,bool bSelected);

	void InvalidateSelectedRows();
	bool IsSelectedRow(int nRow=-1);
	int GetSelectedRowJustify();

	void InvalidateSelectedCols();
	bool IsSelectedCol(int nCol=-1);
	int GetSelectedColJustify();

	int GetSelectedRowColJustify();

	BOOL DoClick(const CRect &rc,CPoint &pt);
	virtual void ShapeSelected(CMDShape* pShape);

	virtual ~CReportEditView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	
	afx_msg void OnInplaceEditChange();
	afx_msg void OnUpdateTextJustify(CCmdUI* pCmdUI);
	// Generated message map functions
	//{{AFX_MSG(CReportEditView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnTextJustifyCenter();
	afx_msg void OnTextJustifyLeft();
	afx_msg void OnTextJustifyRight();
	afx_msg void OnTextFont();
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditDeleteRow();
	afx_msg void OnUpdateEditDeleteRow(CCmdUI* pCmdUI);
	afx_msg void OnEditInsertRow();
	afx_msg void OnFileExportReport();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnUpdateDataFindReplace(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLayoutLayerSetup(CCmdUI* pCmdUI);
	afx_msg void OnFileClose();
	afx_msg void OnUpdateFileExportReport(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REPORTEDITVIEW_H__01490657_BEA2_11D2_98F2_000000000000__INCLUDED_)
