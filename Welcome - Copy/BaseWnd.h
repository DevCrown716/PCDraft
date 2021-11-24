// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#pragma once
#include <afxwin.h>
#include "ControlBox.h"


class WelcomeLeftWnd;


class BaseWnd :
   public CWnd
{
public:


   CWnd *            m_pWndFocus;
   ControlBox        m_controlbox;
   CWnd *            m_pParentWnd;
   HWND              m_hWndTop;
   double            m_dDpiScaler;
   double            m_dSizeScaler;
   bool              m_bResizable;
   bool              m_bResizeGrip;
   Gdiplus::Color    m_colorBackground;
   bool              m_bDefaultDrawBackground;
   bool              m_bControlBox;


   BaseWnd();
   ~BaseWnd() override;

   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
   void ShowFonts(Gdiplus::Graphics * pgraphics);

   virtual BOOL Create();


   virtual void OnEditChangeText(int nID);

   bool is_narrow_window();

   LPCTSTR GetIconWndClass(DWORD dwDefaultStyle, UINT nIDResource);
   DECLARE_MESSAGE_MAP()
   afx_msg LRESULT OnNcHitTest(CPoint mouse);
   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
   afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
   afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnPaint();

   void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI); // Interface Update - Infinisys Ltd.
   void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);


   virtual void OnDraw(CDC * pdc, Gdiplus::Graphics * pgraphics);
public:

   virtual INT_PTR DoModal();

   virtual void EndDialog(int nResult);

   virtual HWND PreModal();

   void PostModal();

   virtual void  OnButtonPress(int i);

   virtual double font_scaler();
   virtual double size_scaler();


   afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
   afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


CSize GetDesktopSize();
