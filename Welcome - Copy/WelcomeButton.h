// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#pragma once
#include "BaseControl.h"


class WelcomeButton :
   public BaseControl
{
public:


   Gdiplus::Font * m_pfont;


   bool m_bSetHover;

   WelcomeButton();
   ~WelcomeButton() override;


   void OnDraw(CDC * pdc, Gdiplus::Graphics * pgraphics) override;

   DECLARE_MESSAGE_MAP()
   //afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
   afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
   afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

   afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   afx_msg void OnMouseLeave();
   afx_msg BOOL OnEraseBkgnd(CDC* pDC);

   //afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnEnable(BOOL bEnable);
};


