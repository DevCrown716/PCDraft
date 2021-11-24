// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__F8C83DF8_0F91_11D2_9806_000000000000__INCLUDED_)
#define AFX_MAINFRM_H__F8C83DF8_0F91_11D2_9806_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ToolPalette.h"
#include "AttribPalette.h"

#ifndef _PERSONAL_EDITION

#include "AccPalette.h"

#endif

#include "DimPalette.h"
#include "ResizePalette.h"
#include "ShowSizePalette.h"
#include "AlignPalette.h"
#include "CustomMenus.h"	// Added by ClassView
#include "FindReplaceDlg.h"
#include "LayerSetupDlg.h"
#include "RibbonBar.h" // Interface Update - Infinisys Ltd.
#include "TabBar.h" // Interface Update - Infinisys Ltd.
#include "ProgressBar.h" // Interface Update - Infinisys Ltd.

class BaseWnd;


class CMainFrame: public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();
// Attributes
public:
	CToolTipCtrl* GetToolTip() { return &m_ToolTip; }

// Operations
public:
	void	DoIdleUpdate();
	void	DoActivateResizePalette();
   bool m_bTopLeftButtons;
   bool m_bIsFirstShow;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
   CProgStatusBar  m_wndStatusBar;

	void UpdatePallettesForCommand(UINT nId);
	bool IsShowSizeVisible();
   CMenu * GetMenu();
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
   CRect m_rectCaption;
   CTabBar m_TabBar; // Interface Upgrade - Infinisys Ltd.
   CRibbonBar m_RibbonBar; // Interface Upgrade - Infinisys Ltd.
   HICON m_hiconCaption; // Interface Update - Infinisys Ltd.
   Gdiplus::Bitmap * m_pbitmapSave; // Interface Update - Infinisys Ltd.
   Gdiplus::Bitmap * m_pbitmapUndo; // Interface Update - Infinisys Ltd.
   Gdiplus::Bitmap * m_pbitmapRedo; // Interface Update - Infinisys Ltd.
   HICON m_hiconSave;
   HICON m_hiconUndo;
   HICON m_hiconRedo;
   //CToolBar       m_toolbar;
   CAttribPalette m_wndAttribPalette;
protected:  // control bar embedded members
	CToolPalette m_wndToolPalette;
#ifndef _PERSONAL_EDITION
	CAccPalette m_wndAccPalette;
#endif
   CMenu * m_pMenu;
	CDimPalette m_wndDimPalette;
	CResizePalette m_wndResizePalette;
	CShowSizePalette m_wndShowSizePalette;
	CAlignPalette m_wndAlignPalette;

	CColorMenu m_ColorMenu;
	CPatternMenu m_PatternMenu;
   CPatternMenu m_DupPatternMenu; // Gradient/Pattern - Infinisys Ltd
	CLineStyleMenu m_LineStyleMenu;

	CFindReplaceDlg m_dlgFindReplace;
	CLayerSetupDlg m_dlgLayerSetup;
	CPtrList m_listHiddenBars;

	CToolTipCtrl m_ToolTip;
// Generated message map functions

   virtual CMenu* GetMenu() const;// Interface Update - Infinisys Ltd.


