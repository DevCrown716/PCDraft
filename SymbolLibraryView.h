// SymbolLibraryView.h : interface of the CSymbolLibraryView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYMBOLLIBRARYVIEW_H__D9BAD20F_B870_11D2_98EB_000000000000__INCLUDED_)
#define AFX_SYMBOLLIBRARYVIEW_H__D9BAD20F_B870_11D2_98EB_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSymbolScrollView;

class CSymbolLibraryView : public CFormView
{
protected: // create from serialization only
	CSymbolLibraryView();
	DECLARE_DYNCREATE(CSymbolLibraryView)

public:
	//{{AFX_DATA(CSymbolLibraryView)
	enum { IDD = IDD_SYMBOL_LIBRARY_FORM };
	CStatic	m_ScaleUnits;
	CButton	m_Rename;
	CButton	m_Find;
	CButton	m_Datum;
	CListBox	m_List;
	CEdit	m_F5;
	CEdit	m_F4;
	CEdit	m_F3;
	CEdit	m_F2;
	CEdit	m_Name;
	//}}AFX_DATA

// Attributes
public:
	CSymbolLibraryDoc* GetDocument();

// Operations
public:
	void LoadShapeList(bool bSelect=true);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSymbolLibraryView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSymbolLibraryView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	bool	m_bDatumDone;
	CSymbolScrollView* m_pSymbolView;

// Generated message map functions
protected:
	BOOL DoCopy();
	void DoClear();
	HBITMAP ShapeToBitmap(CMDShape* pShape);

	//{{AFX_MSG(CSymbolLibraryView)
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnRename();
	afx_msg void OnSelchangeList();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnEditClear();
	afx_msg void OnUpdateEditClear(CCmdUI* pCmdUI);
	afx_msg void OnFind();
	afx_msg void OnDatum();
	afx_msg void OnCenter();
	afx_msg void OnDblclkList();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnResizeLess();
	afx_msg void OnChangeName();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in SymbolLibraryView.cpp
inline CSymbolLibraryDoc* CSymbolLibraryView::GetDocument()
   { return (CSymbolLibraryDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYMBOLLIBRARYVIEW_H__D9BAD20F_B870_11D2_98EB_000000000000__INCLUDED_)
