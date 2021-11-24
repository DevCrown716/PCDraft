// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#pragma once
#include <afxwin.h>


class BaseWnd;

class CScrollHelper;

class BaseControl :
   public CWnd
{
public:

   
   bool                    m_bComposited;
   BaseWnd  *              m_pbasewnd;
   bool                    m_bHoverNotify;
   bool                    m_bSetHover;
   bool                    m_bDefaultClick;
   COLORREF                m_crBackground;
   bool                    m_bBaseControlArrowNavigation;


   BaseControl();
   ~BaseControl() override;

   LPCTSTR GetIconWndClass(DWORD dwDefaultStyle, UINT nIDResource);

   virtual void SetFocusNext();
   virtual void SetFocusPrevious();
   virtual void OnClick();


   virtual void OnDraw(CDC * pdc, Gdiplus::Graphics * pgraphics);


   virtual double font_scaler();
   virtual double size_scaler();


protected:
   // ClassWizard generated virtual function overrides.
   //{{AFX_VIRTUAL(CScrollWnd)
   virtual void PostNcDestroy();
   //}}AFX_VIRTUAL

   // Generated message map functions.
   //{{AFX_MSG(CScrollWnd)
   afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg BOOL OnEraseBkgnd(CDC* pDC);
   afx_msg int  OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
   afx_msg void OnPaint();
   afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
   afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
   afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
   afx_msg void OnSize(UINT nType, int cx, int cy);
   //}}AFX_MSG

   void DrawScrollInfo(CDC* pDC);

   void enable_scroll_helper();

   CScrollHelper* m_scrollHelper;
   DECLARE_MESSAGE_MAP()
   afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
   afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
   //afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   //afx_msg void OnPaint();
   afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   afx_msg void OnMouseLeave();


   DECLARE_DYNAMIC(BaseControl);

   afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
   afx_msg void OnKillFocus(CWnd* pNewWnd);
   afx_msg void OnSetFocus(CWnd* pOldWnd);
   //afx_msg BOOL OnEraseBkgnd(CDC* pDC);
   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
   //afx_msg void OnNcPaint();
   //afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
};


CSize GetDesktopSize();
