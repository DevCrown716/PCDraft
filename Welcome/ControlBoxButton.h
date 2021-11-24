// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#pragma once
#include "BaseControl.h"


class ControlBoxButton :
   public BaseControl
{
public:


   Gdiplus::Color          m_colorBackground;
   Gdiplus::Color          m_colorForeground;
   Gdiplus::Font *         m_pfontMarlett;


   ControlBoxButton();
   ~ControlBoxButton() override;
   
   
   
   void OnDraw(CDC * pdc, Gdiplus::Graphics * pgraphics) override;



   DECLARE_MESSAGE_MAP()
   afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
   afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
   afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

   afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   afx_msg void OnMouseLeave();
   afx_msg BOOL OnEraseBkgnd(CDC* pDC);

};