protected:
	BOOL m_bActivateApp;
	afx_msg void OnMoreWindows();
   afx_msg LRESULT OnProgress(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnNetGuardMsg(WPARAM wParam,LPARAM lParam);
	afx_msg BOOL OnTipCustomDraw(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnToolTipNotify(UINT id, NMHDR * pNMHDR, LRESULT * pResult);
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnUpdateIndicatorProgress(CCmdUI* pCmdUI);
   afx_msg void OnUpdateViewShowToolbar(CCmdUI* pCmdUI);
   afx_msg void OnViewShowToolbar();
   afx_msg void OnUpdateViewHideToolbar(CCmdUI* pCmdUI);
   afx_msg void OnViewHideToolbar();
   afx_msg void OnUpdateViewPaletteTool(CCmdUI* pCmdUI);
	afx_msg void OnViewPaletteTool();
	afx_msg void OnUpdateViewPaletteAttrib(CCmdUI* pCmdUI);
	afx_msg void OnViewPaletteAttrib();
	afx_msg void OnUpdateViewPaletteDim(CCmdUI* pCmdUI);
	afx_msg void OnViewPaletteDim();
#ifndef _PERSONAL_EDITION	
	afx_msg void OnUpdateViewPaletteAcc(CCmdUI* pCmdUI);
	afx_msg void OnViewPaletteAcc();
#endif
	afx_msg void OnViewPaletteAll();
	afx_msg void OnInitMenu(CMenu* pMenu);
	afx_msg void OnUpdateViewPaletteResize(CCmdUI* pCmdUI);
	afx_msg void OnViewPaletteResize();
	afx_msg void OnUpdateViewPaletteShowsize(CCmdUI* pCmdUI);
	afx_msg void OnViewPaletteShowsize();
	afx_msg void OnUpdateViewPaletteAlign(CCmdUI* pCmdUI);
	afx_msg void OnViewPaletteAlign();
	afx_msg void OnDataFindReplace();
	afx_msg void OnLayoutLayerSetup();
	afx_msg BOOL OnQueryNewPalette();
   afx_msg void OnViewLayer();// Layer - Infinisys Ltd.
   afx_msg void OnUpdateViewLayer(CCmdUI* pCmdUI); // Layer - Infinisys Ltd.
	afx_msg void OnClose();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnUpdateDataFindReplace(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLayoutLayerSetup(CCmdUI* pCmdUI);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
   afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
	afx_msg void OnSize(UINT nType,int cx,int cy); // Interface Upgrade - Infinisys Ltd.
   afx_msg void OnPaint(); // Interface Upgrade - Infinisys Ltd.
   
	//}}AFX_MSG

   virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs,CCreateContext*); // Interface Upgrade - Infinisys Ltd.
	DECLARE_MESSAGE_MAP()
public:
   void CreateAngleSnapSubMenu(CPCDraftDoc * pDoc, CMenu * pPopupMenu);
   void CreateGridSubMenu(CPCDraftDoc * pDoc, CMenu * pPopupMenu);
   void TrackPopupMenu(int iID); // Interface Upgrade - Infinisys Ltd.
   afx_msg LRESULT OnDwm(WPARAM wparam,LPARAM lparam); // Interface Upgrade - Infinisys Ltd.
   afx_msg LRESULT OnVoid(WPARAM wparam,LPARAM lparam); // Interface Upgrade - Infinisys Ltd.
   afx_msg void OnNcCalcSize(BOOL bCalcValidRects,NCCALCSIZE_PARAMS* lpncsp); // Interface Upgrade - Infinisys Ltd.
   afx_msg LRESULT OnNcHitTest(CPoint point); // Interface Upgrade - Infinisys Ltd.
   virtual void RecalcLayout(BOOL bNotify = TRUE); // Interface Upgrade - Infinisys Ltd.
   afx_msg void OnLButtonDown(UINT nFlags,CPoint point); // Interface Upgrade - Infinisys Ltd.
   virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
   virtual void UpdateFrameTitleForDocument2(LPCTSTR lpszDocName);
   afx_msg BOOL OnCopyData(CWnd* pWnd,COPYDATASTRUCT* pCopyDataStruct); // Infinisys Ltd.
   afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
   afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
protected:
   afx_msg int OnSetText(LPCTSTR lpszText);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.


__checkReturn HRESULT InitRibbon(__in CMainFrame* pMainFrame,__deref_out_opt IUnknown** ppFramework); // Interface Update - Infinisys Ltd.

void DestroyRibbon(__in IUnknown* pFramework); // Interface Update - Infinisys Ltd.

HRESULT SetModes(__in IUnknown* pFramework,UINT modes); // Interface Update - Infinisys Ltd.

HRESULT RibbonInvalidate(__in IUnknown* pFramework); // Interface Update - Infinisys Ltd.


#endif // !defined(AFX_MAINFRM_H__F8C83DF8_0F91_11D2_9806_000000000000__INCLUDED_)


