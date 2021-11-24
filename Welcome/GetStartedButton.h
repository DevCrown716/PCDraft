// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#pragma once
#include "BaseControl.h"


class GetStartedButton :
   public BaseControl
{
public:


   Gdiplus::Font * m_pfontLine1;
   Gdiplus::Font * m_pfontLine2;
   Gdiplus::Image * m_pimage44;


   bool m_bSetHover;

   GetStartedButton();
   ~GetStartedButton() override;


   void OnDraw(CDC * pdc, Gdiplus::Graphics * pgraphics) override;


   DECLARE_MESSAGE_MAP()
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

   afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   afx_msg void OnMouseLeave();
   afx_msg BOOL OnEraseBkgnd(CDC* pDC);

   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};


