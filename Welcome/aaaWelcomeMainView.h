// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#pragma once


class CWelcomeSplitView;
class CWelcomeImageSplitView;
class BaseWnd;

class CWelcomeMainView : public CView
{
public:


   bool                          m_bWelcome;
   CWelcomeSplitView *           m_plibrary;
   CWelcomeImageSplitView *      m_pimage;
   CPoint                        m_ptResize;
   CSize                         m_sizeParent;
   bool                          m_bResize;
   DWORD                         m_dwLastResize;
   BaseWnd *                  m_pWelcomeWnd;


   CWelcomeMainView();           // protected constructor used by dynamic creation
   virtual ~CWelcomeMainView();

   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
   virtual void OnDraw(CDC* pDC);      // overridden to draw this view

#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;
#endif

   void layout();
   void defer_resize(CPoint pt, bool bFinal);
   bool is_resize(CPoint pt);

   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnDestroy();


   DECLARE_DYNCREATE(CWelcomeMainView)
   DECLARE_MESSAGE_MAP()

   afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
   afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
   afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


