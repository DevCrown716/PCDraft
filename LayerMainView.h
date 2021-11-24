// Layer - PCDraft 6 - Infinisys Ltd.
#pragma once


class CLayerHeader;
class CLayerListView;
class CLayerFooter;


class CLayerMainView : 
   public CView
{
public:


   int                  m_y1;
   int                  m_y2;
   int                  m_iFooterHeight;
   CLayerHeader *       m_pheader;
   CLayerListView *     m_plistview;
   CLayerFooter *       m_pfooter;

   CPoint                        m_ptResize;
   CSize                         m_sizeParent;
   bool                          m_bResize;
   DWORD                         m_dwLastResize;

   
   CLayerMainView();
   virtual ~CLayerMainView();

#ifdef _DEBUG
   void AssertValid() const;
   void Dump(CDumpContext& dc) const;
#endif

   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
   virtual void OnDraw(CDC * pdc);

   void layout();

   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnSize(UINT nType,int cx,int cy);
   afx_msg void OnDestroy();

   DECLARE_DYNCREATE(CLayerMainView)
   DECLARE_MESSAGE_MAP()
   virtual bool is_resize(CPoint pt);
   virtual void defer_resize(CPoint pt,bool bFinal);
   afx_msg BOOL OnEraseBkgnd(CDC* pDC);

   afx_msg void OnMouseMove(UINT nFlags,CPoint point);
   afx_msg void OnLButtonDown(UINT nFlags,CPoint point);
   afx_msg void OnLButtonUp(UINT nFlags,CPoint point);
   afx_msg void OnNcMouseMove(UINT nHitTest,CPoint point);
};


